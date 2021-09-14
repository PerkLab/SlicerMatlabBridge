#include "MatlabCommanderCLP.h" 

#include <iostream>
#include <fstream>
#include <math.h>
#include <cstdlib>

#include "igtlOSUtil.h"
#include "igtlStringMessage.h"
#include "igtlClientSocket.h"

#include "vtksys/SystemTools.hxx"
#include "vtksys/Process.h"

const std::string CALL_MATLAB_FUNCTION_ARG="--call-matlab-function";
const std::string EXIT_MATLAB_ARG="--exit-matlab";
const std::string MATLAB_DEFAULT_HOST="127.0.0.1";
const int MATLAB_DEFAULT_PORT=4100;

const int MAX_MATLAB_STARTUP_TIME_SEC=60; // maximum time allowed for Matlab to start

// If the Matlab function response string starts with this string then it means
// the function execution failed
const std::string RESPONSE_ERROR_PREFIX="ERROR:";

enum ExecuteMatlabCommandStatus
{
  COMMAND_STATUS_FAILED=0,
  COMMAND_STATUS_SUCCESS=1
};

std::string ReceiveString(igtl::Socket * socket, igtl::MessageHeader::Pointer& header)
{
  // Create a message buffer to receive transform data
  igtl::StringMessage::Pointer stringMsg;
  stringMsg = igtl::StringMessage::New();
  stringMsg->SetMessageHeader(header);
  stringMsg->AllocatePack();

  // Receive transform data from the socket
  bool receiveTimedOut = false;
  int received=socket->Receive(stringMsg->GetPackBodyPointer(), stringMsg->GetPackBodySize(), receiveTimedOut);
  if (received!=stringMsg->GetPackBodySize())
  {
    std::cerr << "WARNING: failed to receive complete message body" << std::endl;
  }
  if (receiveTimedOut)
  {
    std::cerr << "WARNING: receiving timed out" << std::endl;
  }

  // Deserialize the transform data
  // If you want to skip CRC check, call Unpack() without argument.
  int c = stringMsg->Unpack();

  if (c & igtl::MessageHeader::UNPACK_BODY) // if CRC check is OK
  {
    return stringMsg->GetString();
  }

  // error
  return "";
}

void SetReturnValues(const std::string &returnParameterFile,const char* reply, bool completed)
{
  // Write out the return parameters in "name = value" form
  std::ofstream rts;
  rts.open(returnParameterFile.c_str() );
  rts << "reply = " << reply << std::endl;
  rts << "completed = " << (completed?"true":"false") << std::endl;
  rts.close(); 
}

// Returns true if execution is successful. Matlab start may take an additional minute after this function returns.
bool StartMatlabServer()
{
  const char* matlabExecutablePath=getenv("SLICER_MATLAB_EXECUTABLE_PATH");
  const char* matlabCommandServerScriptPath=getenv("SLICER_MATLAB_COMMAND_SERVER_SCRIPT_PATH");

  if ( matlabExecutablePath == NULL )
  {
    std::cerr << "ERROR: The SLICER_MATLAB_EXECUTABLE_PATH environment variable is not set. Cannot start the Matlab command server." << std::endl;
    return false; 
  }
  if ( matlabCommandServerScriptPath == NULL )
  {
    std::cerr << "ERROR: The SLICER_MATLAB_COMMAND_SERVER_SCRIPT_PATH environment variable is not set. Cannot start the Matlab command server." << std::endl;
    return false; 
  }

  if ( !vtksys::SystemTools::FileExists( matlabExecutablePath, true) )
  {
    std::cerr << "ERROR: Unable to find Matlab executable defined in the SLICER_MATLAB_EXECUTABLE_PATH environment variable: " 
      << matlabExecutablePath << std::endl;
    return false; 
  }

  bool success = true; 
  try 
  {
    std::vector<const char*> command;
    command.clear();

    // Compose Matlab launching command
    std::cout << "Starting Matlab server: " << matlabExecutablePath; 
    command.push_back(matlabExecutablePath);

    // start in minimized, with text console only
    std::cout << " -automation"; 
    command.push_back("-automation");

    // script directory (-sd) option does not work with the automation option, so need to use the run command to specify full script path

    // run script after startup
#if defined( _WIN32 ) && !defined(__CYGWIN__) 
    // Windows requires parameter and script name as two separate arguments
    std::string startupCommand1=std::string("-r");
    std::cout << " " << startupCommand1;
    command.push_back(startupCommand1.c_str());
    std::string startupCommand2=std::string("\"run('")+matlabCommandServerScriptPath+"');\"";
    std::cout << " " << startupCommand2;
    command.push_back(startupCommand2.c_str());
#else
    // Linux/Mac OS X requires parameter and script name as one argument
    std::string startupCommand=std::string("-r \"run('")+matlabCommandServerScriptPath+"');\"";
    std::cout << " " << startupCommand; 
    command.push_back(startupCommand.c_str());
#endif

    // The array must end with a NULL pointer.
    std::cout << std::endl;
    command.push_back(0); 

    // Create new process 
    vtksysProcess* gp = vtksysProcess_New();

    // Set command
    vtksysProcess_SetCommand(gp, &*command.begin());

    // Hide window
    vtksysProcess_SetOption(gp,vtksysProcess_Option_HideWindow, 1);

#if defined( _WIN32 ) && !defined(__CYGWIN__) 
    // No need to redirect process output on Windows
#else
    // Need to redirect the outputs to dev/null on Linux and Mac OS X,
    // otherwise Matlab terminates right after it is started
    vtksysProcess_SetPipeFile(gp, vtksysProcess_Pipe_STDOUT, "/dev/null");
    vtksysProcess_SetPipeFile(gp, vtksysProcess_Pipe_STDERR, "/dev/null");
#endif

    // Start a detached process, because we don't want to block the MatlabCommander if the started Matlab process does not exit
    // (usually the Matlab process exits within a few seconds, but on some configurations it does not)
    vtksysProcess_SetOption(gp, vtksysProcess_Option_Detach, 1);

    // Run the application
    vtksysProcess_Execute(gp);

    // Release the application and let it run in the background
    vtksysProcess_Disown(gp);

    switch (vtksysProcess_GetState(gp))
    {
    case vtksysProcess_State_Disowned:
      // This is the success case
      std::cout << "Successfully detached from the started Matlab process" <<std::endl;
      break;
    case vtksysProcess_State_Error:
      // This is the most common error case (process could not be started)
      std::cerr << "ERROR: Error starting the Matlab process: "<<matlabExecutablePath<<" ["<<vtksysProcess_GetErrorString(gp)<<"]" << std::endl;
      success=false;
      break;
    case vtksysProcess_State_Starting:
      std::cerr << "ERROR: No Matlab process ("<<matlabExecutablePath<<") has been executed." << std::endl; 
      success=false;
      break;
    case vtksysProcess_State_Executing:
      std::cerr << "ERROR: The Matlab process ("<<matlabExecutablePath<<") is still executing." << std::endl; 
      success=false;
      break;
    case vtksysProcess_State_Expired:
      std::cerr << "ERROR: Matlab process ("<<matlabExecutablePath<<") was killed when timeout expired." <<std::endl; 
      success=false;
      break;
    case vtksysProcess_State_Exited:
      std::cerr << "ERROR: Matlab process ("<<matlabExecutablePath<<") exited with value = " << vtksysProcess_GetExitValue(gp) << std::endl; 
      success=false;
      break;
    case vtksysProcess_State_Killed:
      std::cerr << "ERROR: Matlab process ("<<matlabExecutablePath<<") was killed by parent." << std::endl; 
      success=false;
      break;
    case vtksysProcess_State_Exception:
      std::cerr << "ERROR: Matlab process ("<<matlabExecutablePath<<") terminated abnormally: " << vtksysProcess_GetExceptionString(gp) << std::endl; 
      success=false;
      break;
    default:
      std::cerr << "ERROR: Unknown error when trying to start the Matlab process ("<<matlabExecutablePath<<")"<<std::endl; 
      success=false;
      break;
    }

  }
  catch (...)
  {
    std::cerr << "ERROR: Unknown exception while trying to execute Matlab process ("<<matlabExecutablePath<<")" << std::endl; 
    success=false;
  }
  
  return success;
}

ExecuteMatlabCommandStatus ExecuteMatlabCommand(const std::string& hostname, int port, const std::string &cmd, std::string &reply, int receiveTimeoutMsec = 0)
{
  //------------------------------------------------------------
  // Establish Connection
  igtl::ClientSocket::Pointer socket = igtl::ClientSocket::New();
  int connectErrorCode = socket->ConnectToServer(hostname.c_str(), port);
  if (connectErrorCode!=0)
  {
    // Maybe Matlab server has not been started, try to start it
    if (StartMatlabServer())
    {
      // process start requested, try to connect
      for (int retryAttempts=0; retryAttempts<MAX_MATLAB_STARTUP_TIME_SEC; retryAttempts++)
      {
        connectErrorCode = socket->ConnectToServer(hostname.c_str(), port);
        if (connectErrorCode==0)
        {
          // success
          break;
        }
        // Failed to connect, wait some more and retry
        vtksys::SystemTools::Delay(1000); // msec
        std::cerr << "Waiting for Matlab startup ... " << retryAttempts << "sec" << std::endl;
      }
    }
    else
    {
      std::cerr << "ERROR: Failed to start Matlab process" << std::endl;
    }
  }
  if (connectErrorCode != 0)
  {        
    reply="ERROR: Cannot connect to the server";
    return COMMAND_STATUS_FAILED;
  }

  //------------------------------------------------------------
  // Send command
  igtl::StringMessage::Pointer stringMsg;
  stringMsg = igtl::StringMessage::New();
  stringMsg->SetDeviceName("CMD");
  std::cout << "Sending string: " << cmd << std::endl;
  stringMsg->SetString(cmd.c_str());
  stringMsg->Pack();
  socket->SetSendTimeout(5000); // timeout in msec
  if (!socket->Send(stringMsg->GetPackPointer(), stringMsg->GetPackSize()))
  {
    // Failed to send the message
    std::cerr << "Failed to send message to Matlab process" << std::endl;
    socket->CloseSocket();
    return COMMAND_STATUS_FAILED;
  }

  //------------------------------------------------------------
  // Receive reply
  // Create a message buffer to receive header
  igtl::MessageHeader::Pointer headerMsg;
  headerMsg = igtl::MessageHeader::New();
  // Initialize receive buffer
  headerMsg->InitPack();
  // Receive generic header from the socket
  if (receiveTimeoutMsec>0)
  {
    socket->SetReceiveTimeout(receiveTimeoutMsec); // timeout in msec
  }

  bool receiveTimedOut = false;
  int receivedBytes = socket->Receive(headerMsg->GetPackPointer(), headerMsg->GetPackSize(), receiveTimedOut);
  if (receivedBytes == 0)
  {
    reply="No reply";
    socket->CloseSocket();
    return COMMAND_STATUS_FAILED;
  }
  if (receivedBytes != headerMsg->GetPackSize() || receiveTimedOut)
  {
    reply = "Bad reply";
    socket->CloseSocket();
    return COMMAND_STATUS_FAILED;
  }
  // Deserialize the header
  headerMsg->Unpack();
  if (strcmp(headerMsg->GetDeviceType(), "STRING") != 0)
  {
    reply = std::string("Receiving unsupported message type: ") + headerMsg->GetDeviceType();
    socket->Skip(headerMsg->GetBodySizeToRead(), 0);
    socket->CloseSocket();
    return COMMAND_STATUS_FAILED;
  }
  // Get the reply string
  reply=ReceiveString(socket, headerMsg);

  //------------------------------------------------------------
  // Close connection
  socket->CloseSocket();

  return COMMAND_STATUS_SUCCESS;
}

int ExitMatlab()
{
  igtl::ClientSocket::Pointer socket = igtl::ClientSocket::New();
  int connectErrorCode = socket->ConnectToServer(MATLAB_DEFAULT_HOST.c_str(), MATLAB_DEFAULT_PORT);
  if (connectErrorCode!=0)
  {
    // The server has not been started, nothing to do
    std::cout << "Matlab process is already stopped" << std::endl;
    return EXIT_SUCCESS;
  }

  std::string cmd = "exit";

  //------------------------------------------------------------
  // Send command
  igtl::StringMessage::Pointer stringMsg;
  stringMsg = igtl::StringMessage::New();
  stringMsg->SetDeviceName("CMD");
  std::cout << "Sending string: " << cmd << std::endl;
  stringMsg->SetString(cmd.c_str());
  stringMsg->Pack();
  socket->SetSendTimeout(5000); // timeout in msec
  if (!socket->Send(stringMsg->GetPackPointer(), stringMsg->GetPackSize()))
  {
    // Failed to send the message
    std::cerr << "Failed to send message to Matlab process" << std::endl;
    socket->CloseSocket();
    return COMMAND_STATUS_FAILED;
  }

  // Close connection
  socket->CloseSocket();

  std::cout << "Matlab process exit requested" << std::endl;
  return EXIT_SUCCESS;  
}


int CallMatlabFunction(int argc, char * argv [])
{
  // Search for the --returnparameterfile argument. If it is present then arguments shall be returned.
  const std::string returnParameterFileArgName="--returnparameterfile";
  std::string returnParameterFileArgValue;
  for (int argvIndex=3; argvIndex<argc; argvIndex++)
  {
    if (returnParameterFileArgName.compare(argv[argvIndex])==0)
    {
      // found the return parameter file name
      if (argvIndex+1>=argc)
      {
        std::cerr << "ERROR: --returnparameterfile value is not defined" << std::endl;
        break;
      }
      returnParameterFileArgValue=argv[argvIndex+1];
      if ( returnParameterFileArgValue.at(0) == '"' ) 
      {
        // this is a quoted string => remove the quotes (Matlab uses different quotes anyway)
        returnParameterFileArgValue.erase( 0, 1 ); // erase the first character
        returnParameterFileArgValue.erase( returnParameterFileArgValue.size() - 1 ); // erase the last character
      }
      break;
    }
  }

  std::string cmd;

  // Change directory to the current working directory (where the Matlab function .m file is located)
  cmd += "cd('"+vtksys::SystemTools::GetCurrentWorkingDirectory()+"'); "; 

  // No return value:
  //   myfunction( cli_argsread({"--paramName1","paramValue1",...}) );
  // With return value:
  //   cli_argswrite( myfunction( cli_argsread({"--paramName1","paramValue1",...}) ) );

  if (!returnParameterFileArgValue.empty())
  {
    // with return value
    cmd+="cli_argswrite('"+returnParameterFileArgValue+"',";
  }
  std::string functionName=argv[2];
  cmd+=functionName+"(cli_argsread({";

  for (int argvIndex=3; argvIndex<argc; argvIndex++)
  {
    std::string arg=argv[argvIndex];
    if ( arg.at(0) == '"' ) 
    {
      // this is a quoted string => remove the quotes (Matlab uses different quotes anyway)
      arg.erase( 0, 1 ); // erase the first character
      arg.erase( arg.size() - 1 ); // erase the last character
    }
    cmd+=std::string("'")+arg+"'";
    if (argvIndex+1<argc)
    {
      // not the last argument, so add a separator
      cmd+=",";
    }
  }
  cmd+="}))";
  if (!returnParameterFileArgValue.empty())
  {
    // with return value
    cmd+=")";
  }
  cmd+=";";

  std::cout << "Command: " << cmd << std::endl;

  std::string reply;
  ExecuteMatlabCommandStatus status=ExecuteMatlabCommand(MATLAB_DEFAULT_HOST, MATLAB_DEFAULT_PORT, cmd, reply);
  if (status!=COMMAND_STATUS_SUCCESS)
  {
    std::cerr << reply << std::endl;
    return EXIT_FAILURE;
  }
  // Response is usually 'OK' (if the function did not have any output) or some printouts.
  // In case of an error, the response starts with ERROR:...
  if (reply.size()>RESPONSE_ERROR_PREFIX.size() && reply.compare(0,RESPONSE_ERROR_PREFIX.size(),RESPONSE_ERROR_PREFIX)==0)
  {
    // the response starts with "ERROR:", so the execution failed
    std::cerr << "Failed to execute Matlab function: " << functionName << ", received the following error message: " << std::endl;
    std::cerr << reply << std::endl;
    return EXIT_FAILURE;
  }

  std::cout << reply << std::endl;
  return EXIT_SUCCESS;  
}


int CallStandardCli(int argc, char * argv [])
{
  PARSE_ARGS;

  if (!cmd.empty())
  {
    // Execute command
    ExecuteMatlabCommandStatus status=ExecuteMatlabCommand(hostname, port, cmd, reply);
    if (status==COMMAND_STATUS_SUCCESS)
    {
      std::cout << reply << std::endl;
      completed=true;
    }
    else
    {
      std::cerr << reply << std::endl;
      completed=false;
    }

    // Remove newline characters, as it would confuse the return parameter file
    std::replace( reply.begin(), reply.end(), '\r', ' ');
    std::replace( reply.begin(), reply.end(), '\n', ' ');
  }
  else
  {
    // Empty command, for example when we just want to exit Matlab
    reply.clear();
    completed = true;
  }
  
  SetReturnValues(returnParameterFile,reply.c_str(),completed);

  // Exit Matlab
  if (exitmatlab == true)
  {
    return ExitMatlab();
  }

  return EXIT_SUCCESS; // always return with EXIT_SUCCESS, otherwise Slicer ignores the return values and we cannot show the reply on the module GUI
}

int main (int argc, char * argv [])
{
  if (argc>2 && CALL_MATLAB_FUNCTION_ARG.compare(argv[1])==0)
  {
    // MatlabCommander is called with arguments: --call-matlab-function function_name parameter1 parameter2 ...
    return CallMatlabFunction(argc, argv);
  }
  else if (argc==2 && EXIT_MATLAB_ARG.compare(argv[1])==0)
  {
    // MatlabCommander is called with arguments: --exit-matlab
    return ExitMatlab();
  }
  else
  {
    // MatlabCommander is called as a standard CLI modul
    return CallStandardCli(argc, argv);
  }
} 

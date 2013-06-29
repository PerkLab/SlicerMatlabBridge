function params=cli_argsread(args)
% Retrieve parameters in a structure from a list of command-line arguments
% The output structure contains all the named arguments (field name is the command-line argument name)
% and an "unnamed" argument containing the list of unnamed arguments in a cell.

args=fixupArgumentList(args);

params={};
params.unnamed={};

curArgName='';
curArgValue='';
for curArgIndex=1:length(args)
    if (regexp(args{curArgIndex},'^-[-]?[a-zA-Z]')==1)
        % This is an argument name (as it starts with - or -- followed by letters)
        % There may be an argument name without a value (flag parameter) or value
        % without a name (unnamed parameter) => save it before we start
        % processing the new argument
        params=saveParam(params,curArgName,curArgValue);
        curArgName='';
        curArgValue='';       
        % Save the new argument name (but don't store it yet, it may be
        % followed by an argument value)
        % Strip the - or -- from the argument name
        [tokens matches]=regexp(args{curArgIndex},'^-[-]?([^-]+)','tokens','match');
        curArgName=tokens{1}{1};
    else
        % This is an argument value
        curArgValue=getValueFromString(args{curArgIndex});
        % We have a pair of argument name and value => save it
        params=saveParam(params,curArgName,curArgValue);
        curArgName='';
        curArgValue='';
    end
end

% If there is an argument that has not yet stored, store it
params=saveParam(params,curArgName,curArgValue);

end

%% Helper functions

% Fixing up the parameter list
% Fiducial point name and value is stored in one argument:
%  '--somefiducial 12.3,14.6, 18.7'
% Detect this case and convert it to
%  '--somefiducial' '12.3,14.6, 18.7'
function updatedArgs=fixupArgumentList(originalArgs)
    updatedArgs={};
    for curArgIndex=1:length(originalArgs)
        if (regexp(originalArgs{curArgIndex},'^-[-]?[a-zA-Z].*[ ].+')==1)
            % Detected a space in an argument name, e.g., '--somefiducial 12.3,14.6,18.7'
            [matchedTokens matchedString]=regexp(originalArgs{curArgIndex},'^(-[-]?[a-zA-Z]+[^ ]*)[ ](.+)','tokens','match');
            argName=matchedTokens{1}{1}; % '--somefiducial'
            argValue=matchedTokens{1}{2}; % '12.3,14.6,18.7'
            % Insert the split argName and argValue in the argument list
            updatedArgs=[updatedArgs {argName} {argValue}];
        else
            updatedArgs=[updatedArgs {originalArgs{curArgIndex}}];
        end
    end
end

function paramValue=getValueFromString(paramString)
% Store numerical values as numbers (to don't require the user to call str2num to get a numerical parameter from a string)
  [paramValue paramCount paramError]=sscanf(paramString,'%f,'); % reads one or more comma-separated floating point values
  if (isempty(paramError))
    % No error, so it was a number or vector of numbers
    return;
  end
  % There was an error, so we interpret this value as a string
  paramValue=paramString;
end

function updatedParams=saveParam(originalParams,curArgName,curArgValue)
    updatedParams=originalParams;   
    if (isempty(curArgName))        
        if (~isempty(curArgValue))
            updatedParams.unnamed{length(updatedParams.unnamed)+1}=curArgValue;
        end
    else
        if (isfield(updatedParams, curArgName))
            % This field exists already, so add the value
            updatedParams.(curArgName)=[updatedParams.(curArgName) {curArgValue}];
        else
            updatedParams.(curArgName)=curArgValue;
        end
    end
end

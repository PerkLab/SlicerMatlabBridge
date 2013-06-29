function measurement = cli_measurementread(filename)
%cli_measurementread  Read measurement table from Slicer measurement CSV file
%

% Sample file contents:
%
% # measurement file C:/Users/someuser/AppData/Local/Temp/FBGA_vtkMRMLDoubleArrayNodeB.csv
% # columns = x,y,yerr
% 2.0,4.0,0.002
% 5.2,1.5,0.001
% 5.1,2.5,0.005
%

try
  measurement=csvread(filename,2);
catch ME
  % failed with and without an output, the command must be invalid
  disp(['Could not retrieve any measurement values from: ' filename]);
  measurement=[];
end

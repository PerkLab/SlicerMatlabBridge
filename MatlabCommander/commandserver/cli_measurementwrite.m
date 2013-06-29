function cli_measurementwrite(outputfilename, measurements)
%cli_measurementwrite  Write a list of measurements to a "Insight Transform File V1.0" text file
%

% Sample file contents:
%
% # measurement file C:/Users/someuser/AppData/Local/Temp/FBGA_vtkMRMLDoubleArrayNodeB.csv
% # columns = x,y,yerr
% 2.0,4.0,0.002
% 5.2,1.5,0.001
% 5.1,2.5,0.005
%

[rows columns] = size(measurements);
assert(rows>0, 'The measurement matrix is empty');
assert(columns==3, 'The measurement matrix must have 3 columns');

% Write header
fid=fopen(outputfilename, 'w');
assert(fid > 0, 'Could not open file %s', outputfilename);
fprintf(fid,'# measurement file %s\n', outputfilename);
fprintf(fid,'# columns = x,y,yerr\n');
fclose(fid);

dlmwrite(outputfilename,measurements,'-append','delimiter',',');

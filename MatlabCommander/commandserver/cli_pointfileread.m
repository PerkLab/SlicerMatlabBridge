function points = cli_pointfileread(filename)
% Read image markups from a Slicer fcsv file
%   points = cli_pointfileread(filename) reads the image volume and associated metadata
%
%   points.position: point positions (Nx3)
%   points.orientation: point orientations as quaternion (Nx4)
%   points.id: point identifiers
%   points.visibility: non-zero if point is visible
%   points.selected: non-zero if point is selected
%   points.locked: non-zero if point is protected from editing by the user
%   points.label: displayed label string
%   points.description: description of the point
%   points.associatedNodeID: ID od te node where the point was marked on
%
%   points.headerVersion: first header line
%   points.headerCoordinateSystem: second header line
%   points.headerColumns: third header line
%
% Sample file contents:
%
% # Markups fiducial file version = 4.7
% # CoordinateSystem = 1
% # columns = id,x,y,z,ow,ox,oy,oz,vis,sel,lock,label,desc,associatedNodeID
% vtkMRMLMarkupsFiducialNode_122,-31.27,-28.3827,40.0248,0,0,0,1,1,0,0,,,
% vtkMRMLMarkupsFiducialNode_123,-30.7427,-28.8846,40.7103,0,0,0,1,1,0,0,,,
% vtkMRMLMarkupsFiducialNode_124,-30.2028,-29.4316,41.3501,0,0,0,1,1,0,0,,,
% vtkMRMLMarkupsFiducialNode_125,-29.6359,-30.022,41.9246,0,0,0,1,1,0,0,,,
%

points = {}

fid = fopen(filename, 'r');
assert(fid > 0, 'Could not open file.');
cleaner = onCleanup(@() fclose(fid));

% Ignore the first 3 lines, they are just headers
% (it may be useful to parse these lines in the future to improve robustnes
% and compatibility)
points.headerVersion = fgetl(fid);
points.headerCoordinateSystem = fgetl(fid);
points.headerColumns = fgetl(fid);

columns = textscan(fid, '%s %f%f%f %f%f%f%f %d%d%d %s %s %s','delimiter',',')

points.id = columns{1}
points.position = [columns{2}, columns{3}, columns{4}]
points.orientation = [columns{5}, columns{6}, columns{7}, columns{8}]
points.visibility = columns{9}
points.selected = columns{10}
points.locked = columns{11}
points.label = columns{12}
points.description = columns{13}
points.associatedNodeID = columns{14}

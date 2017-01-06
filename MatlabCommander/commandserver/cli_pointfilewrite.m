function cli_pointfilewrite(outputfilename, points)
%cli_measurementwrite  Write a list of measurements to a Slicer fcsv text file
%
% See more details at cli_pointfileread
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

% Write header
fid=fopen(outputfilename, 'w');
assert(fid > 0, 'Could not open file %s', outputfilename);
fprintf(fid,'%s\n', points.headerVersion);
fprintf(fid,'%s\n', points.headerCoordinateSystem);
fprintf(fid,'%s\n', points.headerColumns);
numOfPoints = size(points.position,1);
for k = 1:numOfPoints
  % Retrieve optional properties
  label = '';
  if k <= size(points.label,1)
    label = points.label(k);
  end
  description = '';
  if k <= size(points.description,1)
    description = points.description(k);
  end
  associatedNodeID = '';
  if k <= size(points.associatedNodeID,1)
    associatedNodeID = points.associatedNodeID(k);
  end
  % Write to file
  fprintf(fid,'%s,%g,%g,%g,%g,%g,%g,%g,%d,%d,%d,%s,%s,%s\n',...
    points.id(k,:),...
    points.position(k,1), points.position(k,2), points.position(k,3),...
    points.orientation(k,1), points.orientation(k,2), points.orientation(k,3), points.orientation(k,4),...
    points.visibility(k), points.selected(k), points.locked(k),...
    label, description, associatedNodeID);
end
fclose(fid);

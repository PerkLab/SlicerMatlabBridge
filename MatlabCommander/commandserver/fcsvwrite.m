function fcsvwrite(outputfilename, position, id, visibile, selected, locked, label, description, orientation)
%fcsvwrite  Write a list of fiducial markups to Slicer fcsv file
%

% Sample file contents:
%
% # Markups fiducial file version = 4.5
% # CoordinateSystem = 0
% # columns = id,x,y,z,ow,ox,oy,oz,vis,sel,lock,label,desc,associatedNodeID
% vtkMRMLMarkupsFiducialNode_0,-2.49926,8.41401,78.7389,0,0,0,1,1,1,0,F-1,,vtkMRMLModelNode5
% vtkMRMLMarkupsFiducialNode_1,-7.29697,13.0602,74.4859,0,0,0,1,1,1,0,F-2,,vtkMRMLModelNode5
% vtkMRMLMarkupsFiducialNode_2,-5.4786,10.4094,76.1044,0,0,0,1,1,1,0,F-3,,vtkMRMLModelNode5
%

[rows columns] = size(position);
if rows>0
  assert(columns==3, 'Position matrix must have 3 columns');
end

if nargin < 3
  id = strtrim(cellstr(num2str([1:rows]'))');
end
if nargin < 4
  visible = ones(rows,1);
end
if nargin < 5
  selected = ones(rows,1);
end
if nargin < 6
  locked = zeros(rows,1);
end
if nargin < 7
  label = cell(rows,1);
end
if nargin < 8
  description = cell(rows,1);
end
if nargin < 9
  orientation = [zeros(rows,3) ones(rows,1)];
else
  [orientationRows orientationColumns] = size(orientation)
  assert(orientationRows==rows, 'Orientation matrix must the same number of rows as the position matrix');
  assert(orientationColumns==4, 'Orientation matrix must have 4 columns');
end

associatedNodeID = ''; % use just a constant empty associated node ID for all points

% Write header
fid=fopen(outputfilename, 'w');
assert(fid > 0, 'Could not open file %s', outputfilename);
fprintf(fid,'# Markups fiducial file version = 4.5\n');
fprintf(fid,'# CoordinateSystem = 0\n');
fprintf(fid,'# columns = id,x,y,z,ow,ox,oy,oz,vis,sel,lock,label,desc,associatedNodeID\n');
for rowIndex=1:rows
  fprintf(fid,'%s,%f,%f,%f,%f,%f,%f,%f,%d,%d,%d,%s,%s,%s\n',char(id(rowIndex)),
    position(rowIndex,1),position(rowIndex,2),position(rowIndex,3),
    orientation(rowIndex,1),orientation(rowIndex,2),orientation(rowIndex,3),orientation(rowIndex,4),
    visible(rowIndex),selected(rowIndex),locked(rowIndex),
    char(label(rowIndex)),char(description(rowIndex)),associatedNodeID);
end
fclose(fid);

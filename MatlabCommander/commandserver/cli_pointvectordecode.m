function points=cli_pointvectordecode(paramValue)
% Creates a [3xN] matrix with the point positions in LPS coordinate system

if (iscell(paramValue))
  points=cell2mat(paramValue);
else
  % Single fiducial point, it's not stored in a cell
  points=paramValue;
end

% Point coordinates are provided by Slicer in RAS coordinate system
% Multiply the first two components by -1 to obtain points in LPS coordinate system
points(1:2,:)=-1*points(1:2,:);

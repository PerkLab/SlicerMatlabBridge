function cli_geometrywrite(outputFilename, geometry)
% Function for writing mesh data to PLY or STL file
%
%   geometry.vertices contains the vertices for all triangles in LPS coordinate system [3*n x 3].
%   geometry.faces contains the vertex lists defining each triangle face [n x 3].
%
%   Current limitations/caveats:
%   * Only triangle mesh geometry is supported.

% Slicer expects mesh points in RAS coordinate system by default, convert from LPS now
geometry.vertices(:,1:2) = -1*geometry.vertices(:,1:2)

[pathstr,name,ext] = fileparts(outputFilename);
if (strcmpi(ext,'.stl'))
  stlwrite(outputFilename, geometry);
else
  write_ply(geometry.vertices, geometry.faces, outputFilename);
end

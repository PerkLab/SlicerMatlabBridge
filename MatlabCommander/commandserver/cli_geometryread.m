function geometry = cli_geometryread(filename)
%cli_geometryread  Read geometry for the command-line interface module from file (in PLY or STL format) into LPS coordinate system
%
%   geometry = cli_geometryread(filename) reads the mesh
%
%   geometry.vertices contains the vertices for all triangles in LPS coordinate system [3*n x 3].
%   geometry.faces contains the vertex lists defining each triangle face [n x 3].
% 
%   Current limitations/caveats:
%   * Only triangle mesh geometry is supported.
%
%   Example:
%     g=cli_geometryread('SimpleGeom.stl');
%     patch(g, 'FaceColor', 'red');
%

[pathstr,name,ext] = fileparts(filename);
if (strcmpi(ext,'.stl'))
  [v, f, n, c, stltitle] = stlread(filename);
else
  [v, f] = read_ply(filename);
end

geometry.vertices = v;
geometry.faces = f;

% Slicer writes mesh points in RAS coordinate system by default, convert to LPS now
geometry.vertices(:,1:2) = -1*geometry.vertices(:,1:2)

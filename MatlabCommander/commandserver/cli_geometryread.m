function geometry = cli_geometryread(filename)
%cli_geometryread  Read geometry for the command-line interface module from file (in STL format)
%
%   geometry = cli_geometryread(filename) reads the mesh
%
%   geometry.vertices contains the vertices for all triangles [3*n x 3].
%   geometry.faces contains the vertex lists defining each triangle face [n x 3].
% 
%   Current limitations/caveats:
%   * Only triangle mesh geometry is supported.
%
%   Example:
%     g=cli_geometryread('SimpleGeom.stl');
%     patch(g, 'FaceColor', 'red');
%

[v, f, n, c, stltitle] = stlread(filename);

geometry.vertices = v;
geometry.faces = f;

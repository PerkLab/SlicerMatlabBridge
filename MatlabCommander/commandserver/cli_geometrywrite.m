function cli_geometrywrite(outputFilename, geometry)
% Function for writing mesh data to file
%
%   geometry.vertices contains the vertices for all triangles [3*n x 3].
%   geometry.faces contains the vertex lists defining each triangle face [n x 3].
%
%   Current limitations/caveats:
%   * Only triangle mesh geometry is supported.

stlwrite(outputFilename, geometry);

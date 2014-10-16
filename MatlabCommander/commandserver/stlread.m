function [v, f, n, c, stltitle] = stlread(filename, verbose)
% This function reads an STL file in binary format into vertex and face
% matrices v and f.
%
% USAGE: [v, f, n, c, stltitle] = stlread(filename, verbose);
%
% verbose is an optional logical argument for displaying some loading
%   information (default is false).
%
% v contains the vertices for all triangles [3*n x 3].
% f contains the vertex lists defining each triangle face [n x 3].
% n contains the normals for each triangle face [n x 3].
% c is optional and contains color rgb data in 5 bits [n x 3].
%   if C is not specified, the item is determined to be of uniform temp
%   if color is specified, a max and min temp are specified and blue is
%   used to range the temperature from min to max in 32 steps
%   Valid - (15) - 1 color is used, 0 color is not used
%   Red  - (10:14) - 0:31 - Reserved
%   Green - (5:9) - 0:31 - Reserved
%   Blue - (0:4) - 0:31 - Used for temperature scaling

% stltitle contains the title of the specified stl file [1 x 80].
%
% To see plot the 3D surface use:
%   patch('Faces',f,'Vertices',v,'FaceVertexCData',c);
% or
%   plot3(v(:,1),v(:,2),v(:,3),'.');
%
% Duplicate vertices can be removed using:
%   [v, f]=patchslim(v, f);
%
% ===================================================
%
% For more information see:
%  http://www.esmonde-white.com/home/diversions/matlab-program-for-loading-stl-files
%
% Based on code originally written by:
%    Doron Harlev
% and combined with some code by:
%    Eric C. Johnson, 11-Dec-2008
%    Copyright 1999-2008 The MathWorks, Inc.
% 
% Re-written and optimized by Francis Esmonde-White, May 2010.
%
% ===================================================
% Minor Bugfix. 
% Bug in color algorithm fixed by William Grant Lohsen, May 2012.
%
% NOTE: Color implementation is compatible with VisCAM and SolidView
% From Wikipedia:
%     The VisCAM and SolidView software packages use the two 'attribute byte count' 
%     bytes at the end of every triangle to store a 15 bit RGB colour:
%         bit 0 to 4 are the intensity level for blue (0 to 31)
%         bits 5 to 9 are the intensity level for green (0 to 31)
%         bits 10 to 14 are the intensity level for red (0 to 31)
%         bit 15 is 1 if the colour is valid
%         bit 15 is 0 if the colour is not valid (as with normal STL files)
%

use_color=(nargout>=4);

fid=fopen(filename, 'r'); %Open the file, assumes STL Binary format.
if fid == -1 
    error('File could not be opened, check name or path.')
end

if ~exist('verbose','var')
    verbose = false;
end

ftitle=fread(fid,80,'uchar=>schar'); % Read file title
numFaces=fread(fid,1,'int32'); % Read number of Faces

T = fread(fid,inf,'uint8=>uint8'); % read the remaining values
fclose(fid);

stltitle = char(ftitle');

if verbose
    fprintf('\nTitle: %s\n', stltitle);
    fprintf('Number of Faces: %d\n', numFaces);
    disp('Please wait...');
end

% Each facet is 50 bytes
%  - Three single precision values specifying the face normal vector
%  - Three single precision values specifying the first vertex (XYZ)
%  - Three single precision values specifying the second vertex (XYZ)
%  - Three single precision values specifying the third vertex (XYZ)
%  - Two color bytes (possibly zeroed)

% 3 dimensions x 4 bytes x 4 vertices = 48 bytes for triangle vertices
% 2 bytes = color (if color is specified)

trilist = 1:48;

ind = reshape(repmat(50*(0:(numFaces-1)),[48,1]),[1,48*numFaces])+repmat(trilist,[1,numFaces]);
Tri = reshape(typecast(T(ind),'single'),[3,4,numFaces]);

n=squeeze(Tri(:,1,:))';
n=double(n);

v=Tri(:,2:4,:);
v = reshape(v,[3,3*numFaces]);
v = double(v)';

f = reshape(1:3*numFaces,[3,numFaces])';

if use_color
    c0 = typecast(T(49:50),'uint16');
    if (bitget(c0(1),16)==1)
        trilist = 49:50;
        ind = reshape(repmat(50*(0:(numFaces-1)),[2,1]),[1,2*numFaces])+repmat(trilist,[1,numFaces]);
        c0 = reshape(typecast(T(ind),'uint16'),[1,numFaces]);
        
        %modified by WGL
        r=bitshift(bitand(2^15-1, c0),-10);
        g=bitshift(bitand(2^10-1, c0),-5);
        b=bitand(2^5-1, c0);
        %end modification
        c=[r; g; b]';
        
    else
        c = zeros(numFaces,3);
    end
end

if verbose
    disp('Done!');
end
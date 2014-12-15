function img = cli_imageread(filename)
%cli_imageread  Read images for the command-line interface module from file (in NRRD format, see http://teem.sourceforge.net/nrrd/format.html)
%  img = cli_imageread(filename) reads the image volume and associated metadata
%
%  See detailed description of the img structure in nrrdread.m
% 

img = nrrdread(filename);

function cli_imagewrite(outputFilename, img)
% Function for writing pixel and meta data struct to a NRRD file
%
% See detailed description of the input data format description in nrrdwrite.m
%

nrrdwrite(outputFilename, img);

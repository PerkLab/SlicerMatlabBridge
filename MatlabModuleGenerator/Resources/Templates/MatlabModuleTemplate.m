function outputParams=MatlabModuleTemplate(inputParams)
% Example function that returns the minimum and maximum voxel value in a volume
% and performs thresholding operation on the volume.
%
% Parameters:
%  inputParams.threshold: threshold value
%  inputParams.unnamed{1}: input image filename
%  inputParams.unnamed{2}: output image filename, result of the processing
%  outputParams.min: image minimum value
%  outputParams.max: image maximum value
%

img=cli_imageread(inputParams.unnamed{1});

outputParams.min=min(min(min(img.pixelData)));
outputParams.max=max(max(max(img.pixelData)));

img.pixelData=(double(img.pixelData)>inputParams.threshold)*100;

cli_imagewrite(inputParams.unnamed{2}, img);

%% Help
%
% Reading input parameters
%
%  integer, integer-vector, float, float-vector, double, double-vector, string, string-enumeration, file, point:
%    value=inputParams.name;
%  string-vector:
%    value=cli_stringvectordecode(inputParams.name);
%  boolean:
%    value=isfield(inputParams,'name');
%  image:
%    value=cli_imageread(inputParams.name);
%  transform:
%    value=cli_lineartransformread(inputParams.name);
%   or (for generic transforms):
%    value=cli_transformread(inputParams.name);
%  measurement:
%    value=cli_measurementread(inputParams.name);
%
%  Note: for retrieving indexth-th unnamed parameter use inputParams.unnamed{index+1} instead of inputParams.name
%
%
% Writing output parameters
%
%  integer, integer-vector, float, float-vector, double, double-vector, string, string-enumeration, file:
%    outputParams.name=value;
%  image:
%    cli_imagewrite(inputParams.name, value);
%  transform:
%    cli_lineartransformwrite(inputParams.name, value);
%   or (for generic transforms):
%    cli_transformwrite(inputParams.name, value);
%  measurement:
%    cli_measurementwrite(inputParams.name, value);
%

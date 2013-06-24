function outputParams=MatlabModuleTemplate(inputParams)
% Example function that returns the minimum and maximum voxel value in a volume
% and performs thresholding operation on the volume.
%
% Parameters:
%  inputParams.threshold: threshold value
%  inputParams.unnamed{1}: input image filename
%  inputParams.unnamed{2}: output image filename (value is 0 if below threshold, 100 if above threshold)
%  outputParams.min: image minimum value
%  outputParams.max: image maximum value
%

img=cli_imageread(inputParams.unnamed{1});

outputParams.min=min(min(min(img.pixelData)));
outputParams.max=max(max(max(img.pixelData)));

img.pixelData=(double(img.pixelData)>inputParams.threshold)*100;

cli_imagewrite(inputParams.unnamed{2}, img);

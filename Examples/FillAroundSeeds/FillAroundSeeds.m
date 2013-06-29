function outputParams=FillAroundSeeds(inputParams)
% Example function for filling the image with a selected intensity around the chosen seed points
%
% Parameters:
%  inputParams.seed: seed locations
%  inputParams.radius: radius around the seed for filling (in mm)
%  inputParams.fillvalue: pixel value to fill
%  inputParams.unnamed{1}: input image filename
%  inputParams.unnamed{2}: output image filename (value is 0 if below threshold, 100 if above threshold)
%

img=cli_imageread(inputParams.inputvolume);

seeds_LPS=cli_pointvectordecode(inputParams.seed);
[seedDim seedCount]=size(seeds_LPS);
radius_LPS=[inputParams.radius inputParams.radius inputParams.radius 0]';
radius_IJK=abs(round(img.ijkToLpsTransform\radius_LPS));
for seedIndex=1:seedCount
  seed_IJK=round(img.ijkToLpsTransform\[seeds_LPS(:,seedIndex); 1]);
  img.pixelData(seed_IJK(1)-radius_IJK(1):seed_IJK(1)+radius_IJK(1),seed_IJK(2)-radius_IJK(2):seed_IJK(2)+radius_IJK(2),seed_IJK(3)-radius_IJK(3):seed_IJK(3)+radius_IJK(3))=ones(radius_IJK(1)*2+1,radius_IJK(2)*2+1,radius_IJK(3)*2+1)*inputParams.fillvalue;
end

cli_imagewrite(inputParams.outputvolume, img);

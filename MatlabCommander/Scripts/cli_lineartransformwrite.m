function cli_lineartransformwrite(outputfilename, transformMatrix, transformType)
%cli_transformwrite  Write a single 4x4 linear transformation matrix to an "Insight Transform File V1.0" text file
%
%   By default the output transform is in LPS anatomical coordinate sytem and
%   defined according to ITK ("image processing") convention.
%   If transformType is set to 'slicer' then the output transform is in RAS
%   coordinate system and defined according to Slicer ("computer graphics")
%   convention.
%   See more details at: http://www.slicer.org/slicerWiki/index.php/Documentation/4.2/FAQ#The_registration_transform_file_saved_by_Slicer_does_not_seem_to_match_what_is_shown
%

if nargin < 3
  transformType = 'itk';
end

assert(isequal(size(transformMatrix),[4 4]), 'transformMatrix size must be 4x4');

if (strcmpi(transformType,'itk'))
  transform_lps=transformMatrix;
elseif (strcmpi(transformType, 'slicer'))
  lps2ras=diag([-1 -1 1 1]);
  ras2lps=diag([-1 -1 1 1]);
  transform_lps=ras2lps*inv(transformMatrix)*lps2ras;  
else
  error('Unknown transformType: %s',transformType);
end

allTransforms{1}.Transform='AffineTransform_double_3_3';
allTransforms{1}.Parameters=[reshape(transform_lps(1:3,1:3)',1,9) transform_lps(1:3,4)'];
allTransforms{1}.FixedParameters=[0 0 0];

cli_transformwrite(outputfilename, allTransforms);

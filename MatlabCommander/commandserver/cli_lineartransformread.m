function transform = cli_lineartransformread(filename, transformType)
%cli_transformread  Read a single 4x4 linear transformation matrix from an "Insight Transform File V1.0" text file
%
%   By default the output transform is in LPS anatomical coordinate sytem and
%   defined according to ITK ("image processing") convention.
%   If transformType is set to 'slicer' then the output transform is in RAS
%   coordinate system and defined according to Slicer ("computer graphics")
%   convention.
%   See more details at: http://www.slicer.org/slicerWiki/index.php/Documentation/4.2/FAQ#The_registration_transform_file_saved_by_Slicer_does_not_seem_to_match_what_is_shown
%

if nargin < 2
  transformType = 'itk';
end

allTransforms=cli_transformread(filename);

assert(length(allTransforms)==1, 'The transform file should contain exactly one transform');
assert(strcmpi(allTransforms{1}.Transform,'AffineTransform_double_3_3'), 'The transform file should contain an AffineTransform_double_3_3 transform');

params=str2num(allTransforms{1}.Parameters);

transform_lps=[[reshape(params(1:9),3,3)' params(10:12)']; 0 0 0 1];

if (strcmpi(transformType,'itk'))
  transform=transform_lps;    
elseif (strcmpi(transformType, 'slicer'))
  lps2ras=diag([-1 -1 1 1]);
  ras2lps=diag([-1 -1 1 1]);
  transform=lps2ras*inv(transform_lps)*ras2lps;  
else
  error('Unknown transformType: %s',transformType);
end

function img = nrrdaddmetafield(img, fullFieldName, fieldValue)
% Add a metadata field to an image
%
% Example:
%
%  img = nrrdaddmetafield(img, 'MultiVolume.DICOM.EchoTime', 1.348);

  fieldName=regexprep(fullFieldName,'\W','_');
  if ~strcmp(fieldName,fullFieldName)
    img.metaDataFieldNames.(fieldName) = fullFieldName;
  end  
  img.metaData.(fieldName) = fieldValue;

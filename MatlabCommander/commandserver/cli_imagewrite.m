function cli_imagewrite(outputFilename, img)
% Function for writing pixel and meta data struct to a NRRD file
% See input data format description in cli_imageread.m

% Open file for writing
fid=fopen(outputFilename, 'w');
if(fid<=0) 
  fprintf('Could not open file: %s\n', outputFilename);
end

fprintf(fid,'NRRD0004\n');
fprintf(fid,'# Complete NRRD file format specification at:\n');
fprintf(fid,'# http://teem.sourceforge.net/nrrd/format.html\n');

img.metaData.sizes = num2str(size(img.pixelData));
img.metaData.type = getMetaType(class(img.pixelData));
metaDataCellArr = struct2cell(img.metaData);

% Get string names for each field in the meta data
fields = fieldnames(img.metaData);

% Print the header data to the output file
for i=1:numel(fields)
  % Cannot use spaces in field names, they were replace by underscore, restore them to spaces here
  field=strrep(fields{i},'_',' ');
  fprintf(fid,field);
  writeDataByType(fid,metaDataCellArr{i});
end

fprintf(fid,'\n');

% Write pixel data
fwrite(fid, img.pixelData,class(img.pixelData));

fclose('all');

function writeDataByType(fid, data)
% Function that writes the header data to file based on the type of data
% params: - fid of file to write to
%         - data from header to write
  if ischar(data)
    fprintf(fid,': %s\n',data);  
  else
    fprintf(fid,': ');
    fprintf(fid,'%d ',data);  
    fprintf(fid,'\n');
  end

function metaType = getMetaType(matlabType)
% Determine the metadata type from the Matlab type
  switch (matlabType)
   case {'int8'}
    metaType = 'int8';  
   case {'uint8'}
    metaType = 'uint8';
   case {'int16'}
    metaType = 'int16';
   case {'uint16'}
    metaType = 'uint16';
   case {'int32'}
    metaType = 'int32';
   case {'uint32'}
    metaType = 'uint32';
   case {'int64'}
    metaType = 'int64';
   case {'uint64'}
    metaType = 'uint64';
   case {'single'}
    metaType = 'float';
   case {'double'}
    metaType = 'double';
   otherwise
    assert(false, 'Unsupported Matlab data type')
  end 

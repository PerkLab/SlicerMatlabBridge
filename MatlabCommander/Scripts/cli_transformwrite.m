function cli_transformwrite(outputfilename, transforms)
%cli_transformwrite  Write transforms to a "Insight Transform File V1.0" text file
%

% Open file for writing
fid=fopen(outputfilename, 'w');

assert(fid > 0, 'Could not open file %s', outputfilename);
cleaner = onCleanup(@() fclose(fid));

fprintf(fid,'#Insight Transform File V1.0\n');

for transformIndex=1:length(transforms)
  % Get string names for each field in transform descriptor
  fields = fieldnames(transforms{transformIndex});
  metaDataCellArr = struct2cell(transforms{transformIndex});
  for i=1:numel(fields)
    fprintf(fid,fields{i});
    writeDataByType(fid,metaDataCellArr{i});
  end    
end

fprintf(fid,'\n');

%fclose(fid);

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

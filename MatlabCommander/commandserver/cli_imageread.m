function img = cli_imageread(filename)
%cli_imageread  Read images for the command-line interface module from file (in NRRD format, see http://teem.sourceforge.net/nrrd/format.html)
%   img = cli_imageread(filename) reads the image volume and associated
%
%   img.pixelData: pixel data array
%   img.ijkToLpsTransform: pixel (IJK) to physical (LPS) coordinate system transformation, the origin of the IJK coordinate system is (1,1,1) to match Matlab matrix indexing
%   img.metaData: contains all the metainformation in the image header
% 
%   Current limitations/caveats:
%   * "Block" datatype is not supported.
%   * Only tested with "gzip" and "raw" file encodings.
%   * Very limited testing on actual files.
%   * I only spent a couple minutes reading the NRRD spec.
%
% Partly based on the nrrdread.m function with copyright 2012 The MathWorks, Inc.

fid = fopen(filename, 'rb');
assert(fid > 0, 'Could not open file.');
cleaner = onCleanup(@() fclose(fid));

% NRRD files must start with the NRRD word and a version number
theLine = fgetl(fid);
assert(numel(theLine) >= 4, 'Bad signature in file.')
assert(isequal(theLine(1:4), 'NRRD'), 'Bad signature in file.')

% The general format of a NRRD file (with attached header) is:
% 
%     NRRD000X
%     <field>: <desc>
%     <field>: <desc>
%     # <comment>
%     ...
%     <field>: <desc>
%     <key>:=<value>
%     <key>:=<value>
%     <key>:=<value>
%     # <comment>
% 
%     <data><data><data><data><data><data>...

img.metaData = struct([]);

% Parse the file a line at a time.
while (true)

  theLine = fgetl(fid);
  
  if (isempty(theLine) || feof(fid))
    % End of the header.
    break;
  end
  
  if (isequal(theLine(1), '#'))
      % Comment line.
      continue;
  end
  
  % "fieldname:= value" or "fieldname: value" or "fieldname:value"
  parsedLine = regexp(theLine, ':=?\s*', 'split','once');
  
  assert(numel(parsedLine) == 2, 'Parsing error')
  
  field = lower(parsedLine{1});
  value = parsedLine{2};
      
  % Cannot use spaces in field names, so replace them by underscore
  field=strrep(field,' ','_');
  
  img.metaData(1).(field) = value;
  
end

datatype = getDatatype(img.metaData.type);

% Get the size of the data.
assert(isfield(img.metaData, 'sizes') && ...
       isfield(img.metaData, 'dimension') && ...
       isfield(img.metaData, 'encoding'), ...
       'Missing required metadata fields (sizes, dimension, or encoding).')

dims = sscanf(img.metaData.sizes, '%d');
ndims = sscanf(img.metaData.dimension, '%d');
assert(numel(dims) == ndims);

data = readData(fid, img.metaData, datatype);
if isfield(img.metaData, 'endian')
    data = adjustEndian(data, img.metaData);
end

img.pixelData = reshape(data, dims');

% For convenience, compute the transformation matrix between physical and pixel coordinates
axes_directions=reshape(sscanf(img.metaData.space_directions,'(%f,%f,%f) (%f,%f,%f) (%f,%f,%f)'),3,3);
axes_origin=sscanf(img.metaData.space_origin,'(%f,%f,%f)');
ijkZeroBasedToLpsTransform=[[axes_directions, axes_origin]; [0 0 0 1]];
ijkOneBasedToIjkZeroBasedTransform=[[eye(3), [-1;-1;-1] ]; [0 0 0 1]];
ijkOneBasedToLpsTransform=ijkZeroBasedToLpsTransform*ijkOneBasedToIjkZeroBasedTransform;
% Use the one-based IJK transform (origin is at [1,1,1])
img.ijkToLpsTransform=ijkOneBasedToLpsTransform;

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function datatype = getDatatype(metaType)
% Determine the datatype
switch (metaType)
 case {'signed char', 'int8', 'int8_t'}
  datatype = 'int8';  
 case {'uchar', 'unsigned char', 'uint8', 'uint8_t'}
  datatype = 'uint8';
 case {'short', 'short int', 'signed short', 'signed short int', ...
       'int16', 'int16_t'}
  datatype = 'int16';
 case {'ushort', 'unsigned short', 'unsigned short int', 'uint16', ...
       'uint16_t'}
  datatype = 'uint16';
 case {'int', 'signed int', 'int32', 'int32_t'}
  datatype = 'int32';
 case {'uint', 'unsigned int', 'uint32', 'uint32_t'}
  datatype = 'uint32';
 case {'longlong', 'long long', 'long long int', 'signed long long', ...
       'signed long long int', 'int64', 'int64_t'}
  datatype = 'int64';
 case {'ulonglong', 'unsigned long long', 'unsigned long long int', ...
       'uint64', 'uint64_t'}
  datatype = 'uint64';
 case {'float'}
  datatype = 'single';
 case {'double'}
  datatype = 'double';
 otherwise
  assert(false, 'Unknown datatype')
end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function data = readData(fidIn, meta, datatype)

switch (meta.encoding)
 case {'raw'}
  data = fread(fidIn, inf, [datatype '=>' datatype]);
 case {'gzip', 'gz'}
  % Create temporary file copy for GZIP uncompression
  tmpBase = tempname();
  tmpFile = [tmpBase '.gz'];
  fidTmp = fopen(tmpFile, 'wb');
  assert(fidTmp > 3, 'Could not open temporary file for GZIP decompression')  
  tmp = fread(fidIn, inf, 'uint8=>uint8');
  fwrite(fidTmp, tmp, 'uint8');
  fclose(fidTmp);
  % Uncompress
  gunzip(tmpFile)
  % Read uncompressed data
  fidTmp = fopen(tmpBase, 'rb');
  cleaner = onCleanup(@() fclose(fidTmp));  
  meta.encoding = 'raw';
  data = readData(fidTmp, meta, datatype);  
 case {'txt', 'text', 'ascii'}  
  data = fscanf(fidIn, '%f');
  data = cast(data, datatype);  
 otherwise
  assert(false, 'Unsupported encoding')
end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function data = adjustEndian(data, meta)
% For ignoring unused parameters dummy variables (dummy1 and dummy2) are
% used instead of ~ to maintain compatibility with Matlab R2009b version
[dummy1,dummy2,endian] = computer();
needToSwap = (isequal(endian, 'B') && isequal(lower(meta.endian), 'little')) || ...
         (isequal(endian, 'L') && isequal(lower(meta.endian), 'big'));
if (needToSwap)
data = swapbytes(data);
end

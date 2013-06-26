function transforms = cli_transformread(filename)
%cli_transformread  Read transforms from a "Insight Transform File V1.0" text file
%
%   The output is a cell that contains a structure for each transform in
%   the file.
%   The structure contains the properties of each transform (typically
%   Transform, Parameters, and FixedParameters field).
%

fid = fopen(filename, 'rt');
assert(fid > 0, 'Could not open file.');
cleaner = onCleanup(@() fclose(fid));

% Transform files are expected to start with a specific first line
expectedFirstLine='#Insight Transform File V1.0';
theLine = fgetl(fid);
assert(strcmpi(theLine,expectedFirstLine), 'The first line of the transform file expected to be #Insight Transform File V1.0')

% The contents of a linear transform file:
% 
% #Insight Transform File V1.0
% #Transform 0
% Transform: AffineTransform_double_3_3
% Parameters: 0.9961969233988567 0.05233595624294384 -0.0696608749212155 -0.041992154494529144 0.9889109407697044 0.14244932497869228 0.07634362299210169 -0.13898236906210146 0.9873477362704213 6.995316014085823 -9.993983402051885 0.021204305831741033
% FixedParameters: 0 0 0
%

transforms = {};
latestTransform={};

% Parse the file a line at a time.
while (~feof(fid))
  theLine = fgetl(fid);  
  if (isempty(theLine))
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
  
  field = parsedLine{1};
  value = parsedLine{2};
  
  if (strcmpi(field,'Transform'))
    % This is a new transform    
    if (isfield(latestTransform,'Transform'))
      % We already have some transform info, so save it
      transforms{length(transforms)+1}=latestTransform;
      latestTransform={};
    end
    latestTransform.Transform=value;
  else
    % This is a new field for the latest transform
    latestTransform.(field)=value;
  end
end

% Save the latestTransform
if (isfield(latestTransform,'Transform'))
  % We already have some transform info, so save it
  transforms{length(transforms)+1}=latestTransform;
end

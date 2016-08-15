function convertSignal(FLS_file,frameRate)
  addpath ML_Headers
  outputFilenames = FLS_file;
  
  % *** options ***
  if nargin < 2
    frameRate = 25;
  end
  
  % *** open FLS file ***
  fid = fopen(outputFilenames);
  settings = textscan(fid,'%s %s');
  fclose(fid);
  
  % *** get variable names and file paths ***
  N = size(settings{1},1);
  var = matlab.lang.makeValidName(settings{1});
  files = settings{2};
  
  corridx = 0;				% index of corrupted file
  
  % *** open files ***
  for i = 1:N    
    file = files{i};
    eval([var{i} '= loadSignal(file);']);    
    if strcmp(var{i}, 'corrupted')
      corridx = i;
    end
  end
  
  
  % *** get output directory ***
  outputDirTemp = strsplit(files{corridx},'/');
  outputDir = '';
  for k = 1:length(outputDirTemp) - 1
    outputDir = strcat(outputDir,outputDirTemp{k},'/');
  end
  
  % *** convert files ***
  for i = 1:N
    disp(['Converting   ' var{i} ' ...']);
    % name of output files is same as input files but with 
    % appropriate extensions (.png or .avi instead of .txt)            
    split = strsplit(files{i},'/');
    fname = split{end};
    split2 = strsplit(fname,'.');
    stem = split2{1};
    for k = 2:length(split2)-1
      stem = strcat(stem, '.', split2{k});
    end
    
    % get dimensions
    eval(['[height, width, frames] = size(' var{i} ');']);
    
    if frames == 1			% image
      name = strcat(outputDir, stem, '.png');
      if strcmp(var{i}, 'mask')                
	eval(['imwrite(' var{i} ', name);']);
      else
	eval(['imwrite(uint8(' var{i} '), name);']);                                
      end            
    else				% video
      name = strcat(outputDir, stem);
      vid = VideoWriter(name, 'Grayscale AVI');
      vid.FrameRate = frameRate;
      open(vid);
      for j = 1:frames
	if strcmp(var{i}, 'mask')
	  eval(['writeVideo(vid,' var{i} '(:,:,j));']);	  
	else
	  eval(['writeVideo(vid, uint8(' var{i} '(:,:,j)));']);	  
	end
      end
      close(vid);
    end
  end
  disp(['Media output saved in directory:        ' outputDir]);  
end

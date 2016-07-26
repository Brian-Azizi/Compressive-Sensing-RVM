addpath ML_Headers

% *** options ***
if ~exist('frameRate','var')    
    frameRate = 30;
end

outputFilenames = './rvmOutputFilenames.txt';
fid = fopen(outputFilenames);
settings = textscan(fid,'%s %s');
fclose(fid);
N = size(settings{1},1);
var = matlab.lang.makeValidName(settings{1});
files = settings{2};
corridx = 0;
for i = 1:N    
    file = files{i};
    eval([var{i} '= loadSignal(file);']);    
    if strcmp(var{i}, 'corrupted')
        corridx = i;
    end
end

eval(['[height, width, frames] = size(' var{1} ');']);

% get output directory
outputDirTemp = strsplit(files{corridx},'/');
%outputDir = outputDirTemp{1};
outputDir = '';
for k = 1:length(outputDirTemp) - 1
    outputDir = strcat(outputDir,outputDirTemp{k},'/');
end

% convert files
    for i = 1:N
        % trying out something: name of output files is same as input
        % files but with appropriate extensions (.png or .avi instead
        % of .txt)            
        split = strsplit(files{i},'/');
        fname = split{end};
        split2 = strsplit(fname,'.');
        stem = split2{1};
        for k = 2:length(split2)-1
            stem = strcat(stem, '.', split2{k});
        end
        
        %        
        if frames == 1            
            name = strcat(outputDir, stem, '.png');
            if strcmp(var{i}, 'mask')                
                eval(['imwrite(' var{i} ', name);']);
            else
                eval(['imwrite(uint8(' var{i} '), name);']);                                
            end            
        else
            name = strcat(outputDir, stem);
            %vid = VideoWriter(strcat(mediaOutputDirectory, var{i}), 'Grayscale AVI');
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


clear fid file h i outputFilenames settings ans ...
    displayOutput saveAsMedia var files N j vid ...
    mediaOutputDirectory name split fname split2 stem name
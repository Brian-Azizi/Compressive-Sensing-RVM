% *** options ***
displayOutput = true;
saveAsMedia = false;
frameRate = 30;
mediaOutputDirectory = '/local/data/public/ba308/Media/';
%mediaOutputDirectory = '/Users/brianazizi/Data/';



% check if matlab is being run in GUI mode or from command line
if usejava('jvm') && ~feature('ShowFigureWindows')
    %# use text-based alternative (input)
    if displayOutput
        disp('Matlab GUI mode required to display figure.');
    end
    displayOutput = false;    
%else
    %# use GUI dialogs (questdlg)
end

cd ..
addpath Headers
outputFilenames = 'rvmOutputFilenames.txt';
fid = fopen(outputFilenames);
settings = textscan(fid,'%s %s');
fclose(fid);
N = size(settings{1},1);

var = matlab.lang.makeValidName(settings{1});
files = settings{2};

for i = 1:N    
    file = files{i};
    eval([var{i} '= loadSignal(file);']);    
end

eval(['[height, width, frames] = size(' var{1} ');']);

if displayOutput
    for i = 1:N
        if frames == 1        
            figure('name',files{i});
            if strcmp(var{i}, 'mask')
                eval(['imshow(' var{i} ');']);
            else
                eval(['imshow(' var{i} ', [0 255]);']);
            end            
        else
            if strcmp(var{i},'mask')
                eval(['h = implay(' var{i} ', frameRate);']);
            else
                eval(['h = implay(uint8(' var{i} '), frameRate);']);
            end
            set(h.Parent,'name',files{i});
        end
    end
end

if saveAsMedia
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
        %name = strcat(mediaOutputDirectory, var{i}, '.png');        
        if frames == 1            
            name = strcat(mediaOutputDirectory, stem, '.png');
            if strcmp(var{i}, 'mask')                
                eval(['imwrite(' var{i} ', name);']);
            else
                eval(['imwrite(uint8(' var{i} '), name);']);                                
            end            
        else
            name = strcat(mediaOutputDirectory, stem);
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
    disp(['Output saved in directory' mediaOutputDirectory]);
end
cd Headers
clear fid file h i outputFilenames settings ans ...
     displayOutput saveAsMedia var files N j vid ...
     mediaOutputDirectory name split fname split2 stem name
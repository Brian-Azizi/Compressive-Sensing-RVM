% *** options ***
displayOutput = false;
saveAsMedia = true;
frameRate = 30;
%mediaOutputDirectory = '/local/data/public/ba308/Media/';
mediaOutputDirectory = '/Users/brianazizi/Data/';

outputFilenames = './rvmOutputFilenames.txt';
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
        if frames == 1            
            name = strcat(mediaOutputDirectory, var{i}, '.png');
            if strcmp(var{i}, 'mask')                
                eval(['imwrite(' var{i} ', name);']);
            else
                eval(['imwrite(uint8(' var{i} '), name);']);                
            end            
        else
            vid = VideoWriter(strcat(mediaOutputDirectory, var{i}), 'Grayscale AVI');
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
end

clear fid file h i outputFilenames settings ans ...
     displayOutput saveAsMedia var files N j vid ...
     mediaOutputDirectory name 
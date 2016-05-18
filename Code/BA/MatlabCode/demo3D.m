% settings
name = 'soccer';
height = 288;
width = 352;
frames = 300;

input = strcat(name,'_',num2str(height),'-',num2str(width),'-',num2str(frames));
blockDim = '4-4-4';
corrPerc = '60%';
corrMode = 'uniform'; 
scale = '1';

frameRate = 30;

origFile = strcat('/local/data/public/ba308/InputFiles/',input,'.txt');
corruptFile = strcat('/local/data/public/ba308/Simulations/',input,'/',blockDim,...
    '_',corrPerc,'_',corrMode,'_',scale,...
    '_corrupted_',input,'.txt');
recoverFile = strcat('/local/data/public/ba308/Simulations/',input,'/',blockDim,...
    '_',corrPerc,'_',corrMode,'_',scale,...
    '_recovered_',input,'.txt');


original = txt2rawBW(origFile, height, width, frames);
corrupt = txt2rawBW(corruptFile, height, width, frames);
recover = txt2rawBW(recoverFile, height, width, frames);

%implay(uint8(corrupt), frameRate);
%implay(uint8(recover), frameRate);
%implay(uint8(original), frameRate);



% mx = 500;
% mn = -200;
mx = max(max(max(recover)));
mn = min(min(min(recover)));
% recScale = 255*(recover - mn)/(mx-mn);
% implay(uint8(recScale))

original = uint8(original);
corrupt = uint8(corrupt);
recover = uint8(recover);

vidRaw = VideoWriter(strcat(name,'_raw'),'Grayscale AVI');
vidRaw.FrameRate = frameRate;
open(vidRaw);
for i = 1:frames
    writeVideo(vidRaw,original(:,:,i));
end
close(vidRaw);

vidCorr = VideoWriter(strcat(name,'_corr'),'Grayscale AVI');
vidCorr.FrameRate = frameRate;
open(vidCorr);
for i = 1:frames
    writeVideo(vidCorr,corrupt(:,:,i));
end
close(vidCorr);

vidRec = VideoWriter(strcat(name,'_rec'),'Grayscale AVI');
vidRec.FrameRate = frameRate;
open(vidRec);
for i = 1:frames
    writeVideo(vidRec,recover(:,:,i));
end
close(vidRec);
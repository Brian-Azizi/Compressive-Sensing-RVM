% settings
name = 'foreman';
height = 128;
width = 128;
frames = 64;

use_cascade = true;

input = strcat(name,'_',num2str(height),'-',num2str(width),'-',num2str(frames));
blockDim = '4-4-4';
corrPerc = '90%';
corrMode = 'uniform'; 
if use_cascade == true
    cscd = 'cascade_';
else
    cscd = '';
end
cascades = [1 2];

scale = '1';

frameRate = 20;

origFile = strcat('/local/data/public/ba308/InputFiles/',input,'.txt');
corruptFile = strcat('/local/data/public/ba308/Simulations/',input,'/',blockDim,...
    '_',corrPerc,'_',corrMode,'_',cscd, scale,...
    '_corrupted_',input,'.txt');
original = txt2rawBW(origFile, height, width, frames);
corrupt = txt2rawBW(corruptFile, height, width, frames);

%implay(uint8(original), frameRate);
implay(uint8(corrupt), frameRate);


recovers = zeros(height,width,frames,length(cascades));

for si = 1:length(cascades)
    s = cascades(si);
    if use_cascade == true
        label = strcat('_recovered_',num2str(s),'_');
    else
        label = 'recovered_';
    end
    
    recoverFile = strcat('/local/data/public/ba308/Simulations/',input,'/',blockDim,...
    '_',corrPerc,'_',corrMode,'_',cscd, scale,...
        label, input,'.txt');
    recovers(:,:,:,si) = txt2rawBW(recoverFile, height, width, frames);
    implay(uint8(recovers(:,:,:,si)), frameRate);
end




% mx = 500;
% mn = -200;
% mx = max(max(max(recover)));
% mn = min(min(min(recover)));
% recScale = 255*(recover - mn)/(mx-mn);
% implay(uint8(recScale))

% 
% original = uint8(original);
% corrupt = uint8(corrupt);
% recover = uint8(recover);
% 
% vidRaw = VideoWriter(strcat(name,'_raw'),'Grayscale AVI');
% vidRaw.FrameRate = frameRate;
% open(vidRaw);
% for i = 1:frames
%     writeVideo(vidRaw,original(:,:,i));
% end
% close(vidRaw);
% 
% vidCorr = VideoWriter(strcat(name,'_corr'),'Grayscale AVI');
% vidCorr.FrameRate = frameRate;
% open(vidCorr);
% for i = 1:frames
%     writeVideo(vidCorr,corrupt(:,:,i));
% end
% close(vidCorr);
% 
% vidRec = VideoWriter(strcat(name,'_rec'),'Grayscale AVI');
% vidRec.FrameRate = frameRate;
% open(vidRec);
% for i = 1:frames
%     writeVideo(vidRec,recover(:,:,i));
% end
% close(vidRec);
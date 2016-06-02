% settings
name = 'lenna';
height = 512;
width = 512;
frames = 1;

input = strcat(name,'_',num2str(height),'-',num2str(width),'-',num2str(frames));
blockDim = '32-32-1';
corrPerc = '99%';
corrMode = 'uniform'; 
basisMode = 'haar';
topScale = 5;

cascades = 1:topScale;

frameRate = 30;

origFile = strcat('/local/data/public/ba308/InputFiles/',input,'.txt');
corruptFile = strcat('/local/data/public/ba308/Simulations/',input,'/',blockDim,...
    '_',corrPerc,'_',corrMode,'_',basisMode, '_corrupted_',input,'.txt');
original = txt2rawBW(origFile, height, width, frames);
corrupt = txt2rawBW(corruptFile, height, width, frames);

surpressOutput = false;

if ~surpressOutput
if frames == 1
    figure('name',origFile);
    imshow(original, [0 255]);
%     title(origFile);
    
    figure('name',corruptFile);
    imshow(corrupt, [0 255]);
%     title(corruptFile);
else
    oh = implay(uint8(original), frameRate);
    set(oh.Parent,'name',origFile);
    
    ch = implay(uint8((corrupt)), frameRate);
    set(ch.Parent,'name',corruptFile);
end
end


recovers = zeros(height,width,frames,length(cascades));

for si = 1:length(cascades)
    s = cascades(si);
    label = strcat('_recovered_',num2str(s),'_of_',num2str(cascades(end)),'_'); 
    
    recoverFile = strcat('/local/data/public/ba308/Simulations/',input,'/',blockDim,...
    '_',corrPerc,'_',corrMode,'_', basisMode,...
        label, input,'.txt');
    recovers(:,:,:,si) = txt2rawBW(recoverFile, height, width, frames);
    
    if ~surpressOutput
    if frames == 1
        figure('name',recoverFile);
        imshow(recovers(:,:,:,si),[0 255]);
%         title(recoverFile);
    else
        rh = implay(uint8((recovers(:,:,:,si))), frameRate);
        set(rh.Parent,'name',recoverFile);
    end
    end
end

analyzePix


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

clear s si

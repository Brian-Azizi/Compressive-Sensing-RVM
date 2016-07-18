% settings
name = 'lenna';
height = 128;
width = 128;
frames = 1;

input = strcat(name,'_',num2str(height),'-',num2str(width),'-',num2str(frames));
blockDim = '8-8-1';
corrPerc = '20%';
corrMode = 'uniform'; 
basisMode = 'haar';
topScale = 2;

numFiles = (512/16)*(512/16)*(1/1);

cascades = 1:topScale;




frameRate = 30;

origFile = strcat('/local/data/public/ba308/InputFiles/',input,'.txt');
corruptFile = strcat('/local/data/public/ba308/Simulations/',input,'/',blockDim,...
    '_',corrPerc,'_',corrMode,'_',basisMode, '_corrupted_',input,'.txt');
maskFile = strcat('/local/data/public/ba308/Simulations/',input,'/',blockDim,...
    '_',corrPerc,'_',corrMode,'_',basisMode, '_mask_',input,'.txt');


original = txt2rawBW(origFile, height, width, frames);
corrupt = txt2rawBW(corruptFile, height, width, frames);
mask = txt2rawBW(maskFile, height, width, frames);

surpressOutput = false;

if ~surpressOutput
if frames == 1
    figure('name',origFile);
    imshow(original, [0 255]);
%     title(origFile);
    
    figure('name',corruptFile);
    imshow(corrupt, [0 255]);
%     title(corruptFile);

    figure('name',maskFile);
    imshow(mask);
else
    oh = implay(uint8(original), frameRate);
    set(oh.Parent,'name',origFile);
    
    ch = implay(uint8((corrupt)), frameRate);
    set(ch.Parent,'name',corruptFile);
    
    mh = implay(mask, frameRate);
    set(mh.Parent,'name',maskFile);
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

%testBlock2

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

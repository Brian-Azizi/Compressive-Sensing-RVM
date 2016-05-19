% settings
name = 'test';
height = 8;
width = 8;
frames = 16;

input = strcat(name,'_',num2str(height),'-',num2str(width),'-',num2str(frames));
blockDim = '8-8-8';
corrPerc = '70%';
corrMode = 'timeRays'; 
cascades = [1 2 3];

frameRate = 2;

origFile = strcat('/local/data/public/ba308/InputFiles/',input,'.txt');
corruptFile = strcat('/local/data/public/ba308/Simulations/',input,'/',blockDim,...
    '_',corrPerc,'_',corrMode, '_corrupted_',input,'.txt');
original = txt2rawBW(origFile, height, width, frames);
corrupt = txt2rawBW(corruptFile, height, width, frames);


if frames == 1
%     figure;
%     imshow(uint8(original));
%     title([name,': original']);
    
    figure;
    imshow(uint8(corrupt));
    title([name,': ', corrPerc, ' corrupted']);
else
%     oh = implay(uint8(original), frameRate);
%     set(oh.Parent,'name',[name,': original']);
    
    ch = implay(uint8(corrupt), frameRate);
    set(ch.Parent,'name',[name,': ',corrPerc,' corrupted']);
end

recovers = zeros(height,width,frames,length(cascades));

for si = 1:length(cascades)
    s = cascades(si);
    
    %label = strcat('_recovered_',num2str(s),'_');    
    label = strcat('_recovered_',num2str(s),'_of_',num2str(cascades(end)),'_'); 
    
    recoverFile = strcat('/local/data/public/ba308/Simulations/',input,'/',blockDim,...
    '_',corrPerc,'_',corrMode,...
        label, input,'.txt');
    recovers(:,:,:,si) = txt2rawBW(recoverFile, height, width, frames);
    if frames == 1
        figure;
        imshow(uint8(recovers(:,:,:,si)));
        title([name,': cascade stage ', num2str(s), ' \ ', num2str(cascades(end))]);
    else
        rh = implay(uint8(recovers(:,:,:,si)), frameRate);
        set(rh.Parent,'name',[name,': cascade stage ', num2str(s), ' \ ', num2str(cascades(end))]);
    end
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
% settings
height = 288;
width = 352;
frames = 80;

input = 'stefan-288-352-80';
blockDim = '4-4-2';
corrPerc = '50%';
corrMode = 'timeRays'; 
scale = '1';

frameRate = 30;

origFile = strcat('../',input,'.txt');
corruptFile = strcat('/local/data/public/ba308/Simulations/',input,'/',blockDim,...
    '_',corrPerc,'_',corrMode,'_',scale,...
    '_corrupted_',input,'.txt');
recoverFile = strcat('/local/data/public/ba308/Simulations/',input,'/',blockDim,...
    '_',corrPerc,'_',corrMode,'_',scale,...
    '_recovered_',input,'.txt');


original = txt2rawBW(origFile, height, width, frames);
corrupt = txt2rawBW(corruptFile, height, width, frames);
recover = txt2rawBW(recoverFile, height, width, frames);

implay(uint8(corrupt), frameRate);
implay(uint8(recover), frameRate);
implay(uint8(original), frameRate);

% mx = 500;
% mn = -200;
mx = max(max(max(recover)));
mn = min(min(min(recover)));
% recScale = 255*(recover - mn)/(mx-mn);
% implay(uint8(recScale))
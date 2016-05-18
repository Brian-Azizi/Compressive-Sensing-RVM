
inStem = 'soccer';
inExtension = '_cif.yuv';
input = strcat('/local/data/public/ba308/Sequences/',inStem,inExtension);

heightOut = 288; % power of 2
widthOut = 352;  % power of 2
framesOut = 300; %  power of 2

rateRaw = 30;  % fps
rateHaar = 20;

saver = true;
output = strcat('/local/data/public/ba308/InputFiles/',inStem,'_',num2str(heightOut),...
        '-',num2str(widthOut),'-',num2str(framesOut),'.txt');


raw = yuv2rawBW_save(input,288,352,'420',output,heightOut,widthOut,framesOut,saver);
haar = haar3D(raw,1);
% 
% implay(raw,rateRaw);
% implay(uint8(haar),rateHaar);
% 
% 
% % save to .avi
% vidRaw = VideoWriter('soccer_raw','Grayscale AVI');
% vidRaw.FrameRate = rateRaw;
% open(vidRaw);
% for i = 1:framesOut
%     writeVideo(vidRaw,raw(:,:,i));
% end
% close(vidRaw);
% 
% 
% hh = uint8(haar);
% vidHaar = VideoWriter('soccer_haar','Grayscale AVI');
% vidHaar.FrameRate = rateHaar;
% open(vidHaar);
% for i = 1:framesOut
%     writeVideo(vidHaar,hh(:,:,i));
% end
% close(vidHaar);

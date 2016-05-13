inStem = 'stefan';
inExtension = '_cif.yuv';
input = strcat('Sequences/',inStem,inExtension);

heightOut = 288; % power of 2
widthOut = 352;  % power of 2
framesOut = 80; %  power of 2

rateRaw = 20;  % fps
rateHaar = 20;

saver = true;
output = strcat('../',inStem,'-',num2str(heightOut),...
        '-',num2str(widthOut),'-',num2str(framesOut),'.txt');


raw = yuv2rawBW_save(input,288,352,'420',output,heightOut,widthOut,framesOut,saver);
haar = haar3D(raw,1);

implay(raw,rateRaw);
implay(uint8(haar),rateHaar);

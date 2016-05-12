inStem = 'foreman';
inExtension = '_cif.yuv';
input = strcat('Sequences/',inStem,inExtension);

heightOut = 128; % power of 2
widthOut = 128;  % power of 2
framesOut = 64; %  power of 2

rateRaw = 20;  % fps
rateHaar = 20;

saver = false;
output = strcat('../',inStem,'-',num2str(heightOut),...
        '-',num2str(widthOut),'-',num2str(framesOut),'.txt');


raw = yuv2rawBW_save(input,288,352,'420',output,heightOut,widthOut,framesOut,saver);
haar = haar3D(raw);

implay(raw,rateRaw);
implay(uint8(haar),rateHaar);
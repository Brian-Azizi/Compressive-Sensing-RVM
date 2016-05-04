input = 'Sequences/soccer_cif.yuv';

heightOut = 256; %     "
widthOut = 256;  % power of 2
framesOut = 256; %     "

rateRaw = 30;  % fps
rateHaar = 30;

saver = false;
output = 'akiyo-128-128-64.txt'; % dummy


raw = yuv2rawBW_save(input,288,352,'420',output,heightOut,widthOut,framesOut,saver);
haar = haar3D(raw);

clear output;

implay(raw,rateRaw);
implay(uint8(haar),rateHaar);
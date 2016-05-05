input = 'Sequences/foreman_cif.yuv';

heightOut = 128; %     "
widthOut = 128;  % power of 2
framesOut = 64; %     "

rateRaw = 20;  % fps
rateHaar = 20;

saver = true;
output = '../foreman-128-128-64.txt'; % dummy


raw = yuv2rawBW_save(input,288,352,'420',output,heightOut,widthOut,framesOut,saver);
haar = haar3D(raw);

clear output;

implay(raw,rateRaw);
implay(uint8(haar),rateHaar);
input = 'Sequences/akiyo_cif.yuv';
widthOut = 256;  % power of 2
heightOut = 256; %     "
framesOut = 256; %     "

rateRaw = 30;  % fps
rateHaar = 30;

output = 'foreman-64-128-256.txt'; % dummy

raw = yuv2rawBW_save(input,288,352,'420',output,heightOut,widthOut,framesOut,false);
haar = haar3D(raw);

mn = min(min(min(haar)));
mx = max(max(max(haar)));
haarScaled = (haar-mn)*255/(mx-mn);
clear output;

implay(raw,rateRaw);
implay(uint8(haarScaled),rateHaar);
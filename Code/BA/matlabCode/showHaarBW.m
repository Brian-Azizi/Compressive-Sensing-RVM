input = 'Sequences/foreman_cif.yuv';
widthOut = 64;  % power of 2
heightOut = 128; %     "
framesOut = 256; %     "

rateRaw = 30;  % fps
rateHaar = 30;

output = 'foreman-64-128-256.txt'; % dummy

raw = yuv2rawBW_save(input,352,288,'420',output,widthOut,heightOut,framesOut,true);
haar = haar3D(raw);

mn = min(min(min(haar)));
mx = max(max(max(haar)));
haarScaled = (haar-mn)*255/(mx-mn);
clear output;

implay(raw,rateRaw);
implay(uint8(haarScaled),rateHaar);
input = 'Sequences/foreman_cif.yuv';
widthOut = 256;  % power of 2
heightOut = 256; %     "
framesOut = 256; %     "

rateRaw = 30;  % fps
rateHaar = 30;

output = 'out.txt'; % dummy

raw = yuv2rawBW_save(input,352,288,'420',output,widthOut,heightOut,framesOut,false);
haar = haar3D(raw);

mn = min(min(min(haar)));
mx = max(max(max(haar)));
haarScaled = (haar-mn)*255/(mx-mn);
clear output;

implay(raw,rateRaw);
implay(uint8(haarScaled),rateHaar);
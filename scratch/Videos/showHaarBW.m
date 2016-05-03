input = 'Sequences/akiyo_cif.yuv';
rateRaw = 30;  % fps
rateHaar = 15; 

output = 'out.txt'; % dummy

raw = yuv2rawBW_save(input,352,288,'420',output,256,256,256,false);
haar = uint8(haar3D(raw));

clear output;

implay(raw,rateRaw);
implay(haar,rateHaar);
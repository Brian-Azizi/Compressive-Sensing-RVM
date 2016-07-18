input = 'Sequences/soccer_cif.yuv';
rateHaar = 15;
rateRaw = 30;

output = 'out.txt'; % dummy

raw = yuv2raw_save(input,288,352,'420',output,256,256,256,false);
haar = uint8(haar3D_RGB(raw));

clear output;

implay(raw,rateRaw);
implay(haar,rateHaar);

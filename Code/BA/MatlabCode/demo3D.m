origFile = '../foreman-128-128-64.txt';
corruptFile = '../Simulations/F30/30%_corrupted_foreman-128-128-64.txt';
recoverFile = '../Simulations/F30/30%_recovered_foreman-128-128-64.txt';

height = 128;
width = 128;
frames = 64;

frameRate = 20;

original = txt2rawBW(origFile, height, width, frames);
corrupt = txt2rawBW(corruptFile, height, width, frames);
recover = txt2rawBW(recoverFile, height, width, frames);

implay(uint8(corrupt), frameRate);
implay(uint8(recover), frameRate);
%implay(uint8(original), frameRate);
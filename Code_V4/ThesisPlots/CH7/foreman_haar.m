addpath('../../ML_Headers/');
A = loadSignal('/local/data/public/ba308/ResultsDump/foreman_haar_MASKED.txt');

% implay(uint8(A),20)

f1 = A(:,:,64);

imwrite(uint8(f1),'foreman_haar_masked_64.png')
% imwrite(uint8(f2),'soccer40_masked_11.png')

B = loadSignal('/local/data/public/ba308/ResultsDump/foreman_haar_RECOVERED_3_OF_3.txt');

% implay(uint8(B),20)

r1 = B(:,:,64);
% r2 = B(:,:,11);

imwrite(uint8(r1),'foreman_haar_rec_64.png')
% imwrite(uint8(r2),'soccer40_rec_11.png')


C = loadSignal('/local/data/public/ba308/InputFiles/foreman_128-128-64.txt');

% implay(uint8(C),20)
% 
o1 = C(:,:,64);
imwrite(uint8(o1),'foreman_haar_orig_64.png')


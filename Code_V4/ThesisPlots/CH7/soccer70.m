addpath('../ML_Headers/');
A = loadSignal('/local/data/public/ba308/ResultsDump/soccer70_MASKED.txt');

implay(uint8(A),20)

f1 = A(:,:,12);
f2 = A(:,:,11);

imwrite(uint8(f1),'soccer70_masked_12.png')
imwrite(uint8(f2),'soccer70_masked_11.png')

B = loadSignal('/local/data/public/ba308/ResultsDump/soccer70_RECOVERED_3_OF_3.txt');

implay(uint8(B),20)

r1 = B(:,:,12);
r2 = B(:,:,11);

imwrite(uint8(r1),'soccer70_rec_12.png')
imwrite(uint8(r2),'soccer70_rec_11.png')


C = loadSignal('/local/data/public/ba308/InputFiles/soccer_128-128-64.txt');

implay(uint8(C),20)

o1 = C(:,:,12);
o2 = C(:,:,11);

imwrite(uint8(o1),'soccer70_orig_12.png')
imwrite(uint8(o2),'soccer70_orig_11.png')

addpath('../ML_Headers/');
A = loadSignal('/local/data/public/ba308/ResultsDump/akiyo40_MASKED.txt');

implay(uint8(A),20)



f1 = A(:,:,2);
f2 = A(:,:,19);

imwrite(uint8(f1),'akiyo40_masked_2.png')
imwrite(uint8(f2),'akiyo40_masked_19.png')

B = loadSignal('/local/data/public/ba308/ResultsDump/akiyo40_RECOVERED_3_OF_3.txt');

implay(uint8(B),20)

r1 = B(:,:,2);
r2 = B(:,:,19);

imwrite(uint8(r1),'akiyo40_rec_2.png')
imwrite(uint8(r2),'akiyo40_rec_19.png')


C = loadSignal('/local/data/public/ba308/InputFiles/akiyo_128-128-64.txt');

implay(uint8(C),20)

o1 = C(:,:,2);
o2 = C(:,:,19);

imwrite(uint8(o1),'akiyo40_orig_2.png')
imwrite(uint8(o2),'akiyo40_orig_19.png')

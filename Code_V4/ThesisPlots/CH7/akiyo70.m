addpath('../ML_Headers/');
A = loadSignal('/local/data/public/ba308/ResultsDump/akiyo70_MASKED.txt');

implay(uint8(A),20)

f1 = A(:,:,32);
f2 = A(:,:,52);

imwrite(uint8(f1),'akiyo70_masked_32.png')
imwrite(uint8(f2),'akiyo70_masked_52.png')

B = loadSignal('/local/data/public/ba308/ResultsDump/akiyo70_RECOVERED_3_OF_3.txt');

implay(uint8(B),20)

r1 = B(:,:,32);
r2 = B(:,:,52);

imwrite(uint8(r1),'akiyo70_rec_32.png')
imwrite(uint8(r2),'akiyo70_rec_52.png')


C = loadSignal('/local/data/public/ba308/InputFiles/akiyo_128-128-64.txt');

implay(uint8(C),20)

o1 = C(:,:,32);
o2 = C(:,:,52);

imwrite(uint8(o1),'akiyo70_orig_32.png')
imwrite(uint8(o2),'akiyo70_orig_52.png')

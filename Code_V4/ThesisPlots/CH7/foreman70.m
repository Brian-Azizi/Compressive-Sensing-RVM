addpath('../ML_Headers/');
A = loadSignal('/local/data/public/ba308/ResultsDump/foreman70_MASKED.txt');

implay(uint8(A),20)

f1 = A(:,:,22);
f2 = A(:,:,18);

imwrite(uint8(f1),'foreman70_masked_22.png')
imwrite(uint8(f2),'foreman70_masked_18.png')

B = loadSignal('/local/data/public/ba308/ResultsDump/foreman70_RECOVERED_3_OF_3.txt');

implay(uint8(B),20)

r1 = B(:,:,22);
r2 = B(:,:,18);

imwrite(uint8(r1),'foreman70_rec_22.png')
imwrite(uint8(r2),'foreman70_rec_18.png')


C = loadSignal('/local/data/public/ba308/InputFiles/foreman_128-128-64.txt');

implay(uint8(C),20)

o1 = C(:,:,22);
o2 = C(:,:,18);

imwrite(uint8(o1),'foreman70_orig_22.png')
imwrite(uint8(o2),'foreman70_orig_18.png')

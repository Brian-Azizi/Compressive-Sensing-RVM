addpath('../ML_Headers/');
A = loadSignal('/local/data/public/ba308/ResultsDump/foreman_MASKED.txt');

implay(uint8(A),20)

f1 = A(:,:,21);
f2 = A(:,:,151);

imwrite(uint8(f1),'foreman_masked_21.png')
imwrite(uint8(f2),'foreman_masked_151.png')

B = loadSignal('/local/data/public/ba308/ResultsDump/foreman_RECOVERED_2_OF_2.txt');

implay(uint8(B),20)

r1 = B(:,:,21);
r2 = B(:,:,151);

imwrite(uint8(r1),'foreman_rec_21.png')
imwrite(uint8(r2),'foreman_rec_151.png')


C = loadSignal('/local/data/public/ba308/InputFiles/foreman_288-352-288.txt');

implay(uint8(C),20)

o1 = C(:,:,21);
o2 = C(:,:,151);

imwrite(uint8(o1),'foreman_orig_21.png')
imwrite(uint8(o2),'foreman_orig_151.png')

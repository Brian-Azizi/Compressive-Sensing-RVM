A = load('lenna_512-512-1.txt'); % load data


t = 15; %threshold

M = numel(A);
B0 = dct2(A);  % DCT
B = B0;
N = sum(abs(B(:))>=t);
B(abs(B(:))<t) = 0;

fprintf('original # pixels = %d\n', M);
fprintf('compressed # pixels = %d\n', N);
fprintf('compression ratio = %f\n', N/M);

C = idct2(B);
figure, imagesc(uint8(A)), colormap gray;
figure, imagesc(uint8(C)), colormap gray;


imwrite(uint8(A),'lenna512.png'); % save original
imwrite(uint8(C),'lenna512_dct.png'); % save original
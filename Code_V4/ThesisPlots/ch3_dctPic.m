A = double(imread('cameraman.tif'))/255;

B0 = dct2(A);

B = B0;
B(abs(B(:))>10) = 10; % highlight large coefficients
B = abs(B);
B = B/10; % get in [0,1] range
imwrite(B,'dctCoeff.png');

imwrite(A,'cameraman.png');

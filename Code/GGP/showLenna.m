L0 = load('output-lenna512.txt-0-.txt');
L1 = load('output-lenna512.txt-1-.txt');
L2 = load('output-lenna512.txt-2-.txt');
L3 = load('output-lenna512.txt-3-.txt');

figure, colormap gray;
subplot(2,2,1), imshow(uint8(L0))%imagesc(L0);
subplot(2,2,2), imshow(uint8(L1))%imagesc(L1);
subplot(2,2,3), imshow(uint8(L2))%imagesc(L2);
subplot(2,2,4), imshow(uint8(L3))%imagesc(L3);

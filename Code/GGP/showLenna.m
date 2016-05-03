L0 = load('output-lenna.txt-0-.txt');
L1 = load('output-lenna.txt-1-.txt');
L2 = load('output-lenna.txt-2-.txt');
L3 = load('output-lenna.txt-3-.txt');

figure, colormap gray;
subplot(2,2,1), imagesc(L0);
subplot(2,2,2), imagesc(L1);
subplot(2,2,3), imagesc(L2);
subplot(2,2,4), imagesc(L3);

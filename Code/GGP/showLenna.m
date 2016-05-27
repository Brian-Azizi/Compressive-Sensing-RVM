name = 'lenna';



L0 = load(strcat('output-',name,'.txt-0-.txt'));
L1 = load(strcat('output-',name,'.txt-1-.txt'));
% L2 = load(strcat('output-',name,'.txt-2-.txt'));
% L3 = load(strcat('output-',name,'.txt-3-.txt'));

figure, colormap gray;
subplot(1,2,1), imshow(L0,[0 255])%imagesc(L0);
subplot(1,2,2), imshow(L1,[0, 255])%imagesc(L1);
% subplot(2,2,3), imshow(uint8(L2))%imagesc(L2);
% subplot(2,2,4), imshow(uint8(L3))%imagesc(L3);

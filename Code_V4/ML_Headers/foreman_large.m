% A = loadSignal('/local/data/public/ba308/ResultsDump/foreman_large_MASKED.txt');
% B = loadSignal('/local/data/public/ba308/ResultsDump/foreman_large_RECOVERED_1_OF_1.txt');
% C = loadSignal('/local/data/public/ba308/InputFiles/foreman_256-256-256.txt');

outFile = 'foreman_large';
% 
% implay(uint8(A),25)
% implay(uint8(B),25)
% implay(uint8(C),25)

frame1 = 42;
frame2 = 43;


imwrite(uint8(A(:,:,frame1)),strcat(outFile,'_masked_',num2str(frame1),'.png'));
imwrite(uint8(A(:,:,frame2)),strcat(outFile,'_masked_',num2str(frame2),'.png'));

imwrite(uint8(B(:,:,frame1)),strcat(outFile,'_rec_',num2str(frame1),'.png'));
imwrite(uint8(B(:,:,frame2)),strcat(outFile,'_rec_',num2str(frame2),'.png'));

imwrite(uint8(C(:,:,frame1)),strcat(outFile,'_orig_',num2str(frame1),'.png'));
imwrite(uint8(C(:,:,frame2)),strcat(outFile,'_orig_',num2str(frame2),'.png'));

lenna = double(imread('128.png')');
S = lenna(:);

sz = 7;
II = eye(2^sz);
PHI1 = zeros(2^(sz-1),2^sz);
PSI1 = zeros(2^(sz-1),2^sz);
for j = 1:2^(sz-1)
    PHI1(j,:) = (II(2*j-1,:)+II(2*j,:))/sqrt(2);
    PSI1(j,:) = (II(2*j-1,:)-II(2*j,:))/sqrt(2);
end

W1 = [kron(PHI1,PHI1);kron(PHI1,PSI1);kron(PSI1,PHI1);kron(PSI1,PSI1)];
S1 = W1*S;

% On Individual filters (for testing)(works)
% WLL = kron(PHI1,PHI1);
% LLS = WLL*S;
% LLA = reshape(LLS, 64,64)';
% figure, colormap gray, subplot(2,2,1), imagesc(LLA);
% 
% WLH = kron(PHI1,PSI1);
% LHS = WLH*S;
% LHA = reshape(LHS, 64,64)';
% subplot(2,2,2), imagesc(LHA);
% 
% WHL = kron(PSI1,PHI1);
% HLS = WHL*S;
% HLA = reshape(HLS, 64,64)';
% subplot(2,2,3), imagesc(HLA);
% 
% WHH = kron(PSI1,PSI1);
% HHS = WHH*S;
% HHA = reshape(HHS, 64,64)';
% subplot(2,2,4), imagesc(HHA);
% 
% A = reshape(S1,128,128)';  %% doesn't work, your splitting the signal;
% %figure, colormap gray, imagesc(A);

split = 2^(2*sz-2);
LL = S1(1:split);
LH = S1(split+1:2*split);
HL = S1(2*split+1:3*split);
HH = S1(3*split+1:4*split);
BLL = reshape(LL,64,64)';
BLH = reshape(LH,64,64)';
BHL = reshape(HL,64,64)';
BHH = reshape(HH,64,64)';
figure, colormap gray;
subplot(2,2,1);
imagesc(BLL);
subplot(2,2,2);
imagesc(BLH);
subplot(2,2,3);
imagesc(BHL);
subplot(2,2,4);
imagesc(BHH);

% put them in a single matrix (imagesc does not work as well on this -
% ranges are messy
%B = [reshape(LL,64,64)',reshape(LH,64,64)';reshape(HL,64,64)',reshape(HH,64,64)'];
%figure, colormap gray;
%imagesc(B);

% Compress
S_com = S1;
S_com(abs(S_com)<=100) = 0;
% Lossy Reconstruction
S_rec = W1'*S_com;
A_rec = reshape(S_rec,128,128)';
figure, colormap gray;
subplot(1,2,1), imagesc(lenna');
subplot(1,2,2), imagesc(A_rec);

% Full reconstruction
S1_rec = W1'*S1;
A1_rec = reshape(S1_rec,128,128)';
figure, colormap gray;
subplot(1,2,1), imagesc(lenna');
subplot(1,2,2), imagesc(A1_rec);





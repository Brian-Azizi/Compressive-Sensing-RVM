orig = double(imread('512.bmp'));
sz = 9;
II = eye(2^sz);
PHI1 = zeros(2^(sz-1),2^sz);
PSI1 = zeros(2^(sz-1),2^sz);
for j = 1:2^(sz-1)
    PHI1(j,:) = (II(2*j-1,:)+II(2*j,:))/sqrt(2);
    PSI1(j,:) = (II(2*j-1,:)-II(2*j,:))/sqrt(2);
end

W1 = [PHI1; PSI1];
S1 = W1*orig*W1';

sz = sz - 1;
PHI2 = PHI1(1:2^(sz-1),1:2^sz);
PSI2 = PSI1(1:2^(sz-1),1:2^sz);
LL = S1(1:2^sz,1:2^sz);
W2 = [PHI2;PSI2];
S1(1:2^sz,1:2^sz) = W2*LL*W2'; 

ll = transform(1:2^(sz-1+1),1:2^(sz-1+1));
T2 = W2'*ll*W2;
transform(1:2^(sz-1+1),1:2^(sz-1+1)) = T2;

T1 = W1'*transform*W1;



%Rec = W1'*S2*W1;

figure, colormap gray;
subplot(1,2,1);
imagesc(orig);
subplot(1,2,2);
imagesc(T1);
%subplot(1,3,3);
%imagesc(Rec);


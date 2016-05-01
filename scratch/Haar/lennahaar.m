%B = imread('128.png');
B = imread('512.bmp');
init_scale = 9;
final_scale = 8;

A = B;
transform = zeros(size(A));
for scale = init_scale:-1:final_scale 
 %scale = 7;   
    sz = 2^scale;
    HrA = zeros(sz,sz/2);   % high pass
    LrA = zeros(sz,sz/2);   % low pass    
    for j=1:sz/2
        HrA(:,j) = (A(:,2*j-1) - A(:,2*j))/sqrt(2);
        LrA(:,j) = (A(:,2*j-1) + A(:,2*j))/sqrt(2);
    end

    HcHrA = zeros(sz/2);
    HcLrA = zeros(sz/2);
    LcHrA = zeros(sz/2);
    LcLrA = zeros(sz/2);
    for j = 1:sz/2
        HcHrA(j,:) = (HrA(2*j-1,:) - HrA(2*j,:))/sqrt(2);
        LcHrA(j,:) = (HrA(2*j-1,:) + HrA(2*j,:))/sqrt(2);
        HcLrA(j,:) = (LrA(2*j-1,:) - LrA(2*j,:))/sqrt(2);
        LcLrA(j,:) = (LrA(2*j-1,:) + LrA(2*j,:))/sqrt(2);
    end
    transform(1:sz/2,1:sz/2) = LcLrA;
    transform(sz/2+1:sz,1:sz/2) = HcLrA;
    transform(1:sz/2,sz/2+1:sz) = LcHrA;
    transform(sz/2+1:sz, sz/2+1:sz) = HcHrA;
    
    A = LcLrA;
end
%transform(abs(transform(:)) < 50) = 0;

figure; colormap gray;
subplot(1,2,1);
imagesc(B);
%imshow(B);
subplot(1,2,2);
imagesc(transform);
%imshow(transform);
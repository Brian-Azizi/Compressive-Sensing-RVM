close all

%A = load('../../Data/InputFiles/soccer_256_256_64.txt');

a = zeros(256,256,64);
for i = 1:64
    a(:,:,i) = A(256*(i-1)+1:256*i,:);
end

w = dwt3(a,'haar');

b = zeros(size(a));
for i=1:2
    for j=1:2
        for k=1:2
            if i~=1 || j~=1 || k~=1
                if i~=j
                    b(128*(i-1)+1:128*i,128*(j-1)+1:128*j,32*(k-1)+1:32*k) = 3*abs(w.dec{j,i,k});
                else
                    b(128*(i-1)+1:128*i,128*(j-1)+1:128*j,32*(k-1)+1:32*k) = 3*abs(w.dec{i,j,k});
                end
                disp([i, j, k]);
            else
                b(128*(i-1)+1:128*i,128*(j-1)+1:128*j,32*(k-1)+1:32*k) = abs(w.dec{i,j,k});
            end
        end
    end
end
b = unify(b);

implay(b)

% frame:
f = 6;

p = b(:,:,f);
q = b(:,:,32+f);

figure,imshow(p);
figure,imshow(q);

op = a(:,:,2*f-1);
oq = a(:,:,2*f);

figure,imshow(uint8(op));
figure,imshow(uint8(oq));

imwrite(uint8(op),'frame_11.png');
imwrite(uint8(oq),'frame_12.png');
imwrite(p,'dwt_6.png');
imwrite(q,'dwt_38.png');
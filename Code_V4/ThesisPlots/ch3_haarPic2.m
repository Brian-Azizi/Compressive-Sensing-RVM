close all;
A = imread('house.tif');
A = double(A(:,:,1))/255;
[N M] = size(A);

[ba,bh,bv,bd] = dwt2(A,'haar');
ba = abs(ba);
bh = 10*abs(bh);
bv = 10*abs(bv);
bd = 15*abs(bd);

B1 = [ba,bh;bv,bd];
figure, colormap(gray), imagesc(A);
axis square; axis off;
imwrite(A,'house.png');

figure, colormap(gray), imagesc(B1);
axis square; axis off;
imwrite(unify(B1),'dwt1.png');

[ba2, bh2,bv2,bd2] = dwt2(ba,'haar');
ba2 = abs(ba2)/2;
bh2 = abs(bh2);
bv2 = abs(bv2);
bd2 = abs(bd2);

B2 = [[ba2,bh2;bv2,bd2],bh;bv,bd];
figure, colormap(gray), imagesc(B2);
axis square; axis off;
imwrite(unify(B2),'dwt2.png');

[ba3, bh3,bv3,bd3] = dwt2(ba2,'haar');
ba3 = abs(ba3)/2;
bh3 = abs(bh3);
bv3 = abs(bv3);
bd3 = abs(bd3);

B3 = [[[ba3,bh3;bv3,bd3],bh2;bv2,bd2],bh;bv,bd];
figure, colormap(gray), imagesc(B3);
axis square; axis off;
imwrite(unify(B3),'dwt3.png');

[ba4, bh4,bv4,bd4] = dwt2(ba3,'haar');
ba4 = abs(ba4)/2;
bh4 = abs(bh4);
bv4 = abs(bv4);
bd4 = abs(bd4);

B4 = [[[[ba4,bh4;bv4,bd4],bh3;bv3,bd3],bh2;bv2,bd2],bh;bv,bd];
figure, colormap(gray), imagesc(B4);
axis square; axis off;
imwrite(unify(B4),'dwt4.png');

[ba5, bh5,bv5,bd5] = dwt2(ba4,'haar');
ba5 = abs(ba5)/2;
bh5 = abs(bh5);
bv5 = abs(bv5);
bd5 = abs(bd5);

B5 = [[[[[ba5,bh5;bv5,bd5],bh4;bv4,bd4],bh3;bv3,bd3],bh2;bv2,bd2],bh;bv,bd];
figure, colormap(gray), imagesc(B5);
axis square; axis off;
imwrite(unify(B5),'dwt5.png');

[ba6, bh6,bv6,bd6] = dwt2(ba5,'haar');
ba6 = abs(ba6)/2;
bh6 = abs(bh6);
bv6 = abs(bv6);
bd6 = abs(bd6);

B6 = [[[[[[ba6,bh6;bv6,bd6],bh5;bv5,bd5],bh4;bv4,bd4],bh3;bv3,bd3],bh2;bv2,bd2],bh;bv,bd];
figure, colormap(gray), imagesc(B6);
axis square; axis off;
imwrite(unify(B6),'dwt6.png');

[ba7, bh7,bv7,bd7] = dwt2(ba6,'haar');
ba7 = abs(ba7)/2;
bh7 = abs(bh7);
bv7 = abs(bv7);
bd7 = abs(bd7);

B7 = [[[[[[[ba7,bh7;bv7,bd7],bh6;bv6,bd6],bh5;bv5,bd5],bh4;bv4,bd4],bh3;bv3,bd3],bh2;bv2,bd2],bh;bv,bd];
figure, colormap(gray), imagesc(B7);
axis square; axis off;
imwrite(unify(B7),'dwt7.png');

[ba8, bh8,bv8,bd8] = dwt2(ba7,'haar');
ba8 = abs(ba8)/2;
bh8 = abs(bh8);
bv8 = abs(bv8);
bd8 = abs(bd8);

B8 = [[[[[[[[ba8,bh8;bv8,bd8],bh7;bv7,bd7],bh6;bv6,bd6],bh5;bv5,bd5],bh4;bv4,bd4],bh3;bv3,bd3],bh2;bv2,bd2],bh;bv,bd];
figure, colormap(gray), imagesc(B8);
axis square; axis off;
imwrite(unify(B8),'dwt8.png');

[ba9, bh9,bv9,bd9] = dwt2(ba8,'haar');
ba9 = abs(ba9)/2;
bh9 = abs(bh9);
bv9 = abs(bv9);
bd9 = abs(bd9);

B9 = [[[[[[[[[ba9,bh9;bv9,bd9],bh8;bv8,bd8],bh7;bv7,bd7],bh6;bv6,bd6],bh5;bv5,bd5],bh4;bv4,bd4],bh3;bv3,bd3],bh2;bv2,bd2],bh;bv,bd];
figure, colormap(gray), imagesc(B9);
axis square; axis off;
imwrite(unify(B9),'dwt9.png');



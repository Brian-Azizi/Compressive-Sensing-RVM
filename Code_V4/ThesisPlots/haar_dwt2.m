close all;
A = imread('house.tif');
A = double(A(:,:,1));
[N M] = size(A);

[ba,bh,bv,bd] = dwt2(A,'haar');
B1 = [ba,bh;bv,bd];

[ba2, bh2,bv2,bd2] = dwt2(ba,'haar');
B2 = [[ba2,bh2;bv2,bd2],bh;bv,bd];

[ba3, bh3,bv3,bd3] = dwt2(ba2,'haar');
B3 = [[[ba3,bh3;bv3,bd3],bh2;bv2,bd2],bh;bv,bd];

[ba4, bh4,bv4,bd4] = dwt2(ba3,'haar');
B4 = [[[[ba4,bh4;bv4,bd4],bh3;bv3,bd3],bh2;bv2,bd2],bh;bv,bd];

[ba5, bh5,bv5,bd5] = dwt2(ba4,'haar');
B5 = [[[[[ba5,bh5;bv5,bd5],bh4;bv4,bd4],bh3;bv3,bd3],bh2;bv2,bd2],bh;bv,bd];

[ba6, bh6,bv6,bd6] = dwt2(ba5,'haar');
B6 = [[[[[[ba6,bh6;bv6,bd6],bh5;bv5,bd5],bh4;bv4,bd4],bh3;bv3,bd3],bh2;bv2,bd2],bh;bv,bd];

[ba7, bh7,bv7,bd7] = dwt2(ba6,'haar');
B7 = [[[[[[[ba7,bh7;bv7,bd7],bh6;bv6,bd6],bh5;bv5,bd5],bh4;bv4,bd4],bh3;bv3,bd3],bh2;bv2,bd2],bh;bv,bd];

[ba8, bh8,bv8,bd8] = dwt2(ba7,'haar');
B8 = [[[[[[[[ba8,bh8;bv8,bd8],bh7;bv7,bd7],bh6;bv6,bd6],bh5;bv5,bd5],bh4;bv4,bd4],bh3;bv3,bd3],bh2;bv2,bd2],bh;bv,bd];

[ba9, bh9,bv9,bd9] = dwt2(ba8,'haar');
B9 = [[[[[[[[[ba9,bh9;bv9,bd9],bh8;bv8,bd8],bh7;bv7,bd7],bh6;bv6,bd6],bh5;bv5,bd5],bh4;bv4,bd4],bh3;bv3,bd3],bh2;bv2,bd2],bh;bv,bd];

th = 10;

perc = 100*(1-sum(abs(B9(:))<th) / numel(B9))

energy_perc = 100*(1-sum(B9(abs(B9(:))<th).^2) / sum(B9(:).^2))

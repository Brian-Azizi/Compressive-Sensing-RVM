margin = 4;
IM = ones(8*8+7*margin,'uint8')*200;
for i = 1:4
    for j = 1:4
    A = ones(8,'uint8')*100;
    A(2*i-1:2*i,2*j-1:2*j) = 255;
    IM(((i-1)*margin+(i-1)*8+1):((i-1)*margin+i*8),((j-1)*margin+(j-1)*8+1):((j-1)*margin+j*8)) = A; 
    end
end

for i = 1:4
    for j = 5:8
    A = ones(8,'uint8')*100;
    A(2*i-1:2*i,2*(j-4)-1) = 255;
    A(2*i-1:2*i,2*(j-4)) = 0;
    IM(((i-1)*margin+(i-1)*8+1):((i-1)*margin+i*8),((j-1)*margin+(j-1)*8+1):((j-1)*margin+j*8)) = A; 
    end
end

for i = 5:8
    for j = 1:4
    A = ones(8,'uint8')*100;
    A(2*(i-4)-1,2*j-1) = 255;
    A(2*(i-4)-1,2*j) = 255;
    A(2*(i-4),2*j-1) = 0;
    A(2*(i-4),2*j) = 0;
    IM(((i-1)*margin+(i-1)*8+1):((i-1)*margin+i*8),((j-1)*margin+(j-1)*8+1):((j-1)*margin+j*8)) = A; 
    end
end

for i = 5:8
    for j = 5:8
    A = ones(8,'uint8')*100;
    A(2*(i-4)-1,2*(j-4)-1) = 255;
    A(2*(i-4)-1,2*(j-4)) = 0;
    A(2*(i-4),2*(j-4)-1) = 0;
    A(2*(i-4),2*(j-4)) = 255;
    IM(((i-1)*margin+(i-1)*8+1):((i-1)*margin+i*8),((j-1)*margin+(j-1)*8+1):((j-1)*margin+j*8)) = A; 
    end
end

smIM = ones(80,'uint8')*100;
smIM(24:40,41:48) = 255;
smIM(24:40,49:56) = 0;
imshow(IM)
figure,
imshow(smIM)

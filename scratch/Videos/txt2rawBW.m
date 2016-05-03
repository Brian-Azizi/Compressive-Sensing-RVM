function raw = txt2rawBW(txt,width,height,frames)
A = load(txt);
raw = zeros(height,width,frames);
tmp = reshape(A',[width,height,frames]);
for k = 1:frames
    raw(:,:,k) = tmp(:,:,k)';
end
clear A tmp;
end
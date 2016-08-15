function S = loadSignal(fileName)

[h, w, f] = getDimFromSignalFile(fileName);

C = load(fileName);
S = zeros(h,w,f);
T = reshape(C',w, h, f);
for i = 1:f
    S(:,:,i) = T(:,:,i)';
end

end
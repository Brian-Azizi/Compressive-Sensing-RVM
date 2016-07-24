function S = loadSignal(fileName)

[h, w, f] = getDimFromSignalFile(fileName);

C = load(fileName);
S = reshape(C',h, w, f);
for i = 1:f
    S(:,:,i) = S(:,:,i)';
end

end
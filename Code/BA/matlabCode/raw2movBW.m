function mov = raw2movBW(raw)
    sz = size(raw);
    width = sz(2);
    height = sz(1);
    frames = sz(3);
    mov = [];
    for k = 1:frames
        mov(k).cdata = uint8(raw(:,:,k));
        mov(k).colormap = gray(256);
    end
    
end
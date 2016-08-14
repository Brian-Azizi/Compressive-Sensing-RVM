function mov = raw2movBW(raw)
    sz = size(raw);
    height = sz(1);
    width = sz(2);
    frames = sz(3);
    mov = [];
    for k = 1:frames
        mov(k).cdata = uint8(raw(:,:,k));
        mov(k).colormap = gray(256);
    end
    
end
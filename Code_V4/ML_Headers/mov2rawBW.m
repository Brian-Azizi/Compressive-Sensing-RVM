function raw = mov2rawBW(mov)
    numFrames = size(mov,2);
    sz = size(mov(1).cdata);
    raw = uint8(zeros(sz(1),sz(2),numFrames));

    for i = 1:numFrames        
        raw(:,:,i) = mov(i).cdata(:,:,1);
    end    
end

function raw = mov2raw(mov)
    numFrames = size(mov,2);
    sz = size(mov(1).cdata);
    raw = uint8(zeros(sz(1),sz(2),3,numFrames));

    for i = 1:numFrames        
        raw(:,:,:,i) = mov(i).cdata;
    end    
end

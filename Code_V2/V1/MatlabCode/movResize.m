function out = movResize(mov,newHeight,newWidth)
    out = mov;
    numFrames = size(mov,2);
    for i = 1:numFrames
        frame = mov(i).cdata;
        out(i).cdata = imresize(frame,[newHeight,newWidth]);
    end
end

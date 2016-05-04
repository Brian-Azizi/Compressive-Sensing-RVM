function out = rawCorrupter(raw, percentage, height, width, numFrames, channels)
out = raw;
idx = rand(height,width);
frame = uint8(zeros(height,width));
for n = 1:numFrames
    if channels > 1
        for c = 1:channels
            frame(:,:) = raw(:,:,c,n);
            frame(idx < percentage/100) = 0;
            out(:,:,c,n) = frame;
        end
    else
        frame(:,:) = raw(:,:,n);
        frame(idx < percentage/100) = 0;
        out(:,:,n) = frame;
    end
end
end
function out = yuv2rawBW(File,width,height,format)
    mov = yuv2mov(File,width,height,format);
    numFrames = size(mov,2);
    out = uint8(zeros(height,width,numFrames));
    for i = 1:numFrames
        out(:,:,i) = rgb2gray(mov(i).cdata);
    end
end
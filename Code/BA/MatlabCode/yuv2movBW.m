function out = yuv2movBW(File,height,width,format)
    mov = yuv2mov(File,height,width,format);
    numFrames = size(mov,2);
    out = mov;
    for i = 1:numFrames
        out(i).cdata = rgb2gray(mov(i).cdata);
        out(i).colormap = gray(256);
    end
end
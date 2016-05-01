function out = yuv2movBW(File,width,height,format)
    mov = yuv2mov(File,width,height,format);
    numFrames = size(mov,2);
    out = mov;
    for i = 1:numFrames
        out(i).cdata = rgb2gray(mov(i).cdata);
        out(i).colormap = gray(256);
    end
end
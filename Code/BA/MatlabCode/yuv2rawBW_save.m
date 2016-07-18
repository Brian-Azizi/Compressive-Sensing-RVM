function raw = yuv2rawBW_save(fileIn, heightIn, widthIn, format, ...
    fileOut, heightOut, widthOut, framesOut, saving)
% yuv2rawBW_save(fileIn,heightIn,widthIn,format,fileOut,heightOut,widthOut,framesOut)
% Converts a .yuv video sequence to a raw integer array (in gray-scale) of
% dimensions [heightOut,widthOut,framesOut] and saves the output as 
% a white-space delimited .txt file of dimensions
% [heightOut*framesOut,widthOut]
%
% fileIn - input .yuv file string
% heightIn - height of input video
% widthIn - width of input video
% format - format of input file as string; eg '420' for 4:2:0 format
% fileOut - output .txt file string
% heightOut - height of output video
% widthOut - width of output video
% framesOut - frames of input video to be saved. Cannot exceed number of frames of input video
% 
% Example: raw = yuv2rawBW_save('foreman_cif.yuv',288,352,'420',...
% 'foreman.txt',256,256,256);
    mov = yuv2movBW(fileIn, heightIn, widthIn, format);
    numFrames = size(mov,2);
    if framesOut > numFrames
        error('Number of frames of output file cannot exceed number of frames of input file');
    end
    new_mov = movResize(mov,heightOut,widthOut);
    cropped_mov = new_mov(1:framesOut);
    raw = mov2rawBW(cropped_mov);
    
    if saving
        h = waitbar(0,'Saving to file');
        fid = fopen(fileOut,'wt');
        for k = 1:framesOut
            waitbar(k/framesOut,h);
            for i = 1:heightOut
                for j = 1:widthOut
                    fprintf(fid,'%u ',raw(i,j,k));
                end
                fprintf(fid,'\n');
            end
            fprintf(fid,'\n');
        end
        fclose(fid);
        close(h);
    end
end
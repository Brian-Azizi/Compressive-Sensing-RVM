function [H W F] = getDimFromSignalFile(file)
fid = fopen(file);
if fid == -1
    error(['Could not open file ' file]);
end

tline = '';
entry = 0;
rows = 0;
cols = 0;
frames = 0;
prevCols = 0;
prevFramesCols = 0;
prevRows = 0;
firstRow = true;
firstFrame = true;
lastLineEmpty = true;

while ischar(tline)
    tline = fgetl(fid);
    if tline == -1
        break;
    end
    if isempty(tline)
        if ~lastLineEmpty
            if ~firstFrame
                if prevRows ~= rows
                    error(['frames do not have consistent height in file ' file]);
                end
                if prevFramesCols ~= cols
                    error(['frames do not have consistent width in file ' file]);
                end
            else
                firstFrame = true;
            end
            prevRows = rows;
            prevFramesCols = cols;  
        end
        lastLineEmpty = true;
        continue;
    elseif lastLineEmpty
        frames = frames + 1;
        rows = 0;
        firstRow = true;
        lastLineEmpty = false;
    end
    rows = rows+1;   
    entry = textscan(tline,'%d');
    cols = size(entry{1},1);
    
    if firstRow
        firstRow = false;
    else
        if cols ~= prevCols
            error(['rows must have same number of entries in file ' file]);
        end
    end
    prevCols = cols;
end

H = rows;
W = cols;
F = frames;
end
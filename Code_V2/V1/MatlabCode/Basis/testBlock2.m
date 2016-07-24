%numFiles = 256;
cols = zeros(numFiles,1);

dupes = zeros(numFiles,1);
numFaults = 0;

for i=1:numFiles
    filename = strcat('../scratch/scratch',num2str(i),'.txt');
    A = load(filename);
    A = round(100000000*A);
    [B, id] = unique(A','rows');
    id = id';
    B = B';
    cols(i) = size(B,2);
    Acols=size(A,2);
    Bcols=size(B,2);
    dupes(i)=Acols-Bcols;
    if Bcols ~= Acols
        numFaults = numFaults + 1;
    end
    fprintf('%d\n',i);
end

fprintf('Number of blocks with duplicate cols = %d / %d\n',numFaults,numFiles);
fprintf('Total num of duplicate column = %d / %d\n',sum(dupes),numFiles*size(A,2));

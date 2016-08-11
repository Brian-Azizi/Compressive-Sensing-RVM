pic_dir = '../Data/Sequences/';
out_dir = '../Data/InputFiles/';
pic_name = 'peppers';
pic_ext = 'tiff';

pic_file = strcat(pic_dir,pic_name,'.',pic_ext);

A = imread(pic_file);
[M N D] = size(A);
if D ~= 1
    A = rgb2gray(A);
end

out_name = strcat(pic_name,'_',num2str(M),'-',num2str(N),'-1.txt');
out_file = strcat(out_dir,out_name);

% fid = fopen(out_file,'w');
% 
% for i = 1:M
%     for j = 1:M
%         fprintf(fid,'%d ',A(i,j));
%     end
%     fprintf(fid,'\n');
% end

A = ones(M);
for i = 1:M
    for j = 1:M
        if mod(i+j,2) == 0 || mod(i+j,3) == 0
            A(i,j) = 0;
        end
    end
end


fid = fopen(',,/Data/Masks/mask_pattern_1024-1024-1.txt','w');


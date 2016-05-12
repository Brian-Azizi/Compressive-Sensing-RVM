clear,clc;

height = 16;
width = 8;
frames = 8;

normFactor = sqrt(2);

rPhi = zeros(height/2,height);
rPsi = zeros(height/2,height);
cPhi = zeros(width/2,width);
cPsi = zeros(width/2,width);
sPhi = zeros(frames/2,frames);
sPsi = zeros(frames/2,frames);


I = eye(height/2);
for j = 1:height/2
    rPhi(:,2*j-1) = I(:,j)/normFactor;
    rPhi(:,2*j)   = I(:,j)/normFactor;
    rPsi(:,2*j-1) = I(:,j)/normFactor;
    rPsi(:,2*j)   = -I(:,j)/normFactor;
end

I = eye(width/2);
for j = 1:width/2
    cPhi(:,2*j-1) = I(:,j)/normFactor;
    cPhi(:,2*j)   = I(:,j)/normFactor;
    cPsi(:,2*j-1) = I(:,j)/normFactor;
    cPsi(:,2*j)   = -I(:,j)/normFactor;
end

I = eye(frames/2);
for j = 1:frames/2
    sPhi(:,2*j-1) = I(:,j)/normFactor;
    sPhi(:,2*j)   = I(:,j)/normFactor;
    sPsi(:,2*j-1) = I(:,j)/normFactor;
    sPsi(:,2*j)   = -I(:,j)/normFactor;
end

BASIS = [ kron(sPhi,kron(cPhi,rPhi)); ...
          kron(sPhi,kron(cPhi,rPsi)); ...
          kron(sPhi,kron(cPsi,rPhi)); ...
          kron(sPhi,kron(cPsi,rPsi)); ...
          kron(sPsi,kron(cPhi,rPhi)); ...
          kron(sPsi,kron(cPhi,rPsi)); ...
          kron(sPsi,kron(cPsi,rPhi)); ...
          kron(sPsi,kron(cPsi,rPsi))];

BASIS = BASIS';      
% cpp_basis = load('../test_basis_2.txt');

% A = int32(normFactor^3*BASIS);
% B = int32(normFactor^3*cpp_basis);
% isequal(A,B)
% isequal(BASIS,cpp_basis)
% sum(sum(abs(BASIS-cpp_basis)))/height/frames/width
% %examineHaar2

outFile = strcat('../testFiles/basis_1_',num2str(height),'-',num2str(width),'-',num2str(frames),'.txt');
save(outFile,'BASIS','-ascii');

clear cPhi cPsi rPhi rPsi sPhi sPsi I j

clear,clc;

height = 8;
width = 8;
frames = 8;
dSize = height*width*frames;

normFactor = sqrt(2);

% scale 1 filters;
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

% scale 2 filters
rPhi2 = zeros(height/4,height/2);
rPsi2 = zeros(height/4,height/2);
cPhi2 = zeros(width/4,width/2);
cPsi2 = zeros(width/4,width/2);
sPhi2 = zeros(frames/4,frames/2);
sPsi2 = zeros(frames/4,frames/2);

I = eye(height/4);
for j = 1:height/4
    rPhi2(:,2*j-1) = I(:,j)/normFactor;
    rPhi2(:,2*j)   = I(:,j)/normFactor;
    rPsi2(:,2*j-1) = I(:,j)/normFactor;
    rPsi2(:,2*j)   = -I(:,j)/normFactor;
end

I = eye(width/4);
for j = 1:width/4
    cPhi2(:,2*j-1) = I(:,j)/normFactor;
    cPhi2(:,2*j)   = I(:,j)/normFactor;
    cPsi2(:,2*j-1) = I(:,j)/normFactor;
    cPsi2(:,2*j)   = -I(:,j)/normFactor;
end

I = eye(frames/4);
for j = 1:frames/4
    sPhi2(:,2*j-1) = I(:,j)/normFactor;
    sPhi2(:,2*j)   = I(:,j)/normFactor;
    sPsi2(:,2*j-1) = I(:,j)/normFactor;
    sPsi2(:,2*j)   = -I(:,j)/normFactor;
end



BASIS2 = [ kron(sPhi2*sPhi,kron(cPhi2*cPhi,rPhi2*rPhi)); ...
          kron(sPhi2*sPhi,kron(cPhi2*cPhi,rPsi2*rPhi)); ...
          kron(sPhi2*sPhi,kron(cPsi2*cPhi,rPhi2*rPhi)); ...
          kron(sPhi2*sPhi,kron(cPsi2*cPhi,rPsi2*rPhi)); ...
          kron(sPsi2*sPhi,kron(cPhi2*cPhi,rPhi2*rPhi)); ...
          kron(sPsi2*sPhi,kron(cPhi2*cPhi,rPsi2*rPhi)); ...
          kron(sPsi2*sPhi,kron(cPsi2*cPhi,rPhi2*rPhi)); ...
          kron(sPsi2*sPhi,kron(cPsi2*cPhi,rPsi2*rPhi)); ...
          ...
          kron(sPhi,kron(cPhi,rPsi)); ...
          kron(sPhi,kron(cPsi,rPhi)); ...
          kron(sPhi,kron(cPsi,rPsi)); ...
          kron(sPsi,kron(cPhi,rPhi)); ...
          kron(sPsi,kron(cPhi,rPsi)); ...
          kron(sPsi,kron(cPsi,rPhi)); ...
          kron(sPsi,kron(cPsi,rPsi))];
      
% cpp_basis = load('../test_basis_3.txt');
BASIS2 = BASIS2';
outFile = strcat('../testFiles/basis_2_',num2str(height),'-',num2str(width),'-',num2str(frames),'.txt');
save(outFile,'BASIS2','-ascii');
% % 
% A = int32(normFactor^3*BASIS2);
% B = int32(normFactor^3*cpp_basis);
% isequal(A,B)
% isequal(BASIS2,cpp_basis)
% sum(sum(abs(BASIS2-cpp_basis)))/height/frames/width
% 
% CC = abs(BASIS2-cpp_basis)>0.0001;
% fprintf('Error is %d out of %d\n',sum(sum(CC)),(height*width*frames)^2);
% 
% LLL_true = BASIS2(:,1:dSize/8);
% LLL_false = cpp_basis(:,1:dSize/8);
% 
% DD = abs(LLL_true-LLL_false)>0.0001;
% fprintf('Error in LLL is %d\n',sum(sum(DD)));




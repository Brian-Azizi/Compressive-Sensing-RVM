clear,clc;

height = 16;
width = 16;
frames = 16;
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

% scale 3 filters
rPhi3 = zeros(height/8,height/4);
rPsi3 = zeros(height/8,height/4);
cPhi3 = zeros(width/8,width/4);
cPsi3 = zeros(width/8,width/4);
sPhi3 = zeros(frames/8,frames/4);
sPsi3 = zeros(frames/8,frames/4);

I = eye(height/8);
for j = 1:height/8
    rPhi3(:,2*j-1) = I(:,j)/normFactor;
    rPhi3(:,2*j)   = I(:,j)/normFactor;
    rPsi3(:,2*j-1) = I(:,j)/normFactor;
    rPsi3(:,2*j)   = -I(:,j)/normFactor;
end

I = eye(width/8);
for j = 1:width/8
    cPhi3(:,2*j-1) = I(:,j)/normFactor;
    cPhi3(:,2*j)   = I(:,j)/normFactor;
    cPsi3(:,2*j-1) = I(:,j)/normFactor;
    cPsi3(:,2*j)   = -I(:,j)/normFactor;
end

I = eye(frames/8);
for j = 1:frames/8
    sPhi3(:,2*j-1) = I(:,j)/normFactor;
    sPhi3(:,2*j)   = I(:,j)/normFactor;
    sPsi3(:,2*j-1) = I(:,j)/normFactor;
    sPsi3(:,2*j)   = -I(:,j)/normFactor;
end

% scale 4 filters
rPhi4 = zeros(height/16,height/8);
rPsi4 = zeros(height/16,height/8);
cPhi4 = zeros(width/16,width/8);
cPsi4 = zeros(width/16,width/8);
sPhi4 = zeros(frames/16,frames/8);
sPsi4 = zeros(frames/16,frames/8);

I = eye(height/16);
for j = 1:height/16
    rPhi4(:,2*j-1) = I(:,j)/normFactor;
    rPhi4(:,2*j)   = I(:,j)/normFactor;
    rPsi4(:,2*j-1) = I(:,j)/normFactor;
    rPsi4(:,2*j)   = -I(:,j)/normFactor;
end

I = eye(width/16);
for j = 1:width/16
    cPhi4(:,2*j-1) = I(:,j)/normFactor;
    cPhi4(:,2*j)   = I(:,j)/normFactor;
    cPsi4(:,2*j-1) = I(:,j)/normFactor;
    cPsi4(:,2*j)   = -I(:,j)/normFactor;
end

I = eye(frames/16);
for j = 1:frames/16
    sPhi4(:,2*j-1) = I(:,j)/normFactor;
    sPhi4(:,2*j)   = I(:,j)/normFactor;
    sPsi4(:,2*j-1) = I(:,j)/normFactor;
    sPsi4(:,2*j)   = -I(:,j)/normFactor;
end


BASIS4 = [kron(sPhi4*sPhi3*sPhi2*sPhi,kron(cPhi4*cPhi3*cPhi2*cPhi,rPhi4*rPhi3*rPhi2*rPhi)); ...
          kron(sPhi4*sPhi3*sPhi2*sPhi,kron(cPhi4*cPhi3*cPhi2*cPhi,rPsi4*rPhi3*rPhi2*rPhi)); ...
          kron(sPhi4*sPhi3*sPhi2*sPhi,kron(cPsi4*cPhi3*cPhi2*cPhi,rPhi4*rPhi3*rPhi2*rPhi)); ...
          kron(sPhi4*sPhi3*sPhi2*sPhi,kron(cPsi4*cPhi3*cPhi2*cPhi,rPsi4*rPhi3*rPhi2*rPhi)); ...
          kron(sPsi4*sPhi3*sPhi2*sPhi,kron(cPhi4*cPhi3*cPhi2*cPhi,rPhi4*rPhi3*rPhi2*rPhi)); ...
          kron(sPsi4*sPhi3*sPhi2*sPhi,kron(cPhi4*cPhi3*cPhi2*cPhi,rPsi4*rPhi3*rPhi2*rPhi)); ...
          kron(sPsi4*sPhi3*sPhi2*sPhi,kron(cPsi4*cPhi3*cPhi2*cPhi,rPhi4*rPhi3*rPhi2*rPhi)); ...
          kron(sPsi4*sPhi3*sPhi2*sPhi,kron(cPsi4*cPhi3*cPhi2*cPhi,rPsi4*rPhi3*rPhi2*rPhi)); ...
          ...
          kron(sPhi3*sPhi2*sPhi,kron(cPhi3*cPhi2*cPhi,rPsi3*rPhi2*rPhi)); ...
          kron(sPhi3*sPhi2*sPhi,kron(cPsi3*cPhi2*cPhi,rPhi3*rPhi2*rPhi)); ...
          kron(sPhi3*sPhi2*sPhi,kron(cPsi3*cPhi2*cPhi,rPsi3*rPhi2*rPhi)); ...
          kron(sPsi3*sPhi2*sPhi,kron(cPhi3*cPhi2*cPhi,rPhi3*rPhi2*rPhi)); ...
          kron(sPsi3*sPhi2*sPhi,kron(cPhi3*cPhi2*cPhi,rPsi3*rPhi2*rPhi)); ...
          kron(sPsi3*sPhi2*sPhi,kron(cPsi3*cPhi2*cPhi,rPhi3*rPhi2*rPhi)); ...
          kron(sPsi3*sPhi2*sPhi,kron(cPsi3*cPhi2*cPhi,rPsi3*rPhi2*rPhi)); ...
          ...
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

BASIS4 = BASIS4';
outFile = strcat('../testFiles/basis_4_',num2str(height),'-',num2str(width),'-',num2str(frames),'.txt');
save(outFile,'BASIS4','-ascii');
      
%cpp_basis = load('../test_basis_4.txt');
% 
% A = int32(normFactor^3*BASIS4);
% B = int32(normFactor^3*cpp_basis);
% isequal(A,B')
% isequal(BASIS4',cpp_basis)
% sum(sum(abs(BASIS4-cpp_basis')))/height/frames/width
% 
% CC = abs(BASIS4'-cpp_basis)>0.0001;
% fprintf('Error is %d out of %d\n',sum(sum(CC)),(height*width*frames)^2);
% 
% LLL_true = BASIS4(1:dSize/8,:)';
% LLL_false = cpp_basis(:,1:dSize/8);
% 
% DD = abs(LLL_true-LLL_false)>0.0001;
% fprintf('Error in LLL is %d\n',sum(sum(DD)));




function BASIS = haarBasis(height, width,frames)

rPhi = zeros(height/2,height);
rPsi = zeros(height/2,height);
cPhi = zeros(width/2,width);
cPsi = zeros(width/2,width);
sPhi = zeros(frames/2,frames);
sPsi = zeros(frames/2,frames);


I = eye(height/2);
for j = 1:height/2
    rPhi(:,2*j-1) = I(:,j)/sqrt(2);
    rPhi(:,2*j)   = I(:,j)/sqrt(2);
    rPsi(:,2*j-1) = I(:,j)/sqrt(2);
    rPsi(:,2*j)   = -I(:,j)/sqrt(2);
end

I = eye(width/2);
for j = 1:width/2
    cPhi(:,2*j-1) = I(:,j)/sqrt(2);
    cPhi(:,2*j)   = I(:,j)/sqrt(2);
    cPsi(:,2*j-1) = I(:,j)/sqrt(2);
    cPsi(:,2*j)   = -I(:,j)/sqrt(2);
end

I = eye(frames/2);
for j = 1:frames/2
    sPhi(:,2*j-1) = I(:,j)/sqrt(2);
    sPhi(:,2*j)   = I(:,j)/sqrt(2);
    sPsi(:,2*j-1) = I(:,j)/sqrt(2);
    sPsi(:,2*j)   = -I(:,j)/sqrt(2);
end

BASIS = [ kron(sPhi,kron(cPhi,rPhi)); ...
          kron(sPhi,kron(cPhi,rPsi)); ...
          kron(sPhi,kron(cPsi,rPhi)); ...
          kron(sPhi,kron(cPsi,rPsi)); ...
          kron(sPsi,kron(cPhi,rPhi)); ...
          kron(sPsi,kron(cPhi,rPsi)); ...
          kron(sPsi,kron(cPsi,rPhi)); ...
          kron(sPsi,kron(cPsi,rPsi))];
end


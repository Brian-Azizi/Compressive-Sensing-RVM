N = 4;
s = 2;
% 
% Q = haarMatrix2D(N,s);  % non-standard 2D haar matrix 
% 
% PP = haarMatrix1D(N,1)
% P1 = PP(:,1:N/2)
% 
% P2 = haarMatrix1D(2,1)
% 
% P = P1*P2
% 
% PP(:,1:N/2) = P
% 
% R = kron(PP,PP)
% Q
% 


A = haarMatrix2D(N,s)
B = vec(A,2,2);
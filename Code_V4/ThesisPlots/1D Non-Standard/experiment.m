% N = 4;
% A = haarMatrix2D(N,2)
 %P = haarMatrix1D(N,1)
% Q = kron(P,P)
% A-Q

% P = haarMatrix1D(4,1)
% Q = kron(P,P)
% R = haarMatrix2D(4,2)

H2 = [1 1 0 0; 0 0 1 1]/sqrt(2);
G2 = [1 -1 0 0; 0 0 1 -1]/sqrt(2);

H1 = [1 1]/sqrt(2);
G1 = [1 -1]/sqrt(2);

P = [kron(H1*H2,H1*H2);...
    kron(H1*H2,G1*H2);...
    kron(G1*H2,H1*H2);...
    kron(G1*H2,G1*H2);...
    kron(H2,G2);...
    kron(G2,H2);...
    kron(G2,G2)]';


A = vec(P,4,4);
rank(A);


Q = [kron(H2,H2);...        
    kron(H2,G2);...
    kron(G2,H2);...
    kron(G2,G2)]'

B = vec(Q,4,4);
rank(B)

S = [H2;G2];
R = kron(S,S)'
C = vec(R,4,4);
rank(C)
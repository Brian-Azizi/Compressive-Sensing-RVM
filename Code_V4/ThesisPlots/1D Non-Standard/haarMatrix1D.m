%% Generate the basis Matrix for the 1D Haar wavelets
% N = length of each dimension, i,e, total # pixels = N*N. Must be a power
% of 2 and the power must be >= scale.
% scale = scale of haar wavelets. 
%

function P = haarMatrix1D(N,scale)

if mod(log2(N),1) ~= 0
    error('N must be a power of 2');
elseif scale > log2(N)
    error('scale not supported by N');
end  

P = zeros(N);

s = scale;
    k = 1; 
    
    for i=0:N-1  
        u = linspace(0,N,N);
            
        F = haar1D(N,u,i,s);                        
                        
        P(:,k) = F';
        k = k+1;
    end
    
end

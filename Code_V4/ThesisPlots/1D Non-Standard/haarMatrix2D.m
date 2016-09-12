%% Generate the basis Matrix for the 2D Haar wavelets
% N = length of each dimension, i,e, total # pixels = N*N. Must be a power
% of 2 and the power must be >= scale.
% scale = scale of haar wavelets. 
%

function P = haarMatrix2D(N,scale)
    q = log2(N);
    if mod(q,1) ~= 0
        disp('Error: N is not a power of 2');
        return;
    elseif scale > q
        disp('Error: size of matrix does not support the scale');
        return;
    end
    
    n = N; 
    s = scale;

    k = 1;
    P = zeros(N*N);
    for j=0:N-1
        for i=0:N-1
            [u,v] = meshgrid(linspace(0,N,n));
            
            F = haar2D(N,u,v,i,j,s);                        
            
            P(:,k) = reshape(F,N*N,1);
            k = k+1;
        end
    end
end
%% Generate the basis Matrix for the 3D Haar wavelets
% N = length of each dimension, i,e, total # pixels = N*N*N. Must be a power
% of 2 and the power must be >= scale.
% scale = scale of haar wavelets. 
%

function P = haarMatrix3D(N,scale)
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

    idx = 1;
    P = zeros(N*N*N);
%     figure;
    for i=0:N-1
        for j=0:N-1
            for k = 0:N-1
                [u,v,w] = meshgrid(linspace(0,N,n));            
                F = haar3D(N,u,v,w,i,j,k,s);                           
                
%                 F
%                 for jdx = 1:N
%                     subplot(N,1,jdx); imagesc(F(:,:,jdx)); colormap(gray), axis square;
%                 end
%                 pause;
                P(:,idx) = cube2vec(F);
                idx = idx+1;
            end
        end
    end
end
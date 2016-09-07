%% Script to generate pictures that display the 1D Haar wavelets at different scales.

% SETTINGS
N = 16; % Set the length. Must be power of 2
topScale = 4;  % Set the maximum scale. Must not exceed log2(N).
% END SETTINGS


close all; %% WARNING: closes all figures.


if mod(log2(N),1) ~= 0
    error('N must be a power of 2');
elseif topScale > log2(N)
    error('scale not supported by N');
end  

P = zeros(N);

for s = 1:topScale
    k = 1; 
    disp('Press any key to continue');
    for i=0:N-1  
        u = linspace(0,N,N);
            
        F = haar1D(N,u,i,s);                        
        
        bar(F); axis([0,N+1,-1,1]);
        title(strcat('1D Haar. Scale=',num2str(s)));
        pause
            
        P(:,k) = F';
        k = k+1;
    end
    disp(['scale ' num2str(s) ' basis matrix:']);
    disp(P);
end

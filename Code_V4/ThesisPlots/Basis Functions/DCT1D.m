%% Produce illustrations of 1D DCT basis functions.

% SETTINGS:
N = 30;  % Size along each dimension

% END SETTINGS

close all; %% CAREFUL: closes all currently open figures



P = zeros(N);
k = 1;
disp('Hit any key to produce next plot');
for i=0:N-1    
        u = 0:N-1;
        F = cos(pi*(2*u+1)*i*0.5/N)*sqrt(2/N);
        if i == 0
            F = F/sqrt(2);
        end
        bar(F); axis([0,N+1,-2/sqrt(N),2/sqrt(N)]);        
        pause;
        
        P(:,k) = F';
        k=k+1;     
end
disp('DCT basis matrix:');
disp(P); 
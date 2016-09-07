%% Script to generate pictures that display the 2D Haar wavelets at different scales.

topScale = 4;  % Set the maximum scale

N = 2^topScale;  % Number of "pixels" in each dimension

n = 51;  % resolution of the grid (increase for higher quality plot)
b=ceil(0.3*n);  % border inside the grid


for s = 1:topScale
   
    P = ones((n+b)*N+b)*0.7;
    for i=0:N-1
        for j=0:N-1
            [u,v] = meshgrid(linspace(0,N,n));
            
            F = haar2D(N,u,v,i,j,s);                        
            P((b+(n+b)*i+1):(b+(n+b)*i+n),(b+(n+b)*j+1):(b+(n+b)*j+n)) = F;
        end
    end

    figure, imagesc(P), colormap(gray), axis off, axis square;
    title(strcat('2D Haar Wavelets (non-standard construction) - Scale = ',num2str(s)));
end

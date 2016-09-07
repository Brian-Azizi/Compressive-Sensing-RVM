%% Produce illustration of 2D DCT basis functions.

% SETTINGS
N = 16;  % Size along each dimension
n = 301; % Resolution of the image
b=ceil(0.15*n); % border between subplots
% END SETTINGS

P = ones((n+b)*N+b)*0.6;
for i=0:N-1
    for j=0:N-1
        [u,v] = meshgrid(linspace(-0.5,N-0.5,n));
        F = cos(pi*(2*u+1)*i*0.5/N).*cos(pi*(2*v+1)*j*0.5/N);
%         if i == 0
%              F = F / sqrt(2);
%         end
%         if j == 0
%              F = F / sqrt(2);
%         end
        P((b+(n+b)*i+1):(b+(n+b)*i+n),(b+(n+b)*j+1):(b+(n+b)*j+n)) = F;
    end
end

figure, imagesc(P), colormap(gray), axis off, axis square;

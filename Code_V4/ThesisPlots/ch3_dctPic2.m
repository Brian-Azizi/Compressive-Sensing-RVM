n = 601;
b=ceil(0.15*n);
N = 4;

P = ones((n+b)*N+b)*0.6;
for i=0:N-1
    for j=0:N-1
        [u,v] = meshgrid(linspace(-1,N,n));
        F = cos(pi*(2*u+1)*i*0.5/N).*cos(pi*(2*v+1)*j*0.5/N);
        P((b+(n+b)*i+1):(b+(n+b)*i+n),(b+(n+b)*j+1):(b+(n+b)*j+n)) = F;
        
%         subplot(8,8,8*i+j+1);
% %         imagesc(F); colormap(gray);
%         axis off;
%         axis square;
    end
end

figure, imagesc(P), colormap(gray), axis off, axis square;

% imwrite(P,'dct2functions.png');

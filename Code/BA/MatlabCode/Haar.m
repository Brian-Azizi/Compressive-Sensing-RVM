N = 2;
haarmother = ones(1,N);
haarfather = ones(1,N);
haarfather = haarfather/norm(haarfather);
for i=N/2+1:N, 
    haarmother(i) = -1;
end;
haarmother = haarmother/norm(haarmother);
haar1D = zeros(N);
haar1D(1,:) = haarfather; 
haar1D(2,:) = haarmother;
j=1;
while 2^j < N,
    scale = 2^(j/2);
    for i = 1: 2^j,
        for k =1:N,
            index = 2^j*(k-1) - (i-1)*(N-1) + 1;
            if ((index <1) || (index >N)) 
                haar1D(2^j+i,k) = 0;
            else haar1D(2^j+i,k) = haarmother(index);
            end;
        end;
        haar1D(2^j+i,:) = haar1D(2^j+i,:) * scale;
    end;
    j = j+1;
end;
figure;
for j =1:N,
    h = subplot(N/2,2,j);
    stairs([haar1D(j,:) 0],'k-');
    axis(h,[1 N+1 -1.5 1.5]);
    set(gca,'XTick',[1 N+1])
set(gca,'XTickLabel',{'0','1'})
end;

figure;
haar2D = zeros(N,N,N,N);
for i=1:N,
    for j =1:N,
        haar2D(i,j,:,:) = haar1D(i,:)' * haar1D(j,:);
        k = (i-1)*N +(j-1)+1;
        subplot('Position',[(mod(k-1,N))/N 1-(ceil(k/N))/N 1/N 1/N]);
        I = mat2gray(squeeze(haar2D(i,j,:,:)));
        imshow(I);
        axis equal
    end;
end;

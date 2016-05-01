x = linspace(-10,10,500);

figure;

N = 2;
NN = 2*N + 1;
c=0;
for J = -N:1:N
    for K = -N:1:N
        y = haar(x,J,K);
        c = c + 1;
        subplot(NN,NN,c);
        plot(x,y);
    end
end
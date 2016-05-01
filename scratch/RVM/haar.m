function h = haar(x, j, k)
in = 2^j * x - k;
h = zeros(size(x));
idx1 = (0 <= in) & (in < 0.5);
idx2 = (0.5 <= in) & (in < 1);
h(idx1) = 1;
h(idx2) = -1;
h = 2^(j/2)*h;
end
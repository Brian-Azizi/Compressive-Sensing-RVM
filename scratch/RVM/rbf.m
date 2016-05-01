function k = rbf(x,y,r)
k = exp(-norm(x-y)^2 / r);
end
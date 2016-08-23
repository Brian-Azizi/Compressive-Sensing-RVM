function y = unify(x)
    mn = min(x(:));
    mx = max(x(:));
    if abs(mx-mn) < 1e-12 % x is const
        y = zeros(size(x));
    else
        y = (x-mn)/(mx-mn);
    end
end
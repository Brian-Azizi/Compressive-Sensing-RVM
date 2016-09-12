function B = vec(A,p,q)
    s = size(A);
    if numel(s) > 2
        error('A must be a matrix');
    end
    mp = s(1);
    nq = s(2);
    
    if mod(mp,p) ~= 0
        error('p does not divide length of first dimension of A');
    elseif mod(nq,q) ~= 0
        error('q does not divide length of second dimension of A');
    end
    
    m = mp/p;
    n = nq/q;
    
    B = zeros(m*n,p*q);
        
    k = 1;
    for j = 1:n
        for i = 1:m
            temp = A((i-1)*p+1:i*p,(j-1)*q+1:j*q);
            B(k,:) = temp(:)';
            k = k+1;
        end
    end    
end
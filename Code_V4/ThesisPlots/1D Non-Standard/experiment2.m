A = haarMatrix2D(4,2);
% perm = [1,5,2,6,9,13,10,14,3,7,4,8,11,15,12,16];
% perm = [1,2,5,6,3,4,7,8,9,10,13,14,11,12,15,16];

min_rnk = 20;
k = 0;
max_K = 1e8;
while true
    perm = [1,randperm(15)+1];
    B = A(:,perm);
    % B = A';
    rnk = tmp_check(B);
    if rnk < min_rnk
        min_rnk = rnk
        min_perm = perm
    end
    
    if min_rnk == 1
        disp('success')
        break;
    end
    
    if k > max_K
        disp('failure')
        break;
    end

    if mod(k,1e5) == 0
        disp(k);
    end

    k = k+1;
end

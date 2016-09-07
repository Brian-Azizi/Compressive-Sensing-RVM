%% Helper function for haarPic2D.m
function F = haar2D(N,u,v,i,j,s)
    if i < N/2 && j < N/2    
        if s == 1
            F = haarFather(u/2-j).*haarFather(v/2-i);
        else
            F = haar2D(N/2,u/2,v/2,i,j,s-1);
        end
    elseif i < N/2 && ~(j < N/2)
        F = haarFather(u/2-j+N/2).*haarMother(v/2-i);
    elseif ~(i < N/2) && j < N/2
        F = haarMother(u/2-j).*haarFather(v/2-i+N/2);
    else
        F = haarMother(u/2-j+N/2).*haarMother(v/2-i+N/2);
    end

end
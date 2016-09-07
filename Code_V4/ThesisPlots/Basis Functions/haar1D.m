%% Helper function for haarPic2D.m
function F = haar1D(N,u,i,s)
    if i < N/2    
        if s == 1
            F = haarFather(u/2-i);
        else
            F = haar1D(N/2,u/2,i,s-1);
        end        
    else
        F = haarMother(u/2-i+N/2);
    end

end
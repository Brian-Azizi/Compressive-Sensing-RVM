%% Helper function for generating the 3D haar basis functions.
function F = haar3D(N,u,v,w,i,j,k,s)
    if k < N/2 && i < N/2 && j < N/2
        if s == 1
            F = haarFather(w/2-k).*haarFather(u/2-j).*haarFather(v/2-i);
        else
            F = haar3D(N/2,u/2,v/2,w/2,i,j,k,s-1);
        end
    elseif k < N/2 && i < N/2 && ~(j < N/2)
        F = haarFather(w/2-k).*haarFather(u/2-j+N/2).*haarMother(v/2-i);
    elseif k < N/2 && ~(i < N/2) && j < N/2
        F = haarFather(w/2-k).*haarMother(u/2-j).*haarFather(v/2-i+N/2);
    elseif k < N/2 && ~(i < N/2) && ~(j < N/2)
        F = haarFather(w/2-k).*haarMother(u/2-j+N/2).*haarMother(v/2-i+N/2);
        
    elseif ~(k < N/2) && i < N/2 && j < N/2
        F = haarMother(w/2-k+N/2).*haarFather(u/2-j).*haarFather(v/2-i);
    elseif ~(k < N/2) && i < N/2 && ~(j < N/2)
        F = haarMother(w/2-k+N/2).*haarFather(u/2-j+N/2).*haarMother(v/2-i);
    elseif ~(k < N/2) && ~(i < N/2) && j < N/2
        F = haarMother(w/2-k+N/2).*haarMother(u/2-j).*haarFather(v/2-i+N/2);
    elseif ~(k < N/2) && ~(i < N/2) && ~(j < N/2)
        F = haarMother(w/2-k+N/2).*haarMother(u/2-j+N/2).*haarMother(v/2-i+N/2);    
    end
end
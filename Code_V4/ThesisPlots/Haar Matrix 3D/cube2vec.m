%% Vectorizes a 3D-array in a row-major fashion
function v = cube2vec(F)
    s = size(F);
    if numel(s) > 3
        disp('Error: Dimensionality of array is too large');
        return;
    elseif numel(s) == 2
        s = [s,1];
    end
    
    A = zeros(s(2),s(1),s(3));
    
    for k = 1:s(3)
        A(:,:,k) = F(:,:,k)';
    end
    
    v = A(:);
end
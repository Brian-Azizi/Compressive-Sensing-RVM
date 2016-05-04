function haar = haar3D(raw)
    sz = size(raw); % assumed to be powers of 2
    width = sz(2);
    height = sz(1);
    frames = sz(3);
    haar = zeros(sz);
    tmp = double(raw);
    haar(:,:,:) = double(tmp);
    % columns
    for j = 1:width/2
        haar(:,j,:) = (tmp(:,2*j-1,:)+tmp(:,2*j,:))/sqrt(2);
        haar(:,width/2+j,:) = (tmp(:,2*j-1,:) - tmp(:,2*j,:))/sqrt(2);
    end
    % rows
    tmp = haar;
    for i = 1:height/2
        haar(i,:,:) = (tmp(2*i-1,:,:) + tmp(2*i,:,:))/sqrt(2);
        haar(height/2+i,:,:) = (tmp(2*i-1,:,:) - tmp(2*i,:,:))/sqrt(2);
    end
    % frames
    tmp = haar;
    for k = 1:frames/2
        haar(:,:,k) = (tmp(:,:,2*k-1) + tmp(:,:,2*k))/sqrt(2);
        haar(:,:,frames/2+k) = (tmp(:,:,2*k-1) - tmp(:,:,2*k))/sqrt(2);
    end
    
end
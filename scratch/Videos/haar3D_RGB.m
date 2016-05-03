function haar = haar3D_RGB(raw)
    sz = size(raw); % assumed to be powers of 2
    width = sz(2);
    height = sz(1);
    frames = sz(4);
    haar = zeros(sz);
    haar(:,:,:,:) = raw;
    % columns
    for j = 1:width/2
        haar(:,j,:,:) = (raw(:,2*j-1,:,:)+raw(:,2*j,:,:))/sqrt(2);
        haar(:,width/2+j,:,:) = (raw(:,2*j-1,:,:) - raw(:,2*j,:,:))/sqrt(2);
    end
    % rows
    raw = haar;
    for i = 1:height/2
        haar(i,:,:,:) = (raw(2*i-1,:,:,:) + raw(2*i,:,:,:))/sqrt(2);
        haar(height/2+i,:,:,:) = (raw(2*i-1,:,:,:) - raw(2*i,:,:,:))/sqrt(2);
    end
    % frames
    raw = haar;
    for k = 1:frames/2
        haar(:,:,:,k) = (raw(:,:,:,2*k-1) + raw(:,:,:,2*k))/sqrt(2);
        haar(:,:,:,frames/2+k) = (raw(:,:,:,2*k-1) - raw(:,:,:,2*k))/sqrt(2);
    end
    
end
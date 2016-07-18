function haar = haar2D(raw,scale)
    sz = size(raw); % assumed to be powers of 2
    width = sz(2);
    height = sz(1);
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
    LL = haar(1:height/2,1:width/2);
    if (scale > 1)
        haar2 = haar2D(LL,scale -1);
        haar(1:height/2,1:width/2) = haar2;
    end
end
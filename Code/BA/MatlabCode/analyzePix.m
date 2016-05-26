tabStats = zeros(topScale,7);
figure; title(name);
N = height*width*frames;

for i = 1:topScale
    R = recovers(:,:,:,i);
    mn = min(min(min(R)));
    mx = max(max(max(R)));
    
    subplot(max(floor(sqrt(topScale)),2),ceil(sqrt(topScale)),i);
    histogram(R(abs(R(:))<10000),int16(mx-mn)+1); title([name,' Stage ', num2str(i)]);
    hold on;
    [m, f] = mode(R(:));
    plot([255; 255],[0; f],'r');
    plot([0; 0],[0; f],'r');
    %axis([-200 400 0 numel(R)]);
    
    low = 100*sum(R(:) < 0)/N;
    high = 100*sum(R(:) > 255)/N;
    
    E = uint8(original) - uint8(R);
    mse = mean(E(:).^2);
    psnr = 10*log10(256^2/mse);
    
    tabStats(i,:) = [i, mse, psnr, mn, mx, low, high];
end


fprintf('\nStage\tmse\t\tpsnr\t\tmin\t\tmax\t\tbelow 0 (%%)\tabove 255 (%%)\n');
for i = 1:topScale
    fprintf('%u\t',tabStats(i,1));
    for j = 2:7
        fprintf('%f\t',tabStats(i,j));
    end
    fprintf('\n');
end

function y = haarFather(x)
    y = zeros(size(x));
    for i = 1:numel(x)
        if x(i) >= 0 && x(i) < 1
            y(i) = 1;
        else
            y(i) = 0;
        end
    end
end
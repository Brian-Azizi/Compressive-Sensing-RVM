function scale = scaleHaar(haar)
mx = max(max(max(haar)));
mn = min(min(min(haar)));
%scale = 255*(haar - mn)/(mx-mn);

scale = log(haar+1-mn);

mx = max(max(max(scale)));
mn = min(min(min(scale)));
scale = 255*(scale - mn)/(mx-mn);

end
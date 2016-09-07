function F = lowPassHaar2D(u,v,i,j,scale)
  F = haarFather(u/2-j).*haarFather(v/2-i);
end
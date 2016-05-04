template<class T>
void vectorize3D(T*** cube, T* column, int height, int width, int frames)
{
    for (int k = 0; k < frames; ++k) {
	for (int j = 0; j < width; ++j) {
	    for (int i = 0; i < height; ++i) {
		column[k*width*height + j*height + i] = cube[i][j][k];
	    }
	}
    }
    return;
}


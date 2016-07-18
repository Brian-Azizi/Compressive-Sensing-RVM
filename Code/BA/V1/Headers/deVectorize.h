template <class T, class V>
void deVectorize(T *vector, V ***block, int height, int width, int frames)
{
    for (int k = 0; k < frames; ++k) {
	for (int j = 0; j < width; ++j) {
	    for (int i = 0; i < height; ++i) {
		block[i][j][k] = vector[k*height*width + j*height + i];
	    }
	}
    }

    return;
}

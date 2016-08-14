template <class T>
void dctBasis(T **BASIS, int height, int width, int frames)
{
    if (frames == 1) {
	return dctBasis2D(BASIS, height, width);
    } else {
    
    /* Allocate space */
    
    // Row filter
    T **rowDCT = new T*[height];
    for (int i = 0; i < height; ++i) {
	rowDCT[i] = new T[height];
    }
    
    // Column filter
    T **colDCT = new T*[width];
    for (int i = 0; i < width; ++i) {
	colDCT[i] = new T[width];
    }

    // Slice filter
    T **sliceDCT = new T*[frames];
    for (int i = 0; i < frames; ++i) {
	sliceDCT[i] = new T[frames];
    }

    // tmp    
    int tmpDim = width*height;
    T **tmp = new T*[tmpDim];
    for (int i = 0; i < tmpDim; ++i) {
	tmp[i] = new T[tmpDim];
    }
    
    // transpose;
    int dictionarySize = height*width*frames;

    T **BASIS_tran = new T*[dictionarySize];
    for (int i = 0; i < dictionarySize; ++i) {
	BASIS_tran[i] = new T[dictionarySize];
    }
    
    /* Generate Filters */
    dctMatrix(rowDCT, height);
    dctMatrix(colDCT, width);
    dctMatrix(sliceDCT, frames);

    /* Produce Kronecker product */
    kronecker(colDCT, rowDCT, tmp, width, width, height, height);
    kronecker(sliceDCT, tmp, BASIS_tran, frames, frames, tmpDim, tmpDim);
    transpose2D(BASIS_tran, BASIS, dictionarySize, dictionarySize);

    /* Free space */
    for (int i = 0; i < dictionarySize; ++i) {
	delete[] BASIS_tran[i];
    }
    delete[] BASIS_tran;

    for (int i = 0; i < tmpDim; ++i) {
	delete[] tmp[i];
    }
    delete[] tmp;

    for (int i = 0; i < height; ++i) {
	delete[] rowDCT[i];
    }
    delete[] rowDCT;

    for (int i = 0; i < width; ++i) {
	delete[] colDCT[i];
    }
    delete[] colDCT;

    for (int i = 0; i < frames; ++i) {
	delete[] sliceDCT[i];
    }
    delete[] sliceDCT;

    return;
    }
}

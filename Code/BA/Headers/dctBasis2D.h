template <class T>
void dctBasis2D(T **BASIS, int height, int width)
{
    
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

    /* Generate Filters */
    dctMatrix(rowDCT, height);
    dctMatrix(colDCT, width);

    /* Produce Kronecker product */
    kronecker(colDCT, rowDCT, BASIS, width, width, height, height);

    /* Free space */
    for (int i = 0; i < height; ++i) {
	delete[] rowDCT[i];
    }
    delete[] rowDCT;

    for (int i = 0; i < width; ++i) {
	delete[] colDCT[i];
    }
    delete[] colDCT;

    return;
}

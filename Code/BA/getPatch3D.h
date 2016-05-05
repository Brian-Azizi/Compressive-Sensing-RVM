/*Assumes that blocks are correctly dividing up the signal and indeces are in valid range. This function does not know how large the signal is.*/
template<class T>
void getPatch3D(T const*const*const*const signal , T ***patch, int blockHeight, int blockWidth, int blockFrames, int rowIdx, int colIdx, int frameIdx)
{
    for (int i = 0; i < blockHeight; ++i) {
	for (int j = 0; j < blockWidth; ++j) {
	    for (int k = 0; k < blockFrames; ++k) {
		patch[i][j][k] = signal[blockHeight*rowIdx + i][blockWidth*colIdx + j][blockFrames*frameIdx + k];
	    }
	}
    }
    
    return;
}

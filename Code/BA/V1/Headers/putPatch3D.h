template<class T>
void putPatch3D(T ***patch, T ***signal, int blockHeight, int blockWidth, int blockFrames, int rowIdx, int colIdx, int frameIdx) 
{
    for (int i = 0; i < blockHeight; ++i) {
	for (int j = 0; j < blockWidth; ++j) {
	    for (int k = 0; k < blockFrames; ++k) {
		signal[blockHeight*rowIdx + i][blockWidth*colIdx + j][blockFrames*frameIdx + k] = patch[i][j][k];
	    }
	}
    }
    
    return;
}

template <class T>
void getDesignMatrix(T **orig, T **designMatrix, bool *sensed, int origSize, int targetSize)
{
    int currentIdx = 0;
    for (int i = 0; i < origSize; ++i) {
	if (sensed[i]) {
	    for (int j = 0; j < origSize; ++j) {
		designMatrix[currentIdx][j] = orig[i][j];
	    }
	    ++currentIdx;
	}
    }

    // sanity check
    assert(targetSize == currentIdx && "measurements do not match number of sensed entries"); // && is a little hack that allows the message to be shown when the assert condition (before &&) fails

    return;
}

template<class T>
void corruptSignal(T ***signal, T ***corruptedSignal, bool ***sensed, int height, int width, int frames, double percentage)
{
    double randNum;
    bool **filterMatrix = new bool*[height];
    for (int i = 0; i < height; ++i) {
	filterMatrix[i] = new bool[width];
    }
    
    for (int i = 0; i < height; ++i) {
	for (int j = 0; j < width; ++j) {
	    randNum = ((double) rand()) / RAND_MAX; // between 0 and 1;
	    if (randNum < percentage/100) {
		filterMatrix[i][j] = false;
	    } else {
		filterMatrix[i][j] = true;
	    }
	}
    }
    
    for (int k = 0; k < frames; ++k) {
	for (int i = 0; i < height; ++i) {
	    for (int j = 0; j < width; ++j) {
		sensed[i][j][k] = filterMatrix[i][j];
		if (filterMatrix[i][j]) {
		    corruptedSignal[i][j][k] = signal[i][j][k];
		} else {
		    corruptedSignal[i][j][k] = 0;
		}
	    }
	}
    }

    // Clean-up
    for (int i = 0; i < height; ++i) {
	delete[] filterMatrix[i];
    }
    delete[] filterMatrix;

    return;
}

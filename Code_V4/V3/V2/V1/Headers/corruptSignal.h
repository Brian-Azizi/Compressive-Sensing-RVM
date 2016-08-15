template<class T>
void corruptSignal(T ***signal, T ***corruptedSignal, bool ***sensed, int height, int width, int frames, double percentage, corruptionMode setting)
{
    double randNum;
    bool ***filterBlock = new bool**[height];
    for (int i = 0; i < height; ++i) {
	filterBlock[i] = new bool*[width];
	for (int j = 0; j < width; ++j) {
	    filterBlock[i][j] = new bool[frames];
	}
    }
    
    
    for (int i = 0; i < height; ++i) {
	for (int j = 0; j < width; ++j) {
	    for (int k = 0; k < frames; ++k) {
		randNum = ((double) rand()) / RAND_MAX; // between 0 and 1;
		if (randNum < percentage/100) {
		    filterBlock[i][j][k] = false;
		} else {
		    filterBlock[i][j][k] = true;
		}
	    }
	}
    }
    

    for (int k = 0; k < frames; ++k) {
	for (int i = 0; i < height; ++i) {
	    for (int j = 0; j < width; ++j) {
		switch (setting) {
		case uniform:
		    sensed[i][j][k] = filterBlock[i][j][k];
		    break;
		case timeRays:
		    sensed[i][j][k] = filterBlock[i][j][0];
		    break;
		case verticalFlicker:
		    sensed[i][j][k] = filterBlock[0][j][k];
		    break;
		case horizontalFlicker:
		    sensed[i][j][k] = filterBlock[i][0][k];
		    break;
		case missingFrames:
		    sensed[i][j][k] = filterBlock[0][0][k];
		    break;
		case verticalLines:
		    sensed[i][j][k] = filterBlock[0][j][0];
		    break;
		case horizontalLines:
		    sensed[i][j][k] = filterBlock[i][0][0];
		    break;
		}
		
		if (sensed[i][j][k]) {
		    corruptedSignal[i][j][k] = signal[i][j][k];
		} else {
		    corruptedSignal[i][j][k] = 0;
		}
	    }
	}
    }
    
    // Clean-up
    for (int i = 0; i < height; ++i) {
	for (int j = 0; j < width; ++j) {
	    delete[] filterBlock[i][j];
	}
	delete[] filterBlock[i];
    }
    delete[] filterBlock;

    return;
}

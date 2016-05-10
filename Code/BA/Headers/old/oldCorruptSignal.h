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
    
    if (setting == timeRays) {
	for (int k = 0; k < frames; ++k) {
	    for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
		    sensed[i][j][k] = filterBlock[i][j][0];
		    if (sensed[i][j][k]) {
			corruptedSignal[i][j][k] = signal[i][j][k];
		    } else {
			corruptedSignal[i][j][k] = 0;
		    }
		}
	    }
	}
    } else if (setting == uniform) {
	for (int i = 0; i < height; ++i) {
	    for (int j = 0; j < width; ++j) {
		for (int k = 0; k < frames; ++k) {
		    sensed[i][j][k] = filterBlock[i][j][k];
		    if (sensed[i][j][k]) {
			corruptedSignal[i][j][k] = signal[i][j][k];
		    } else {
			corruptedSignal[i][j][k] = 0;
		    }
		}
	    }
	}
    } else if (setting == missingFrames) {
	for (int k = 0; k < frames; ++k) {
	    for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
		    sensed[i][j][k] = filterBlock[0][0][k];
		    if (sensed[i][j][k]) {
			corruptedSignal[i][j][k] = signal[i][j][k];
		    } else {
			corruptedSignal[i][j][k] = 0;
		    }
		}
	    }
	}
    } else if (setting == verticalLines) {
	for (int k = 0; k < frames; ++k) {
	    for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
		    sensed[i][j][k] = filterBlock[0][j][k];
		    if (sensed[i][j][k]) {
			corruptedSignal[i][j][k] = signal[i][j][k];
		    } else {
			corruptedSignal[i][j][k] = 0;
		    }
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

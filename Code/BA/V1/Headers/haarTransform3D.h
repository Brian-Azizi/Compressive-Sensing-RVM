/* Not needed in current application */

/*Assumes that signalWidth, signalHeight and signalFrames are powers of 2*/
template<class T>
void haarTransform3D (T const*const*const*const signal, double *** transform, int signalHeight, int signalWidth, int signalFrames)
{
    double ***tmp = new double**[signalHeight];
    for (int i = 0; i < signalHeight; ++i) {
	tmp[i] = new double*[signalWidth];
	for (int j = 0; j < signalWidth; ++j) { 
	    tmp[i][j] = new double[signalFrames];
	}
    }

    // Rows

    /* Store original signal in double array 'tmp'. If signalType is unsigned int, then taking differences of signal elements within an expression could cause the result to wrap around. Casting it to double before doing the Haar manipulations should deal with this.
     Demo: use "test-4-8-2.txt" with signalType = unsigned int and with signalType = int.*/
 
    for (int i = 0; i < signalHeight; ++i) { 
	for (int j = 0; j < signalWidth; ++j) {
	    for (int k = 0; k < signalFrames; ++k) {
		tmp[i][j][k] = signal[i][j][k];
	    }
	}
    }
    for (int j = 0; j < signalWidth; ++j) {
	for (int k = 0; k < signalFrames; ++k) { 
	    for (int i = 0; i < signalHeight/2; ++i) {
		transform[i][j][k] = (tmp[2*i][j][k] + tmp[2*i+1][j][k])/sqrt(2);
		transform[i+signalHeight/2][j][k] = (tmp[2*i][j][k] - tmp[2*i+1][j][k])/sqrt(2);
	    }
	}
    }
 
   // Columns
    for (int i = 0; i < signalHeight; ++i) { 
	for (int j = 0; j < signalWidth; ++j) {
	    for (int k = 0; k < signalFrames; ++k) {
		tmp[i][j][k] = transform[i][j][k];
	    }
	}
    }
    for (int k = 0; k < signalFrames; ++k) {
	for (int i = 0; i < signalHeight; ++i) {
	    for (int j = 0; j < signalWidth/2; ++j) {
		transform[i][j][k] = (tmp[i][2*j][k] + tmp[i][2*j+1][k])/sqrt(2);
		transform[i][j+signalWidth/2][k] = (tmp[i][2*j][k] - tmp[i][2*j+1][k])/sqrt(2);
	    }
	}
    }
 
    // Slices
    for (int i = 0; i < signalHeight; ++i) {
	for (int j = 0; j < signalWidth; ++j) {
	    for (int k = 0; k < signalFrames; ++k) {
		tmp[i][j][k] = transform[i][j][k];
	    }
	}
    }
    for (int i = 0; i < signalHeight; ++i) {
	for (int j = 0; j < signalWidth; ++j) {
	    for (int k = 0; k < signalFrames/2; ++k) {
		transform[i][j][k] = (tmp[i][j][2*k] + tmp[i][j][2*k+1])/sqrt(2);
		transform[i][j][k+signalFrames/2] = (tmp[i][j][2*k] - tmp[i][j][2*k+1])/sqrt(2);
	    }
	}
    }

    // Clean up
    for (int i = 0; i < signalHeight; ++i) {
	for (int j = 0; j < signalWidth; ++j) {
	    delete[] tmp[i][j];
	}
	delete[] tmp[i];
    }
    delete[] tmp;

    return;
}

#include <iostream>
#include <fstream>
#include <cmath>
#include <string>

#include "settings_file.h"
#include "input3D.h"
#include "print3D.h"
#include "haarTransform3D.h"

int main()
{
    // Get original input signal
    int ***signal = new int**[signalHeight];
    for (int i = 0; i < signalHeight; ++i) {
    	signal[i] = new int*[signalWidth];
    	for (int j = 0; j < signalWidth; ++j) {
    	    signal[i][j] = new int[signalFrames];
    	}
    }
    input3D(signal, inputFile, signalWidth, signalHeight, signalFrames);
    
    // Print signal to 'cout'
    print3D(std::cout, signal, signalWidth, signalHeight, signalFrames);
    return 0;

    
}

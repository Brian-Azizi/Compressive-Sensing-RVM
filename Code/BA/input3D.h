void input3D(int ***signal, const std::string& inputFile, int signalWidth, int signalHeight, int signalFrames)
{    
    // int ***signal = new int**[signalHeight];
    // for (int i = 0; i < signalHeight; ++i) {
    // 	signal[i] = new int*[signalWidth];
    // 	for (int j = 0; j < signalWidth; ++j) {
    // 	    signal[i][j] = new int[signalFrames];
    // 	}
    // }
    
    std::ifstream myFile(inputFile.c_str());
    //    myFile.open(inputFile.c_str());
    int number;
    int cols = 0;
    int rows = 0;
    int slices = 0;
    
    while (myFile >> number) {
	signal[rows][cols][slices] = number;
	if (cols == signalWidth - 1) {
	    cols = 0;
	    if (rows == signalHeight - 1) {
		rows = 0;
		++slices;
	    }
	    else {
		++rows;
	    }
	}
	else {
	    ++cols;
	}
    }
    myFile.close();
    
    return;
}

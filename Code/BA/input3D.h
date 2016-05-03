template<class T>
void input3D(T ***signal, const std::string& inputFile, int signalWidth, int signalHeight, int signalFrames)
{    
    std::ifstream myFile(inputFile.c_str());
    T number;
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

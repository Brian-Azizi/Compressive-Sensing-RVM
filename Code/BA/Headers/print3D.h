template<class T>
void print3D(std::ostream& out, T const*const*const*const signal, int signalHeight, int signalWidth, int signalFrames)
{
    for (int k = 0; k < signalFrames; ++k) { 
	for (int i = 0; i < signalHeight; ++i) {
	    for (int j = 0; j < signalWidth; ++j) { 
		out << signal[i][j][k] << " ";
	    }
	    out << std::endl;
	}
	out << std::endl;
    }

    return;
}

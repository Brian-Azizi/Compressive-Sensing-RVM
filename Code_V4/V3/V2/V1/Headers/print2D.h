template<class T>
void print2D(std::ostream& out, T** signal, int height, int width)
{
    for (int i = 0; i < height; ++i) {
	for (int j = 0; j < width; ++j) {
	    out << signal[i][j] << " ";
	}
	out << std::endl;
    }

    return;
}

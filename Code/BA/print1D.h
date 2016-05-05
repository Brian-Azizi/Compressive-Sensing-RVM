template<class T>
void print1D(std::ostream& out, T* signal, int height)
{
    for (int i = 0; i < height; ++i) {
	out << signal[i] << " " << std::endl;
    }
    out << std::endl;
    return;
}

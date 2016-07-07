#include <iostream>
#include <fstream>
#include <sstream>

int main() 
{
    int height = 3;
    int width = 3;
    int frames = 1;
    
    std::stringstream ss;
    ss << "test_" << height << "-" << width << "-" << frames << ".txt"; 
        
    std::ofstream outFile(ss.str().c_str());
    for (int k = 0; k < frames; ++k) {
	for (int i = 0; i < height; ++i) {
	    for (int j = 0; j < width; ++j) {
		outFile << ( i*width + k*width*height + j) % 256 << " ";
	    }
	    outFile << std::endl;
	}
    }

    outFile.close();
}

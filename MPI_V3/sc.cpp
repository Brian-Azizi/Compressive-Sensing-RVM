#include "Headers/Signal.hpp"


int main() {
    Signal<double> A(3,2,4);
    for (int i = 0; i < A.size(); ++i) A.data()[i] = i;

    std::cout << A;

    for (int i = 0; i < A.size(); ++i) 
	std::cout << i << ":\t" << transpose(A.index(i)) << std::endl;

}

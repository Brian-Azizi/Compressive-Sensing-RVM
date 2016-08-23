#include "Headers/Signal.hpp"

int main() {
    int h = 4, w = 8, f = 1, scale = 1;
    
    Signal<double> W1 = haarBasis(h,w,f,scale);
    Signal<double> W2 = haarBasisDirect(h,w,f,scale);

    //    std::cout << W1 << std::endl << W2;
}

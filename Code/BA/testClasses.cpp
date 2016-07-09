#include <iostream>

#include "Errors.hpp"
#include "Corrupter.hpp"
#include "SignalBasis.hpp"
#include "Dim.hpp"
#include "Signal.hpp"

int main() {
    Signal<double> A(2,2);
    Signal<double> B(A.dim());
    B.reshape(Dim(1,1,4));
    std::cout << B;

}

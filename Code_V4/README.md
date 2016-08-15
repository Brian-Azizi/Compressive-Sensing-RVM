# Video Interpolation/Reconstruction using Bayesian Compressive Sensing #

## Desciption ##
A Bayesian reconstruction algorithm that reconstruct digital videos and images from a very small number of measurements.

The core of the algorithm uses the Relevance Vector Machine (RVM).
Signals are represented using Transform Coding.

## Features ##
* Handles 2D signals (images) and 3D signals (videos) (1 channel only at the moment).
- Uses Discrete Cosine Transform (DCT) or Discrete Wavelet Transform (DWT) to transform the signal
  - So far, the only supported wavelet functions are *Haar wavelets*
- Uses the Multi-Scale Cascade of Estimations algorithm to boost the RVM performance by exploiting the multi-resolution analysis of wavelet functions.


## Instalation ##
For **serial** compilation:

    g++ main.cpp -llapack -lblas -std=c++11 -O3 -o leaf

or

	g++ main.cpp -llapack -lblas -std=c++0x -O3 -o leaf

For **parallel** compilation:

    mpic++ main.cpp -llapack -lblas -std=c++11 -O3 -o leaf

or 

    mpic++ main.cpp -llapack -lblas -std=c++0x -O3 -o leaf

## Usage ##
For **serial**

    ./leaf [configfile]

For **parallel**
    
    mpirun -np <n> ./leaf [configfile]

where `n` is the number of processes. For more options on MPI, check the [open-mpi faq](https://www.open-mpi.org/faq/?category=running).

The default `[configfile]` is `.rvmsettings.cfg`.

## Configuration File ##
The format of the configuration file is:

    setting1 = value1
    setting2 = value2

Leading and trailing whitespace is mostly ignored.
Each line can hold at most one setting/value pair.
Setting/value statements must contain `=`.
The order of the statements is irrelevant.
To use the default value for a particular setting, remove (or comment it out using `#`) that setting from the configuration file.

### Parameter Settings ###

Setting	| Possible Values | Description
:--------|:-----------------:|------------:
inputFile | any file name |name of the original signal 
outputDirectory | any valid directory that you have access to | the output data will be saved in this directory
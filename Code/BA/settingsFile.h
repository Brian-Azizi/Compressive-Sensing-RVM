std::string const inputFile = "foreman-128-128-64.txt";
//std::string const inputFile = "test-4-8-2.txt";
//std::string const inputFile = "akiyo-64-64-64.txt";

typedef unsigned short int signalType; 
typedef double basisType;

unsigned int const signalHeight = 128;
unsigned int const signalWidth = 128;
unsigned int const signalFrames = 64;

unsigned int const blockHeight = 8;
unsigned int const blockWidth = 8;
unsigned int const blockFrames = 8;

double const percentage = 30;

// RVM parameters
double const noiseStD = 0.5;
int const deltaML_threshold = 1;
bool const use_cascade = false;

// Output files
std::string const corruptSigFile = "corruptedSignal.txt";
std::string const recovSigFile = "recoveredSignal.txt";

/*** Input settings ***/
//std::string const inputFileStem = "akiyo_128-128-64.txt";
//std::string const inputFileStem = "foreman_128-128-64.txt";
//std::string const inputFileStem = "foreman_288-352-300.txt";
//std::string const inputFileStem = "foreman_288-352-288.txt";
//std::string const inputFileStem = "soccer_128-128-64.txt";
//std::string const inputFileStem = "soccer_288-352-300.txt";
//std::string const inputFileStem = "stefan_288-352-80.txt";
//std::string const inputFileStem = "waterfall_128-128-64.txt";
//std::string const inputFileStem = "lenna_512-512-1.txt";
//std::string const inputFileStem = "lenna_128-128-1.txt";
//std::string const inputFileStem = "test_4-8-2.txt";
//std::string const inputFileStem = "test_8-8-8.txt";
//std::string const inputFileStem = "test_8-8-16.txt";

//std::string const inputFile = "/local/data/public/ba308/InputFiles/" + inputFileStem;


std::string const inputFile = "lenna.txt";

typedef unsigned int signalType; 
typedef double basisType;

const SigDim blockDim(8,8,1);

/*** print progress to standard output? ***/
bool printToCOut = true;

/*** RNG Settings ***/
int const rngSeed = time(NULL);

/*** Corrupter settings ***/
Corrupter corr(20, Corrupter::uniform);

/*** Basis Function settings ***/
basisFunctionMode const basisMode = dct;

int const endScale = 1;	     	// For DCT keep Scale at 1

int const startScale = 1;	// Keep it at 1. Code doesn't work well at all if we skip 1.
/*** RVM settings ***/
double const noiseStD = 1;
double const deltaML_threshold = 1;


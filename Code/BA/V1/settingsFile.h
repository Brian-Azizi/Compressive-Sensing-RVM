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

//std::string const inputFileStem = "lenna_128-128-1.txt";


//std::string const inputFileStem = "test_4-8-2.txt";
std::string const inputFileStem = "test_4-4-4.txt";
//std::string const inputFileStem = "test_8-8-8.txt";
//std::string const inputFileStem = "test_8-8-16.txt";

//std::string const inputFile = "/local/data/public/ba308/InputFiles/" + inputFileStem;
std::string const inputFile = "./" + inputFileStem;
bool const actualSimulation = true; // set to false to prepend outfile names with "DUMMY_"

typedef unsigned int signalType; 
typedef double basisType;

unsigned int const signalHeight = 4; // unsigned ints: be careful with wrap-arounds!
unsigned int const signalWidth = 4;
unsigned int const signalFrames = 4;

unsigned int const blockHeight = 4;
unsigned int const blockWidth = 4;
unsigned int const blockFrames = 4;


/*** print progress to standard output? ***/
bool printToCOut = true;

/*** RNG Settings ***/
int const rngSeed = 0;

/*** Corrupter settings ***/
corruptionMode const corrupterSetting = uniform;
double const percentage = 30;

/*** Basis Function settings ***/
basisFunctionMode const basisMode = dct;
int const endScale = 1;	     	// For DCT keep Scale at 1

/*** RVM settings ***/
double const noiseStD = 1;
double const deltaML_threshold = 1;


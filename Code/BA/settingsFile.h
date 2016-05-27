/*** Not settings ***/
enum corruptionMode {uniform, timeRays, missingFrames, verticalFlicker, horizontalFlicker, verticalLines, horizontalLines};
std::string settingStrings[] = {"uniform", "timeRays", "missingFrames", "verticalFlicker","horizontalFlicker","verticalLines","horizontalLines"};


/*** Input settings ***/
//std::string const inputFileStem = "akiyo_128-128-64.txt";
std::string const inputFileStem = "foreman_128-128-64.txt";
//std::string const inputFileStem = "foreman_288-352-300.txt";
//std::string const inputFileStem = "foreman_288-352-288.txt";
//std::string const inputFileStem = "soccer_128-128-64.txt";
//std::string const inputFileStem = "soccer_288-352-300.txt";
//std::string const inputFileStem = "stefan_288-352-80.txt";
//std::string const inputFileStem = "waterfall_128-128-64.txt";
//std::string const inputFileStem = "lenna_512-512-1.txt";

//std::string const inputFileStem = "test_4-8-2.txt";
//std::string const inputFileStem = "test_8-8-8.txt";
//std::string const inputFileStem = "test_8-8-16.txt";

std::string const inputFile = "/local/data/public/ba308/InputFiles/" + inputFileStem;
bool const actualSimulation = true; // set to false to prepend outfile names with "DUMMY_"

typedef unsigned int signalType; 
typedef double basisType;

unsigned int const signalHeight = 128; // unsigned ints: be careful with wrap-arounds!
unsigned int const signalWidth = 128;
unsigned int const signalFrames = 64;

unsigned int const blockHeight = 8;
unsigned int const blockWidth = 8;
unsigned int const blockFrames = 8;

/*** print progress to standard output? ***/
bool printToCOut = true;

/*** Corrupter settings ***/
corruptionMode const corrupterSetting = uniform;
double const percentage = 50;

/*** Wavelet settings ***/
int const startScale = 1;	// Keep it at 1. Code doesn't work well at all if we skip 1.
int const endScale = 3;	     	// For DCT keep Scale at 1

/*** RVM settings ***/
double const noiseStD = 1;
int const deltaML_threshold = 1;

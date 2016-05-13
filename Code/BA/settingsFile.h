/*** Not settings ***/
enum corruptionMode {uniform, timeRays, missingFrames, verticalFlicker, horizontalFlicker, verticalLines, horizontalLines};
std::string settingStrings[] = {"uniform", "timeRays", "missingFrames", "verticalFlicker","horizontalFlicker","verticalLines","horizontalLines"};



/*** Input settings ***/

//std::string const inputFile = "akiyo-128-128-64.txt";
//std::string const inputFile = "foreman-128-128-64.txt";
//std::string const inputFile = "soccer-128-128-64.txt";
//std::string const inputFile = "foreman-288-352-300.txt";
std::string const inputFile = "stefan-288-352-80.txt";

//std::string const inputFile = "test-4-8-2.txt";
//std::string const inputFile = "test-8-8-8.txt";
//std::string const inputFile = "test-8-8-16.txt";

bool const actualSimulation = true; // set to false to prepend outfile names with "DUMMY_"
typedef unsigned int signalType; 
typedef double basisType;

unsigned int const signalHeight = 288; // unsigned ints: be careful with wrap-arounds!
unsigned int const signalWidth = 352;
unsigned int const signalFrames = 80;

unsigned int const blockHeight = 4;
unsigned int const blockWidth = 4;
unsigned int const blockFrames = 2;

/*** print progress to standard output? ***/
bool printToCOut = true;

/*** Corrupter settings ***/
corruptionMode const corrupterSetting = timeRays;
double const percentage = 50;

/*** Wavelet settings ***/
int const scale = 1;		// now works for arbitrary scale > 0

/*** RVM settings ***/
double const noiseStD = 1;
int const deltaML_threshold = 1;
bool const use_cascade = false;



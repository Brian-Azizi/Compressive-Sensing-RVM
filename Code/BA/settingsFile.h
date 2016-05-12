/*** Not settings ***/
enum corruptionMode {uniform, timeRays, missingFrames, verticalFlicker, horizontalFlicker, verticalLines, horizontalLines};
std::string settingStrings[] = {"uniform", "timeRays", "missingFrames", "verticalFlicker","horizontalFlicker","verticalLines","horizontalLines"};



/*** Input settings ***/

//std::string const inputFile = "akiyo-128-128-64.txt";
std::string const inputFile = "foreman-128-128-64.txt";
//std::string const inputFile = "soccer-128-128-64.txt";

bool const actualSimulation = true; // set to false to prepend outfile names with "DUMMY_"


//std::string const inputFile = "test-4-8-2.txt";
//std::string const inputFile = "test-8-8-8.txt";
//std::string const inputFile = "test-8-8-16.txt";

typedef unsigned int signalType; 
typedef double basisType;

unsigned int const signalHeight = 128;
unsigned int const signalWidth = 128;
unsigned int const signalFrames = 64;

unsigned int const blockHeight = 8;
unsigned int const blockWidth = 8;
unsigned int const blockFrames = 8;

/*** print progress to standard output? ***/
bool printToCOut = true;


/*** Corrupter settings ***/
corruptionMode const corrupterSetting = timeRays;
double const percentage = 30;

/*** Wavelet settings ***/
int const scale = 3;		// now works for arbitrary scale > 0

/*** RVM settings ***/
double const noiseStD = 0.5;
int const deltaML_threshold = 1;
bool const use_cascade = false;


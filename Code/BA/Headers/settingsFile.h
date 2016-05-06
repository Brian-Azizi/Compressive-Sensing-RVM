std::string const inputFile = "foreman-128-128-64.txt";
//std::string const inputFile = "test-4-8-2.txt";

/*** input settings ***/
typedef unsigned short int signalType; 
typedef double basisType;

unsigned int const signalHeight = 128;
unsigned int const signalWidth = 128;
unsigned int const signalFrames = 64;

unsigned int const blockHeight = 8;
unsigned int const blockWidth = 8;
unsigned int const blockFrames = 8;


/*** corrupter settings ***/
enum corruptionMode {uniform, timeRays};
corruptionMode const corrupterSetting = uniform;

double const percentage = 70;
std::string const perc_label = "70%"; // labels for output files
std::string const settings_label = "u";

/*** RVM parameters ***/
double const noiseStD = 0.5;
int const deltaML_threshold = 1;
bool const use_cascade = false;


/*** Output settings ***/
std::string const corruptSigFile = perc_label + (settingsLabel + ("_corrupted_" + inputFile));
std::string const recovSigFile = perc_label + (settingsLabel + ("_recovered_" + inputFile));


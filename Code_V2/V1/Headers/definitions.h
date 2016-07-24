/*** Not settings ***/
enum corruptionMode {uniform, timeRays, missingFrames, verticalFlicker, horizontalFlicker, verticalLines, horizontalLines};
std::string settingStrings[] = {"uniform", "timeRays", "missingFrames", "verticalFlicker","horizontalFlicker","verticalLines","horizontalLines"};

enum basisFunctionMode {haar, dct};
std::string basisFunctionStrings[] = {"haar", "dct"};

int const startScale = 1;	// Keep it at 1. Code doesn't work well at all if we skip 1.


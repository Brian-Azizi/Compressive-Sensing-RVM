/***************Outside the RVM - Application specific************************/

int signalResolution = 512; //define the dimension of the problem
int blockResolution = 8; //define the dimension of the signal patches used - higher the better accuracy, but longer computational time
double init_noiseStd = 0.5;
int percentage = 99; //define the percentage of original measurements to be sensed


//First layer basis functions

void get_parameters(double **BASIS)
{

//dctbasis(BASIS,blockResolution);
haarbasis(BASIS,blockResolution,1); //set the scale of the Haar wavelets (1-3) for three stages.. more can be added in the basis.h file but the reconstruction accuracy achieved does not improve significantly to justify the extra computational time.
}

//Specify the usage of Cascade or not

bool use_cascade = true; //set to true if intenting to use cascade, if using only one Discrete Cosine Transform dictionary set to FALSE;


/***************Inside the RVM */

int change_in_ML_threshold = 1; // change depending on the dynamic range of the data. The smaller, the longer it takes to converge


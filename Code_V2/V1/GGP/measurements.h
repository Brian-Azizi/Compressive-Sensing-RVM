/***This header file is used to create a PHI matrix (measurement matrix) and a measurement signal. It uses a predefined basis matrix (ie wavelet, discrete cosine transforms) or a learned dictionary of basis functions. It requires as input the percentage of measurements assumed to be used in the sensing system and then randomly removes the rows/data points from the patchGroundTruth_ and the basis matrix at hand.


input: 
	-patchGroundTruth_ #vector containing the original signal
	-blockResolution   #constant declaring the size of the block's dimension #default=8
	-dictionarySize    #constant declaring the number of available basis functions
	-percentage	   #constant controlling the percentage of original measurements used
	-PSI		   #empty double matrix 
	-scale		   #scale of the wavelet transform to be used
	-intermediate_	   #empty double vector
	-efficientlySensedPatch_ #empty double vector
	-measurements	   #constant returning the number of measurements sensed
	-sensed_	   #empty bool vector
output: 
	- intermediate_	#vector containing the acquired data points by the sensing system
	- PSI		#matrix containing the basis functions in its columns for all data points 
	- efficientlySensedPatch_ #vector containing the efficiently sensed signal patch
	- measurements #constant used to inform the system of the number of data points sensed
	- sensed_      #vector containing whether data points were sensed or not
	***/


void target_and_PSI_generator(vector<double> patchGroundTruth_, int blockResolution, int percentage, double *intermediate_, double *efficientlySensedPatch_, int &measurements, bool *sensed_)
{

/***START the patch target generator***/
	
	int random_number = 0; //store the number that will be generated randomly and based on this, it will be chosen whether it will be sensed
	

	//go through the signal patch and sense randomly with a pre-defined percentage
	//for (int rows=0;rows<blockResolution;rows++)
	//	for (int cols=0;cols<blockResolution;cols++)
	for (int rows=0;rows<blockResolution*blockResolution;rows++)
	{
		random_number = rand() % 100 + 1; //generate numbers in sequence

		if (random_number>percentage)
			{
			intermediate_[measurements] = patchGroundTruth_[rows];
			efficientlySensedPatch_[rows] = patchGroundTruth_[rows];
			sensed_[rows] = true;			
			measurements = measurements + 1;
			}
		else
			{
			sensed_[rows] =false;
			efficientlySensedPatch_[rows] = 0;
			}
	}

	//cout << "Measurements sensed: " << measurements << endl;

/***END of patch target generator***/


	
}

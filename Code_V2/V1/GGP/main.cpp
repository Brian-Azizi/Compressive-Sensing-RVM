using namespace std;
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>
#include <string>

#include <stdlib.h>
#include <math.h>
#include "utilities.h"
#include "fast_updates.h"
#include "basis.h"
#include "dctbasis.h"

#include "settings_file.h"
#include "input.h"
#include "output.h"
#include "measurements.h"

 
/***Definition of dimensionality constants of problem of interest***/
	

int signalSize = signalResolution*signalResolution; //the complete dimensionality of the problem - square signals


int blockSize = blockResolution*blockResolution; //the dimensionality of the signal patches;
	
int dictionarySize = blockSize; //define the size of basis functions' dictionary


	
/***End of constant definitions***/


int main()
{
/*! \file */
	vector<double> signalGroundTruth_(signalSize);

	string filename = "";
	signalGroundTruth_ = input(signalResolution,filename);

	vector<double> join_signal_one(signalSize);
	vector<double> distorted_signal_one(signalSize);
	vector<double> join_signal_two(signalSize);
	vector<double> join_signal_three(signalSize);
	
	
	for (int blockIndexRows=0;blockIndexRows<sqrt(signalSize)/sqrt(blockSize);blockIndexRows++)
		for (int blockIndexCols=0;blockIndexCols<sqrt(signalSize)/sqrt(blockSize);blockIndexCols++)
	{
	cout << "Starting patch " << blockIndexRows*sqrt(signalResolution)/sqrt(blockSize) + blockIndexCols << endl;
	
	vector<double> patchGroundTruth_(blockSize); //container for the storing of original signal patches
	bool *first_sensed_ = new bool[blockSize]; //container to mark whether the data point was sensed
	for (int rows=0;rows<blockSize;rows++)
		first_sensed_[rows] = false;
	double *efficientlySensedPatch_ = new double[blockSize]; //container to store the distorted signal for display purposes


	double **BASIS = new double*[blockSize];
		for (int i=0;i<blockSize;i++)
			BASIS[i] = new double[blockSize];

	get_parameters(BASIS);

	for (int rows=0;rows<sqrt(blockSize);rows++)
		for (int cols=0;cols<sqrt(blockSize);cols++)
			patchGroundTruth_[rows*sqrt(blockSize) + cols] = signalGroundTruth_[(rows+blockIndexRows*sqrt(blockSize))*sqrt(signalSize)+cols+blockIndexCols*sqrt(blockSize)];


	/********* STARTING THE FIRST STAGE of THE RVM ********/

	int measurements = 0;
	double *intermediate_ = new double[blockSize]; //container to store the intermediate values of the signal 
	
	target_and_PSI_generator(patchGroundTruth_, blockResolution, percentage, intermediate_, efficientlySensedPatch_, measurements, first_sensed_);

	double *patchTarget_ = new double[measurements];
	
	for (int i=0;i<measurements;i++)
		patchTarget_[i] = intermediate_[i];

	
	double *estimatedCoefficients_ = new double[dictionarySize];
	for (int rows=0;rows<dictionarySize;rows++)
		estimatedCoefficients_[rows] = 0;

/***START of PHI generator***/

	double **PHI = new double*[measurements]; //define a multi-dimensional array to store the measurement matrix
		for (int i=0;i<measurements;i++)
			PHI[i] = new double[dictionarySize];

	//go through the PSI and remove data points/rows which have not been sensed using the container sensed_
	for (int cols=0;cols<dictionarySize;cols++)
	{
		int rows_kept = 0; //stores the number of data points/rows to keep = measurements
		for (int rows=0;rows<blockSize;rows++)
			if (first_sensed_[rows])
			{
				PHI[rows_kept][cols] = BASIS[rows][cols];	
				rows_kept=rows_kept+1;
			}
	}
	
/***END of PHI generator***/	
	double noiseStd = init_noiseStd;
	
	double *errors = new double[dictionarySize];
	for (int rows=0;rows<dictionarySize;rows++)
		errors[rows] = 0;

	fast_updates(PHI, patchTarget_, estimatedCoefficients_, measurements, dictionarySize, noiseStd,errors,BASIS,use_cascade, change_in_ML_threshold); //set last variable to true only if you use the cascade and this is not the last RVM in the cascade
	

	double *recovered = new double[blockSize];
	

	multiply2D1D(BASIS,estimatedCoefficients_,recovered,blockSize,dictionarySize); 	


	for (int rows=0;rows<blockSize;rows++)
		if (first_sensed_[rows])
			recovered[rows] = patchGroundTruth_[rows];


	for (int rows=0;rows<sqrt(blockSize);rows++)
		for (int cols=0;cols<sqrt(blockSize);cols++)
			distorted_signal_one[(rows+blockIndexRows*sqrt(blockSize))*sqrt(signalSize)+cols+blockIndexCols*sqrt(blockSize)] = efficientlySensedPatch_[rows*int(sqrt(blockSize))+cols];
	

	for (int rows=0;rows<sqrt(blockSize);rows++)
		for (int cols=0;cols<sqrt(blockSize);cols++)
			join_signal_one[(rows+blockIndexRows*sqrt(blockSize))*sqrt(signalSize)+cols+blockIndexCols*sqrt(blockSize)] = recovered[rows*int(sqrt(blockSize))+cols];
	

	for (int i=0;i<blockSize;i++)
		delete[] BASIS[i];
	delete[] BASIS;			
	delete[] intermediate_;
	delete[] estimatedCoefficients_;


	
	for (int i=0;i<measurements;i++)
	delete[] PHI[i];
		delete[] PHI;
	delete[] patchTarget_;
	delete[] efficientlySensedPatch_;


	/***SECOND STAGE OF THE CASCADE that uses the recovered signal excluding the data points where the errors are exactly zero - no basis function was selected***/
if (use_cascade)
	{
	int measurements2 = 0;
	double *intermediate2_ = new double[blockSize]; //container to store the intermediate values of the signal 
	bool *sensed2_ = new bool[blockSize];
	
	for (int rows=0;rows<blockResolution*blockResolution;rows++)
	{
		if (errors[rows]!=0)
			{
			intermediate2_[measurements2] = recovered[rows];
			sensed2_[rows] = true;			
			measurements2 = measurements2 + 1;
			}
		else
			{
			sensed2_[rows] =false;
			}
	}

	double *patchTarget2_ = new double[measurements2];
	
	for (int i=0;i<measurements2;i++)
		patchTarget2_[i] = intermediate2_[i];

	
	double *estimatedCoefficients2_ = new double[dictionarySize];
	for (int rows=0;rows<dictionarySize;rows++)
		estimatedCoefficients2_[rows] = 0;

/***START of PHI generator***/


	double **BASIS2 = new double*[blockSize];
		for (int i=0;i<blockSize;i++)
			BASIS2[i] = new double[blockSize];



	//dctbasis(BASIS,blockResolution);
	haarbasis(BASIS2,blockResolution,2);


	double **PHI2 = new double*[measurements2]; //define a multi-dimensional array to store the measurement matrix
		for (int i=0;i<measurements2;i++)
			PHI2[i] = new double[dictionarySize];

	//go through the PSI and remove data points/rows which have not been sensed using the container sensed_
	for (int cols=0;cols<dictionarySize;cols++)
	{
		int rows_kept = 0; //stores the number of data points/rows to keep = measurements
		for (int rows=0;rows<blockSize;rows++)
			if (sensed2_[rows])
			{
				PHI2[rows_kept][cols] = BASIS2[rows][cols];	
				rows_kept=rows_kept+1;
			}
	}
	
/***END of PHI generator***/	
	
	double *errors2 = new double[dictionarySize];
	for (int rows=0;rows<dictionarySize;rows++)
		errors2[rows] = 0;

	fast_updates(PHI2, patchTarget2_, estimatedCoefficients2_, measurements2, dictionarySize, noiseStd,errors2,BASIS2,use_cascade, change_in_ML_threshold); //set last variable to true only if you use the cascade and this is not the last RVM in the cascade
	

	double *recovered2 = new double[blockSize];
	

	multiply2D1D(BASIS2,estimatedCoefficients2_,recovered2,blockSize,dictionarySize);


	for (int rows=0;rows<blockSize;rows++)
		if (first_sensed_[rows])
			recovered2[rows] = patchGroundTruth_[rows];

	for (int rows=0;rows<sqrt(blockSize);rows++)
		for (int cols=0;cols<sqrt(blockSize);cols++)
			join_signal_two[(rows+blockIndexRows*sqrt(blockSize))*sqrt(signalSize)+cols+blockIndexCols*sqrt(blockSize)] = recovered2[rows*int(sqrt(blockSize))+cols];
					
		
	for (int i=0;i<blockSize;i++)
		delete[] BASIS2[i];
	delete[] BASIS2;

	delete[] intermediate2_;

	delete[] estimatedCoefficients2_;

	delete[] sensed2_;
	for (int i=0;i<measurements2;i++)
	delete[] PHI2[i];
		delete[] PHI2;
	delete[] patchTarget2_;
	delete[] recovered;
	delete[] errors;	
			
/***THIRD STAGE OF THE CASCADE that uses the recovered signal excluding the data points where the errors are exactly zero - no basis function was selected***/
	
	int measurements3 = 0;
	double *intermediate3_ = new double[blockSize]; //container to store the intermediate values of the signal 
	bool *sensed3_ = new bool[blockSize];
	
	for (int rows=0;rows<blockResolution*blockResolution;rows++)
	{
		if (errors2[rows]!=0)
			{
			intermediate3_[measurements3] = recovered2[rows];
			sensed3_[rows] = true;			
			measurements3 = measurements3 + 1;
			}
		else
			{
			sensed3_[rows] =false;
			}
	}

	double *patchTarget3_ = new double[measurements3];
	
	for (int i=0;i<measurements3;i++)
		patchTarget3_[i] = intermediate3_[i];

	
	double *estimatedCoefficients3_ = new double[dictionarySize];
	for (int rows=0;rows<dictionarySize;rows++)
		estimatedCoefficients3_[rows] = 0;

/***START of PHI generator***/


	double **BASIS3 = new double*[blockSize];
		for (int i=0;i<blockSize;i++)
			BASIS3[i] = new double[blockSize];



	haarbasis(BASIS3,blockResolution,3);


	double **PHI3 = new double*[measurements3]; //define a multi-dimensional array to store the measurement matrix
		for (int i=0;i<measurements3;i++)
			PHI3[i] = new double[dictionarySize];

	//go through the PSI and remove data points/rows which have not been sensed using the container sensed_
	for (int cols=0;cols<dictionarySize;cols++)
	{
		int rows_kept = 0; //stores the number of data points/rows to keep = measurements
		for (int rows=0;rows<blockSize;rows++)
			if (sensed3_[rows])
			{
				PHI3[rows_kept][cols] = BASIS3[rows][cols];	
				rows_kept=rows_kept+1;
			}
	}
	
/***END of PHI generator***/	
	
	double *errors3 = new double[dictionarySize];
	for (int rows=0;rows<dictionarySize;rows++)
		errors3[rows] = 0;

	fast_updates(PHI3, patchTarget3_, estimatedCoefficients3_, measurements3, dictionarySize, noiseStd,errors3,BASIS3, false, change_in_ML_threshold); //set last variable to true only if you use the cascade and this is not the last RVM in the cascade
	

	double *recovered3 = new double[blockSize];
	
	multiply2D1D(BASIS3,estimatedCoefficients3_,recovered3,blockSize,dictionarySize);


	for (int rows=0;rows<blockSize;rows++)
		if (first_sensed_[rows])
			recovered3[rows] = patchGroundTruth_[rows];
	
	for (int rows=0;rows<sqrt(blockSize);rows++)
		for (int cols=0;cols<sqrt(blockSize);cols++)
			join_signal_three[(rows+blockIndexRows*sqrt(blockSize))*sqrt(signalSize)+cols+blockIndexCols*sqrt(blockSize)] = recovered3[rows*int(sqrt(blockSize))+cols];
	
	
			for (int i=0;i<blockSize;i++)
				delete[] BASIS3[i];
			delete[] BASIS3;
			
			delete[] intermediate3_;

			delete[] estimatedCoefficients3_;

			delete[] recovered3;
			delete[] sensed3_;
			for (int i=0;i<measurements3;i++)
			delete[] PHI3[i];
				delete[] PHI3;
			delete[] patchTarget3_;
			delete[] errors3;				
			delete[] errors2;
			delete[] recovered2;	
			
					
}

		delete[] first_sensed_;
			
		cout << "Finishing patch " << blockIndexRows*sqrt(signalSize)/sqrt(blockSize) + blockIndexCols << " / " << signalSize/blockSize-1  << endl;			

	}
	
		output(join_signal_one,1,signalResolution,filename);	
		output(distorted_signal_one,0,signalResolution,filename);
		
		if (use_cascade)
		{
			output(join_signal_two,2,signalResolution,filename); //output to files if the system uses the cascade. Comment out or add more if using less/more stages in the cascade.
			output(join_signal_three,3,signalResolution,filename);
		}	
	

	return 0;
}

/***This header file calculates the PSI matrix, ie the basis matrix of our sensing system given the scale required. Scale can vary between 1 to 3 for Haar wavelets. ***/

#include "kronecker.h"
void haarbasis(double **W_transpose,int blockResolution,int scale)
{

	double **W = new double*[blockResolution*blockResolution];
	for (int i=0;i<blockResolution*blockResolution;++i)
		W[i] = new double[(blockResolution*blockResolution)];

	double **H = new double*[blockResolution>>1];
	for (int i=0;i<blockResolution>>1;++i)
		H[i] = new double[blockResolution];

	double **G = new double*[blockResolution>>1];
	for (int i=0;i<blockResolution>>1;++i)
		G[i] = new double[blockResolution];

	double **HH = new double*[(blockResolution>>1)*(blockResolution>>1)];
	for (int i=0;i<(blockResolution>>1)*(blockResolution>>1);++i)
		HH[i] = new double[(blockResolution*blockResolution)];

	double **HG = new double*[(blockResolution>>1)*(blockResolution>>1)];
	for (int i=0;i<(blockResolution>>1)*(blockResolution>>1);++i)
		HG[i] = new double[(blockResolution*blockResolution)];

	double **GH = new double*[(blockResolution>>1)*(blockResolution>>1)];
	for (int i=0;i<(blockResolution>>1)*(blockResolution>>1);++i)
		GH[i] = new double[(blockResolution*blockResolution)];

	double **GG = new double*[(blockResolution>>1)*(blockResolution>>1)];
	for (int i=0;i<(blockResolution>>1)*(blockResolution>>1);++i)
		GG[i] = new double[(blockResolution*blockResolution)];

	for (int i=0;i<blockResolution>>1;i++)
		for (int j=0;j<blockResolution;j++)
		{
	
			if( (j==2*i) || (j==2*i+1))		
			{
				H[i][j] = 1/sqrt(2);			
			}
			else
			H[i][j] = 0;
		}

	for (int i=0;i<blockResolution>>1;i++)
		for (int j=0;j<blockResolution;j++)
		{	
			if (j==2*i) 
			{
				G[i][j] = 1/sqrt(2);
			
			}		
			else if (j==2*i+1)
			{
				G[i][j] =-1/sqrt(2);
			
			}
			else
			G[i][j] = 0;
		
		}

	kronecker(H,H,HH,blockResolution>>1,blockResolution,blockResolution>>1,blockResolution);
	kronecker(H,G,HG,blockResolution>>1,blockResolution,blockResolution>>1,blockResolution);
	kronecker(G,H,GH,blockResolution>>1,blockResolution,blockResolution>>1,blockResolution);
	kronecker(G,G,GG,blockResolution>>1,blockResolution,blockResolution>>1,blockResolution);

	//second scale

		if ((scale==2) || (scale==3))
		{
				double **H_2 = new double*[blockResolution>>2];
			for (int i=0;i<blockResolution>>2;++i)
				H_2[i] = new double[blockResolution>>1];

			double **G_2 = new double*[blockResolution>>2];
			for (int i=0;i<blockResolution>>2;++i)
				G_2[i] = new double[blockResolution>>1];

			double **HH_2 = new double*[(blockResolution>>2)*(blockResolution>>2)];
			for (int i=0;i<(blockResolution>>2)*(blockResolution>>2);++i)
				HH_2[i] = new double[(blockResolution*blockResolution)];

			double **HG_2 = new double*[(blockResolution>>2)*(blockResolution>>2)];
			for (int i=0;i<(blockResolution>>2)*(blockResolution>>2);++i)
				HG_2[i] = new double[(blockResolution*blockResolution)];

			double **GH_2 = new double*[(blockResolution>>2)*(blockResolution>>2)];
			for (int i=0;i<(blockResolution>>2)*(blockResolution>>2);++i)
				GH_2[i] = new double[(blockResolution*blockResolution)];

			double **GG_2 = new double*[(blockResolution>>2)*(blockResolution>>2)];
			for (int i=0;i<(blockResolution>>2)*(blockResolution>>2);++i)
				GG_2[i] = new double[(blockResolution*blockResolution)];


			for (int i=0;i<blockResolution>>2;i++)
				for (int j=0;j<blockResolution>>1;j++)
				{
	
					if( (j==2*i) || (j==2*i+1))		
					{
						H_2[i][j] = 1/sqrt(2);			
					}
					else
					H_2[i][j] = 0;
				}

			for (int i=0;i<blockResolution>>2;i++)
				for (int j=0;j<blockResolution>>1;j++)
				{	
					if (j==2*i) 
					{
						G_2[i][j] = 1/sqrt(2);
			
					}		
					else if (j==2*i+1)
					{
						G_2[i][j] =-1/sqrt(2);
			
					}
					else
					G_2[i][j] = 0;
		
				}
			
			double **tempHH = new double*[(blockResolution>>2)];
				for (int i=0;i<(blockResolution>>2);++i)
					tempHH[i] = new double[blockResolution];

			double **tempGH = new double*[(blockResolution>>2)];
				for (int i=0;i<(blockResolution>>2);++i)
					tempGH[i] = new double[blockResolution];
	
			multiply2D2D(H_2,H,tempHH,blockResolution>>2,blockResolution,blockResolution>>1);
			multiply2D2D(G_2,H,tempGH,blockResolution>>2,blockResolution,blockResolution>>1);
	
			kronecker(tempHH,tempHH,HH_2,blockResolution>>2,blockResolution,blockResolution>>2,blockResolution);
			kronecker(tempHH,tempGH,HG_2,blockResolution>>2,blockResolution,blockResolution>>2,blockResolution);
			kronecker(tempGH,tempHH,GH_2,blockResolution>>2,blockResolution,blockResolution>>2,blockResolution);
			kronecker(tempGH,tempGH,GG_2,blockResolution>>2,blockResolution,blockResolution>>2,blockResolution);

			if (scale==3)
			{
				double **H_3 = new double*[blockResolution>>3];
				for (int i=0;i<blockResolution>>3;++i)
					H_3[i] = new double[blockResolution>>2];

				double **G_3 = new double*[blockResolution>>3];
				for (int i=0;i<blockResolution>>3;++i)
					G_3[i] = new double[blockResolution>>2];

				double **HH_3 = new double*[(blockResolution>>3)*(blockResolution>>3)];
				for (int i=0;i<(blockResolution>>3)*(blockResolution>>3);++i)
					HH_3[i] = new double[(blockResolution*blockResolution)];

				double **HG_3 = new double*[(blockResolution>>3)*(blockResolution>>3)];
				for (int i=0;i<(blockResolution>>3)*(blockResolution>>3);++i)
					HG_3[i] = new double[(blockResolution*blockResolution)];

				double **GH_3 = new double*[(blockResolution>>3)*(blockResolution>>3)];
				for (int i=0;i<(blockResolution>>3)*(blockResolution>>3);++i)
					GH_3[i] = new double[(blockResolution*blockResolution)];

				double **GG_3 = new double*[(blockResolution>>3)*(blockResolution>>3)];
				for (int i=0;i<(blockResolution>>3)*(blockResolution>>3);++i)
					GG_3[i] = new double[(blockResolution*blockResolution)];

				for (int i=0;i<blockResolution>>3;i++)
					for (int j=0;j<blockResolution>>2;j++)
					{
	
						if( (j==2*i) || (j==2*i+1))		
						{
							H_3[i][j] = 1/sqrt(2);			
						}
						else
						H_3[i][j] = 0;
					}

				for (int i=0;i<blockResolution>>3;i++)
					for (int j=0;j<blockResolution>>2;j++)
					{	
						if (j==2*i) 
						{
							G_3[i][j] = 1/sqrt(2);
			
						}		
						else if (j==2*i+1)
						{
							G_3[i][j] =-1/sqrt(2);
			
						}
						else
						G_3[i][j] = 0;
		
					}
					
					
				double **tempHH_2 = new double*[(blockResolution>>3)];
				for (int i=0;i<(blockResolution>>3);++i)
					tempHH_2[i] = new double[blockResolution>>1];

				double **tempGH_2 = new double*[(blockResolution>>3)];
				for (int i=0;i<(blockResolution>>3);++i)
					tempGH_2[i] = new double[blockResolution>>1];

				double **tempHH_3 = new double*[(blockResolution>>3)];
				for (int i=0;i<(blockResolution>>3);++i)
					tempHH_3[i] = new double[blockResolution];

				double **tempHG_3 = new double*[(blockResolution>>3)];
				for (int i=0;i<(blockResolution>>3);++i)
					tempHG_3[i] = new double[blockResolution];

				double **tempGH_3 = new double*[(blockResolution>>3)];
				for (int i=0;i<(blockResolution>>3);++i)
					tempGH_3[i] = new double[blockResolution];

				double **tempGG_3 = new double*[(blockResolution>>3)];
				for (int i=0;i<(blockResolution>>3);++i)
					tempGG_3[i] = new double[blockResolution];
			
			
				multiply2D2D(H_3,H_2,tempHH_2,blockResolution>>3,blockResolution>>1,blockResolution>>2);
				multiply2D2D(G_3,H_2,tempGH_2,blockResolution>>3,blockResolution>>1,blockResolution>>2);

				multiply2D2D(tempHH_2, H,tempHH_3,blockResolution>>3,blockResolution,blockResolution>>1);
				multiply2D2D(tempGH_2,H,tempGH_3,blockResolution>>3,blockResolution,blockResolution>>1);
			
				kronecker(tempHH_3,tempHH_3,HH_3,blockResolution>>3,blockResolution,blockResolution>>3,blockResolution);
				kronecker(tempHH_3,tempGH_3,HG_3,blockResolution>>3,blockResolution,blockResolution>>3,blockResolution);
				kronecker(tempGH_3,tempHH_3,GH_3,blockResolution>>3,blockResolution,blockResolution>>3,blockResolution);
				kronecker(tempGH_3,tempGH_3,GG_3,blockResolution>>3,blockResolution,blockResolution>>3,blockResolution);
			
			
				for (int i=0;i<(blockResolution>>3)*(blockResolution>>3);i++)
					for (int j=0;j<blockResolution*blockResolution;j++)
					{
						HH_2[i][j] = HH_3[i][j];
						HH_2[i+(blockResolution>>3)*(blockResolution>>3)][j] = HG_3[i][j];
						HH_2[i+2*(blockResolution>>3)*(blockResolution>>3)][j] = GH_3[i][j];
						HH_2[i+3*(blockResolution>>3)*(blockResolution>>3)][j] = GG_3[i][j];	
					}

			
				for (int i=0;i<blockResolution>>3;++i)
					delete[] H_3[i];
						delete []H_3;

				for (int i=0;i<blockResolution>>3;++i)
					delete[] G_3[i];
				delete[] G_3;

				for (int i=0;i<(blockResolution>>3)*(blockResolution>>3);++i)
					delete[] HG_3[i];
				delete[] HG_3;

				for (int i=0;i<(blockResolution>>3)*(blockResolution>>3);++i)
					delete[] HH_3[i];
				delete[] HH_3;

				for (int i=0;i<(blockResolution>>3)*(blockResolution>>3);++i)
					delete[] GH_3[i];
				delete[] GH_3;

				for (int i=0;i<(blockResolution>>3)*(blockResolution>>3);++i)
					delete[] GG_3[i];
				delete[] GG_3;
			
				for (int i=0;i<blockResolution>>3;++i)
					delete[] tempHH_2[i];
				delete[] tempHH_2;
		
				for (int i=0;i<blockResolution>>3;++i)
					delete[] tempGH_2[i];
				delete[] tempGH_2;
				
				for (int i=0;i<blockResolution>>3;++i)
					delete[] tempHH_3[i];
				delete[] tempHH_3;
			
				for (int i=0;i<blockResolution>>3;++i)
					delete[] tempHG_3[i];
				delete[] tempHG_3;
				
				for (int i=0;i<blockResolution>>3;++i)
					delete[] tempGH_3[i];
				delete[] tempGH_3;
				for (int i=0;i<blockResolution>>3;++i)
					delete[] tempGG_3[i];
				delete[] tempGG_3;
				
			}

		
			for (int i=0;i<(blockResolution>>2)*(blockResolution>>2);i++)
				for (int j=0;j<blockResolution*blockResolution;j++)
				{
					HH[i][j] = HH_2[i][j];
					HH[i+(blockResolution>>2)*(blockResolution>>2)][j] = HG_2[i][j];
					HH[i+2*(blockResolution>>2)*(blockResolution>>2)][j] = GH_2[i][j];
					HH[i+3*(blockResolution>>2)*(blockResolution>>2)][j] = GG_2[i][j];	
				}

			for (int i=0;i<blockResolution>>2;++i)
				delete[] H_2[i];
					delete []H_2;

			for (int i=0;i<blockResolution>>2;++i)
				delete[] G_2[i];
			delete[] G_2;

			for (int i=0;i<(blockResolution>>2)*(blockResolution>>2);++i)
				delete[] HG_2[i];
			delete[] HG_2;

			for (int i=0;i<(blockResolution>>2)*(blockResolution>>2);++i)
				delete[] HH_2[i];
			delete[] HH_2;

			for (int i=0;i<(blockResolution>>2)*(blockResolution>>2);++i)
				delete[] GH_2[i];
			delete[] GH_2;

			for (int i=0;i<(blockResolution>>2)*(blockResolution>>2);++i)
				delete[] GG_2[i];
			delete[] GG_2;
		
			for (int i=0;i<blockResolution>>2;++i)
				delete[] tempHH[i];
			delete[] tempHH;
		
			for (int i=0;i<blockResolution>>2;++i)
				delete[] tempGH[i];
			delete[] tempGH;

		}




	for (int i=0;i<(blockResolution>>1)*(blockResolution>>1);i++)
		for (int j=0;j<blockResolution*blockResolution;j++)
		{
			W[i][j] = HH[i][j];
			W[i+(blockResolution>>1)*(blockResolution>>1)][j] = HG[i][j];
			W[i+2*(blockResolution>>1)*(blockResolution>>1)][j] = GH[i][j];
			W[i+3*(blockResolution>>1)*(blockResolution>>1)][j] = GG[i][j];
		}


	transpose2D(W,W_transpose,blockResolution*blockResolution,blockResolution*blockResolution);

	//clean up when done

	for (int i=0;i<blockResolution*blockResolution;++i)
		delete[] W[i];
	delete []W;

	for (int i=0;i<blockResolution>>1;++i)
		delete[] H[i];
	delete []H;

	for (int i=0;i<blockResolution>>1;++i)
		delete[] G[i];
	delete[] G;

	for (int i=0;i<(blockResolution>>1)*(blockResolution>>1);++i)
		delete[] HG[i];
	delete[] HG;

	for (int i=0;i<(blockResolution>>1)*(blockResolution>>1);++i)
		delete[] HH[i];
	delete[] HH;

	for (int i=0;i<(blockResolution>>1)*(blockResolution>>1);++i)
		delete[] GH[i];
	delete[] GH;

	for (int i=0;i<(blockResolution>>1)*(blockResolution>>1);++i)
		delete[] GG[i];
	delete[] GG;












}






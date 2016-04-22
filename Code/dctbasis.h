//Discrete Cosine Transform
void dctbasis(double **BASIS,int blockResolution)
{

	int blockSize = blockResolution*blockResolution;
	
			
	double pi = atan( 1.0 ) * 4.0;

	


	for (int l=0;l<blockResolution;l++)
	for (int k=0;k<blockResolution;k++)
	{
		double *phi = new double[blockSize];
		for (int i=0;i<blockSize;i++)
			phi[i] = 0;
			
		for (int x1=0;x1<blockResolution;x1++)
			for (int x2=0;x2<blockResolution;x2++)
			{
		
				phi[x2+x1*blockResolution] =phi[x2+x1*blockResolution] + cos( (2*x1 +1)*l*pi/(2*blockResolution))*cos( (2*x2 +1)*k*pi/(2*blockResolution));
			}
			   	
		 if ((l==0) || (k==0))
				for (int i=0;i<blockSize;i++)
					phi[i] = phi[i]/2;
		
		 for (int rows=0;rows<blockSize;rows++)
		 	BASIS[rows][k+l*blockResolution] = phi[rows];     
		 
		 delete[] phi;   
	}
				
	
	
	}

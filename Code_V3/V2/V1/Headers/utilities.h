void multiply2D2D(double **A, double **B, double **C,int Arows,int Bcols,int Acols )
{

	for (int i=0;i<Arows;i++)
	{
		for (int j=0;j<Bcols;j++)
		{
			C[i][j] =0.0000;
			for (int k=0;k<Acols;k++)
				C[i][j] = C[i][j] + A[i][k]*B[k][j];
		}  
	}
}

double calcVariance(double *patchTarget_, int N)
	{
		double sum = 0;
		double m =0;
		double variance =0;
	
		for (int i=0;i<N;i++)
			{
			sum = sum + patchTarget_[i];
			}
		
		m = sum/N;
		
		for (int i=0;i<N;i++)
			{
				variance = variance + (patchTarget_[i] - m)*(patchTarget_[i] - m);
				
			}

		variance = variance / N;
		
		return variance;
	}



double multiply1D1Dnum(double *A, double *B,int length)
{
	double C=0;

	for (int rows=0;rows<length;rows++)
			C = C + A[rows]*B[rows];

	return C;
}

void multiply2D1D(double **A, double *B, double *C,int rows,int cols)
{


	for (int i=0;i<rows;i++)
	{
		C[i] = 0;
		for (int j=0;j<cols;j++)
		{
			C[i] = C[i] + A[i][j]*B[j];
		}  
	}

}

void multiply1D2D(double *A, double **B,double *C,int rows, int cols)
{

	for (int i=0;i<rows;i++)
	{
		C[i] = 0;
		for (int j=0;j<cols;j++)
		{
			C[i] = C[i] + B[j][i]*A[j];
		}  
	}


}

void multiply1D1Dmat(double *A, double *B, double **C,int row_size,int col_size)
{

	for (int rows=0;rows<row_size;rows++)
		for (int cols=0;cols<col_size;cols++)
			C[cols][rows] = A[rows]*B[cols];

}

void transpose2D(double **A,double **B,int rows,int cols)
{
	for (int i=0;i<rows;i++)
		for (int j=0;j<cols;j++)
			B[i][j] = A[j][i];
}


double norm2vectors(double *A, double *B,int length)
{
	double C=0;

	for (int rows=0;rows<length;rows++)
		C = C + ((A[rows] - B[rows])*(A[rows] - B[rows]));

	return sqrt(C);
}



double norm1vector(double *A,int length)
{
	double C=0;

	for (int rows=0;rows<length;rows++)
		C = C + (A[rows]*A[rows]);

	return sqrt(C);


}

void bubble_sort(int *A,int *loc,int numLength)
{

      int i, j, flag = 1;   
      int temp;             

      for(i = 1; (i <= numLength) && flag; i++)
     {
          flag = 0;

          for (j=0; j < (numLength -1); j++)
         {

               if (A[j+1] > A[j])     
              { 
                    temp = A[j];      
                    A[j] = A[j+1];
                    A[j+1] = temp;
                    flag = 1;     
		    temp = loc[j];
		    loc[j] = loc[j+1];
		    loc[j+1] = temp;

               }
          }
     }

}

// calculate the cofactor of element
int GetMinor(double **src, double **dest, int row, int col, int order)
{
    // indicate which col and row is being copied to dest
    int colCount=0,rowCount=0;
 
    for(int i = 0; i < order; i++ )
    {
        if( i != row )
        {
            colCount = 0;
            for(int j = 0; j < order; j++ )
            {
                // when j is not the element
                if( j != col )
                {
                    dest[rowCount][colCount] = src[i][j];
                    colCount++;
                }
            }
            rowCount++;
        }
    }
 
    return 1;
}

// Calculate the determinant recursively.
double CalcDeterminant( double **mat, int order)
{
    // order must be >= 0
    // stop the recursion when matrix is a single element
    if( order == 1 )
        return mat[0][0];
 
    // the determinant value
    double det = 0;
 
    // allocate the cofactor matrix
    double **minor;
    minor = new double*[order-1];
    for(int i=0;i<order-1;i++)
        minor[i] = new double[order-1];
 
    for(int i = 0; i < order; i++ )
    {

        GetMinor( mat, minor, 0, i , order);

 
        det += (i%2==1?-1.0:1.0) * mat[0][i] * CalcDeterminant(minor,order-1);

    }
 
    for(int i=0;i<order-1;i++)
        delete [] minor[i];
    delete [] minor;
 
    return det;
}

void MatrixInversion(double **A, int order, double **Y)
{
    // get the determinant of a
    double det = 1.0/CalcDeterminant(A,order);
 
    // memory allocation
    double *temp = new double[(order-1)*(order-1)];
    double **minor = new double*[order-1];
    for(int i=0;i<order-1;i++)
        minor[i] = temp+(i*(order-1));
 
    for(int j=0;j<order;j++)
    {
        for(int i=0;i<order;i++)
        {
            // get the co-factor (matrix) of A
            GetMinor(A,minor,j,i,order);
            Y[i][j] = det*CalcDeterminant(minor,order-1);
            if( (i+j)%2 == 1)
                Y[i][j] = -Y[i][j];
        }
    }
 
    delete [] temp;
    delete [] minor;
}

void cholesky(double **orig, double **chol, int n)	
{
   int i, j, k, l;
  int ofs=0;
  for (int rows=0;rows<n;rows++)
  	for (int cols=0;cols<n;cols++)
  		chol[rows][cols] = 0;

   for (i=ofs; i<n+ofs; i++) {
      chol[i][i] = orig[i][i];
      for (k=ofs; k<i; k++)
	 chol[i][i] -= chol[k][i]*chol[k][i];
     
      chol[i][i] = sqrt(chol[i][i]);


      for (j=i+1; j<n+ofs; j++) {
	 chol[i][j] = orig[i][j];
	 for (k=ofs; k<i; k++)
	    chol[i][j] -= chol[k][i]*chol[k][j];
	 chol[i][j] /= chol[i][i];
      }
   }

   
}

void inversed(double **in, int n, double **out)
{

	double *R = new double[n*n];
	double *Rinv = new double[n*n];

	for (int rows=0;rows<n;rows++)
		for (int cols=0;cols<n;cols++)
			R[rows*n + cols] = in[rows][cols];
		

	int i, j, k;

	for(i=0; i<n*n; i++)
	    Rinv[i] = 0.0;

	for(j=0; j<n; j++)
	{
	    Rinv[n*j+j] = 1./R[n*j+j];
	    
	    for(i=0; i<(j); i++)
	    {
		for(k=0; k<(j); k++)
		{
		    Rinv[n*i+j] += Rinv[n*i+k]*R[n*k+j];
		}
	    }
	    
	    for(k=0; k<(j); k++)
	    {
		Rinv[n*k+j] /= -R[n*j+j];
	    }
	}

	for (int rows=0;rows<n;rows++)
		for (int cols=0;cols<n;cols++)
			out[rows][cols] = Rinv[rows*n + cols];


	delete[] R;
	delete[] Rinv;

}


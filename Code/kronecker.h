
/*Computes the Kronecker Product of two matrices and stores it as matrix C*/

void kronecker(double **A ,double **B,double **C,int ma,int na,int mb,int nb)
{

	int i,j,k,p;
    

	for( i=0; i<ma; i++)
        	for( j=0; j<na; j++)
			for(k=0; k<mb; k++)
				for(p=0; p<nb; p++)
					C[k + i*mb][p + j*nb] = A[i][j]*B[k][p];
                                                             
}



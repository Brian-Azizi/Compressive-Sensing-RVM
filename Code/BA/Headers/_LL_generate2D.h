template<class T>
void _LL_generate(T **LL, int scale, int currentScale,\
		    int h, int w, T **rPreFactor, T **cPreFactor)
{
    int const dimDivider = pow(2,scale-1);
    int const currentDimDivider = pow(2,currentScale);
    int const smallH = h/currentDimDivider;
    int const smallW = w/currentDimDivider;
    int const smallestH = h/dimDivider;
    int const smallestW = w/dimDivider;
    
    if (currentScale == scale) {
	int full_dim1 = w*h;
	int full_dim2 = smallestW * smallestH / 4;
	T **tempFull = new T*[full_dim1];
	for (int i = 0; i < full_dim1; ++i) {
	    tempFull[i] = new T[full_dim2];
	}

	kronecker(cPreFactor,rPreFactor,LL, w,smallestW/2, h, smallestH/2);
	for (int i = 0; i < full_dim1; ++i) {
	    delete[] tempFull[i];
	}
	delete[] tempFull;

	return;
    }

    if (currentScale < scale) {
	
	/* 1) RHS factors:
	   new_cPhiT  and  new_cPsiT (smallW x smallW/2),
	   new_rPhiT  and  new_rPsiT (smallH x smallH/2) */
	T **new_cPhiT = new T*[smallW];
	T **new_cPsiT = new T*[smallW];
	T **new_rPhiT = new T*[smallH];
	T **new_rPsiT = new T*[smallH];
	for (int i = 0; i < smallW; ++i) {
	    new_cPhiT[i] = new T[smallW/2];
	    new_cPsiT[i] = new T[smallW/2];
	    for (int j = 0; j < smallW/2; ++j) {
		if (i == 2*j) {
		    new_cPhiT[i][j] = 1/sqrt(2);
		    new_cPsiT[i][j] = 1/sqrt(2);
		} else if (i == 2*j + 1) {
		    new_cPhiT[i][j] = 1/sqrt(2);
		    new_cPsiT[i][j] = -1/sqrt(2);
		} else {
		    new_cPhiT[i][j] = 0;
		    new_cPsiT[i][j] = 0;
		}
	    }
	}
	for (int i = 0; i < smallH; ++i) {
	    new_rPhiT[i] = new T[smallH/2];
	    new_rPsiT[i] = new T[smallH/2];
	    for (int j = 0; j < smallH/2; ++j) {
		if (i == 2*j) {
		    new_rPhiT[i][j] = 1/sqrt(2);
		    new_rPsiT[i][j] = 1/sqrt(2);
		} else if (i == 2*j + 1) {
		    new_rPhiT[i][j] = 1/sqrt(2);
		    new_rPsiT[i][j] = -1/sqrt(2);
		} else {
		    new_rPhiT[i][j] = 0;
		    new_rPsiT[i][j] = 0;
		}	
	    }
	}
	
	/* 2) LHS factors: 
	   cPreFactor (w x smallW),
	   rPreFactor (h x smallH),
	   These are passed as arguments into the function */
	
	/* 3) Matrix products:
	   cPreFactor*new_cPhiT, cPreFactor*new_cPsiT
	   rPreFactor*new_rPhiT, rPreFactor*new_rPsiT */
	T **cPre_Phi = new T*[w];
	T **cPre_Psi = new T*[w];
	T **rPre_Phi = new T*[h];
	T **rPre_Psi = new T*[h];
	for (int i = 0; i < w; ++i) {
	    cPre_Phi[i] = new T[smallW/2];
	    cPre_Psi[i] = new T[smallW/2];
	}
	for (int i = 0; i < h; ++i) {
	    rPre_Phi[i] = new T[smallH/2];
	    rPre_Psi[i] = new T[smallH/2];
	}
	multiply2D2D(cPreFactor,new_cPhiT,cPre_Phi,w,smallW/2,smallW);
	multiply2D2D(cPreFactor,new_cPsiT,cPre_Psi,w,smallW/2,smallW);
	multiply2D2D(rPreFactor,new_rPhiT,rPre_Phi,h,smallH/2,smallH);
	multiply2D2D(rPreFactor,new_rPsiT,rPre_Psi,h,smallH/2,smallH);

	/* 4) Form Kronecker products and save in LLL
	   tempFull - save kronecker of C matrix and R matrix 
	     size is (wh)x(smallW*smallH/4)
	   new_LL - like temp but we'll use it only for new LL part */
	int full_dim1 = w*h;
	int full_dim2 = smallW * smallH / 4;
	T **tempFull = new T*[full_dim1];
	T **new_LL = new T*[full_dim1];
	for (int i = 0; i < full_dim1; ++i) {
	    tempFull[i] = new T[full_dim2];
	    new_LL[i] = new T[full_dim2];
	}

	// new LH
	kronecker(cPre_Phi, rPre_Psi, tempFull, w, smallW/2, h, smallH/2);
	for (int i = 0; i < full_dim1; ++i) {
	    for (int j = 0; j < full_dim2; ++j) {
		LL[i][full_dim2 + j] = tempFull[i][j];
	    }
	}

	// new HL
	kronecker(cPre_Psi, rPre_Phi, tempFull, w, smallW/2, h, smallH/2);
	for (int i = 0; i < full_dim1; ++i) {
	    for (int j = 0; j < full_dim2; ++j) {
		LL[i][2*full_dim2 + j] = tempFull[i][j];
	    }
	}

	// new HH
	kronecker(cPre_Psi, rPre_Psi, tempFull, w, smallW/2, h, smallH/2);
	for (int i = 0; i < full_dim1; ++i) {
	    for (int j = 0; j < full_dim2; ++j) {
		LL[i][3*full_dim2 + j] = tempFull[i][j];
	    }
	}
	
	/* 5) form new_LLL */
	_LL_generate(new_LL, scale, currentScale + 1, h, w, rPre_Phi, cPre_Phi);
	for (int i = 0; i < full_dim1; ++i) {
	    for (int j = 0; j < full_dim2; ++j) {
		LL[i][j] = new_LL[i][j];
	    }
	}
	
	for (int i = 0; i < smallW; ++i) {
	    delete[] new_cPhiT[i];
	    delete[] new_cPsiT[i];
	}
	delete[] new_cPhiT;
	delete[] new_cPsiT;
	for (int i = 0; i < smallH; ++i) {
	    delete[] new_rPhiT[i];
	    delete[] new_rPsiT[i];
	}
	delete[] new_rPhiT;
	delete[] new_rPsiT;
	for (int i = 0; i < w; ++i) {
	    delete[] cPre_Phi[i];
	    delete[] cPre_Psi[i];
	}
	delete[] cPre_Phi;
	delete[] cPre_Psi;
	for (int i = 0; i < h; ++i) {
	    delete[] rPre_Phi[i];
	    delete[] rPre_Psi[i];
	}
	delete[] rPre_Phi;
	delete[] rPre_Psi;
	for (int i = 0; i < full_dim1; ++i) {
	    delete[] tempFull[i];
	    delete[] new_LL[i];
	}
	delete[] tempFull;
	delete[] new_LL;
    }
}

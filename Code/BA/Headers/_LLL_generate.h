template<class T>
void _LLL_generate(T **LLL, int scale, int currentScale, int h, int w, int f, T **rPreFactor, T **cPreFactor, T **sPreFactor)
{
    int const dimDivider = pow(2,scale-1);
    int const currentDimDivider = pow(2,currentScale);
    int const smallH = h/currentDimDivider;
    int const smallW = w/currentDimDivider;
    int const smallF = f/currentDimDivider;
    int const smallestH = h/dimDivider;
    int const smallestW = w/dimDivider;
    int const smallestF = f/dimDivider;
    
    if (currentScale == scale) {
	int CR_dim1 = w*h;
	int CR_dim2 = smallestW * smallestH / 4;
	int full_dim1 = f*w*h;
	int full_dim2 = smallestF * smallestW * smallestH / 8;
	T **tempCR = new T*[CR_dim1];
	T **tempFull = new T*[full_dim1];
	for (int i = 0; i < CR_dim1; ++i) {
	    tempCR[i] = new T[CR_dim2];
	}
	for (int i = 0; i < full_dim1; ++i) {
	    tempFull[i] = new T[full_dim2];
	}

	kronecker(cPreFactor,rPreFactor,tempCR, w,smallestW/2, h, smallestH/2);
	kronecker(sPreFactor,tempCR,LLL, f, smallestF/2, CR_dim1, CR_dim2);
	for (int i = 0; i < CR_dim1; ++i) {
	    delete[] tempCR[i];
	}
	delete[] tempCR;
	for (int i = 0; i < full_dim1; ++i) {
	    delete[] tempFull[i];
	}
	delete[] tempFull;

	return;
    }

    if (currentScale < scale) {
	
	/* 1) RHS factors:
	   new_sPhiT  and  new_sPsiT (smallF x smallF/2),
	   new_cPhiT  and  new_cPsiT (smallW x smallW/2),
	   new_rPhiT  and  new_rPsiT (smallH x smallH/2) */
	T **new_sPhiT = new T*[smallF];
	T **new_sPsiT = new T*[smallF];
	T **new_cPhiT = new T*[smallW];
	T **new_cPsiT = new T*[smallW];
	T **new_rPhiT = new T*[smallH];
	T **new_rPsiT = new T*[smallH];
	for (int i = 0; i < smallF; ++i) {
	    new_sPhiT[i] = new T[smallF/2];
	    new_sPsiT[i] = new T[smallF/2];
	    for (int j = 0; j < smallF/2; ++j) {
		if (i == 2*j) {
		    new_sPhiT[i][j] = 1/sqrt(2);
		    new_sPsiT[i][j] = 1/sqrt(2);
		} else if (i == 2*j + 1) {
		    new_sPhiT[i][j] = 1/sqrt(2);
		    new_sPsiT[i][j] = -1/sqrt(2);
		} else {
		    new_sPhiT[i][j] = 0;
		    new_sPsiT[i][j] = 0;
		}
	    }
	}
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
	   sPreFactor (f x smallF),
	   cPreFactor (w x smallW),
	   rPreFactor (h x smallH),
	   These are passed as arguments into the function */
	
	/* 3) Matrix products:
	   sPreFactor*new_sPhiT, sPreFactor*new_sPsiT
	   cPreFactor*new_cPhiT, cPreFactor*new_cPsiT
	   rPreFactor*new_rPhiT, rPreFactor*new_rPsiT */
	T **sPre_Phi = new T*[f];
	T **sPre_Psi = new T*[f];
	T **cPre_Phi = new T*[w];
	T **cPre_Psi = new T*[w];
	T **rPre_Phi = new T*[h];
	T **rPre_Psi = new T*[h];
	for (int i = 0; i < f; ++i) {
	    sPre_Phi[i] = new T[smallF/2];
	    sPre_Psi[i] = new T[smallF/2];
	}
	for (int i = 0; i < w; ++i) {
	    cPre_Phi[i] = new T[smallW/2];
	    cPre_Psi[i] = new T[smallW/2];
	}
	for (int i = 0; i < h; ++i) {
	    rPre_Phi[i] = new T[smallH/2];
	    rPre_Psi[i] = new T[smallH/2];
	}
	multiply2D2D(sPreFactor,new_sPhiT,sPre_Phi,f,smallF/2,smallF);
	multiply2D2D(sPreFactor,new_sPsiT,sPre_Psi,f,smallF/2,smallF);
	multiply2D2D(cPreFactor,new_cPhiT,cPre_Phi,w,smallW/2,smallW);
	multiply2D2D(cPreFactor,new_cPsiT,cPre_Psi,w,smallW/2,smallW);
	multiply2D2D(rPreFactor,new_rPhiT,rPre_Phi,h,smallH/2,smallH);
	multiply2D2D(rPreFactor,new_rPsiT,rPre_Psi,h,smallH/2,smallH);

	/* 4) Form Kronecker products and save in LLL
	   tempCR - save kronecker of C matrix and R matrix 
	     size is (wh)x(smallW*smallH/4)
	   temp - save kronecker of S matrix and tempCR
	     size is (fwh)x(smallF*smallW*smallH/8)
	   new_LLL - like temp but we'll use it only for new LLL part */
	int CR_dim1 = w*h;
	int CR_dim2 = smallW * smallH / 4;
	int full_dim1 = f*w*h;
	int full_dim2 = smallF * smallW * smallH / 8;
	T **tempCR = new T*[CR_dim1];
	T **tempFull = new T*[full_dim1];
	T **new_LLL = new T*[full_dim1];
	for (int i = 0; i < CR_dim1; ++i) {
	    tempCR[i] = new T[CR_dim2];
	}
	for (int i = 0; i < full_dim1; ++i) {
	    tempFull[i] = new T[full_dim2];
	    new_LLL[i] = new T[full_dim2];
	}

	// new LLH
	kronecker(cPre_Phi, rPre_Psi, tempCR, w, smallW/2, h, smallH/2);
	kronecker(sPre_Phi,tempCR, tempFull,f,smallF/2,CR_dim1,CR_dim2);
	for (int i = 0; i < full_dim1; ++i) {
	    for (int j = 0; j < full_dim2; ++j) {
		LLL[i][full_dim2 + j] = tempFull[i][j];
	    }
	}
	// new LHL
	kronecker(cPre_Psi, rPre_Phi, tempCR, w, smallW/2, h, smallH/2);
	kronecker(sPre_Phi,tempCR, tempFull,f,smallF/2,CR_dim1,CR_dim2);
	for (int i = 0; i < full_dim1; ++i) {
	    for (int j = 0; j < full_dim2; ++j) {
		LLL[i][2*full_dim2 + j] = tempFull[i][j];
	    }
	}
	// new LHH
	kronecker(cPre_Psi, rPre_Psi, tempCR, w, smallW/2, h, smallH/2);
	kronecker(sPre_Phi,tempCR, tempFull,f,smallF/2,CR_dim1,CR_dim2);
	for (int i = 0; i < full_dim1; ++i) {
	    for (int j = 0; j < full_dim2; ++j) {
		LLL[i][3*full_dim2 + j] = tempFull[i][j];
	    }
	}
	// new HLL
	kronecker(cPre_Phi, rPre_Phi, tempCR, w, smallW/2, h, smallH/2);
	kronecker(sPre_Psi,tempCR, tempFull,f,smallF/2,CR_dim1,CR_dim2);
	for (int i = 0; i < full_dim1; ++i) {
	    for (int j = 0; j < full_dim2; ++j) {
		LLL[i][4*full_dim2 + j] = tempFull[i][j];
	    }
	}
	// new HLH
	kronecker(cPre_Phi, rPre_Psi, tempCR, w, smallW/2, h, smallH/2);
	kronecker(sPre_Psi,tempCR, tempFull,f,smallF/2,CR_dim1,CR_dim2);
	for (int i = 0; i < full_dim1; ++i) {
	    for (int j = 0; j < full_dim2; ++j) {
		LLL[i][5*full_dim2 + j] = tempFull[i][j];
	    }
	}
	// new HHL
	kronecker(cPre_Psi, rPre_Phi, tempCR, w, smallW/2, h, smallH/2);
	kronecker(sPre_Psi,tempCR, tempFull,f,smallF/2,CR_dim1,CR_dim2);
	for (int i = 0; i < full_dim1; ++i) {
	    for (int j = 0; j < full_dim2; ++j) {
		LLL[i][6*full_dim2 + j] = tempFull[i][j];
	    }
	}
	// new HHH
	kronecker(cPre_Psi, rPre_Psi, tempCR, w, smallW/2, h, smallH/2);
	kronecker(sPre_Psi,tempCR, tempFull,f,smallF/2,CR_dim1,CR_dim2);
	for (int i = 0; i < full_dim1; ++i) {
	    for (int j = 0; j < full_dim2; ++j) {
		LLL[i][7*full_dim2 + j] = tempFull[i][j];
	    }
	}
	
	/* 5) form new_LLL */
	_LLL_generate(new_LLL, scale, currentScale + 1, h, w, f, rPre_Phi, cPre_Phi, sPre_Phi);
	for (int i = 0; i < full_dim1; ++i) {
	    for (int j = 0; j < full_dim2; ++j) {
		LLL[i][j] = new_LLL[i][j];
	    }
	}
	
	for (int i = 0; i < smallF ; ++i) {
	    delete[] new_sPhiT[i];
	    delete[] new_sPsiT[i];
	}
	delete[] new_sPhiT;
	delete[] new_sPsiT;
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
	for (int i = 0; i < f; ++i) {
	    delete[] sPre_Phi[i];
	    delete[] sPre_Psi[i];
	}
	delete[] sPre_Phi;
	delete[] sPre_Psi;
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
	for (int i = 0; i < CR_dim1; ++i) {
	    delete[] tempCR[i];
	}
	delete[] tempCR;
	for (int i = 0; i < full_dim1; ++i) {
	    delete[] tempFull[i];
	    delete[] new_LLL[i];
	}
	delete[] tempFull;
	delete[] new_LLL;
    }
}

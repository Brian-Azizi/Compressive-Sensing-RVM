template <class T>
void haarBasis(T **BASIS, int height, int width, int frames, int scale = 1)
{
    
    /* Allocate space */
     
    // Row Filters
    T **rPhiT = new T*[height];
    T **rPsiT = new T*[height];
    for (int i = 0; i < height; ++i) {
	rPhiT[i] = new T[height/2];
	rPsiT[i] = new T[height/2];
    }
    
    // Column Filters
    T **cPhiT = new T*[width];
    T **cPsiT = new T*[width];
    for (int i = 0; i < width; ++i) {
	cPhiT[i] = new T[width/2];
	cPsiT[i] = new T[width/2];
    }

    // Slice Filters
    T **sPhiT = new T*[frames];
    T **sPsiT = new T*[frames];
    for (int i = 0; i < frames; ++i) {
	sPhiT[i] = new T[frames/2];
	sPsiT[i] = new T[frames/2];
    }


    // Tmp matrix
    int sizeFull = frames * width * height;
    T **tmp = new T*[sizeFull];
    for (int i = 0; i < sizeFull; ++i) {
	tmp[i] = new T[sizeFull/8];
    }

    int sizeCR = width * height;
    T **tmpCR = new T*[sizeCR];
    for (int i = 0; i < sizeCR; ++i) {
	tmpCR[i] = new T[sizeCR/4];
    }

    // LLL matrix
    T **LLL = new T*[sizeFull];
    for (int i = 0; i < sizeFull; ++i) {
	LLL[i] = new T[sizeFull/8];
    }

    /* Generate filters */
    // Rows
    for (int i = 0; i < height; ++i) {
	for (int j = 0; j < height/2; ++j) {
	    if (i == 2*j) {
		rPhiT[i][j] = 1/sqrt(2);
		rPsiT[i][j] = 1/sqrt(2);
	    } else if (i == 2*j + 1) {
		rPhiT[i][j] = 1/sqrt(2);
		rPsiT[i][j] = -1/sqrt(2);
	    } else {
		rPhiT[i][j] = 0;
		rPsiT[i][j] = 0;
	    }
	}
    }
    // Columns
    for (int i = 0; i < width; ++i) {
	for (int j = 0; j < width/2; ++j) {
	    if (i == 2*j) {
		cPhiT[i][j] = 1/sqrt(2);
		cPsiT[i][j] = 1/sqrt(2);
	    } else if (i == 2*j + 1) {
		cPhiT[i][j] = 1/sqrt(2);
		cPsiT[i][j] = -1/sqrt(2);
	    } else {
		cPhiT[i][j] = 0;
		cPsiT[i][j] = 0;
	    }
	}
    }
    // Slices
    for (int i = 0; i < frames; ++i) {
	for (int j = 0; j < frames/2; ++j) {
	    if (i == 2*j) {
		sPhiT[i][j] = 1/sqrt(2);
		sPsiT[i][j] = 1/sqrt(2);
	    } else if (i == 2*j + 1) {
		sPhiT[i][j] = 1/sqrt(2);
		sPsiT[i][j] = -1/sqrt(2);
	    } else {
		sPhiT[i][j] = 0;
		sPsiT[i][j] = 0;
	    }
	}
    }

    /* Generate LLL filter */
    _LLL_generate(LLL, scale, height, width, frames, rPhiT, cPhiT, sPhiT);
       /* Generate Basis Matrix */

    // LLL
    for (int i = 0; i < sizeFull; ++i) {
	for (int j = 0; j < sizeFull/8; ++j) {
	    BASIS[i][j] = LLL[i][j];
	}
    }

    // LLH
    kronecker(cPhiT, rPsiT, tmpCR, width, width/2, height, height/2);
    kronecker(sPhiT, tmpCR, tmp, frames, frames/2, sizeCR, sizeCR/4);
    for (int i = 0; i < sizeFull; ++i) {
	for (int j = 0; j < sizeFull/8; ++j) {
	    BASIS[i][sizeFull/8 + j] = tmp[i][j];
	}
    }
    // LHL
    kronecker(cPsiT, rPhiT, tmpCR, width, width/2, height, height/2);
    kronecker(sPhiT, tmpCR, tmp, frames, frames/2, sizeCR, sizeCR/4);
    for (int i = 0; i < sizeFull; ++i) {
	for (int j = 0; j < sizeFull/8; ++j) {
	    BASIS[i][2*sizeFull/8 + j] = tmp[i][j];
	}
    }
    // LHH
    kronecker(cPsiT, rPsiT, tmpCR, width, width/2, height, height/2);
    kronecker(sPhiT, tmpCR, tmp, frames, frames/2, sizeCR, sizeCR/4);
    for (int i = 0; i < sizeFull; ++i) {
	for (int j = 0; j < sizeFull/8; ++j) {
	    BASIS[i][3*sizeFull/8 + j] = tmp[i][j];
	}
    }
    // HLL
    kronecker(cPhiT, rPhiT, tmpCR, width, width/2, height, height/2);
    kronecker(sPsiT, tmpCR, tmp, frames, frames/2, sizeCR, sizeCR/4);
    for (int i = 0; i < sizeFull; ++i) {
	for (int j = 0; j < sizeFull/8; ++j) {
	    BASIS[i][4*sizeFull/8 + j] = tmp[i][j];
	}
    }
    // HLH
    kronecker(cPhiT, rPsiT, tmpCR, width, width/2, height, height/2);
    kronecker(sPsiT, tmpCR, tmp, frames, frames/2, sizeCR, sizeCR/4);
    for (int i = 0; i < sizeFull; ++i) {
	for (int j = 0; j < sizeFull/8; ++j) {
	    BASIS[i][5*sizeFull/8 + j] = tmp[i][j];
	}
    }
    // HHL
    kronecker(cPsiT, rPhiT, tmpCR, width, width/2, height, height/2);
    kronecker(sPsiT, tmpCR, tmp, frames, frames/2, sizeCR, sizeCR/4);
    for (int i = 0; i < sizeFull; ++i) {
	for (int j = 0; j < sizeFull/8; ++j) {
	    BASIS[i][6*sizeFull/8 + j] = tmp[i][j];
	}
    }
    // HHH
    kronecker(cPsiT, rPsiT, tmpCR, width, width/2, height, height/2);
    kronecker(sPsiT, tmpCR, tmp, frames, frames/2, sizeCR, sizeCR/4);
    for (int i = 0; i < sizeFull; ++i) {
	for (int j = 0; j < sizeFull/8; ++j) {
	    BASIS[i][7*sizeFull/8 + j] = tmp[i][j];
	}
    }




    /* Clean-Up */
    for (int i = 0; i < sizeFull; ++i) {
	delete[] LLL[i];
	delete[] tmp[i];
    }
    delete[] LLL;
    delete[] tmp;

    for (int i = 0; i < sizeCR; ++i) {
	delete[] tmpCR[i];
    }
    delete[] tmpCR;

    for (int i = 0; i < height; ++i) {
	delete[] rPhiT[i];
	delete[] rPsiT[i];
    }
    delete[] rPhiT;
    delete[] rPsiT;

    for (int i = 0; i < width; ++i) {
	delete[] cPhiT[i];
	delete[] cPsiT[i];
    }
    delete[] cPhiT;
    delete[] cPsiT;

    for (int i = 0; i < frames; ++i) {
	delete[] sPhiT[i];
	delete[] sPsiT[i];
    }
    delete[] sPhiT;
    delete[] sPsiT;
}

template<class T>
void _LLL_generate (T **LLL, int scale, int h, int w, int f, T **rowFilter, T **colFilter, T **sliceFilter)
{
    if (scale == 1) {
	// Tmp matrix
	int szFull = f*w*h;
	T **tmpFull = new T*[szFull];
	for (int i = 0; i < szFull; ++i) {
	    tmpFull[i] = new T[szFull/8];
	}
	
	int szCR = w*h;
	T **tmpColRow = new T*[szCR];
	for (int i = 0; i < szCR; ++i) {
	    tmpColRow[i] = new T[szCR/4];
	}
	
	kronecker(colFilter, rowFilter, tmpColRow, w, w/2, h, h/2);
	kronecker(sliceFilter, tmpColRow, LLL, f, f/2, szCR, szCR/4);

	for (int i = 0; i < szFull; ++i) {
	    delete[] tmpFull[i];
	}
	delete[] tmpFull;
	for (int i = 0; i < szCR; ++i) {
	    delete[] tmpColRow[i];
	}
	delete[] tmpColRow;

	return;
    }

    int const dimDivider = 2^(scale-1);
    int const smallH = h/dimDivider;
    int const smallW = w/dimDivider;
    int const smallF = f/dimDivider;

    
    // Allocate Space
    T **prevLLL_rPhiT = new T*[h];
    T **prevLLL_rPsiT = new T*[h];
    for (int i = 0; i < h; ++i) {
	prevLLL_rPhiT[i] = new T[smallH/2];
	prevLLL_rPsiT[i] = new T[smallH/2];
    }
    T **prevLLL_cPhiT = new T*[w];
    T **prevLLL_cPsiT = new T*[w];
    for (int i = 0; i < h; ++i) {
	prevLLL_cPhiT[i] = new T[smallW/2];
	prevLLL_cPsiT[i] = new T[smallW/2];
    }
    T **prevLLL_sPhiT = new T*[f];
    T **prevLLL_sPsiT = new T*[f];
    for (int i = 0; i < h; ++i) {
	prevLLL_sPhiT[i] = new T[smallF/2];
	prevLLL_sPsiT[i] = new T[smallF/2];
    }

    // Row Filters
    T **rPhiT = new T*[smallH];
    T **rPsiT = new T*[smallH];
    for (int i = 0; i < smallH; ++i) {
	rPhiT[i] = new T[smallH/2];
	rPsiT[i] = new T[smallH/2];
    }
    
    // Column Filters
    T **cPhiT = new T*[smallW];
    T **cPsiT = new T*[smallW];
    for (int i = 0; i < smallW; ++i) {
	cPhiT[i] = new T[smallW/2];
	cPsiT[i] = new T[smallW/2];
    }

    // Slice Filters
    T **sPhiT = new T*[smallF];
    T **sPsiT = new T*[smallF];
    for (int i = 0; i < smallF; ++i) {
	sPhiT[i] = new T[smallF/2];
	sPsiT[i] = new T[smallF/2];
    }

    // Tmp matrix
    int sizeFull = f * w * h;
    T **tmp = new T*[sizeFull];
    for (int i = 0; i < sizeFull; ++i) {
	tmp[i] = new T[sizeFull/(8^scale)];
    }

    int sizeCR = w * h;
    T **tmpCR = new T*[sizeCR];
    for (int i = 0; i < sizeCR; ++i) {
	tmpCR[i] = new T[sizeCR/(4^scale)];
    }

    // newLLL matrix
    T **newLLL = new T*[sizeFull];
    for (int i = 0; i < sizeFull; ++i) {
	newLLL[i] = new T[sizeFull/(8^scale)];
    }


    


/* Generate filters */
    // Rows
    for (int i = 0; i < smallH; ++i) {
	for (int j = 0; j < smallH/2; ++j) {
	    if (i == 2*j) {
		rPhiT[i][j] = 1/sqrt(2);
		rPsiT[i][j] = 1/sqrt(2);
	    } else if (i == 2*j + 1) {
		rPhiT[i][j] = 1/sqrt(2);
		rPsiT[i][j] = -1/sqrt(2);
	    } else {
		rPhiT[i][j] = 0;
		rPsiT[i][j] = 0;
	    }
	}
    }
    // Columns
    for (int i = 0; i < smallW; ++i) {
	for (int j = 0; j < smallW/2; ++j) {
	    if (i == 2*j) {
		cPhiT[i][j] = 1/sqrt(2);
		cPsiT[i][j] = 1/sqrt(2);
	    } else if (i == 2*j + 1) {
		cPhiT[i][j] = 1/sqrt(2);
		cPsiT[i][j] = -1/sqrt(2);
	    } else {
		cPhiT[i][j] = 0;
		cPsiT[i][j] = 0;
	    }
	}
    }
    // Slices
    for (int i = 0; i < smallF; ++i) {
	for (int j = 0; j < smallF/2; ++j) {
	    if (i == 2*j) {
		sPhiT[i][j] = 1/sqrt(2);
		sPsiT[i][j] = 1/sqrt(2);
	    } else if (i == 2*j + 1) {
		sPhiT[i][j] = 1/sqrt(2);
		sPsiT[i][j] = -1/sqrt(2);
	    } else {
		sPhiT[i][j] = 0;
		sPsiT[i][j] = 0;
	    }
	}
    }
    
    multiply2D2D(rowFilter,rPhiT,prevLLL_rPhiT,h,smallH/2,smallH);
    multiply2D2D(rowFilter,rPsiT,prevLLL_rPsiT,h,smallH/2,smallH);
    multiply2D2D(rowFilter,cPhiT,prevLLL_cPsiT,w,smallW/2,smallW);
    multiply2D2D(rowFilter,cPsiT,prevLLL_cPsiT,w,smallW/2,smallW);
    multiply2D2D(rowFilter,sPhiT,prevLLL_sPsiT,f,smallF/2,smallF);
    multiply2D2D(rowFilter,sPsiT,prevLLL_sPsiT,f,smallF/2,smallF);
    
/* Generate newLLL filter */
    _LLL_generate(newLLL, scale-1, h, w, f, prevLLL_rPhiT, prevLLL_cPhiT, prevLLL_sPhiT);
       /* Generate Basis Matrix */

    // LLL
    for (int i = 0; i < sizeFull; ++i) {
    	for (int j = 0; j < sizeFull/(8^scale); ++j) {
    	    LLL[i][j] = newLLL[i][j];
    	}
    }

    // LLH
    kronecker(prevLLL_cPhiT, prevLLL_rPsiT, tmpCR, w, smallW/2, h, smallH/2);
    kronecker(prevLLL_sPhiT, tmpCR, tmp, f, smallF/2, sizeCR, sizeCR/4);
    for (int i = 0; i < sizeFull; ++i) {
	for (int j = 0; j < sizeFull/(8^scale); ++j) {
	    LLL[i][sizeFull/(8^scale) + j] = tmp[i][j];
	}
    }
    // LHL
    kronecker(prevLLL_cPsiT, prevLLL_rPhiT, tmpCR, w, smallW/2, h, smallH/2);
    kronecker(prevLLL_sPhiT, tmpCR, tmp, f, smallF/2, sizeCR, sizeCR/4);
    for (int i = 0; i < sizeFull; ++i) {
	for (int j = 0; j < sizeFull/(8^scale); ++j) {
	    LLL[i][2*sizeFull/(8^scale) + j] = tmp[i][j];
	}
    }
    // LHH
    kronecker(prevLLL_cPsiT, prevLLL_rPsiT, tmpCR, w, smallW/2, h, smallH/2);
    kronecker(prevLLL_sPhiT, tmpCR, tmp, f, smallF/2, sizeCR, sizeCR/4);
    for (int i = 0; i < sizeFull; ++i) {
	for (int j = 0; j < sizeFull/(8^scale); ++j) {
	    LLL[i][3*sizeFull/(8^scale) + j] = tmp[i][j];
	}
    }
    // HLL
    kronecker(prevLLL_cPhiT, prevLLL_rPhiT, tmpCR, w, smallW/2, h, smallH/2);
    kronecker(prevLLL_sPsiT, tmpCR, tmp, f, smallF/2, sizeCR, sizeCR/4);
    for (int i = 0; i < sizeFull; ++i) {
	for (int j = 0; j < sizeFull/(8^scale); ++j) {
	    LLL[i][4*sizeFull/(8^scale) + j] = tmp[i][j];
	}
    }
    // HLH
    kronecker(prevLLL_cPhiT, prevLLL_rPsiT, tmpCR, w, smallW/2, h, smallH/2);
    kronecker(prevLLL_sPsiT, tmpCR, tmp, f, smallF/2, sizeCR, sizeCR/4);
    for (int i = 0; i < sizeFull; ++i) {
	for (int j = 0; j < sizeFull/(8^scale); ++j) {
	    LLL[i][5*sizeFull/(8^scale) + j] = tmp[i][j];
	}
    }
    // HHL
    kronecker(prevLLL_cPsiT, prevLLL_rPhiT, tmpCR, w, smallW/2, h, smallH/2);
    kronecker(prevLLL_sPsiT, tmpCR, tmp, f, smallF/2, sizeCR, sizeCR/4);
    for (int i = 0; i < sizeFull; ++i) {
	for (int j = 0; j < sizeFull/(8^scale); ++j) {
	    LLL[i][6*sizeFull/(8^scale) + j] = tmp[i][j];
	}
    }
    // HHH
    kronecker(prevLLL_cPsiT, prevLLL_rPsiT, tmpCR, w, smallW/2, h, smallH/2);
    kronecker(prevLLL_sPsiT, tmpCR, tmp, f, smallF/2, sizeCR, sizeCR/4);
    for (int i = 0; i < sizeFull; ++i) {
	for (int j = 0; j < sizeFull/(8^scale); ++j) {
	    LLL[i][7*sizeFull/(8^scale) + j] = tmp[i][j];
	}
    }
    



    // Deletions
    for (int i = 0; i < sizeFull; ++i) {
	delete[] newLLL[i];
	delete[] tmp[i];
    }
    delete[] newLLL;
    delete[] tmp;

    for (int i = 0; i < sizeCR; ++i) {
	delete[] tmpCR[i];
    }
    delete[] tmpCR;


    for (int i = 0; i < smallH; ++i) {
	delete[] rPhiT[i];
	delete[] rPsiT[i];
    }
    delete[] rPhiT;
    delete[] rPsiT;

    for (int i = 0; i < smallW; ++i) {
	delete[] cPhiT[i];
	delete[] cPsiT[i];
    }
    delete[] cPhiT;
    delete[] cPsiT;

    for (int i = 0; i < smallF; ++i) {
	delete[] sPhiT[i];
	delete[] sPsiT[i];
    }
    delete[] sPhiT;
    delete[] sPsiT;

    
    for (int i = 0; i < h; ++i) {
	delete[] prevLLL_rPhiT[i];
	delete[] prevLLL_rPsiT[i];
    }
    delete[] prevLLL_rPhiT;
    delete[] prevLLL_rPsiT;

    for (int i = 0; i < w; ++i) {
	delete[] prevLLL_cPhiT[i];
	delete[] prevLLL_cPsiT[i];
    }
    delete[] prevLLL_cPhiT;
    delete[] prevLLL_cPsiT;

    for (int i = 0; i < f; ++i) {
	delete[] prevLLL_sPhiT[i];
	delete[] prevLLL_sPsiT[i];
    }
    delete[] prevLLL_sPhiT;
    delete[] prevLLL_sPsiT;


}

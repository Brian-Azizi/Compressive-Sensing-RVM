template <class T>
void haarBasis(T **BASIS, int height, int width, int frames, int scale)
{
    if (frames == 1) {
	return haarBasis2D(BASIS, height, width, scale);
    } else {

    
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
		rPhiT[i][j] = 1/std::sqrt(2);
		rPsiT[i][j] = 1/std::sqrt(2);
	    } else if (i == 2*j + 1) {
		rPhiT[i][j] = 1/std::sqrt(2);
		rPsiT[i][j] = -1/std::sqrt(2);
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
		cPhiT[i][j] = 1/std::sqrt(2);
		cPsiT[i][j] = 1/std::sqrt(2);
	    } else if (i == 2*j + 1) {
		cPhiT[i][j] = 1/std::sqrt(2);
		cPsiT[i][j] = -1/std::sqrt(2);
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
    /* Generate LLL filter */
    int startScale = 1;
    _LLL_generate(LLL, scale, startScale, height, width, frames, rPhiT, cPhiT, sPhiT);
       /* Generate Basis Matrix */

    // LLL
    for (int i = 0; i < sizeFull; ++i) {
	for (int j = 0; j < sizeFull/8; ++j) {
	    BASIS[i][j] = LLL[i][j];
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
}


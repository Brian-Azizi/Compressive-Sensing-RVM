template <class T>
void haarBasis2D(T **BASIS, int height, int width, int scale)
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

    // Tmp matrix
    int sizeFull = width * height;
    T **tmp = new T*[sizeFull];
    for (int i = 0; i < sizeFull; ++i) {
    	tmp[i] = new T[sizeFull/4];
    }

    // LL matrix
    T **LL = new T*[sizeFull];
    for (int i = 0; i < sizeFull; ++i) {
    	LL[i] = new T[sizeFull/4];
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

    // LH
    kronecker(cPhiT, rPsiT, tmp, width, width/2, height, height/2);
    for (int i = 0; i < sizeFull; ++i) {
	for (int j = 0; j < sizeFull/4; ++j) {
	    BASIS[i][sizeFull/4 + j] = tmp[i][j];
	}
    }

    // HL
    kronecker(cPsiT, rPhiT, tmp, width, width/2, height, height/2);
    for (int i = 0; i < sizeFull; ++i) {
	for (int j = 0; j < sizeFull/4; ++j) {
	    BASIS[i][2*sizeFull/4 + j] = tmp[i][j];
	}
    }

    // HH
    kronecker(cPsiT, rPsiT, tmp, width, width/2, height, height/2);
    for (int i = 0; i < sizeFull; ++i) {
	for (int j = 0; j < sizeFull/4; ++j) {
	    BASIS[i][3*sizeFull/4 + j] = tmp[i][j];
	}
    }

    /* Generate LL filter */
    int startScale = 1;
    _LL_generate(LL, scale, startScale, height, width,\
		 rPhiT, cPhiT);

    /* Generate Basis Matrix */

    // LL
    for (int i = 0; i < sizeFull; ++i) {
	for (int j = 0; j < sizeFull/4; ++j) {
	    BASIS[i][j] = LL[i][j];
	}
    }





    /* Clean-Up */
    for (int i = 0; i < sizeFull; ++i) {
	delete[] LL[i];
	delete[] tmp[i];
    }
    delete[] LL;
    delete[] tmp;

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
}

template <class T>
void getBasis(T **BASIS, int height, int width, int frames, int scale, basisFunctionMode basisMode)
{
    switch (basisMode) {
    case haar:
	haarBasis(BASIS, height, width, frames, scale);
	break;
    case dct:
	dctBasis(BASIS, height, width, frames);
	break;
    }

    return;
}

int countSensed(bool *sensed, int size)
{
    int count = 0;
    for (int i = 0; i < size; ++i) {
	if (sensed[i]) {
	    ++count;
	}
    }
    
    return count;
}

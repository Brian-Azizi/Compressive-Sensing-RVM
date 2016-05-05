template <class T, class V>
void fillSensedInfo(T *corruptedSignal, V* recoveredSignal, bool* sensed, int size)
{
    for (int i = 0; i < size; ++i) {
	if (sensed[i]) {
	    recoveredSignal[i] = corruptedSignal[i];
	}
    }

    return;
}

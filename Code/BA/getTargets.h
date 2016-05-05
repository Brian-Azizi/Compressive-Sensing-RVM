template <class T, class V>
void getTargets (T *corruptedSignal, V *target, bool *sensed, int origSize, int targetSize)
{
    int currentIdx = 0;
    for (int i = 0; i < origSize; ++i) {
	if (sensed[i]) {
	    target[currentIdx] = corruptedSignal[i];
	    ++currentIdx;
	}
    }

    // sanity check
    assert(targetSize == currentIdx && "measurements do not match number of sensed entries"); // && is a little hack that allows the message to be shown when the assert condition (before &&) fails

    return;
}

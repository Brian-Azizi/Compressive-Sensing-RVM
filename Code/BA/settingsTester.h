void testSettings()
{
    // Test if blocks are smaller or equal to input signal
    if (blockWidth > signalWidth) {
	throw std::length_error("width of blocks exceeds width of signal");
    } else if (blockHeight > signalHeight) {
	throw std::length_error("height of blocks exceeds height of signal");
    } else if (blockFrames > signalFrames) {
	throw std::length_error("depth of blocks exceeds depth of signal");
    }
    
    // Test if any block dimension is zero;
    if (blockWidth == 0) {
	throw std::length_error("width of blocks is zero");
    } else if (blockHeight == 0) {
	throw std::length_error("height of blocks is zero");
    } else if (blockFrames == 0) {
	throw std::length_error("depth of blocks is zero");
    }

    // Test signal sizes are powers of 2;
    if (log2(signalWidth) != floor(log2(signalWidth)+0.5)) {
	throw std::length_error("width of signal is not a power of 2");
    } else if (log2(signalHeight) != floor(log2(signalHeight)+0.5)) {
	throw std::length_error("height of signal is not a power of 2");
    } else if (log2(signalFrames) != floor(log2(signalFrames)+0.5)) {
	throw std::length_error("depth of signal is not a power of 2");
    }
    
    // Test block sizes are powers of 2;
    if (log2(blockWidth) != floor(log2(blockWidth)+0.5)) {
	throw std::length_error("width of blocks is not a power of 2");
    } else if (log2(blockHeight) != floor(log2(blockHeight)+0.5)) {
	throw std::length_error("height of blocks is not a power of 2");
    } else if (log2(blockFrames) != floor(log2(blockFrames)+0.5)) {
	throw std::length_error("depth of blocks is not a power of 2");
    }
    
    /* If all tests pass, then the signal is correctly divided up into block patches.*/
    return;
}
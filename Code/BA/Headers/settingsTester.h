void testSettings()
{    
    // Test if any block dimension is zero;
    if (blockHeight == 0) {
	throw std::length_error("height of blocks is zero");
    } else if (blockWidth == 0) {
	throw std::length_error("width of blocks is zero");
    } else if (blockFrames == 0) {
	throw std::length_error("depth of blocks is zero");
    }

    // Test if blocks are smaller or equal to input signal
    if (blockHeight > signalHeight) {
	throw std::length_error("height of blocks exceeds height of signal");
    } else if (blockWidth > signalWidth) {
	throw std::length_error("width of blocks exceeds width of signal");
    } else if (blockFrames > signalFrames) {
	throw std::length_error("depth of blocks exceeds depth of signal");
    }
        
    // Test if blocks divide signal
    if (signalHeight % blockHeight != 0) {
	throw std::length_error("height of block does not divide height of signal");
    } else if (signalWidth % blockWidth != 0) {
	throw std::length_error("width of block does not divide width of signal");
    } else if (signalFrames % blockFrames != 0) {
	throw std::length_error("depth of block does not divide depth of signal");
    }
    
    // Test that scale is greater than zero
    // if (scale < 1) {
    // 	throw std::length_error("scale is less than 1");
    // }
    if (startScale < 1) {
	throw std::length_error("startScale is less than 1");
    }
    if (endScale < 1) {
	throw std::length_error("endScale is less than 1");
    }
    if (endScale < startScale) {
	throw std::length_error("endScale is less than startScale");
    }

    // // Test if block dimensions are divisible by 2^scale
    // int const scaleDim = std::pow(2,scale);
    // if (blockHeight % scaleDim != 0) {
    // 	throw std::length_error("block height does not support scale of wavelet");
    // } else if (blockWidth % scaleDim != 0) {
    // 	throw std::length_error("block width does not support scale of wavelet");
    // } else if (blockFrames % scaleDim != 0) {
    // 	throw std::length_error("block depth does not support scale of wavelet");
    // }

    // Test if block dimensions are divisible by 2^scale
    int const scaleDim = std::pow(2,endScale);
    if (blockHeight % scaleDim != 0) {
	throw std::length_error("block height does not support scale of wavelet");
    } else if (blockWidth % scaleDim != 0) {
	throw std::length_error("block width does not support scale of wavelet");
    } else if (blockFrames % scaleDim != 0) {
	throw std::length_error("block depth does not support scale of wavelet");
    }
    

    // // Test signal sizes are powers of 2;
    // if (log2(signalHeight) != floor(log2(signalHeight)+0.5)) {
    // 	throw std::length_error("height of signal is not a power of 2");
    // } else if (log2(signalWidth) != floor(log2(signalWidth)+0.5)) {
    // 	throw std::length_error("width of signal is not a power of 2");
    // } else if (log2(signalFrames) != floor(log2(signalFrames)+0.5)) {
    // 	throw std::length_error("depth of signal is not a power of 2");
    // }
    
    // // Test block sizes are powers of 2;
    // if (log2(blockHeight) != floor(log2(blockHeight)+0.5)) {
    // 	throw std::length_error("height of blocks is not a power of 2");
    // } else if (log2(blockWidth) != floor(log2(blockWidth)+0.5)) {
    // 	throw std::length_error("width of blocks is not a power of 2");
    // } else if (log2(blockFrames) != floor(log2(blockFrames)+0.5)) {
    // 	throw std::length_error("depth of blocks is not a power of 2");
    // }
    
    /* If all tests pass, then the signal is correctly divided up into block patches.*/
    std::cout << "Settings test successful" << std::endl;
    return;
}

template<class T>
void output3Dsignals(T ***sig, std::string label, bool actualOutput = true)
{
    std::stringstream ss;
    
    /*** save in /local/data/public/ ***/
    ss << "/local/data/public/";
    
    /*** save in ba308 directory if it exists ***/
    struct stat sb;
    if (stat("/local/data/public/ba308/", &sb) == 0 && S_ISDIR(sb.st_mode)) {
	ss << "ba308/";
    }
    
    /*** save in ResultsDump directory if it exists ***/
    
    if (stat("/local/data/public/ba308/ResultsDump", &sb) == 0 && S_ISDIR(sb.st_mode)) {
	ss << "ResultsDump/";
    }

    if (!actualOutput) {
	ss << "DUMMY_";
    }

    ss << blockHeight << "-" << blockWidth << "-" << blockFrames << "_";   
    ss << percentage << "%_" << settingStrings[corrupterSetting];
    
    if (use_cascade) {
	ss << "_cascade";
    }
    
    ss << "_" << scale << "_" << label << "_" << inputFileStem;
    
    std::string sigFile = ss.str();

    std::ofstream sigOut(sigFile.c_str());
    print3D(sigOut, sig, signalHeight, signalWidth, signalFrames);
    sigOut.close();

    return;
}

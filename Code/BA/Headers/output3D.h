void output3Dsignals(signalType ***corrupted, basisType ***recovered, bool actualOutput = true)
{
    std::stringstream cs;
    std::stringstream rs;
    
    /*** save in /local/data/public/ ***/
    cs << "/local/data/public/";
    rs << "/local/data/public/";
    
    /*** save in ba308 directory if it exists ***/
    struct stat sb;
    if (stat("/local/data/public/ba308/", &sb) == 0 && S_ISDIR(sb.st_mode)) {
	cs << "ba308/";
	rs << "ba308/";
    }
    
    /*** save in ResultsDump directory if it exists ***/
    
    if (stat("/local/data/public/ba308/ResultsDump", &sb) == 0 && S_ISDIR(sb.st_mode)) {
	cs << "ResultsDump/";
	rs << "ResultsDump/";
    }

    if (!actualOutput) {
	cs << "DUMMY_";
	rs << "DUMMY_";
    }

    cs << blockHeight << "-" << blockWidth << "-" << blockFrames << "_";
    rs << blockHeight << "-" << blockWidth << "-" << blockFrames << "_";
    
    cs << percentage << "%_" << settingStrings[corrupterSetting];
    rs << percentage << "%_" << settingStrings[corrupterSetting];

    cs << "_" << scale << "_corrupted_" << inputFileStem;
    rs << "_" << scale << "_recovered_" << inputFileStem;
    
    std::string corruptSigFile = cs.str();
    std::string recoveredSigFile = rs.str();


    std::ofstream corrOut(corruptSigFile.c_str());
    print3D(corrOut, corrupted, signalHeight, signalWidth, signalFrames);
    corrOut.close();

    std::ofstream recOut(recoveredSigFile.c_str());
    print3D(recOut, recovered, signalHeight, signalWidth, signalFrames);
    recOut.close();

    return;
}

void output3Dsignals(signalType ***corrupted, basisType ***recovered, bool actualOutput = true)
{
    std::stringstream cs;
    std::stringstream rs;

    /*** save in ResultsDump directory if it exists ***/
    struct stat sb;
    if (stat("ResultsDump", &sb) == 0 && S_ISDIR(sb.st_mode)) {
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

    cs << "_" << scale << "_corrupted_" << inputFile;
    rs << "_" << scale << "_recovered_" << inputFile;
    
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


void output(vector<double> join_signal,int file_layer, int signalResolution, string ori)
{
	ofstream output_file;
	std::string filename;
	std::string name = "output";
	std::string end = ".txt";
	
	std::string Result;         
	std::stringstream convert;   
	convert << file_layer;     
	Result = convert.str(); 
	
	filename =  name + "-" + ori + "-" + Result + "-" + end;

	output_file.open(filename.c_str());

	int cols=0;
	int rows=0;

	while ( (cols+signalResolution*rows)<signalResolution*signalResolution) 
		{
		if (cols==signalResolution-1)
			{
				output_file << join_signal[cols + signalResolution*rows] << "\n";
				cols=0;
				rows = rows + 1;
			}
		else
			{
				output_file << join_signal[cols + signalResolution*rows] << " " ;
				cols = cols + 1;
			}
		}


	
	output_file.close();
	
}

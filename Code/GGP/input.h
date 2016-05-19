vector<double> input(int signalResolution, string &filename)
{
	vector<double> signal(signalResolution*signalResolution); 
	ifstream myfile;
	filename = "lenna512.txt";
	
	myfile.open(filename.c_str()); // change the name of the file to read the signal in


	double **signal_from_file = new double*[signalResolution];
		for (int i=0;i<signalResolution;i++)
			signal_from_file[i] = new double[signalResolution];

	double number;
	int cols=0;
	int rows=0;

	while (myfile >> number) //read data points from the file, until the end of the file
	{
		signal_from_file[rows][cols] = number;
		
		if (cols==signalResolution-1)
			{
				cols=0;
				rows = rows + 1;
			}
		else
			cols = cols+1;

	}
	
	
	myfile.close();
	
	for (int rows=0;rows<signalResolution;rows++)
		for (int cols=0;cols<signalResolution;cols++)
			signal[cols + signalResolution*rows] =  signal_from_file[rows][cols];
	
	
	for (int i=0;i<signalResolution;i++)
				delete[] signal_from_file[i];
			delete[] signal_from_file;


return signal;

}
	

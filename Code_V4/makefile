# For parallel compilation, make sure to uncomment the USE_MPI flag at the top of main.cpp
parallel : main.cpp Headers/ConfigFile.hpp Headers/ConfigFile.tpp Headers/Convert.hpp Headers/Mask.hpp Headers/Mask.tpp Headers/Dim.hpp Headers/Dim.tpp Headers/Errors.hpp Headers/Errors.tpp Headers/Interpolator.hpp Headers/LapackFunctions.hpp Headers/RVM.hpp Headers/RVM.tpp Headers/SignalBasis.hpp Headers/SignalBasis.tpp Headers/Signal.hpp Headers/SignalSettings.hpp Headers/SignalSettings.tpp Headers/Signal.tpp Headers/Timer.hpp Headers/Sensor.hpp Headers/Sensor.tpp

	mpic++ main.cpp	 -o main -std=c++0x -llapack -lblas -O3 # -Wall -Wextra -pedantic

# For serial compilation, make sure to comment out the USE_MPI flag at the top of main.cpp
serial : main.cpp Headers/ConfigFile.hpp Headers/ConfigFile.tpp Headers/Convert.hpp Headers/Mask.hpp Headers/Mask.tpp Headers/Dim.hpp Headers/Dim.tpp Headers/Errors.hpp Headers/Errors.tpp Headers/Interpolator.hpp Headers/LapackFunctions.hpp Headers/RVM.hpp Headers/RVM.tpp Headers/SignalBasis.hpp Headers/SignalBasis.tpp Headers/Signal.hpp Headers/SignalSettings.hpp Headers/SignalSettings.tpp Headers/Signal.tpp Headers/Timer.hpp Headers/Sensor.hpp Headers/Sensor.tpp

	g++ main.cpp -o main_serial -std=c++0x -llapack -lblas -O3 # -Wall -Wextra -pedantic


clean :
	rm main main_serial *.fls *.log .rvmsettings.fls .rvmsettings.log

cleanConfig:
	rm *.cfg



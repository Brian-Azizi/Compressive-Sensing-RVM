parallel : main.cpp Headers/ConfigFile.hpp Headers/ConfigFile.tpp Headers/Convert.hpp Headers/Mask.hpp Headers/Mask.tpp Headers/Dim.hpp Headers/Dim.tpp Headers/Errors.hpp Headers/Errors.tpp Headers/fast_updates.hpp Headers/full_statistics.hpp Headers/init.hpp Headers/Interpolator.hpp Headers/LapackFunctions.hpp Headers/RVM.hpp Headers/SignalBasis.hpp Headers/SignalBasis.tpp Headers/Signal.hpp Headers/SignalSettings.hpp Headers/SignalSettings.tpp Headers/Signal.tpp Headers/Timer.hpp Headers/utilities.hpp
	mpic++ main.cpp	 -o leaf -std=c++0x -llapack -lblas -O3 # -Wall -Wextra -pedantic

serial : main.cpp Headers/ConfigFile.hpp Headers/ConfigFile.tpp Headers/Convert.hpp Headers/Mask.hpp Headers/Mask.tpp Headers/Dim.hpp Headers/Dim.tpp Headers/Errors.hpp Headers/Errors.tpp Headers/fast_updates.hpp Headers/full_statistics.hpp Headers/init.hpp Headers/Interpolator.hpp Headers/LapackFunctions.hpp Headers/RVM.hpp Headers/SignalBasis.hpp Headers/SignalBasis.tpp Headers/Signal.hpp Headers/SignalSettings.hpp Headers/SignalSettings.tpp Headers/Signal.tpp Headers/Timer.hpp Headers/utilities.hpp
	g++ main.cpp -o leaf_serial -std=c++0x -llapack -lblas -O3 # -Wall -Wextra -pedantic


clean :
	rm leaf leaf_serial *.fls *.log
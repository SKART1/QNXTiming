#include "QNXTimingTask6.hpp"
#include "DebugInfoOut.hpp"

void correctFormatInfo();

int parseParametrsMy(int argc, char *argv[], UserParametrsStruct *userParametrsStruct) {
	int c = 0;
	int counter1=0;
	int counter2=0;

	if (argc < 2) {
		correctFormatInfo();
		return (EXIT_FAILURE);
	};

	while ((c = getopt(argc, argv, "A:B:C:D:E")) != -1) {
		switch (c) {
		case 'A':
			counter1++;
			userParametrsStruct->researchedFunctions = SetGetClockPeriod;
			userParametrsStruct->newClockPeriod=atol(optarg);
			break;
		case 'B':
			counter1++;
			userParametrsStruct->researchedFunctions = SetGetPOSIXTimeOfClock;
			userParametrsStruct->secondsFrom1970=atoll(optarg);
			break;
		case 'C':
			counter1++;
			userParametrsStruct->researchedFunctions = SetGetQNXTimeOfClock;
			userParametrsStruct->nanosecAbsoluteTime =atol(optarg);
			break;
		case 'D':
			counter1++;
			userParametrsStruct->researchedFunctions = ClockAdjustFunc;
			userParametrsStruct->nanosecondsAdjustInEveryTick=atol(optarg);
			break;
		case 'E':
			userParametrsStruct->researchedFunctions = GetClockCycles;
			counter1++;
			break;
		}
		if(counter2>1 || counter1>1){
			std	::cout << "[ERROR]: Only one parameter allowed!" << std::endl;
			correctFormatInfo();
			return -1;
		}
	}

	if(counter1<1){
		std::cout<<"[ERROR]: Not enough arguments!"<<std::endl;
		correctFormatInfo();
		return -1;
	};

	if(optind<(argc)){
		//aboutServerInfoStruct->pathToFileWithServerInfo=argv[optind];
	}
	/*else{
		std::cout<<"[ERROR]: Path for the file not specified!"<<std::endl;
		correctFormatInfo();
		return -1;
	}*/


	return 0;
}


void correctFormatInfo(){
	std::cout<<"[ERROR]: Programm usage format: server.out \n -S - signal \n -P - pipe \n -F  %FILEPATH_FOR_FIFO.FIFO_FILE% - fifo \n -m - recievd-blocked messages \n -M - send-blocked messages  \n %FILEPATH_FOR_SERVERINFO_FILE%  "<< std::endl;
}

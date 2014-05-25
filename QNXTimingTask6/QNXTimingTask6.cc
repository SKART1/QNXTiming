#include "QNXTimingTask6.hpp"
#include "ParseParametrsMy.hpp"
#include "DebugInfoOut.hpp"
#include <string>


int main(int argc, char *argv[]) {
	UserParametrsStruct userParametrsStruct;

	//Parse user desires
	if(parseParametrsMy(argc, argv, &userParametrsStruct)!=0){
		return EXIT_FAILURE;
	};

	if(userParametrsStruct.researchedFunctions==SetGetClockPeriod){
		_clockperiod clcnew={userParametrsStruct.newClockPeriod,0}, clcold, clcout;
		std::cerr<<"[INFO]: Setting new ClockPeriod: "<<userParametrsStruct.newClockPeriod<<std::endl;
		ClockPeriod (CLOCK_REALTIME, &clcnew, &clcold, 0);
		ClockPeriod (CLOCK_REALTIME, NULL, &clcout, 0);
		uint64_t cps = SYSPAGE_ENTRY(qtime)->cycles_per_sec;
		std::cerr<<"ClockPeriod: old="<<clcold.nsec<<"new="<<clcout.nsec<<std::endl<<"ClockPerSec="<<CLOCKS_PER_SEC<<"Processor: "<<cps<<"cycles per sec"<<std::flush<<std::endl;
		std::cerr<<"Let`s let OS to pork with this settings for example 2 minutes"<<std::endl;
		sleep(120);
		std::cerr<<"If you see this program still executed. Restoring old settings!"<<std::endl;
		ClockPeriod (CLOCK_REALTIME, &clcold, &clcout, 0);
		ClockPeriod (CLOCK_REALTIME, NULL, &clcout, 0);
		cps = SYSPAGE_ENTRY(qtime)->cycles_per_sec;
		std::cerr<<"ClockPeriod: old="<<clcout.nsec<<"new="<<clcold.nsec<<std::endl<<"ClockPerSec="<<CLOCKS_PER_SEC<<"Processor: "<<cps<<"cycles per sec"<<std::flush<<std::endl;
	}
	else if(userParametrsStruct.researchedFunctions==SetGetPOSIXTimeOfClock){
		/*
		 * This program calculates the time required to
		 * execute the program specified as its first argument.
		 * The time is printed in seconds, on standard out.
		 */
		struct timespec start, stop;
		double accum;

		if( clock_gettime( CLOCK_REALTIME, &start) == -1 ) {
		  perror( "clock gettime" );
		  return EXIT_FAILURE;
		}

		char systemProgramm[]="find | grep test";
		system( systemProgramm );

		if( clock_gettime( CLOCK_REALTIME, &stop) == -1 ) {
		  perror( "clock gettime" );
		  return EXIT_FAILURE;
		}

		accum = ( stop.tv_sec - start.tv_sec ) + (double)( stop.tv_nsec - start.tv_nsec )/ (double)1000000000L;
		printf( "Call system  %s %lf\n", systemProgramm, accum );


		/*Set new time. Restoring old time*/
		struct timespec oldTime, newTimeStart, newTimeFinish;
		if(clock_gettime( CLOCK_REALTIME, &oldTime) == -1 ) {
			perror( "clock gettime" );
			return EXIT_FAILURE;
		}
		std::cout<<"[INFO]: Current time is: "<<oldTime.tv_sec<<" seconds"<<oldTime.tv_nsec<<" nanoseconds"<<std::endl;


		if(clock_settime( CLOCK_REALTIME, &newTimeStart) == -1 ) {
			perror( "clock settime" );
			return EXIT_FAILURE;
		}
		std::cout<<"[INFO]: New time is set. Time is: "<<newTimeStart.tv_sec<<" seconds"<<newTimeStart.tv_nsec<<" nanoseconds"<<std::endl;


		if(clock_gettime( CLOCK_REALTIME, &newTimeFinish) == -1 ) {
					perror( "clock settime" );
					return EXIT_FAILURE;
				}

		oldTime.tv_sec=oldTime.tv_sec+(newTimeFinish.tv_sec-newTimeStart.tv_sec);
		oldTime.tv_nsec=oldTime.tv_nsec+(newTimeFinish.tv_nsec-newTimeStart.tv_nsec);
		if(clock_settime( CLOCK_REALTIME, &oldTime) == -1 ) {
			perror( "clock settime" );
			return EXIT_FAILURE;
		}
		std::cout<<"[INFO]: Restoring previous time. Time is: "<<std::endl;
		return EXIT_SUCCESS;
	}
	else if(userParametrsStruct.researchedFunctions==SetGetQNXTimeOfClock){


	}
	else if(userParametrsStruct.researchedFunctions==ClockAdjustFunc){
		unsigned * threadMask;
		struct timespec startTime;
		uint64_t startClockCycles;
		struct timespec endTime;
		uint64_t endClockCycles;

		_clockadjust newCLockAdjust;
		_clockadjust oldCLockAdjust;


		/*Block program on one thread*/
		threadMask=(unsigned *)malloc(sizeof(*threadMask)*RMSK_SIZE(_syspage_ptr->num_cpu));
		*threadMask=0x1; //let the code be run on only first CPU
		DEBUG_PRINT_MSG_MY("INFO", "Setting new runmask!");
		ThreadCtl(_NTO_TCTL_RUNMASK_GET_AND_SET, (void *)threadMask);
		TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, 3,"[INFO]: Runmask is changed!");
		DEBUG_PRINT_MSG_MY("INFO", "New runmask is set!");

		/*Get time at the start of the program*/
		if(clock_gettime( CLOCK_REALTIME, &startTime) == -1 ) {
			perror( "clock gettime" );
			return EXIT_FAILURE;
		}
		startClockCycles=ClockCycles();



		/*Adjusting CLOCK_REALTIME (only them is allowed)*/
		/*Let time be forwarded for 3 minutes*/
		long long test=(1LL*60LL*1000LL*1000LL*1000LL)/userParametrsStruct.nanosecondsAdjustInEveryTick;
		newCLockAdjust.tick_nsec_inc=userParametrsStruct.nanosecondsAdjustInEveryTick;
		newCLockAdjust.tick_count =test;
		if(ClockAdjust( CLOCK_REALTIME, &newCLockAdjust, &oldCLockAdjust)==-1){
			perror("[ERROR]: ClockAdjust error: ");
		};

		/*How long shall we wait*/
		timespec res;
		clock_getres( CLOCK_REALTIME , &res);
		usleep(1.5*(test*res.tv_nsec/1000));

		/*Restoring old value*/
		if(ClockAdjust( CLOCK_REALTIME, &oldCLockAdjust,NULL )==-1){
			perror("[ERROR]: ClockAdjust error: ");
		}


		/*Get time and clock cycles at the end*/
		if(clock_gettime( CLOCK_REALTIME, &endTime) == -1 ) {
			perror( "clock gettime" );
			return EXIT_FAILURE;
		}
		endClockCycles=ClockCycles();
		double microsecondsPerCycle=(SYSPAGE_ENTRY(qtime) -> cycles_per_sec)/1000000L;
		std::cout<<"[INFO]: Time has passed: "<<((endTime.tv_sec-1)-startTime.tv_sec)*1000LL*1000LL+((endTime.tv_nsec+1000000000LL)-startTime.tv_nsec)/1000LL<<" micro seconds" <<std::endl;
		std::cout<<"[INFO]: Processor said it has passed: "/*<<(endClockCycles-startClockCycles)/cyclesPerSec<<" seconds. Or: "*/<<(endClockCycles-startClockCycles)/microsecondsPerCycle<<" micro seconds"<<std::endl;


		/*Restoring old runmask*/
		(*threadMask)=(unsigned)0x00000000;
		for(int i=_syspage_ptr->num_cpu; i>0; i--){
			(*threadMask)=(*threadMask)|(1<<(i-1));
		};
		DEBUG_PRINT_MSG_MY("INFO", "Restoring old runmask!");
		ThreadCtl(_NTO_TCTL_RUNMASK,(void *)threadMask);
		TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, 3,"[INFO]: Old runmask restored!");
		DEBUG_PRINT_MSG_MY("INFO", "Old runmask restored!");
		free(threadMask);
	}
	else if(userParametrsStruct.researchedFunctions==GetClockCycles){
		TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, 3,"[INFO]: Runmask is standard!");
		//ClockCycles is useless when thread is running on several processors.
		//So we need firstly to "block" thread on one several processor
		/*Setting new mask*/
		unsigned * threadMask=(unsigned *)malloc(sizeof(*threadMask)*RMSK_SIZE(_syspage_ptr->num_cpu));
		*threadMask=0x2; //let the code be run on only first CPU
		DEBUG_PRINT_MSG_MY("INFO", "Setting new runmask!");
		ThreadCtl(_NTO_TCTL_RUNMASK_GET_AND_SET, (void *)threadMask);
		TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, 3,"[INFO]: Runmask is changed!");
		DEBUG_PRINT_MSG_MY("INFO", "New runmask is set!");

		/*Measuring*/
		uint64_t start=ClockCycles();
		sleep(2);
		uint64_t end=ClockCycles();
		uint64_t cyclesPerSec=(SYSPAGE_ENTRY(qtime) -> cycles_per_sec);
		double microsecondsPerCycle=(SYSPAGE_ENTRY(qtime) -> cycles_per_sec)/1000000L;
		std::cout<<"[INFO]: Processor (one core)  has made: "<<end-start<<" cycles with speed: "<<cyclesPerSec<<" cycles per second"<<std::endl;
		std::cout<<"That means it have passed:"<<(end-start)/cyclesPerSec<<" seconds. Or: "<<(end-start)/microsecondsPerCycle<<" micro seconds"<<std::endl;


		/*Restoring old runmask*/
		(*threadMask)=(unsigned)0x00000000;
		for(int i=_syspage_ptr->num_cpu; i>0; i--){
			(*threadMask)=(*threadMask)|(1<<(i-1));
		};
		DEBUG_PRINT_MSG_MY("INFO", "Restoring old runmask!");
		ThreadCtl(_NTO_TCTL_RUNMASK,(void *)threadMask);
		TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, 3,"[INFO]: Old runmask restored!");
		DEBUG_PRINT_MSG_MY("INFO", "Old runmask restored!");
		free(threadMask);
	}

			//_clockperiod clcnew={T,0}, clcold, clcout;
		//ClockPeriod (CLOCK_REALTIME, &clcnew, &clcold, 0);
		//ClockPeriod (CLOCK_REALTIME, NULL, &clcout, 0);
		//uint64_t cps = SYSPAGE_ENTRY(qtime)->cycles_per_sec;
		return EXIT_SUCCESS;
}

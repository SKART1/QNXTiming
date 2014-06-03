/*
 * QNXTiming.hpp
 *
 *  Created on: 14.05.2014
 *      Author: Art
 */

#ifndef QNXTIMING_HPP_
#define QNXTIMING_HPP_
	#include <cstdlib>
	#include <iostream>
	#include <unistd.h>
	#include <inttypes.h>.h > 	//for uint64_t
	#include <unistd.h> 	//for getopt
	#include <stdlib.h>     //for atol
	#include <sys/syspage.h> //for SYSPAGE_ENTRY(qtime)

	#include <sys/neutrino.h>	//for ClockPeriod()

	#include <sys/neutrino.h> //for ClockCycles
	#include <inttypes.h>	//for ClockCycles

	#include <sys/trace.h>//for traceevent

	#include <time.h> //for clock_gettime

	#include <sys/neutrino.h> //for ClockTime()
typedef enum{
	SetGetClockPeriod,
	SetGetPOSIXTimeOfClock,
	SetGetQNXTimeOfClock,
	ClockAdjustFunc,
	GetClockCycles,
} ResearchedFunctions;

typedef struct{
	ResearchedFunctions researchedFunctions;
	//For ClockPeriod() function (SetGetClockPeriod);
	unsigned long int  newClockPeriod;
	//For clock_settime() function (SetGetPOSIXTimeOfClock)
	unsigned long int secondsFrom1970;
	//For ClockTime()  (SetGetQNXTimeOfClock)
	uint64_t nanosecAbsoluteTime;
	//For clock adjust (ClockAdjust)
	long nanosecondsAdjustInEveryTick;
} UserParametrsStruct;

#endif /* QNXTIMING_HPP_ */

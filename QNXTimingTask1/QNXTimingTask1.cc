#include <cstdlib>
#include <iostream>
#include <time.h>

#include <stdio.h> //For perror

#include <string.h>//For strlen

#include <sys/neutrino.h> //For ConnectAttach
#include <sys/netmgr.h>

#include <sys/neutrino.h> //For traceevent
#include <sys/trace.h>

#include <signal.h> //For sigaction

#include <unistd.h>//for usleep

#include <sys/neutrino.h> //for clockcycles
#include <inttypes.h>

#include <sys/syspage.h>


timespec startTimeDescriptorStruct;

/*------------------------------------------------------------------------------------*/
static void sigusr1Handler(int signo,siginfo_t *info, void *other) {
	timespec currentTimeDescriptorStruct;
	char buffEntering[50]="[INFO]: Entering SIGUSR1. Code is: ";
	char buffExiting[50]="[INFO]: Entering SIGUSR1. Code is: ";
	printf(buffEntering+strlen(buffEntering), "%d", info->si_code);
	printf(buffExiting+strlen(buffExiting), "%d", info->si_code);

	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, 1, buffEntering);
	if(clock_gettime( CLOCK_REALTIME, &currentTimeDescriptorStruct)==-1){
		perror("[ERROR]: clock_gettime");
	}
	else{
		if((currentTimeDescriptorStruct.tv_nsec-startTimeDescriptorStruct.tv_nsec)>0){
			std::cout<<"[INFO]: Got signal "<<info->si_code<< " Time passed: "<<((double)((currentTimeDescriptorStruct.tv_sec-startTimeDescriptorStruct.tv_sec)*1000000LL+(currentTimeDescriptorStruct.tv_nsec-startTimeDescriptorStruct.tv_nsec)/1000LL))/1000000LL<<" seconds"<<std::endl;
		}
		else{
			std::cout<<"[INFO]: Got signal "<<info->si_code<< " Time passed:"<<((double)((currentTimeDescriptorStruct.tv_sec-startTimeDescriptorStruct.tv_sec)*1000000LL+(currentTimeDescriptorStruct.tv_nsec-startTimeDescriptorStruct.tv_nsec)/1000LL))/1000000LL<<" seconds"<<std::endl;
		}
    };
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, 2, buffExiting);
}
/*------------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------*/
void closeAll(timer_t *timerMass){
	for(int i=0;i<8;i++){
		 timer_delete( timerMass[i] );
	}
}
/*------------------------------------------------------------------------*/

/*------------------------------------------------------------------------*/
/*Timer 0 Absolute. Single. Signal*/
int inline setTimerAbsoluteSingleSignal(timer_t *timerDescriptor){
	sigevent sigeventObject;
	itimerspec timeDescriptorStruct;
	timespec   currentTimeDescriptorStruct;
	struct sigaction act;

	//Register POSIX signal handler
	act.sa_sigaction   = &sigusr1Handler;
	act.sa_flags = 0;
	if (sigaction(SIGUSR1, &act, 0) < 0) {
		perror("[ERROR]: sigaction registering");
		return -1;
	}

	//Create structure with pulse. Priority is the same as in major thread.
	sigeventObject.sigev_notify = SIGEV_SIGNAL_CODE;
	sigeventObject.sigev_signo = SIGUSR1;
	sigeventObject.sigev_code=SI_MINAVAIL;


	//Create timer
	if(timer_create( CLOCK_REALTIME, &sigeventObject ,timerDescriptor)==-1){
		perror("[ERROR]: timer_create");
		return -1;
	}
	//Set timer mode
	//Fire in %CURRENT_TIME% + 1.5 sec
	if(clock_gettime( CLOCK_REALTIME, &currentTimeDescriptorStruct)==-1){
		perror("[ERROR]: clock_gettime");
		return -1;
	};
	timeDescriptorStruct.it_value.tv_sec = currentTimeDescriptorStruct.tv_sec+1;
	timeDescriptorStruct.it_value.tv_nsec=currentTimeDescriptorStruct.tv_nsec;

	//Single. Means not repeat
	timeDescriptorStruct.it_interval.tv_sec = NULL;
	timeDescriptorStruct.it_interval.tv_nsec = NULL;

	//Set new type
	timer_settime(*timerDescriptor, TIMER_ABSTIME , &timeDescriptorStruct, NULL);

	return 0;
}
/*------------------------------------------------------------------------*/


/*------------------------------------------------------------------------*/
/*Timer 1 Absolute. Single. Pulse*/
int inline setTimerAbsoluteSinglePulse(timer_t *timerDescriptor,int chid){
	sigevent sigeventObject;
	itimerspec timeDescriptorStruct;
	timespec   currentTimeDescriptorStruct;

	//Create structure with pulse. Priority is the same as in major thread.
	sigeventObject.sigev_notify = SIGEV_PULSE;
	sigeventObject.sigev_coid = ConnectAttach(ND_LOCAL_NODE, 0, chid, _NTO_SIDE_CHANNEL, 0);
	sigeventObject.sigev_priority = getprio(0);
	sigeventObject.sigev_code =_PULSE_CODE_MINAVAIL+1;

	//Create timer
	if(timer_create( CLOCK_REALTIME, &sigeventObject ,timerDescriptor)==-1){
		perror("[ERROR]: timer_create");
		return -1;
	}

	//Set timer mode
	//Fire in %CURRENT_TIME% + 1.5 sec
	if(clock_gettime( CLOCK_REALTIME, &currentTimeDescriptorStruct)==-1){
		perror("[ERROR]: clock_gettime");
		return -1;
	};
	timeDescriptorStruct.it_value.tv_sec =currentTimeDescriptorStruct.tv_sec+2;
	timeDescriptorStruct.it_value.tv_nsec=currentTimeDescriptorStruct.tv_nsec;

	//Single. Means not repeat
	timeDescriptorStruct.it_interval.tv_sec = NULL;
	timeDescriptorStruct.it_interval.tv_nsec = NULL;

	//Set new type
	timer_settime(*timerDescriptor, TIMER_ABSTIME , &timeDescriptorStruct, NULL);

	return 0;
}
/*------------------------------------------------------------------------*/

/*------------------------------------------------------------------------*/
/*Timer 2 Absolute. Repeatable. Signal*/
int inline setTimerAbsoluteRepeatableSignal(timer_t *timerDescriptor){
	sigevent sigeventObject;
	itimerspec timeDescriptorStruct;
	timespec   currentTimeDescriptorStruct;
	struct sigaction act;

	//Register POSIX signal handler
	act.sa_sigaction   = &sigusr1Handler;
	act.sa_flags = 0;
	if (sigaction(SIGUSR1, &act, 0) < 0) {
		perror("[ERROR]: sigaction registering");
		return -1;
	}

	//Create structure with pulse. Priority is the same as in major thread.
	sigeventObject.sigev_notify = SIGEV_SIGNAL_CODE;
	sigeventObject.sigev_signo = SIGUSR1;
	sigeventObject.sigev_priority = getprio(0);
	sigeventObject.sigev_code=SI_MINAVAIL+2;
	//SI_MAXAVAIL

	//Create timer
	if(timer_create( CLOCK_REALTIME, &sigeventObject ,timerDescriptor)==-1){
		perror("[ERROR]: timer_create");
		return -1;
	}
	//Set timer mode
	//Fire in %CURRENT_TIME% + 1.5 sec
	if(clock_gettime( CLOCK_REALTIME, &currentTimeDescriptorStruct)==-1){
		perror("[ERROR]: clock_gettime");
		return -1;
	};
	timeDescriptorStruct.it_value.tv_sec = currentTimeDescriptorStruct.tv_sec+3;
	timeDescriptorStruct.it_value.tv_nsec=currentTimeDescriptorStruct.tv_nsec;

	//Single. Means not repeat
	timeDescriptorStruct.it_interval.tv_sec = 3;
	timeDescriptorStruct.it_interval.tv_nsec = 0;

	//Set new type
	timer_settime(*timerDescriptor, TIMER_ABSTIME , &timeDescriptorStruct, NULL);

	return 0;
}
/*------------------------------------------------------------------------*/

/*------------------------------------------------------------------------*/
/*Timer 3 Absolute. Repeatable. Pulse*/
int inline setTimerAbsoluteRepeatablePulse(timer_t *timerDescriptor,int chid){
	sigevent sigeventObject;
	itimerspec timeDescriptorStruct;
	timespec   currentTimeDescriptorStruct;

	//Create structure with pulse. Priority is the same as in major thread.
	sigeventObject.sigev_notify = SIGEV_PULSE;
	sigeventObject.sigev_coid = ConnectAttach(ND_LOCAL_NODE, 0, chid, _NTO_SIDE_CHANNEL, 0);
	sigeventObject.sigev_priority = getprio(0);
	sigeventObject.sigev_code = _PULSE_CODE_MINAVAIL+3;

	//Create timer
	if(timer_create( CLOCK_REALTIME, &sigeventObject ,timerDescriptor)==-1){
		perror("[ERROR]: timer_create");
		return -1;
	}

	//Set timer mode
	//Fire in %CURRENT_TIME% + 1.5 sec
	if(clock_gettime( CLOCK_REALTIME, &currentTimeDescriptorStruct)==-1){
		perror("[ERROR]: clock_gettime");
		return -1;
	};
	timeDescriptorStruct.it_value.tv_sec = currentTimeDescriptorStruct.tv_sec+4;
	timeDescriptorStruct.it_value.tv_nsec=currentTimeDescriptorStruct.tv_nsec;

	//Single. Means not repeat
	timeDescriptorStruct.it_interval.tv_sec =  4;
	timeDescriptorStruct.it_interval.tv_nsec = 0;

	//Set new type
	timer_settime(*timerDescriptor, TIMER_ABSTIME , &timeDescriptorStruct, NULL);

	return 0;
}
/*------------------------------------------------------------------------*/


/*------------------------------------------------------------------------*/
/*Timer 4 Relative. Single. Signal*/
int inline setTimerRelativeSingleSignal(timer_t *timerDescriptor){
	sigevent sigeventObject;
	itimerspec timeDescriptorStruct;
	struct sigaction act;

	//Register POSIX signal handler
	act.sa_sigaction = &sigusr1Handler;
	act.sa_flags = 0;
	if (sigaction(SIGUSR1, &act, 0) < 0) {
		perror("[ERROR]: sigaction registering");
		return -1;
	}

	//Create structure with pulse. Priority is the same as in major thread.
	sigeventObject.sigev_notify = SIGEV_SIGNAL_CODE;
	sigeventObject.sigev_signo = SIGUSR1;
	sigeventObject.sigev_code=SI_MINAVAIL+4;


	//Create timer
	if(timer_create( CLOCK_REALTIME, &sigeventObject ,timerDescriptor)==-1){
		perror("[ERROR]: timer_create");
		return -1;
	}
	//Set timer mode

	timeDescriptorStruct.it_value.tv_sec = 5;
	timeDescriptorStruct.it_value.tv_nsec= 0;

	//Single. Means not repeat
	timeDescriptorStruct.it_interval.tv_sec =  NULL;
	timeDescriptorStruct.it_interval.tv_nsec = NULL;

	//Set new type
	timer_settime(*timerDescriptor, NULL , &timeDescriptorStruct, NULL);

	return 0;
}
/*------------------------------------------------------------------------*/

/*------------------------------------------------------------------------*/
/*Timer 5 Relative. Single. Pulse*/
int inline setTimerRelativeSinglePulse(timer_t *timerDescriptor,int chid){
	sigevent sigeventObject;
	itimerspec timeDescriptorStruct;

	//Create structure with pulse. Priority is the same as in major thread.
	sigeventObject.sigev_notify = SIGEV_PULSE;
	sigeventObject.sigev_coid = ConnectAttach(ND_LOCAL_NODE, 0, chid, _NTO_SIDE_CHANNEL, 0);
	sigeventObject.sigev_priority = getprio(0);
	sigeventObject.sigev_code = _PULSE_CODE_MINAVAIL+5;

	//Create timer
	if(timer_create( CLOCK_REALTIME, &sigeventObject ,timerDescriptor)==-1){
		perror("[ERROR]: timer_create");
		return -1;
	}

	//Set timer mode
	//Fire in %CURRENT_TIME% + 1.5 sec
	timeDescriptorStruct.it_value.tv_sec = 6;
	timeDescriptorStruct.it_value.tv_nsec= 0;

	//Single. Means not repeat
	timeDescriptorStruct.it_interval.tv_sec =  NULL;
	timeDescriptorStruct.it_interval.tv_nsec = NULL;

	//Set new type
	timer_settime(*timerDescriptor, NULL , &timeDescriptorStruct, NULL);

	return 0;
}
/*------------------------------------------------------------------------*/

/*------------------------------------------------------------------------*/
/*Timer 6 Relative. Repeatable. Signal*/
int inline setTimerRelativeRepeatableSignal(timer_t *timerDescriptor){
	sigevent sigeventObject;
	itimerspec timeDescriptorStruct;
	struct sigaction act;

	//Register POSIX signal handler
	act.sa_sigaction = &sigusr1Handler;
	act.sa_flags = 0;
	if (sigaction(SIGUSR1, &act, 0) < 0) {
		perror("[ERROR]: sigaction registering");
		return -1;
	}

	//Create structure with pulse. Priority is the same as in major thread.
	sigeventObject.sigev_notify = SIGEV_SIGNAL_CODE;
	sigeventObject.sigev_signo = SIGUSR1;
	sigeventObject.sigev_code=SI_MINAVAIL+6;


	//Create timer
	if(timer_create( CLOCK_REALTIME, &sigeventObject ,timerDescriptor)==-1){
		perror("[ERROR]: timer_create");
		return -1;
	}
	//Set timer mode

	timeDescriptorStruct.it_value.tv_sec = 7;
	timeDescriptorStruct.it_value.tv_nsec= 0;

	//Single. Means not repeat
	timeDescriptorStruct.it_interval.tv_sec =  7;
	timeDescriptorStruct.it_interval.tv_nsec = 0;

	//Set new type
	timer_settime(*timerDescriptor, NULL , &timeDescriptorStruct, NULL);

	return 0;
}
/*------------------------------------------------------------------------*/

/*------------------------------------------------------------------------*/
/*Timer 7 Relative. Repeatable. Pulse*/
int inline setTimerRelativeRepeatablePulse(timer_t *timerDescriptor,int chid){
	sigevent sigeventObject;
	itimerspec timeDescriptorStruct;

	//Create structure with pulse. Priority is the same as in major thread.
	sigeventObject.sigev_notify = SIGEV_PULSE;
	sigeventObject.sigev_coid = ConnectAttach(ND_LOCAL_NODE, 0, chid, _NTO_SIDE_CHANNEL, 0);
	sigeventObject.sigev_priority = getprio(0);
	sigeventObject.sigev_code = _PULSE_CODE_MINAVAIL+7;

	//Create timer
	if(timer_create( CLOCK_REALTIME, &sigeventObject ,timerDescriptor)==-1){
		perror("[ERROR]: timer_create");
		return -1;
	}

	//Set timer mode
	//Fire in %CURRENT_TIME% + 1.5 sec
	timeDescriptorStruct.it_value.tv_sec = 8;
	timeDescriptorStruct.it_value.tv_nsec= 0;

	//Single. Means not repeat
	timeDescriptorStruct.it_interval.tv_sec =  8;
	timeDescriptorStruct.it_interval.tv_nsec = 0;

	//Set new type
	timer_settime(*timerDescriptor, NULL , &timeDescriptorStruct, NULL);

	return 0;
}
/*------------------------------------------------------------------------*/


/*------------------------------------------------------------------------*/
int main(int argc, char *argv[]) {
	timer_t timerMass[8];
	int chid=-1;

	struct _pulse msgPulse;


	int rcvid=-1;
	timespec currentTimeDescriptorStruct;

	if(clock_gettime( CLOCK_REALTIME , &startTimeDescriptorStruct)==-1){
		perror("[ERROR]: clock_gettime");
	}



	/*Initialization*/
	if((chid = ChannelCreate(0))==-1){
		perror("[ERROR]: Creating channel");
		return EXIT_FAILURE;
	}

	/*Timer 0 Absolute. Single. Signal*/
	if(setTimerAbsoluteSingleSignal(&timerMass[0])==-1){
		closeAll(timerMass);
		return EXIT_FAILURE;
	}
	if(setTimerAbsoluteSinglePulse(&timerMass[1], chid)==-1){
		closeAll(timerMass);
		return EXIT_FAILURE;
	}
	if(setTimerAbsoluteRepeatableSignal(&timerMass[2])==-1){
		closeAll(timerMass);
		return EXIT_FAILURE;
	}
	if(setTimerAbsoluteRepeatablePulse(&timerMass[3], chid)==-1){
		closeAll(timerMass);
		return EXIT_FAILURE;
	}
	if(setTimerRelativeSingleSignal(&timerMass[4])==-1){
		closeAll(timerMass);
		return EXIT_FAILURE;
	}
	if(setTimerRelativeSinglePulse(&timerMass[5],chid)==-1){
		closeAll(timerMass);
		return EXIT_FAILURE;
	}
	if(setTimerRelativeRepeatableSignal(&timerMass[6])==-1){
		closeAll(timerMass);
		return EXIT_FAILURE;
	}
	if(setTimerRelativeRepeatablePulse(&timerMass[7],chid)==-1){
		closeAll(timerMass);
		return EXIT_FAILURE;
	}


	for (;;) {
		rcvid = MsgReceive(chid, &msgPulse, sizeof(msgPulse), NULL);
		if (rcvid == 0) {
			//we got a pulse
			if(clock_gettime( CLOCK_REALTIME, &currentTimeDescriptorStruct)==-1){
				perror("[ERROR]: clock_gettime");
			}
			else{
				if((currentTimeDescriptorStruct.tv_nsec-startTimeDescriptorStruct.tv_nsec)>0){
					std::cout<<"[INFO]: Got pulse number: "<<(int)msgPulse.code<<" Time passed: "<<((double)((currentTimeDescriptorStruct.tv_sec-startTimeDescriptorStruct.tv_sec)*1000000LL+(currentTimeDescriptorStruct.tv_nsec-startTimeDescriptorStruct.tv_nsec)/1000LL))/1000000LL<<" seconds"<<std::endl;
				}
				else{
					std::cout<<"[INFO]: Got pulse number: "<<(int)msgPulse.code<<" Time passed:"<<((double)((currentTimeDescriptorStruct.tv_sec-startTimeDescriptorStruct.tv_sec)*1000000LL+(currentTimeDescriptorStruct.tv_nsec-startTimeDescriptorStruct.tv_nsec)/1000LL))/1000000LL<<" seconds"<<std::endl;
				}
			};
	   };
	}
	return EXIT_SUCCESS;
}
/*------------------------------------------------------------------------*/

#include <cstdlib>
#include <iostream>
#include <sys/trace.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>

/*------------------------------------------------------------------------------------*/
static void sigusr1Handler(int signo,siginfo_t *info, void *other) {
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, 1, "[INFO]: Entering SIGUSR1 handler");
	//Check device availability (usb)
	if(access("/dev/io-usb/io-usb", F_OK )==-1){
		switch(errno){
		//Problems with access
		case EACCES:
			std::cerr<<"[WARNING]: File is not accessible!"<<std::endl;
			break;
		//File system do not support such type of operation
		case ENOSYS:
			std::cerr<<"[WARNING]: File system do not support this function!"<<std::endl;
			break;
		default:
			perror("[ERROR]: access");
			break;
		}
	};
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, 2, "[INFO]: Exiting SIGUSR1 handler");
}
/*------------------------------------------------------------------------------------*/


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

	timeDescriptorStruct.it_value.tv_sec = 2;
	timeDescriptorStruct.it_value.tv_nsec= 0;

	//Single. Means not repeat
	timeDescriptorStruct.it_interval.tv_sec =  2;
	timeDescriptorStruct.it_interval.tv_nsec = NULL;

	//Set new type
	timer_settime(*timerDescriptor, NULL , &timeDescriptorStruct, NULL);

	return 0;
}
/*------------------------------------------------------------------------*/


int main(int argc, char *argv[]) {
	timer_t timerDescriptor;
	setTimerRelativeSingleSignal(&timerDescriptor);

	for (;;) {
		pause();
	}
	return EXIT_SUCCESS;
}

#include <cstdlib>
#include <iostream>

/*------------------------------------------------------------------------------------*/
static void sigusr1Handler(int signo,siginfo_t *info, void *other) {
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, 1, "[INFO]: Entering SIGUSR1 handler");
	access("dev/F_OK);
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


int main(int argc, char *argv[]) {
	std::cout << "Welcome to the QNX Momentics IDE" << std::endl;
	return EXIT_SUCCESS;
}

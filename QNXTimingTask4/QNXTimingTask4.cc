#include <errno.h>
#include <gf/gf.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream.h>

#include <sys/neutrino.h>
#include <sys/trace.h>

#include <gf/gf.h>

#define KEYBOARD_IRQ 1

gf_display_t display;
struct sigevent event;

bool end=0;

/*------------------------------------------------------------------------------------*/
//SIGUSR1 handler
static void sigusr1Handler(int signo,siginfo_t *info, void *other) {
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, 1, "[INFO]: Entering SIGUSR1 handler");
	if(end==0){
		gf_display_set_dpms(display, GF_DPMS_OFF);
	}
	else{
		gf_display_set_dpms(display, GF_DPMS_ON);
	}
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, 1, "[INFO]: Exiting SIGUSR1 handler");
}
/*------------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------------*/
//Interrupt handler
const struct sigevent * intHandler(void *arg, int id){
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, 1, "[INFO]: Entering interrupt handler handler");
	end=1;
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, 1, "[INFO]: Exiting interrupt handler handler");
	return &event;
}
/*------------------------------------------------------------------------------------*/


/*------------------------------------------------------------------------*/
/*Timer Relative. Single. Signal*/
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
	timeDescriptorStruct.it_interval.tv_sec =  NULL;
	timeDescriptorStruct.it_interval.tv_nsec = NULL;

	//Set new type
	timer_settime(*timerDescriptor, NULL , &timeDescriptorStruct, NULL);

	return 0;
}
/*------------------------------------------------------------------------*/



int main(int argc, char *argv[]) {

	gf_dev_t gfx;

	timer_t timerDescriptor;
	int intDescr=-1;

	setTimerRelativeSingleSignal(&timerDescriptor); //Create timer to make monitor switch off

	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, 1, "[INFO]: Before gf_dev_attach");
	//Find device in device list
	gf_dev_attach(&gfx, GF_DEVICE_INDEX(0), NULL);
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, 1, "[INFO]: After gf_dev_attach");

	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, 1, "[INFO]: Before gf_display_attach");
	//Find display in devices
	gf_display_attach(&display, gfx, 0, NULL);
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, 1, "[INFO]: After gf_display_attach");

	//Register event - signal
	SIGEV_SIGNAL_INIT(&event, SIGUSR1);

	//Request for I/O privileges to interrupt attach
	if( ThreadCtl(_NTO_TCTL_IO, 0)==-1){
		perror("[ERROR]: ThreadCtl");
	};

	//Attach interrupt (function const struct sigevent * intHandler(void *arg, int id)
	if((intDescr= InterruptAttach( KEYBOARD_IRQ,	intHandler, NULL, 0, 0| _NTO_INTR_FLAGS_TRK_MSK))==-1){
		perror("[ERROR]: InterruptAttach");

	};

	//Wait while will be at least one key press
	while(end==0);

	//Give enough time to switch on display and not to end program to early(to go through interrupt handler, send signal, receive message, and process it)
	sleep(5);

	//Detach interrupt handler
	InterruptDetach(intDescr);
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, 1, "[INFO]: Exiting programm");
	return EXIT_SUCCESS;
}

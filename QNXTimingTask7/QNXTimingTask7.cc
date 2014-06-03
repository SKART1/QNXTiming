#include <cstdlib>
#include <iostream>
#include <stdio.h>
#include <sys/neutrino.h>
#include <inttypes.h>
#include <pthread.h>
#include <errno.h>
#include <sys/neutrino.h>
#include <sys/trace.h>//for traceevent

/*------------------------------------------------------------------------------------*/
void *endlessThread(void *arg) {
	std::cout<<"[INFO]: SECOND_THRED: Entering client!"<<std::endl;
	while(1);
	std::cout<<"[INFO]: SECOND_THRED: Exiting client!"<<std::endl;
	return NULL;
}
/*------------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------------*/
int main(int argc, char *argv[]) {
	std::cout<<"[INFO]: MAIN_THREAD: Program started"<<std::endl;
	pthread_t threadSecond;
	pthread_attr_t threadAttr;
	uint64_t        timeout;
	struct sigevent event;
	int             rval;
	int chid;

    char msg[10];
    int msgSize=sizeof(msg);

	//Creating thread
	if(pthread_attr_init(&threadAttr)==-1 || pthread_attr_setdetachstate(&threadAttr, PTHREAD_CREATE_JOINABLE)==-1){
		perror("[ERROR]: pthread_attr_init");
		return EXIT_FAILURE;
	}
	std::cout<<"[INFO]: MAIN_THREAD: Thread created"<<std::endl;
	if(pthread_create(&threadSecond, &threadAttr, endlessThread, NULL)==-1){
		perror("[ERROR]: pthread_create");
		return EXIT_FAILURE;
	};

	/*Setting timeout and trying to join thread*/
	// set up the event -- this can be done once
	// This or event.sigev_notify = SIGEV_UNBLOCK:
	SIGEV_UNBLOCK_INIT (&event);
	// set up for 1 second timeout
	timeout = 1LL * 1000000000LL;
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, 3,"[INFO]: Before TimerTimeout registering!");
	if(TimerTimeout(CLOCK_REALTIME, _NTO_TIMEOUT_JOIN, &event, &timeout, NULL)==-1){
		perror("[ERROR]: TimerTimeout creating");
		return EXIT_FAILURE;
	};
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, 3,"[INFO]: After TimerTimeout registering. Before pthread_join!");
    rval = pthread_join (threadSecond, NULL);
    if (rval == ETIMEDOUT) {
    	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, 3,"[INFO]: After pthread_join timeout!");
        printf ("[INFO]: MAIN_THREAD: Thread %d still running after 1 second!\n", threadSecond);
    }
    TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, 3,"[INFO]: After pthread_join!");


    /*Creating channel and trying receive*/
    chid=ChannelCreate(NULL);
	// set up the event -- this can be done once
	// This or event.sigev_notify = SIGEV_UNBLOCK:
	SIGEV_UNBLOCK_INIT (&event);
	// set up for 1 second timeout
	//timeout = 10LL * 1000000000LL;
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, 3,"[INFO]: Before TimerTimeout registering!");
    if(TimerTimeout(CLOCK_REALTIME, _NTO_TIMEOUT_RECEIVE, &event, &timeout, NULL)==-1){
		perror("[ERROR]: TimerTimeout creating");
		return EXIT_FAILURE;
	};
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, 3,"[INFO]: After TimerTimeout registering. Before MsgReceive!");
    MsgReceive(chid,msg,msgSize,NULL);
    if (rval == ETIMEDOUT) {
    	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, 3,"[INFO]: After MsgReceive timeout!");
		printf ("[INFO]: MAIN_THREAD: MsgRecieve do not received!\n");
	}
    TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, 3,"[INFO]: After MsgReceive!");


	std::cout << "[INFO]: MAIN_THREAD: Exiting" << std::endl;
	return EXIT_SUCCESS;
}
/*------------------------------------------------------------------------------------*/

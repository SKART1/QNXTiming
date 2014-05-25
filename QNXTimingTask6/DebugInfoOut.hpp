/*
 * DebugInfoOut.hpp
 *
 *  Created on: 10.05.2014
 *      Author: Art
 */

#ifndef DEBUGINFOOUT_HPP_
#define DEBUGINFOOUT_HPP_
#include <stdio.h> //for printf

#define DEBUG_PRINT_MY

#ifdef DEBUG_PRINT_MY
	#define DEBUG_PRINT_MSG_MY(typeInfoError, format)\
			printf("[%s]: %s\n",typeInfoError, format);
#else
	#define DEBUG_PRINT_MSG_MY(funcname, format, ...)
#endif



#endif /* DEBUGINFOOUT_HPP_ */

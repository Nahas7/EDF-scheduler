/*
 * FreeRTOS V202112.00
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#include <lpc21xx.h>

#include "GPIO.h"


extern int task1_startTime , task1_endTime , task1_totalTime ;
extern int task2_startTime , task2_endTime , task2_totalTime ;
extern int task3_startTime , task3_endTime , task3_totalTime ;
extern int task4_startTime , task4_endTime , task4_totalTime ;
extern int task5_startTime , task5_endTime , task5_totalTime ;
extern int task6_startTime , task6_endTime , task6_totalTime ;
extern int systemTime , CPU_Load ;

/*-----------------------------------------------------------
 * Application specific definitions.
 *
 * These definitions should be adjusted for your particular hardware and
 * application requirements.
 *
 * THESE PARAMETERS ARE DESCRIBED WITHIN THE 'CONFIGURATION' SECTION OF THE
 * FreeRTOS API DOCUMENTATION AVAILABLE ON THE FreeRTOS.org WEB SITE. 
 *
 * See http://www.freertos.org/a00110.html
 *----------------------------------------------------------*/


#define configUSE_PREEMPTION		1
#define configUSE_IDLE_HOOK			0
#define configUSE_TICK_HOOK			0
#define configCPU_CLOCK_HZ			( ( unsigned long ) 60000000 )	/* =12.0MHz xtal multiplied by 5 using the PLL. */
#define configTICK_RATE_HZ			( ( TickType_t ) 1000 )
#define configMAX_PRIORITIES		( 4 )
#define configMINIMAL_STACK_SIZE	( ( unsigned short ) 90 )
#define configTOTAL_HEAP_SIZE		( ( size_t ) 13 * 1024 )
#define configMAX_TASK_NAME_LEN		( 8 )
#define configUSE_TRACE_FACILITY	0
#define configUSE_16_BIT_TICKS		0
#define configIDLE_SHOULD_YIELD		1

#define configQUEUE_REGISTRY_SIZE 	0

/* Co-routine definitions. */
#define configUSE_CO_ROUTINES 		0
#define configMAX_CO_ROUTINE_PRIORITIES ( 2 )

/* Set the following definitions to 1 to include the API function, or zero
to exclude the API function. */

#define INCLUDE_vTaskPrioritySet		1
#define INCLUDE_uxTaskPriorityGet		1
#define INCLUDE_vTaskDelete				1
#define INCLUDE_vTaskCleanUpResources	0
#define INCLUDE_vTaskSuspend			1
#define INCLUDE_vTaskDelayUntil			1
#define INCLUDE_vTaskDelay				1

#define configUSE_EDF_SCHEDULER	1

#define configUSE_APPLICATION_TASK_TAG 1


/*For tracing*/
#define traceTASK_SWITCHED_IN()			do{\
																				if ((int)pxCurrentTCB->pxTaskTag == 1)\
																				{\
																					GPIO_write(PORT_0 , PIN2 , PIN_IS_HIGH);\
																					task1_startTime = T1TC ;\
																				}\
																				else if ((int)pxCurrentTCB->pxTaskTag == 2)\
																				{\
																					GPIO_write(PORT_0 , PIN3 , PIN_IS_HIGH);\
																					task2_startTime = T1TC ;\
																				}\
																				else if ((int)pxCurrentTCB->pxTaskTag == 3)\
																				{\
																					GPIO_write(PORT_0 , PIN4 , PIN_IS_HIGH);\
																					task3_startTime = T1TC ;\
																				}\
																				else if ((int)pxCurrentTCB->pxTaskTag == 4)\
																				{\
																					GPIO_write(PORT_0 , PIN5 , PIN_IS_HIGH);\
																					task4_startTime = T1TC ;\
																				}\
																				else if ((int)pxCurrentTCB->pxTaskTag == 5)\
																				{\
																					task5_startTime = T1TC ;\
																				}\
																				else if ((int)pxCurrentTCB->pxTaskTag == 6)\
																				{\
																					task6_startTime = T1TC ;\
																				}\
																			}while(0)
																				
																				
																				

#define traceTASK_SWITCHED_OUT()			do{\
																					if ((int)pxCurrentTCB->pxTaskTag == 1)\
																					{\
																						GPIO_write(PORT_0 , PIN2 , PIN_IS_LOW);\
																						task1_endTime = T1TC ;\
																						task1_totalTime += (task1_endTime - task1_startTime);\
																					}\
																					else if ((int)pxCurrentTCB->pxTaskTag == 2)\
																					{\
																						GPIO_write(PORT_0 , PIN3 , PIN_IS_LOW);\
																						task2_endTime = T1TC ;\
																						task2_totalTime += (task2_endTime - task2_startTime);\
																					}\
																					else if ((int)pxCurrentTCB->pxTaskTag == 3)\
																					{\
																						GPIO_write(PORT_0 , PIN4 , PIN_IS_LOW);\
																						task3_endTime = T1TC ;\
																						task3_totalTime += (task3_endTime - task3_startTime);\
																					}\
																					else if ((int)pxCurrentTCB->pxTaskTag == 4)\
																					{\
																						GPIO_write(PORT_0 , PIN5 , PIN_IS_LOW);\
																						task4_endTime = T1TC ;\
																						task4_totalTime += (task4_endTime - task4_startTime);\
																					}\
																					else if ((int)pxCurrentTCB->pxTaskTag == 5)\
																					{\
																						task5_endTime = T1TC ;\
																						task5_totalTime += (task5_endTime - task5_startTime);\
																					}\
																					else if ((int)pxCurrentTCB->pxTaskTag == 6)\
																					{\
																						task6_endTime = T1TC ;\
																						task6_totalTime += (task6_endTime - task6_startTime);\
																					}\
																					systemTime = T1TC ;\
																					CPU_Load = (( task1_totalTime + task2_totalTime + task3_totalTime + task4_totalTime + task5_totalTime + task6_totalTime )/(float)systemTime )*100 ;\
																				}while(0)																	
																			



#endif /* FREERTOS_CONFIG_H */

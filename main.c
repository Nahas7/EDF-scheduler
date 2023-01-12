/*
 * FreeRTOS Kernel V10.2.0
 * Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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

/* 
	NOTE : Tasks run in system mode and the scheduler runs in Supervisor mode.
	The processor MUST be in supervisor mode when vTaskStartScheduler is 
	called.  The demo applications included in the FreeRTOS.org download switch
	to supervisor mode prior to main being called.  If you are not using one of
	these demo application projects then ensure Supervisor mode is used.
*/


/*
 * Creates all the demo application tasks, then starts the scheduler.  The WEB
 * documentation provides more details of the demo application tasks.
 * 
 * Main.c also creates a task called "Check".  This only executes every three 
 * seconds but has the highest priority so is guaranteed to get processor time.  
 * Its main function is to check that all the other tasks are still operational.
 * Each task (other than the "flash" tasks) maintains a unique count that is 
 * incremented each time the task successfully completes its function.  Should 
 * any error occur within such a task the count is permanently halted.  The 
 * check task inspects the count of each task to ensure it has changed since
 * the last time the check task executed.  If all the count variables have 
 * changed all the tasks are still executing error free, and the check task
 * toggles the onboard LED.  Should any task contain an error at any time 
 * the LED toggle rate will change from 3 seconds to 500ms.
 *
 */

/* Standard includes. */
#include <stdlib.h>
#include <stdio.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "lpc21xx.h"
#include "semphr.h"


/* Peripheral includes. */
#include "serial.h"
#include "GPIO.h"


/*-----------------------------------------------------------*/

/* Constants to setup I/O and processor. */
#define mainBUS_CLK_FULL	( ( unsigned char ) 0x01 )

/* Constants for the ComTest demo application tasks. */
#define mainCOM_TEST_BAUD_RATE	( ( unsigned long ) 115200 )


/*
 * Configure the processor for use with the Keil demo board.  This is very
 * minimal as most of the setup is managed by the settings in the project
 * file.
 */
static void prvSetupHardware( void );
/*-----------------------------------------------------------*/

/*Queues handlers*/
QueueHandle_t xQueue1	;
QueueHandle_t	xQueue2	;
QueueHandle_t	xQueue3	;

/*Task handlers*/
TaskHandle_t	Button1_Task = NULL ;
TaskHandle_t	Button2_Task = NULL ;
TaskHandle_t	Uart_Task = NULL ;
TaskHandle_t	Periodic_Task = NULL ;
TaskHandle_t	Load_1 = NULL ;
TaskHandle_t Load_2 = NULL;


typedef struct
{
	unsigned char EdgeB1 ;
	unsigned char EdgeB2 ;
	signed char PeriodicT[6];
}msg_t;

msg_t Messages ;

int task1_startTime = 0 , task1_endTime = 0 , task1_totalTime ;
int task2_startTime = 0 , task2_endTime = 0 , task2_totalTime ;
int task3_startTime = 0 , task3_endTime = 0 , task3_totalTime ;
int task4_startTime = 0 , task4_endTime = 0 , task4_totalTime ;
int task5_startTime = 0 , task5_endTime = 0 , task5_totalTime ;
int task6_startTime = 0 , task6_endTime = 0 , task6_totalTime ;
int systemTime , CPU_Load ;

void ReadButton1_Task( void * pvParameters )
{
	unsigned char i = 0 ; 		/*Counter*/
	pinState_t current_s = PIN_IS_LOW , pervious_s = PIN_IS_LOW;	/*States of the button 1*/
	vTaskSetApplicationTaskTag( NULL, ( void * ) 1 );
	
	for( ;; )
    {
				/*initialize states*/
				pervious_s = current_s ;
				
				/*read the current state of B1*/
        current_s = GPIO_read(PORT_0 , PIN0 );
				
				/*if button B1 has a rising edgw*/
				if ( (current_s == PIN_IS_HIGH) && (pervious_s == PIN_IS_LOW) )
				{
					/*rising edge*/
					Messages.EdgeB1 = 0 ; 
				}
				else if ( (current_s == PIN_IS_LOW) && (pervious_s == PIN_IS_HIGH) )
				{
					/*flalling edge*/
					Messages.EdgeB1 = 1;
				}
				else 
				{
					/*No change*/
					Messages.EdgeB1 = 2;
				}
				
				xQueueSend( xQueue1, &Messages,( TickType_t ) 10);
				vTaskDelay(200);
    }

}
void ReadButton2_Task( void * pvParameters )
{
	unsigned char i = 0 ; 		/*Counter*/
	pinState_t current_s = PIN_IS_LOW , pervious_s = PIN_IS_LOW;	/*States of the button 2*/
	vTaskSetApplicationTaskTag( NULL, ( void * ) 2 );
	
	for( ;; )
    {
				/*initialize states*/
				pervious_s = current_s ;
				
				/*read the current state of B1*/
        current_s = GPIO_read(PORT_0 , PIN1 );
				
				/*if button B1 has a rising edgw*/
				if ( (current_s == PIN_IS_HIGH) && (pervious_s == PIN_IS_LOW) )
				{
					/*rising edge*/
					Messages.EdgeB2 = 0;
				}
				else if ( (current_s == PIN_IS_LOW) && (pervious_s == PIN_IS_HIGH) )
				{
					/*flalling edge*/
					Messages.EdgeB2 = 1;
				}
				else 
				{
					/*No change*/
					Messages.EdgeB2 = 2;
				}
				xQueueSend( xQueue1, &Messages,( TickType_t ) 10);
				vTaskDelay(200);
    }
}	

void PeriodicTransmission_Task( void * pvParameters )
{
	signed char Tx[6]={"Hello\n"}; 		/*periodic message*/
	signed char i;														/*counter*/
	vTaskSetApplicationTaskTag( NULL, ( void * ) 3 );
	
	for(;;)
	{	
		/*sending the message*/
		for( i=0 ; i<6 ; i++)
		{
			Messages.PeriodicT[i] = Tx[i];
		}
		xQueueSend( xQueue1, &Messages,( TickType_t ) 10);
		vTaskDelay(100);
	}	
}


void UART_Task( void * pvParameters )
{
	unsigned char i;
	signed char msgPeriodic[6];
	vTaskSetApplicationTaskTag( NULL, ( void * ) 4 );
	
	for(;;)
	{
			/*Recieving Periodic word*/
		xQueueReceive(xQueue1,&Messages,( TickType_t ) 10 );
		for(i=0 ; i<6 ;i++)
		{
			msgPeriodic[i]=Messages.PeriodicT[i];
		}
		vSerialPutString(msgPeriodic,6);
		vTaskDelay(100);
		
		/*Recieving Periodic word & make sure that there's a new state of the button1*/
		if ( (xQueueReceive(xQueue1 , &Messages , ( TickType_t ) 10 ) == pdPASS) && Messages.EdgeB1!= 2)
		{
			if ( Messages.EdgeB1 == 0)
			{
				/*Rising Edge*/
				vSerialPutString((signed char*)"Rising Edge B1\n" , 16);
			}
			else if (Messages.EdgeB1 == 1)
			{
				/*Falling edge*/
				vSerialPutString((signed char*)"Falling Edge B1\n" , 16);
			}
		}
		
		/*Recieving Periodic word & make sure that there's a new state of the button2*/
		if ( (xQueueReceive(xQueue1 , &Messages , ( TickType_t ) 10 ) == pdPASS) && Messages.EdgeB2!= 2)
		{
			if ( Messages.EdgeB2 == 0)
			{
				/*Rising Edge*/
				vSerialPutString((signed char*)"Rising Edge B2\n" , 16);
			}
			else if (Messages.EdgeB2 == 1)
			{
				/*Falling Edge*/
				vSerialPutString((signed char*)"Falling Edge B2\n" , 16);
			}
		}		
	}
}


void Load_1_Simulation( void * pvParameters )
{
		TickType_t xLastWakeTime = xTaskGetTickCount();
		int i = 0;
		int Period = 12000*5; /* (XTAL / 1000U)*time_in_ms  */
		vTaskSetApplicationTaskTag( NULL, ( void * ) 5 );
	
    for( ;; )
    {
			for(i = 0; i <= Period; i++)
			{
				
			}
			vTaskDelayUntil( &xLastWakeTime , 10);
    }
}

/* Task to be created. */
void Load_2_Simulation( void * pvParameters )
{
		TickType_t xLastWakeTime = xTaskGetTickCount();
		int i = 0;
		int Period = 12000*12; /* (XTAL / 1000U)*time_in_ms  */
		vTaskSetApplicationTaskTag( NULL, ( void * ) 6 );
	
    for( ;; )
    {
			for(i = 0; i <= Period; i++)
			{
				
			}
			vTaskDelayUntil( &xLastWakeTime , 100);
    }
}

/*
 * Application entry point:
 * Starts all the other tasks, then starts the scheduler. 
 */
int main( void )
{
	/* Setup the hardware for use with the Keil demo board. */
	prvSetupHardware();
	
    /* Create Tasks here */
	xQueue1 = xQueueCreate( 16, sizeof( unsigned char ) );
	

 xTaskPeriodicCreate(
                    ReadButton1_Task,       /* Function that implements the task. */
                    "Button1",          /* Text name for the task. */
                    100,      /* Stack size in words, not bytes. */
                    ( void * ) 0,    /* Parameter passed into the task. */
                    2,/* Priority at which the task is created. */
                    &Button1_Task
											,50);      /* Used to pass out the created task's handle. */
										
xTaskPeriodicCreate(
                    ReadButton2_Task,       /* Function that implements the task. */
                    "Button2",          /* Text name for the task. */
                    100,      /* Stack size in words, not bytes. */
                    ( void * ) 0,    /* Parameter passed into the task. */
                    2,/* Priority at which the task is created. */
                    &Button2_Task 
										,50 );      /* Used to pass out the created task's handle. */									
				
xTaskPeriodicCreate(
                    UART_Task,       /* Function that implements the task. */
                    "Recieve",          /* Text name for the task. */
                    100,      /* Stack size in words, not bytes. */
                    ( void * ) 0,    /* Parameter passed into the task. */
                    1,/* Priority at which the task is created. */
                    &Uart_Task
											,100);      /* Used to pass out the created task's handle. */
										

xTaskPeriodicCreate(
                    PeriodicTransmission_Task,       /* Function that implements the task. */
                    "Tx",          /* Text name for the task. */
                    100,      /* Stack size in words, not bytes. */
                    ( void * ) 0,    /* Parameter passed into the task. */
                    1,/* Priority at which the task is created. */
                    &Periodic_Task 
										,20 );      /* Used to pass out the created task's handle. */				


 xTaskPeriodicCreate(
                    Load_1_Simulation,       /* Function that implements the task. */
                    "Button_1_Monitor Task",          /* Text name for the task. */
                    100,      /* Stack size in words, not bytes. */
                    ( void * ) 0,    /* Parameter passed into the task. */
                    1,/* Priority at which the task is created. */
                    &Load_1,
										10);      /* Used to pass out the created task's handle. */										

										
  xTaskPeriodicCreate(
                    Load_2_Simulation,       /* Function that implements the task. */
                    "Button_1_Monitor Task",          /* Text name for the task. */
                    100,      /* Stack size in words, not bytes. */
                    ( void * ) 0,    /* Parameter passed into the task. */
                    1,/* Priority at which the task is created. */
                    &Load_2,
										100);      /* Used to pass out the created task's handle. */										


										
										
	/* Now all the tasks have been started - start the scheduler.

	NOTE : Tasks run in system mode and the scheduler runs in Supervisor mode.
	The processor MUST be in supervisor mode when vTaskStartScheduler is 
	called.  The demo applications included in the FreeRTOS.org download switch
	to supervisor mode prior to main being called.  If you are not using one of
	these demo application projects then ensure Supervisor mode is used here. */
	vTaskStartScheduler();

	/* Should never reach here!  If you do then there was not enough heap
	available for the idle task to be created. */
	for( ;; );
}
/*-----------------------------------------------------------*/

void timerReset (void)
{
	T1TCR |= 0x2 ;
	T1TCR &= ~0x2;
}

static void configTimer(void)
{
	T1PR = 1000 ;
	T1TCR |= 0x1 ;
}

static void prvSetupHardware( void )
{
	/* Perform the hardware setup required.  This is minimal as most of the
	setup is managed by the settings in the project file. */

	/* Configure UART */
	xSerialPortInitMinimal(mainCOM_TEST_BAUD_RATE);

	/* Configure GPIO */
	GPIO_init();
	
	/*Config Timer and read T1TC */
	configTimer();
	
	/* Setup the peripheral bus to be the same as the PLL output. */
	VPBDIV = mainBUS_CLK_FULL;
}
/*-----------------------------------------------------------*/



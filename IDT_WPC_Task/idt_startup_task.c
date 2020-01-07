/***********************************************************************************************************************
* File Name    : idt_startup_task.c
* Version      : CodeGenerator for RL78/F13 V2.03.03.01 [28 Oct 2018]
* Device(s)    : R5F10BMG
* Tool-Chain   : CCRL
* Description  : This file is wpc task function.
* Creation Date: 2019/9/4
* Author	   : Daniel.Hsu
***********************************************************************************************************************/
/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include "r_cg_cgc.h"
#include "r_cg_adc.h"
#include "r_cg_port.h"
#include "r_cg_serial.h"
#include "r_cg_timer.h"
/* Start user code for include. Do not edit comment generated here */
#include "wpc_application.h"
#include "acc_task.h"
#include "buzzer_task.h"
#include "led_task.h"
#include "power_sw_task.h"
#include "door_task.h"
#include "trunk_task.h"
#include "p9261_read_task.h"
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"
/***********************************************************************************************************************
Locate variables and functions
***********************************************************************************************************************/
unsigned char IDT_WPC_Startup_Process(void);
void Door_Trunk_Status_Detect_Process(void);
/***********************************************************************************************************************
* Function Name: IDT_WPC_Startup_Task
* Description  : This function is IDT_WPC_Startup_Task function.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void IDT_WPC_Startup_Task(void)
{
	if(IDT_WPC_State.Startup_Task_Flag==TRUE)
	{
		IDT_WPC_State.Startup_Task_Flag = FALSE;
		if((CHARGE_LED!=LEVEL_LOW)||(ERROR_LED!=LEVEL_LOW))
		{
			CHARGE_STATE_LED_OFF;						// charger led off //
		}
		IDT_WPC_TASK = IDT_WPC_STARTUP_TASK;
	}
	Door_Trunk_Status_Detect_Process();
	IDT_WPC_TASK = IDT_WPC_Startup_Process();
}
/***********************************************************************************************************************
* Function Name: IDT_WPC_Startup_Process
* Description  : This function is IDT_WPC_Startup_Process function.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
unsigned char IDT_WPC_Startup_Process(void)					// --- In Tx state 00 --- //
{
	if(ACC_Status.Active_On_Flag==TRUE)
	{
		if(Power_Sw.Active_Flag==TRUE)
		{
			if((Door_Status.Door_Open_Flag==FALSE)&&(Trunk_Status.Trunk_Open_Flag==FALSE))
			{
				/*if(P9261_RegMessage.Over_Temperature_Alarm_Status==TRUE)
				{
					P9261_RegMessage.Over_Temperature_Alarm_Status = FALSE;
					IDT_WPC_State.Over_Temperature_Alarm_Task_Flag = TRUE;
					IDT_WPC_NEXT_TASK = IDT_WPC_OVERTEMPERATURE_TASK;
				}*/
				if(P9261_RegMessage.Idle_Status==TRUE)					// Tx State 01 //
				{
					P9261_RegMessage.Idle_Status = FALSE;
					IDT_WPC_State.Idle_Task_Flag = TRUE;
					IDT_WPC_NEXT_TASK = IDT_WPC_IDLE_TASK;
				}
				else if(P9261_RegMessage.Tx_Under_Voltage_Limit_Status==TRUE)	// Tx Status Tx_Under_Voltage_Limit 0x14 //
				{
					P9261_RegMessage.Tx_Under_Voltage_Limit_Status = FALSE;
					IDT_WPC_State.Tx_Under_Voltage_Limit_Task_Flag = TRUE;	// reset p9261 //
					IDT_WPC_NEXT_TASK = IDT_WPC_TX_UNDER_VOLTAGE_LIMIT_TASK;
				}
				else if(P9261_RegMessage.Analog_Ping_Status==TRUE)		// Tx State 02 //
				{
					P9261_RegMessage.Analog_Ping_Status = FALSE;
					IDT_WPC_State.Analog_Ping_Task_Flag = TRUE;
					IDT_WPC_NEXT_TASK = IDT_WPC_ANALOG_PING_TASK;
				}
				else if(P9261_RegMessage.Q_Measurement_Status==TRUE)	// Tx State 03 //
				{
					P9261_RegMessage.Q_Measurement_Status = FALSE;
					IDT_WPC_State.Q_Measurement_Task_Flag = TRUE;
					IDT_WPC_NEXT_TASK = IDT_WPC_Q_MEASUREMENT_TASK;
				}
				else if(P9261_RegMessage.Digital_Ping_Status==TRUE)		// Tx State 04 //
				{
					P9261_RegMessage.Digital_Ping_Status = FALSE;
					IDT_WPC_State.Digital_Ping_Task_Flag = TRUE;
					IDT_WPC_NEXT_TASK = IDT_WPC_DIGITAL_PING_TASK;
				}
				else if(P9261_RegMessage.Configuration_Status==TRUE)	// Tx State 07 //20191220//
				{
					P9261_RegMessage.Configuration_Status = FALSE;
					IDT_WPC_State.Configuration_Task_Flag = TRUE;
					IDT_WPC_NEXT_TASK = IDT_WPC_CONFIGURATION_TASK;
				}
				else
				{
					IDT_WPC_NEXT_TASK = IDT_WPC_STARTUP_TASK;
				}
			}
			else
			{
				IDT_WPC_State.EUT_Sleep_Task_Flag = TRUE;
				IDT_WPC_NEXT_TASK = IDT_WPC_EUT_SLEEP_TASK;
			}
		}
		else
		{
			IDT_WPC_State.Power_Sw_Off_Task_Flag = TRUE;
			IDT_WPC_NEXT_TASK = IDT_WPC_POWER_SW_OFF_TASK;
		}
	}
	else
	{
		Phone_Placement_Detect();	// phone forget detect //
		WPC_Function_Status.ACC_Off_Dly3_5T_Flag = TRUE;
		IDT_WPC_State.Cellphone_Left_Alert_Task_Flag = TRUE;
		IDT_WPC_NEXT_TASK = IDT_WPC_CELLPHONE_LEFT_ALERT_TASK;
	}
	return IDT_WPC_NEXT_TASK;
}
/***********************************************************************************************************************
* Function Name: Door_Trunk_Status_Detect_Process
* Description  : This function is Door_Trunk_Status_Detect_Process function.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void Door_Trunk_Status_Detect_Process(void)
{
	if((Door_Status.Door_Open_Flag==FALSE)&&(Trunk_Status.Trunk_Open_Flag==FALSE))
	{
		if(CHARGE_EN==LEVEL_LOW)
		{
			CHARGE_ENABLE;	
		}
	}
}

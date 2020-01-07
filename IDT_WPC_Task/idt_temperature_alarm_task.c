/***********************************************************************************************************************
* File Name    : idt_temperature_alarm_task.c
* Version      : CodeGenerator for RL78/F13 V2.03.03.01 [28 Oct 2018]
* Device(s)    : R5F10BMG
* Tool-Chain   : CCRL
* Description  : This file is wpc task function.
* Creation Date: 2019/10/7
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
unsigned char IDT_Temperature_Alarm_Process(void);
/***********************************************************************************************************************
* Function Name: IDT_Temperature_Alarm_Task
* Description  : This function is IDT_Temperature_Alarm_Task function.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void IDT_Temperature_Alarm_Task(void)
{
	if(IDT_WPC_State.Over_Temperature_Alarm_Task_Flag==TRUE)
	{
		IDT_WPC_State.Over_Temperature_Alarm_Task_Flag = FALSE;
		WPC_Function_Status.Temperature_Detect_Start_Flag = FALSE;
		//-----------------------------------------------------//
		#ifdef _USED_P9261_INTERNAL_TE PERATURE_STATUS_
		
		#else
				CHARGE_DISABLE;
				CHARGE_STATE_LED_OFF;
				Over_Temperature_Reset_Time = DELAY_3S;
				LED_Status.Error_LED_Trigger_Flag = TRUE;
				Buzzer_State.Error_Buzzer_Trigger_Flag = TRUE;
		#endif
		//-----------------------------------------------------//
		IDT_WPC_TASK = IDT_WPC_OVERTEMPERATURE_TASK;
	}
	IDT_WPC_TASK = IDT_Temperature_Alarm_Process();
}
/***********************************************************************************************************************
* Function Name: IDT_Temperature_Alarm_Process
* Description  : This function is IDT_Temperature_Alarm_Process function.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
unsigned char IDT_Temperature_Alarm_Process(void)
{
	//-----------------------------------------------------------------------------------------------//
	#ifdef _USED_P9261_INTERNAL_TEMPERATURE_STATUS_
	//-----------------------------------------------------------------------------------------------//
		if(ACC_Status.Active_On_Flag==TRUE)
		{
			if(Power_Sw.Active_Flag==TRUE)
			{
				if((Door_Status.Door_Open_Flag==FALSE)&&(Trunk_Status.Trunk_Open_Flag==FALSE))
				{
					if(P9261_RegMessage.Idle_Status==TRUE)					// Tx State 01 //
					{
						P9261_RegMessage.Idle_Status = FALSE;
						IDT_WPC_State.Idle_Task_Flag = TRUE;
						IDT_WPC_NEXT_TASK = IDT_WPC_IDLE_TASK;
					}
					else if(P9261_RegMessage.Remove_Power_Status==TRUE)
					{
						P9261_RegMessage.Remove_Power_Status = FALSE;
						IDT_WPC_State.Remove_Power_Task_Flag = TRUE;
						IDT_WPC_NEXT_TASK = IDT_WPC_REMOVE_POWER_TASK;
					}
					else
					{
						IDT_WPC_NEXT_TASK = IDT_WPC_OVERTEMPERATURE_TASK;
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
	//-----------------------------------------------------------------------------------------------//
	#else
	//-----------------------------------------------------------------------------------------------//
		if(Over_Temperature_Reset_Time==CLEAR)
		{
			if((Buzzer_State.Active_End_Flag==TRUE)&&(LED_Status.Alarm_LED_Active_End_Flag==TRUE))
			{
				CHARGE_EN = LEVEL_HIGH;
				IDT_WPC_State.Power_Sw_Off_Task_Flag = TRUE;
				IDT_WPC_NEXT_TASK = IDT_WPC_POWER_SW_OFF_TASK;
			}
			else
			{
				IDT_WPC_NEXT_TASK = IDT_WPC_OVERTEMPERATURE_TASK;
			}
		}
		else
		{
			IDT_WPC_NEXT_TASK = IDT_WPC_OVERTEMPERATURE_TASK;
		}
	//-----------------------------------------------------------------------------------------------//
	#endif
	//-----------------------------------------------------------------------------------------------//
	return IDT_WPC_NEXT_TASK;
}
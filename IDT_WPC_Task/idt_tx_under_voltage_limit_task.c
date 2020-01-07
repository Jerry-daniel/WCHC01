/***********************************************************************************************************************
* File Name    : idt_tx_under_voltage_limit_task.c
* Version      : CodeGenerator for RL78/F13 V2.03.03.01 [28 Oct 2018]
* Device(s)    : R5F10BMG
* Tool-Chain   : CCRL
* Description  : This file is wpc task function.
* Creation Date: 2019/11/11
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
unsigned char IDT_TX_Under_Voltage_Limit_Process(void);
/***********************************************************************************************************************
* Function Name: IDT_Other_Eof_Pwr_Rx_Alarm_Task
* Description  : This function is IDT_Other_Eof_Pwr_Rx_Alarm_Task function.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void IDT_TX_Under_Voltage_Limit_Task(void)
{
	if(IDT_WPC_State.Tx_Under_Voltage_Limit_Task_Flag==TRUE)
	{
		IDT_WPC_State.Tx_Under_Voltage_Limit_Task_Flag = FALSE;	
		CHARGE_DISABLE;			/* stop power charging */
		UVOL_Wait_Time = 1000;
		IDT_WPC_TASK = IDT_WPC_TX_UNDER_VOLTAGE_LIMIT_TASK;
	}
	IDT_WPC_TASK = IDT_TX_Under_Voltage_Limit_Process();
}
/***********************************************************************************************************************
* Function Name: IDT_Other_Eof_Pwr_Rx_Alarm_Task
* Description  : This function is IDT_Other_Eof_Pwr_Rx_Alarm_Task function.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
unsigned char IDT_TX_Under_Voltage_Limit_Process(void)
{
	if(ACC_Status.Active_On_Flag==TRUE)
	{
		if(Power_Sw.Active_Flag==TRUE)
		{
			if((Door_Status.Door_Open_Flag==FALSE)&&(Trunk_Status.Trunk_Open_Flag==FALSE))
			{
				if(UVOL_Wait_Time==CLEAR)
				{
					CHARGE_ENABLE;					// IDT system charge power active //
					IDT_WPC_State.Startup_Task_Flag = TRUE;
					IDT_WPC_NEXT_TASK = IDT_WPC_STARTUP_TASK;
				}
				else
				{
					IDT_WPC_NEXT_TASK = IDT_WPC_TX_UNDER_VOLTAGE_LIMIT_TASK;
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
			Reset_Buzzer_LED_State();
			IDT_WPC_State.Power_Sw_Off_Task_Flag = TRUE;
			IDT_WPC_NEXT_TASK = IDT_WPC_POWER_SW_OFF_TASK;
		}
	}
	else
	{
		Buzzer_Disable();
		Alarm_LED_Disable();
		Phone_Placement_Detect();	// phone forget detect //
		WPC_Function_Status.ACC_Off_Dly3_5T_Flag = TRUE;
		IDT_WPC_State.Cellphone_Left_Alert_Task_Flag = TRUE;
		IDT_WPC_NEXT_TASK = IDT_WPC_CELLPHONE_LEFT_ALERT_TASK;
	}
	return IDT_WPC_NEXT_TASK;
}

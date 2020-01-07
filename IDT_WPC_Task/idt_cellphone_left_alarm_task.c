/***********************************************************************************************************************
* File Name    : idt_cellphone_left_alarm_task.c
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
unsigned char IDT_Cellphone_Left_Alert_Process(void);
/***********************************************************************************************************************
* Function Name: IDT_Cellphone_Left_Alert_Task
* Description  : This function is IDT_Cellphone_Left_Alert_Task function.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void IDT_Cellphone_Left_Alert_Task(void)
{
	if(IDT_WPC_State.Cellphone_Left_Alert_Task_Flag==TRUE)
	{
		IDT_WPC_State.Cellphone_Left_Alert_Task_Flag = FALSE;
		//--------------------------------------------------//
		WPC_Function_Status.Temperature_Detect_Start_Flag = FALSE;
		if(WPC_Function_Status.Phone_Forget_Flag==TRUE)
		{
			WPC_Function_Status.Phone_Forget_Flag = FALSE;
			Buzzer_State.Forget_Buzzer_Tigger_Flag = TRUE;
		}
		//--------------------------------------------------//
		IDT_WPC_TASK = IDT_WPC_CELLPHONE_LEFT_ALERT_TASK;	
	}
	IDT_WPC_TASK = IDT_Cellphone_Left_Alert_Process();
}
/***********************************************************************************************************************
* Function Name: IDT_Cellphone_Left_Alert_Task
* Description  : This function is IDT_Cellphone_Left_Alert_Task function.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
unsigned char IDT_Cellphone_Left_Alert_Process(void)
{
	if(ACC_Status.Active_On_Flag==FALSE)
	{
		if(ACC_Off_Reset_Time==CLEAR)
		{
			WPC_Function_Status.ACC_Off_Dly3_5T_Flag = FALSE;
			ACC_ON_RL78 = LEVEL_LOW;	// Control Q33 shuts down : power off //
			ACC_Off_Reset_Time = DELAY_3_5S;
			IDT_WPC_State.Acc_Off_Task_Flag = TRUE;
			IDT_WPC_NEXT_TASK = IDT_WPC_ACC_OFF_TASK;
		}
		else
		{
			IDT_WPC_NEXT_TASK = IDT_WPC_CELLPHONE_LEFT_ALERT_TASK;	
		}
	}
	else
	{
		ACC_Off_Reset_Time = DELAY_3_5S;
		//IDT_WPC_State.Acc_Off_Task_Flag = TRUE;
		WPC_Function_Status.ACC_Off_Dly3_5T_Flag = FALSE;
		Buzzer_State.Forget_Buzzer_Tigger_Flag = FALSE;
		Buzzer_Disable();
		IDT_WPC_NEXT_TASK = IDT_WCP_POWER_TRANSFER_TASK;//IDT_WPC_ACC_OFF_TASK;
	}
	return IDT_WPC_NEXT_TASK;	
}

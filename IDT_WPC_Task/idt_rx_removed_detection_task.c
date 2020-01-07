/***********************************************************************************************************************
* File Name    : idt_rx_removed_detection_task.c
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
unsigned char IDT_WPC_Rx_Removed_Detection_Process(void);
/***********************************************************************************************************************
* Function Name: IDT_WPC_Rx_Removed_Detection_Task
* Description  : This function is IDT_WPC_Rx_Removed_Detection_Task function.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void IDT_WPC_Rx_Removed_Detection_Task(void)
{
	if(IDT_WPC_State.Rx_Removed_Detection_Task_Flag==TRUE)
	{
		IDT_WPC_State.Rx_Removed_Detection_Task_Flag = FALSE;
		if((CHARGE_LED!=LEVEL_LOW)||(ERROR_LED!=LEVEL_LOW))
		{
			CHARGE_STATE_LED_OFF;						// charger led off //
		}
		IDT_WPC_TASK = IDT_WPC_RX_REMOVED_DETECTION_TASK;
	}
	IDT_WPC_TASK = IDT_WPC_Rx_Removed_Detection_Process();
}
/***********************************************************************************************************************
* Function Name: IDT_WPC_Rx_Removed_Detection_Process
* Description  : This function is IDT_WPC_Rx_Removed_Detection_Process function.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
unsigned char IDT_WPC_Rx_Removed_Detection_Process(void)
{
	if(ACC_Status.Active_On_Flag==TRUE)
	{
		if(Power_Sw.Active_Flag==TRUE)
		{
			
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

/***********************************************************************************************************************
* File Name    : idt_acc_off_task.c
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
unsigned char IDT_ACC_Off_Process(void);
void WPC_State_Reset(void);
/***********************************************************************************************************************
* Function Name: IDT_ACC_Off_Task
* Description  : This function is IDT_ACC_Off_Task function.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void IDT_ACC_Off_Task(void)
{
	if(IDT_WPC_State.Acc_Off_Task_Flag==TRUE)
	{
		IDT_WPC_State.Acc_Off_Task_Flag = FALSE;
		//-------------------------------------//
		WPC_State_Reset();
		//-------------------------------------//
		IDT_WPC_TASK = IDT_WPC_ACC_OFF_TASK;
	}
	IDT_WPC_TASK = IDT_ACC_Off_Process();
}
/***********************************************************************************************************************
* Function Name: IDT_ACC_Off_Task
* Description  : This function is IDT_ACC_Off_Task function.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
unsigned char IDT_ACC_Off_Process(void)
{
	if(ACC_Status.Active_On_Flag==TRUE)
	{
		IDT_WPC_State.Power_Sw_Off_Task_Flag = TRUE;
		//-----------------------------------------------//
		WPC_Function_Status.B_Plus_On_Flag = FALSE;
		WPC_Function_Status.ACC_On_Flag = TRUE;
		ACC_ON_RL78 = LEVEL_HIGH;		// Control Q33 wake up : power on 12V //
		Power_Sw.Active_Flag = TRUE;   // First ACC ON don't care power_sw, Power Sw = Active(Enable charge function) //
		KEYBOARD_PWR_SW_GREEN_LED_ON;  // Keyboard pwr_sw led //
		POWER_SW_GREEN_LED_ON;		   // Internal PCBA led //
		//-----------------------------------------------//
		IDT_WPC_NEXT_TASK = IDT_WPC_POWER_SW_OFF_TASK;
	}
	else
	{
		IDT_WPC_NEXT_TASK = IDT_WPC_ACC_OFF_TASK;
	}
	return IDT_WPC_NEXT_TASK;
}
/***********************************************************************************************************************
* Function Name: WPC_State_Reset
* Description  : This function is WPC_State_Reset function.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void WPC_State_Reset(void)
{
	CHARGE_DISABLE;
	KEYBOARD_PWR_SW_LED_OFF;	// Keyboard pwr_sw led //
	POWER_SW_LED_OFF;			// Internal PCBA led //
	CHARGE_STATE_LED_OFF;
	COIL_1_Q_Message = 0xFFFF;
	COIL_2_Q_Message = 0xFFFF;
	COIL_3_Q_Message = 0xFFFF;
	
	//#ifdef _WPC_PCBA_VER_1_2_
		WPC_Function_Status.P9261_ReStart_End_Flag = FALSE;
	//#else
	//#endif
	
	Buzzer_Disable();
	Alarm_LED_Disable();
}
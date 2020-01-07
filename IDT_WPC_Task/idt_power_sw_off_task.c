/***********************************************************************************************************************
* File Name    : idt_power_sw_off_task.c
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
unsigned char IDT_Power_Sw_Off_Process(void);
/***********************************************************************************************************************
* Function Name: IDT_Power_Sw_Off_Task
* Description  : This function is IDT_Power_Sw_Off_Task function.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void IDT_Power_Sw_Off_Task(void)
{
	if(IDT_WPC_State.Power_Sw_Off_Task_Flag==TRUE)
	{
		IDT_WPC_State.Power_Sw_Off_Task_Flag = FALSE;
		//-------------------------------------------------//
		if(WPC_Function_Status.ACC_On_Flag==FALSE)
		{
			KEYBOARD_PWR_SW_RED_LED_ON;	// Keyboard pwr_sw led //
			POWER_SW_RED_LED_ON;		// Internal PCBA led //
		}
		WPC_Function_Status.ACC_On_Flag = FALSE;
		CHARGE_DISABLE;
		COIL_1_Q_Message = 0xFFFF;
		COIL_2_Q_Message = 0xFFFF;
		COIL_3_Q_Message = 0xFFFF;
		
		//#ifdef _WPC_PCBA_VER_1_2_
		//		if(WPC_Function_Status.P9261_ReStart_End_Flag==TRUE)
		//		{
					WPC_Function_Status.P9261_ReStart_End_Flag = FALSE;	
		//		}
		//		COIL_1_Q_Message = 0xFFFF;
		//		COIL_2_Q_Message = 0xFFFF;
		//		COIL_3_Q_Message = 0xFFFF;
		//#else
		//#endif
		
		//-------------------------------------------------//
		IDT_WPC_TASK = IDT_WPC_POWER_SW_OFF_TASK;
	}
	IDT_WPC_TASK = IDT_Power_Sw_Off_Process();
}
/***********************************************************************************************************************
* Function Name: IDT_WPC_Startup_Process
* Description  : This function is IDT_WPC_Startup_Process function.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
unsigned char IDT_Power_Sw_Off_Process(void)
{
	if(ACC_Status.Active_On_Flag==TRUE)
	{
		if(Power_Sw.Active_Flag==TRUE)
		{
			//------------------------------------------------------------------------------//
			//#ifdef _WPC_PCBA_VER_1_2_
			//------------------------------------------------------------------------------//
				if(CHARGE_EN==LEVEL_LOW)	
				{
					POWER_SW_GREEN_LED_ON;			// Internal PCBA led //
					KEYBOARD_PWR_SW_GREEN_LED_ON; 	// Keyboard pwr_sw led //
					CHARGE_ENABLE;
					IDT_WPC_NEXT_TASK = IDT_WPC_POWER_SW_OFF_TASK;
				}
				else
				{
					if(WPC_Function_Status.P9261_ReStart_End_Flag==TRUE)
					{
						IDT_WPC_State.Startup_Task_Flag = TRUE;	
						IDT_WPC_NEXT_TASK = IDT_WPC_STARTUP_TASK;
					}
				}
			//-----------------------------------------------------------------------------//
			//#else			
			//-----------------------------------------------------------------------------//
				/*IDT_WPC_State.Startup_Task_Flag = TRUE;
				//---------------------------------------------//
				POWER_SW_GREEN_LED_ON;			// Internal PCBA led //
				KEYBOARD_PWR_SW_GREEN_LED_ON; 	// Keyboard pwr_sw led //
				CHARGE_ENABLE;					// IDT system charge power active //
				//---------------------------------------------//
				IDT_WPC_NEXT_TASK = IDT_WPC_STARTUP_TASK;*/
			//-----------------------------------------------------------------------------//
			//#endif
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
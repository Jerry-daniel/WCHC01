/***********************************************************************************************************************
* File Name    : idt_power_transfer_task.c
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
unsigned char Charge_Go_Flag = CLEAR;
unsigned char IDT_WCP_Power_Transfer_Process(void);
void Current_Detect(void);
void Q_FOD_Detect(void);
/***********************************************************************************************************************
* Function Name: IDT_WPC_Power_Transfer_Task
* Description  : This function is IDT_WPC_Power_Transfer_Task function.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void IDT_WCP_Power_Transfer_Task(void)
{
	if(IDT_WPC_State.Power_Transfer_Task_Flag==TRUE)
	{
		IDT_WPC_State.Power_Transfer_Task_Flag = FALSE;
		//----------------------------------------------//
		Last_COIL_Current = COIL_Current;
		WPC_Function_Status.Temperature_Detect_Start_Flag = TRUE;
		Reset_Buzzer_LED_State();
		Charge_Starting_Buzzer_Out();
		CHARGE_LED_ON;
		//----------------------------------------------//
		IDT_WPC_TASK = IDT_WCP_POWER_TRANSFER_TASK;
	}
	//Current_Detect();
	//Q_FOD_Detect();
	IDT_WPC_TASK = IDT_WCP_Power_Transfer_Process();
}
/***********************************************************************************************************************
* Function Name: IDT_WPC_Power_Transfer_Process
* Description  : This function is IDT_WPC_Power_Transfer_Process function.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
unsigned char IDT_WCP_Power_Transfer_Process(void)			// --- In Tx state 09 --- //
{
	if(ACC_Status.Active_On_Flag==TRUE)
	{
		if(Power_Sw.Active_Flag==TRUE)
		{
			if((Door_Status.Door_Open_Flag==FALSE)&&(Trunk_Status.Trunk_Open_Flag==FALSE))
			{
				if(P9261_RegMessage.Over_Temperature_Alarm_Status==TRUE)
				{
					if(Buzzer_State.Charge_Start_Flag==FALSE)
					{
						P9261_RegMessage.Over_Temperature_Alarm_Status = FALSE;
						IDT_WPC_State.Over_Temperature_Alarm_Task_Flag = TRUE;
						IDT_WPC_NEXT_TASK = IDT_WPC_OVERTEMPERATURE_TASK;
					}
				}
				else if(P9261_RegMessage.Open_Fod_Alarm_Status==TRUE)		// Tx Open FOD alarm status 25(0x19) //	
				{
					CHARGE_STATE_LED_OFF;						// charger led off //20191022 modify//
					P9261_RegMessage.Open_Fod_Alarm_Status = FALSE;
					IDT_WPC_State.Fod_Alarm_Task_Flag = TRUE;
					IDT_WPC_NEXT_TASK = IDT_WPC_FOD_ALARM_TASK;
				}
				else if(P9261_RegMessage.Power_Loss_Fod_Alarm_Status==TRUE)	// Tx Power loss FOD alarm status 24(0x18) //
				{
					CHARGE_STATE_LED_OFF;						// charger led off //20191022 modify//
					P9261_RegMessage.Power_Loss_Fod_Alarm_Status = FALSE;
					IDT_WPC_State.Fod_Alarm_Task_Flag = TRUE;
					IDT_WPC_NEXT_TASK = IDT_WPC_FOD_ALARM_TASK;
				}
				else if(P9261_RegMessage.Other_End_Of_Power_Rx_Alarm_Status==TRUE) // Tx Status Other end-of-power rx alarm 09 //
				{
					CHARGE_STATE_LED_OFF;						// charger led off //20191022 modify//
					P9261_RegMessage.Other_End_Of_Power_Rx_Alarm_Status = FALSE;
					Other_Alarm.Other_Eof_Pwr_Rx_Alarm_Status_Flag = TRUE; // 20191017 modify //
					IDT_WPC_State.Other_Eof_Pwr_Rx_Alarm_Task_Flag = TRUE;
					IDT_WPC_NEXT_TASK = IDT_WPC_OTHER_EOF_PWR_RX_ALARM_TASK;
				}
				else if(P9261_RegMessage.Rx_Jiggle_Alarm_Status==TRUE)	// Rx Jigle alarm status 27(0x1b) //
				{
					CHARGE_STATE_LED_OFF;						// charger led off //20191022 modify//
					P9261_RegMessage.Rx_Jiggle_Alarm_Status = FALSE;
					Other_Alarm.Rx_Jiggle_Status_Flag = TRUE;	// 20191016 modify //
					IDT_WPC_State.Rx_Jiggle_Alarm_Task_Flag = TRUE;
					IDT_WPC_NEXT_TASK = IDT_WPC_RX_JIGGLE_ALARM_TASK;
				}
				else if(P9261_RegMessage.Rx_Over_Current_Ept_Code_Status==TRUE)	// Rx over_current alarm status 07(0x07) //
				{
					P9261_RegMessage.Rx_Over_Current_Ept_Code_Status = FALSE;
					Other_Alarm.Rx_Ept_Code_Flag = TRUE;	// 20191114 modify //
					IDT_WPC_State.Rx_Over_Current_Ept_Code_Task_Flag = TRUE;
					IDT_WPC_NEXT_TASK = IDT_WPC_OVERCURRENT_TASK;
				}
				else if(P9261_RegMessage.Received_Power_Packet_Timeout_Status==TRUE)	// Rx receiver power packet timeout alarm status 12(0x0c) //
				{
					P9261_RegMessage.Received_Power_Packet_Timeout_Status = FALSE;
					Other_Alarm.Receiver_Timeout_Flag = TRUE;	// 20191114 modify //
					IDT_WPC_State.Received_Power_Packet_Timeout_Task_Flag = TRUE;
					IDT_WPC_NEXT_TASK = IDT_WPC_RECEIVE_POWER_PACKET_TIMEOUT_TASK;
				}
				else if(P9261_RegMessage.No_Response_Rx_Ept_Code_Status==TRUE)	// Rx No_Response_Rx_Ept_Code_Status 4(0x04) //
				{
					P9261_RegMessage.No_Response_Rx_Ept_Code_Status = FALSE;
					Other_Alarm.Rx_Ept_Code_Flag = TRUE;	// 20191114 modify //
					IDT_WPC_State.Rx_Fail_Ept_Code_Task_Flag = TRUE;
					IDT_WPC_NEXT_TASK = IDT_WPC_RX_FAIL_EPT_CODE_TASK;
				}
				else if(P9261_RegMessage.Internal_Fault_Rx_Ept_Code_Status==TRUE)	// Rx Internal_Fault_Rx_Ept_Code_Status 5(0x05) //
				{
					P9261_RegMessage.Internal_Fault_Rx_Ept_Code_Status = FALSE;
					Other_Alarm.Rx_Ept_Code_Flag = TRUE;	// 20191114 modify //
					IDT_WPC_State.Rx_Fail_Ept_Code_Task_Flag = TRUE;
					IDT_WPC_NEXT_TASK = IDT_WPC_RX_FAIL_EPT_CODE_TASK;
				}
				/*else if(P9261_RegMessage.Wireless_Charger_Off_Status==TRUE)	// Rx Wireless_Charger_Off_Status 21(0x15) //
				{
					P9261_RegMessage.Wireless_Charger_Off_Status = FALSE;
					Other_Alarm.Wireless_Charge_Off_Flag = TRUE;	// 20191114 modify //
					IDT_WPC_State.Wireless_Charger_Off_Task_Flag = TRUE;
					IDT_WPC_NEXT_TASK = IDT_WPC_WIRELESS_CHARGER_OFF_TASK;
				}*/
				else if(P9261_RegMessage.Tx_Under_Voltage_Limit_Status==TRUE)	// Tx Status Tx_Under_Voltage_Limit 0x14 //
				{
					P9261_RegMessage.Tx_Under_Voltage_Limit_Status = FALSE;
					IDT_WPC_State.Tx_Under_Voltage_Limit_Task_Flag = TRUE;	// reset p9261 //
					IDT_WPC_NEXT_TASK = IDT_WPC_TX_UNDER_VOLTAGE_LIMIT_TASK;
				}
				else if(P9261_RegMessage.Re_Negotiation_Status==TRUE)
				{
					CHARGE_STATE_LED_OFF;
					P9261_RegMessage.Re_Negotiation_Status = FALSE;
					IDT_WPC_State.Re_Negotiation_Task_Flag = TRUE;
					IDT_WPC_NEXT_TASK = IDT_WPC_RE_NEGOTIATION_TASK;
					
				}
				else if(P9261_RegMessage.Charge_Complete_From_Rx_Status==TRUE)	// Tx Status charge complete from rx 02 //
				{
					CHARGE_STATE_LED_OFF;						// charger led off //20191022 modify//
					P9261_RegMessage.Charge_Complete_From_Rx_Status = FALSE;
					IDT_WPC_State.Idle_Task_Flag = TRUE;
					Charge_Starting_Buzzer_Out();
					IDT_WPC_NEXT_TASK = IDT_WPC_IDLE_TASK;
				}
				else if(P9261_RegMessage.Full_Chargeed_Status==TRUE)			// Tx Status full charged from tx 08 //
				{
					CHARGE_STATE_LED_OFF;						// charger led off //20191022 modify//
					P9261_RegMessage.Full_Chargeed_Status = FALSE;
					IDT_WPC_State.Idle_Task_Flag = TRUE;
					Charge_Starting_Buzzer_Out();
					IDT_WPC_NEXT_TASK = IDT_WPC_IDLE_TASK;
				}
				else if(P9261_RegMessage.Analog_Ping_Status==TRUE)		// Tx State 02 //
				{
					CHARGE_STATE_LED_OFF;						// charger led off //20191022 modify//
					P9261_RegMessage.Analog_Ping_Status = FALSE;
					IDT_WPC_State.Analog_Ping_Task_Flag = TRUE;
					IDT_WPC_NEXT_TASK = IDT_WPC_ANALOG_PING_TASK;
				}
				else if(P9261_RegMessage.Q_Measurement_Status==TRUE)	// Tx State 03 //
				{
					CHARGE_STATE_LED_OFF;						// charger led off //20191022 modify//
					P9261_RegMessage.Q_Measurement_Status = FALSE;
					IDT_WPC_State.Q_Measurement_Task_Flag = TRUE;
					IDT_WPC_NEXT_TASK = IDT_WPC_Q_MEASUREMENT_TASK;
				}
				else
				{
					IDT_WPC_NEXT_TASK = IDT_WCP_POWER_TRANSFER_TASK;
				}
			}
			else
			{
				IDT_WPC_State.EUT_Sleep_Task_Flag = TRUE;
				CHARGE_STATE_LED_OFF;						// charger led off //20191022 modify//
				/*
				WPC_Function_Status.On_Chargeing_State_Flag = FALSE;		// 20191026 test//
				Coil_Current_Count = CLEAR;	//20191028 test modify //
				Last_COIL_Current = CLEAR;	// 20191026 test//
				*/
				IDT_WPC_NEXT_TASK = IDT_WPC_EUT_SLEEP_TASK;
			}
		}
		else
		{
			IDT_WPC_State.Power_Sw_Off_Task_Flag = TRUE;
			CHARGE_STATE_LED_OFF;						// charger led off //20191022 modify//
			/*
			WPC_Function_Status.On_Chargeing_State_Flag = FALSE;		// 20191026 test//
			Coil_Current_Count = CLEAR;	//20191028 test modify //
			Last_COIL_Current = CLEAR;	// 20191026 test//
			*/
			IDT_WPC_NEXT_TASK = IDT_WPC_POWER_SW_OFF_TASK;
		}
	}
	else
	{
		Phone_Placement_Detect();	// phone forget detect //
		/*
		WPC_Function_Status.On_Chargeing_State_Flag = FALSE;		// 20191026 test//
		Coil_Current_Count = CLEAR;	//20191028 test modify //
		Last_COIL_Current = CLEAR;	// 20191026 test//
		*/
		WPC_Function_Status.ACC_Off_Dly3_5T_Flag = TRUE;
		IDT_WPC_State.Cellphone_Left_Alert_Task_Flag = TRUE;
		IDT_WPC_NEXT_TASK = IDT_WPC_CELLPHONE_LEFT_ALERT_TASK;
	}
	return IDT_WPC_NEXT_TASK;
}
/***********************************************************************************************************************
* Function Name: Current_Detect
* Description  : This function is Current_Detect function.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void Q_FOD_Detect(void)
{
	if((COIL_1_Q_Message<120)||(COIL_2_Q_Message<120)||(COIL_3_Q_Message<120))
	{
		P9261_RegMessage.Open_Fod_Alarm_Status = TRUE;
	}
}
/***********************************************************************************************************************
* Function Name: Current_Detect
* Description  : This function is Current_Detect function.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void Current_Detect(void)
{
	/*
	if(WPC_Function_Status.On_Chargeing_State_Flag==FALSE)
	{
		if(Last_COIL_Current>1000)
		{
			WPC_Function_Status.On_Chargeing_State_Flag = TRUE;
		}
		Last_COIL_Current = COIL_Current;
	}
	else
	{
		if(COIL_Current<400)
		{
			if(Coil_Current_Count>=150)
			{
				WPC_Function_Status.On_Chargeing_State_Flag = FALSE;
				Coil_Current_Count = CLEAR;
				CHARGE_STATE_LED_OFF;
				Last_COIL_Current = COIL_Current;
			}
		}
		else
		{
			if(Coil_Current_Count!=CLEAR)
			{
				Coil_Current_Count = CLEAR;	
			}
		}
	}
	*/
}

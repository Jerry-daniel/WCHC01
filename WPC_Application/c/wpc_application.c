/***********************************************************************************************************************
* File Name    : wpc_application.c
* Version      : CodeGenerator for RL78/F13 V2.03.03.01 [28 Oct 2018]
* Device(s)    : R5F10BAC
* Tool-Chain   : CCRL
* Description  : This file implements main function.
* Creation Date: 2019/9/1
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
#include "r_cg_userdefine.h"
#include "wpc_application.h"
#include "power_sw_task.h"
#include "buzzer_task.h"
#include "led_task.h"
#include "acc_task.h"
#include "door_task.h"
#include "trunk_task.h"
#include "p9261_read_task.h"
/***********************************************************************************************************************
Pragma directive
***********************************************************************************************************************/
/* Start user code for pragma. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
IDT_WPC_STATE_TASK	IDT_WPC_TASK_NUM;
struct IDT_WPC_FLAG IDT_WPC_State;
struct WPC_FUNCTION_STATUS_FLAG WPC_Function_Status;
struct WPC_OTHER_ALARM_STATUS_FLAG	Other_Alarm;

unsigned int Read_I2C_Data_Duty_Time = CLEAR;
unsigned char FOD_Alarm_Count = CLEAR;
unsigned int FOD_Alarm_Wait_Time = CLEAR;
unsigned int FOD_Alarm_Continuous_Time = FOD_DETECT_TIME;

unsigned int test_alarm_count = CLEAR;
unsigned int test_warring_alarm_count = CLEAR;



//unsigned char p9261_led_state = CLEAR;
//unsigned int caculator_time = CLEAR;
unsigned int UVOL_Wait_Time = CLEAR;
//unsigned char Coil_Current_Count = CLEAR;
unsigned int Temperature_Value = RESET_TEMPERATURE_VALUE;
//unsigned int Door_Close_Waitting_Time;
unsigned int EUT_Waitting_Time = CLEAR;//DELAY_1S;
unsigned int ACC_Off_Reset_Time = DELAY_3_5S;
unsigned int Over_Temperature_Reset_Time = CLEAR;
volatile unsigned char IDT_WPC_TASK;
volatile unsigned char IDT_WPC_NEXT_TASK;
unsigned int *WPC_Function_State_Flag_address;
volatile unsigned int *wpc_function_state_flag_address;

unsigned char function_test_flag;

unsigned long p9261_message_case = 0x00000000;
unsigned char read_p9261_message(unsigned long const *address);
//=====================================================================//
unsigned char read_p9261_message(unsigned long const *address)
{
	p9261_message_case = *address;
}
/***********************************************************************************************************************
* Function Name: Clear_WPC_Function_State_Flag
* Description  : This function is Clear_WPC_Function_State_Flag function. 
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
unsigned char Clear_WPC_Function_State_Flag(unsigned long const *address)
{
	wpc_function_state_flag_address = address;
	*wpc_function_state_flag_address = CLEAR;
}
/***********************************************************************************************************************
* Function Name: IO_Interface_Init
* Description  : This function is IO_Interface_Init function.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void IO_Interface_Init(void)
{
	CAN0_STB = LEVEL_HIGH;
	ACC_ON_RL78 = LEVEL_HIGH;
	CHARGE_DISABLE;
	BUZZER = OFF;				// Buzzer Disable //
	KEYBOARD_PWR_SW_LED_OFF;	// Keyboard pwr_sw led //
	POWER_SW_LED_OFF;			// Internal PCBA led //
	CHARGE_STATE_LED_OFF;
}
/***********************************************************************************************************************
* Function Name: Variable_Init
* Description  : This function is Variable_Init function.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void Variable_Init(void)
{
	Clear_WPC_Function_State_Flag(&IDT_WPC_State);
	Clear_ACC_State_Flag(&ACC_Status);
	Clear_Power_Sw_State_Flag(&Power_Sw);
	Clear_Buzzer_State_Flag(&Buzzer_State);
	Clear_Led_State_Flag(&LED_Status);
	Clear_Door_State_Flag(&Door_Status);
	Clear_Trunk_State_Flag(&Trunk_Status);
	Clear_P9261_IIC_State_Flag(&P9261_IIC_State);
	Clear_P9261_RegMessage_State_Flag(&P9261_RegMessage);
	Clear_P9261_Combined_State_Flag(&P9261_CombinedMessage);
}
/***********************************************************************************************************************
* Function Name: P9261_Reg_Msg_Init
* Description  : This function is P9261_Reg_Msg_Init function.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void P9261_Reg_Msg_Init(void)
{
	P9261_IIC_FLOW = 0xD0;
	P9261_Reg_State.COIL_NUMBER = RESET_VALUE;
	P9261_Reg_State.TX_STATE_MESSAGE = RESET_VALUE;
	P9261_Reg_State.TX_STATUS_MESSAGE = RESET_VALUE;
	P9261_Reg_State.COMBINED_MESSAGE = RESET_VALUE;
	P9261_TX_STATE_MSG = P9261_Reg_State.TX_STATE_MESSAGE;
	P9261_TX_STATUS_MSG = P9261_Reg_State.TX_STATUS_MESSAGE;
	P9261_COMBINED_MSG = P9261_Reg_State.COMBINED_MESSAGE;
}
/***********************************************************************************************************************
* Function Name: I2C_Variable_Init
* Description  : This function is I2C_Variable_Init function.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void I2C_Variable_Init(void)
{	
	P9261_Reg_Msg_Init();
	P9261_IIC_State.iic_send_end_flag = FALSE;
	P9261_IIC_State.iic_receiver_end_flag = TRUE;
}
/***********************************************************************************************************************
* Function Name: Check_Door_Trunk_Statue
* Description  : This function is Check_Door_Trunk_Statue function.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void WPC_System_Initial(void)
{
	IO_Interface_Init();
	Variable_Init();
	I2C_Variable_Init();
	WPC_Function_Status.B_Plus_On_Flag = TRUE;
	IDT_WPC_TASK = IDT_WPC_ACC_OFF_TASK;
}
/***********************************************************************************************************************
* Function Name: Phone_Placement_Detect
* Description  : This function implements wpc task function.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void Phone_Placement_Detect(void)
{		
	//if(((P9261_CombinedMessage.In_Charge_complete_From_Rx==TRUE)||(P9261_CombinedMessage.In_Charging==TRUE))&&(COIL_Current>1000))
	if((P9261_CombinedMessage.In_Charge_complete_From_Rx==TRUE)||(P9261_CombinedMessage.In_Charging==TRUE))
	{
		WPC_Function_Status.Phone_Forget_Flag = TRUE;
	}
	else
	{
		if(Last_P9261_Tx_State_Code==0x09)
		{
			WPC_Function_Status.Phone_Forget_Flag = TRUE;
		}
		else
		{
			WPC_Function_Status.Phone_Forget_Flag = FALSE;
		}
	}
}
/***********************************************************************************************************************
* Function Name: IDT_WPC_Processer_Task
* Description  : This function implements wpc task function.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void IDT_WPC_Processer_Task(void)
{
	if(P9261_IIC_State.iic_receiver_end_flag==TRUE)
	{	
		Read_I2C_Data_Duty_Time = 50;
		P9261_IIC_State.iic_receiver_end_flag = FALSE;
		Clear_P9261_RegMessage_State_Flag(&P9261_RegMessage); // clear read p9261 register message //
		Receive_P9261_Tx_State_Task(P9261_TX_STATE_MSG);
		Receive_P9261_Tx_Status_Task(P9261_TX_STATUS_MSG);
		Receive_P9261_Combined_Msg_Task(P9261_COMBINED_MSG);
		
		//read_p9261_message(&P9261_RegMessage);
		#ifdef _USED_P9261_INTERNAL_TEMPERATURE_STATUS_
		
		#else
			OverTemperature_Detect();
		#endif
		switch(IDT_WPC_TASK)
		{
			//---------------------------------------------------------------------------------------------------------//
			case	IDT_WPC_STARTUP_TASK:																		//= 0x00
														IDT_WPC_Startup_Task();
			break;
			//---------------------------------------------------------------------------------------------------------//
			case	IDT_WPC_IDLE_TASK:																			//= 0x01
														IDT_WPC_Idle_Task();
			break;
			//---------------------------------------------------------------------------------------------------------//
			case	IDT_WPC_ANALOG_PING_TASK:																	//= 0x02
														IDT_WPC_Analog_Ping_Task();
			break;
			//---------------------------------------------------------------------------------------------------------//
			case	IDT_WPC_Q_MEASUREMENT_TASK:																	//= 0x03
														IDT_WPC_Q_Measurement_Task();
			break;
			//---------------------------------------------------------------------------------------------------------//
			case	IDT_WPC_DIGITAL_PING_TASK:																	//= 0x04
														IDT_WPC_Digital_Ping_Task();
			break;
			//---------------------------------------------------------------------------------------------------------//
			case	IDT_WPC_IDENTIFICATION_TASK:																//= 0x05
														IDT_WPC_Identification_Task();
			break;
			//---------------------------------------------------------------------------------------------------------//
			case	IDT_WPC_RX_REMOVED_DETECTION_TASK:															//= 0x06
														IDT_WPC_Rx_Removed_Detection_Task();
			break;
			//---------------------------------------------------------------------------------------------------------//
			case	IDT_WPC_CONFIGURATION_TASK:																	//= 0x07
														IDT_WPC_Configuration_Task();
			break;
			//---------------------------------------------------------------------------------------------------------//
			case	IDT_WPC_POWER_TRANSFER_INIT_TASK:															//= 0x08
														IDT_WPC_Power_Transfer_Init_Task();
			break;
			//---------------------------------------------------------------------------------------------------------//
			case	IDT_WCP_POWER_TRANSFER_TASK:																//= 0x09
														IDT_WCP_Power_Transfer_Task();
			break;
			//---------------------------------------------------------------------------------------------------------//
			case	IDT_WPC_ID_CONFIRMATION_TASK:																//= 0x0A
														IDT_WPC_Id_Confirmation_Task();
			break;
			//---------------------------------------------------------------------------------------------------------//
			case	IDT_WPC_REMOVE_POWER_TASK:																	//= 0x0B
														IDT_WPC_Remove_Power_Task();
			break;
			//---------------------------------------------------------------------------------------------------------//
			case	IDT_WPC_RESTART_TASK:																		//= 0x0C
														IDT_WPC_Restart_Task();
			break;
			//---------------------------------------------------------------------------------------------------------//
			case	IDT_WPC_NEGOTIATION_TASK:																	//= 0x0D
														IDT_WPC_Negotiation_Task();
			break;
			//---------------------------------------------------------------------------------------------------------//
			case	IDT_UNKNOW:																					//= 0x0E
														IDT_Unknow();
			break;
			//---------------------------------------------------------------------------------------------------------//
			case	IDT_WPC_RE_NEGOTIATION_TASK:																//= 0x0F
														IDT_WPC_Re_Negotiation_Task();
			break;
			//---------------------------------------------------------------------------------------------------------//
			case	IDT_WPC_CALIBRATION_TASK:																	//= 0x10
														IDT_WPC_Calibration_Task();
			break;
			//---------------------------------------------------------------------------------------------------------//
			case	IDT_WPC_FOD_ALARM_TASK:																		//= 0x11
														IDT_FOD_Alarm_Task();
			break;
			//---------------------------------------------------------------------------------------------------------//
			case	IDT_WPC_POWER_SW_OFF_TASK:																	//= 0x12
														IDT_Power_Sw_Off_Task();
			break;
			//---------------------------------------------------------------------------------------------------------//
			case	IDT_WPC_CELLPHONE_LEFT_ALERT_TASK:															//= 0x13
														IDT_Cellphone_Left_Alert_Task();
			break;
			//---------------------------------------------------------------------------------------------------------//
			case	IDT_WPC_ACC_OFF_TASK:																		//= 0x14
														IDT_ACC_Off_Task();
			break;
			//---------------------------------------------------------------------------------------------------------//
			case	IDT_WPC_EUT_SLEEP_TASK:																		//= 0x15
														IDT_EUT_Sleep_Task();
			break;
			//---------------------------------------------------------------------------------------------------------//
			case	IDT_WPC_OVERTEMPERATURE_TASK:																//= 0x16
														IDT_Temperature_Alarm_Task();
			break;
			//---------------------------------------------------------------------------------------------------------//
			case	IDT_WPC_RX_JIGGLE_ALARM_TASK:																//= 0x17
														IDT_RX_Jiggle_Alarm_Task();
			break;
			//---------------------------------------------------------------------------------------------------------//
			case	IDT_WPC_OTHER_EOF_PWR_RX_ALARM_TASK:														//= 0x18
														IDT_Other_Eof_Pwr_Rx_Alarm_Task();
			break;
			//---------------------------------------------------------------------------------------------------------//
			case	IDT_WPC_TX_UNDER_VOLTAGE_LIMIT_TASK:														//= 0x19
														IDT_TX_Under_Voltage_Limit_Task();
			break;
			//---------------------------------------------------------------------------------------------------------//
			case	IDT_WPC_RX_FAIL_EPT_CODE_TASK:																//= 0x1A
														IDT_RX_Fail_Ept_Code_Task();
			break;
			//---------------------------------------------------------------------------------------------------------//
			case	IDT_WPC_OVERCURRENT_TASK:																	//= 0x1B
														IDT_Current_Alarm_Task();
			break;
			//---------------------------------------------------------------------------------------------------------//
			case	IDT_WPC_RECEIVE_POWER_PACKET_TIMEOUT_TASK:													//= 0x1C
														IDT_Receive_Power_Packet_Timeout_Task();
			break;
			//---------------------------------------------------------------------------------------------------------//
			case	IDT_WPC_WIRELESS_CHARGER_OFF_TASK:															//= 0x1D
														IDT_Wireless_Charger_Off_Task();
			break;
			//---------------------------------------------------------------------------------------------------------//
			default:
			break;
		}
		P9261_TX_STATE_MSG = RESET_VALUE;
		P9261_TX_STATUS_MSG = RESET_VALUE;
		P9261_COMBINED_MSG = RESET_VALUE;
		//I2C_Commumication_Start();
	}
	else
	{
		if(Read_I2C_Data_Duty_Time==CLEAR)
		{
			if(P9261_IIC_State.read_reg_start_flag==FALSE)
			{
				I2C_Commumication_Start();
			}
		}
	}
}
/***********************************************************************************************************************
* Function Name: OverTemperature_Detect
* Description  : This function is OverTemperature_Detect function.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void OverTemperature_Detect(void)
{
	if(WPC_Function_Status.Temperature_Detect_Start_Flag==TRUE)
	{
		if((P9261_NTC_ADC_OUT>NTC_OVER_TEMPERATURE)&&(P9261_NTC_ADC_OUT<=NTC_DOWN_TEMPERATURE))
		{
			Temperature_Value = RESET_TEMPERATURE_VALUE;
			P9261_RegMessage.Over_Temperature_Alarm_Status = TRUE;	
		}
	}
}
/***********************************************************************************************************************
* Function Name: P9261_Restart_Init
* Description  : This function is P9261_Restart_Init function.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void P9261_Restart_Init(void)
{
	if(((COIL_1_Q_Message>CLEAR)&&(COIL_1_Q_Message<1000))||((COIL_2_Q_Message>CLEAR)&&(COIL_2_Q_Message<1000))||((COIL_3_Q_Message>CLEAR)&&(COIL_3_Q_Message<1000)))
	{
		WPC_Function_Status.P9261_ReStart_End_Flag = TRUE;
	}
}
/***********************************************************************************************************************
* Function Name: IDT_Unknow
* Description  : This function is IDT_Unknow function.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void IDT_Unknow(void)
{
	function_test_flag = 1;
}

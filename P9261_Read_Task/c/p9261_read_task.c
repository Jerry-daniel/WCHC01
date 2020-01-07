/***********************************************************************************************************************
* File Name    : trunk_task.c
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
#include "p9261_read_task.h"
#include "acc_task.h"
#include "power_sw_task.h"
#include "wpc_application.h"
/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
struct P9261_IIC_FLAG P9261_IIC_State;
struct P9261_STATE_MESSAGE	P9261_RegMessage;
struct P9261_COMBINED_MESSAGE P9261_CombinedMessage;
P9261_REG_STATE P9261_Reg_State;
P9261_TRANSMITTER_STATE P9261_TX_STATE_MSG;
P9261_TRANSMITTER_STATUS P9261_TX_STATUS_MSG;
P9261_COMBINED_STATUS_VALUE P9261_COMBINED_MSG;
//unsigned int RX_ID;
unsigned char	Last_P9261_Tx_State_Code = 0xFF;
unsigned int	P9261_NTC_ADC_OUT = 0x0FFF;
unsigned int 	COIL_1_Q_Message = 0xFFFF;
unsigned int 	COIL_2_Q_Message = 0xFFFF;
unsigned int 	COIL_3_Q_Message = 0xFFFF;
unsigned int	COIL_Current = CLEAR;
unsigned int 	Last_COIL_Current = CLEAR;
unsigned char	P9261_IIC_FLOW = 0xD0;
unsigned char	P9261_Message;
unsigned char 	P9261_Address[2];
unsigned int *P9261_IIC_State_Flag_address;
unsigned int *P9261_Combined_Flag_address;
unsigned int *P9261_RegMessage_Flag_address;
volatile unsigned int *p9261_iic_state_flag_address;
volatile unsigned int *p9261_combined_flag_address;
volatile unsigned long *p9261_regmessage_flag_address;
unsigned char 	delay_count; // for test //
unsigned char over_temp_1;
unsigned char over_temp_2;
/***********************************************************************************************************************
* Function Name: Clear_P9261_IIC_State_Flag
* Description  : This function is Clear_P9261_IIC_State_Flag function. 
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
unsigned char Clear_P9261_IIC_State_Flag(unsigned long const *address)
{
	p9261_iic_state_flag_address = address;
	*p9261_iic_state_flag_address = CLEAR;
}
/***********************************************************************************************************************
* Function Name: Clear_P9261_RegMessage_State_Flag
* Description  : This function is Clear_P9261_RegMessage_State_Flag function. 
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
unsigned char Clear_P9261_RegMessage_State_Flag(unsigned long const *address)
{
	p9261_regmessage_flag_address = address;
	*p9261_regmessage_flag_address = CLEAR;
}
/***********************************************************************************************************************
* Function Name: Clear_P9261_Combined_State_Flag
* Description  : This function is Clear_P9261_Combined_State_Flag function. 
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
unsigned char Clear_P9261_Combined_State_Flag(unsigned long const *address)
{
	p9261_combined_flag_address = address;
	*p9261_combined_flag_address = CLEAR;
}
/***********************************************************************************************************************
* Function Name: Receive_P9261_Tx_State_Task
* Description  : This function is Receive_P9261_Tx_State_Task function.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void Receive_P9261_Tx_State_Task(unsigned char P9261_TX_STATE)
{
	switch(P9261_TX_STATE)
	{
		//-------------------------------------------------------------------------------------------------------------//
		case	STATE_startup:							//0x00
				if((ACC_Status.Active_On_Flag==TRUE)&&(Power_Sw.Active_Flag==TRUE))
				{
					P9261_RegMessage.Startup_Status = TRUE;
				}
		break;
		//-------------------------------------------------------------------------------------------------------------//
		case	STATE_idle:								//0x01
				P9261_RegMessage.Idle_Status = TRUE;
		break;
		//-------------------------------------------------------------------------------------------------------------//
		case	STATE_analog_ping:						//0x02
				P9261_RegMessage.Analog_Ping_Status = TRUE;
		break;
		//-------------------------------------------------------------------------------------------------------------//
		case	STATE_q_measurement:					//0x03
				P9261_RegMessage.Q_Measurement_Status = TRUE;
		break;
		//-------------------------------------------------------------------------------------------------------------//
		case	STATE_digital_ping:						//0x04
				P9261_RegMessage.Digital_Ping_Status = TRUE;
		break;
		//-------------------------------------------------------------------------------------------------------------//
		case	STATE_wpc_identificantion:				//0x05
				P9261_RegMessage.Identification_Status = TRUE;
		break;
		//-------------------------------------------------------------------------------------------------------------//
		case	STATE_rx_removed_detection:				//0x06
				P9261_RegMessage.Rx_Removed_Detection_Status = TRUE;
		break;
		//-------------------------------------------------------------------------------------------------------------//
		case	STATE_wpc_configuration:				//0x07
				P9261_RegMessage.Configuration_Status = TRUE;
		break;
		//-------------------------------------------------------------------------------------------------------------//
		case	STATE_power_transfer_initialization:	//0x08
				P9261_RegMessage.Power_Transfer_Init_Status = TRUE;
		break;
		//-------------------------------------------------------------------------------------------------------------//
		case	STATE_power_transfer_state:				//0x09
				P9261_RegMessage.Power_Transfer_Status = TRUE;
		break;
		//-------------------------------------------------------------------------------------------------------------//
		case	STATE_id_confirmation:					//0x0A
				P9261_RegMessage.Id_Confirmation_Status = TRUE;
		break;
		//-------------------------------------------------------------------------------------------------------------//
		case	STATE_remove_power:						//0x0B
				P9261_RegMessage.Remove_Power_Status = TRUE;
		break;
		//-------------------------------------------------------------------------------------------------------------//
		case	STATE_restart:							//0x0C
				P9261_RegMessage.Restart_Status = TRUE;
		break;
		//-------------------------------------------------------------------------------------------------------------//
		case	STATE_wpc_negotiation:					//0x0D
				P9261_RegMessage.Negotiation_Status = TRUE;
		break;
		//-------------------------------------------------------------------------------------------------------------//
		case	unknow_state:							//0x0E
				P9261_RegMessage.Unknow_Status = TRUE;
		break;
		//-------------------------------------------------------------------------------------------------------------//
		case	STATE_wpc_re_negotiation:				//0x0F
				P9261_RegMessage.Re_Negotiation_Status = TRUE;
		break;
		//-------------------------------------------------------------------------------------------------------------//
		case	STATE_calibration:						//0x10
				P9261_RegMessage.Calibration_Status = TRUE;
		break;
		//-------------------------------------------------------------------------------------------------------------//
		default:
		break;
	}
}
/***********************************************************************************************************************
* Function Name: Receive_P9261_Tx_Status_Task
* Description  : This function is Receive_P9261_Tx_Status_Task function.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void Receive_P9261_Tx_Status_Task(unsigned char P9261_TX_STATUS)
{
	switch(P9261_TX_STATUS)
	{
		//-------------------------------------------------------------------------------------------------------------//
		case	STATUS_ok:										//= 0x00
		//-------------------------------------------------------------------------------------------------------------//
				if(P9261_RegMessage.Full_Chargeed_Status==TRUE)
				{
					P9261_RegMessage.Full_Chargeed_Status = FALSE;	
				}
		break;
		//-------------------------------------------------------------------------------------------------------------//
		case	STATUS_qfod_alarm:								//= 0x01
		//-------------------------------------------------------------------------------------------------------------//
				P9261_RegMessage.Q_Fod_Alarm_Status = TRUE;
		break;
		//-------------------------------------------------------------------------------------------------------------//
		case	STATUS_charge_complete_rx_ept_code:				//= 0x02
		//-------------------------------------------------------------------------------------------------------------//
				if(WPC_Function_Status.P9261_ReStart_End_Flag==TRUE)
				{
					P9261_RegMessage.Charge_Complete_From_Rx_Status = TRUE;
				}
				else
				{
					P9261_RegMessage.Charge_Complete_From_Rx_Status = FALSE;
				}
		break;
		//-------------------------------------------------------------------------------------------------------------//
		case	unknow_status:									//= 0x03
		//-------------------------------------------------------------------------------------------------------------//
		
		break;
		//-------------------------------------------------------------------------------------------------------------//
		case	STATUS_no_response_rx_ept_code:					//= 0x04
		//-------------------------------------------------------------------------------------------------------------//
				if(WPC_Function_Status.P9261_ReStart_End_Flag==TRUE)
				{
					P9261_RegMessage.No_Response_Rx_Ept_Code_Status = TRUE;
				}
				else
				{
					P9261_RegMessage.No_Response_Rx_Ept_Code_Status = FALSE;
				}
		break;
		//-------------------------------------------------------------------------------------------------------------//
		case	STATUS_internal_fault_rx_ept_code:				//= 0x05
		//-------------------------------------------------------------------------------------------------------------//
				if(WPC_Function_Status.P9261_ReStart_End_Flag==TRUE)
				{
					P9261_RegMessage.Internal_Fault_Rx_Ept_Code_Status = TRUE;
				}
				else
				{
					P9261_RegMessage.Internal_Fault_Rx_Ept_Code_Status = FALSE;
				}
		break;
		//-------------------------------------------------------------------------------------------------------------//
		case	STATUS_over_temperature_rx_ept_code:			//= 0x06
		//-------------------------------------------------------------------------------------------------------------//
				if(WPC_Function_Status.P9261_ReStart_End_Flag==TRUE)
				{
					P9261_RegMessage.Rx_Over_Temperature_Alarm_Status = TRUE;
				}
				else
				{
					P9261_RegMessage.Rx_Over_Temperature_Alarm_Status = FALSE;
				}
		break;
		//-------------------------------------------------------------------------------------------------------------//
		case	STATUS_over_current_rx_ept_code:				//= 0x07
		//-------------------------------------------------------------------------------------------------------------//
				if(WPC_Function_Status.P9261_ReStart_End_Flag==TRUE)
				{
					P9261_RegMessage.Rx_Over_Current_Ept_Code_Status = TRUE;
				}
				else
				{
					P9261_RegMessage.Rx_Over_Current_Ept_Code_Status = FALSE;
				}
		break;
		//-------------------------------------------------------------------------------------------------------------//
		case	STATUS_full_charged:							//= 0x08
		//-------------------------------------------------------------------------------------------------------------//
				P9261_RegMessage.Full_Chargeed_Status = TRUE;
		break;
		//-------------------------------------------------------------------------------------------------------------//
		case	STATUS_other_end_of_power_reason_rx_ept_code:	//= 0x09
		//-------------------------------------------------------------------------------------------------------------//
				if(WPC_Function_Status.P9261_ReStart_End_Flag==TRUE)
				{
					P9261_RegMessage.Other_End_Of_Power_Rx_Alarm_Status = TRUE;
				}
				else
				{
					P9261_RegMessage.Other_End_Of_Power_Rx_Alarm_Status = FALSE;
				}
		break;
		//-------------------------------------------------------------------------------------------------------------//
		case	STATUS_negotiation_failed:						//= 0x0A
		//-------------------------------------------------------------------------------------------------------------//
		
		break;
		//-------------------------------------------------------------------------------------------------------------//
		case	STATUS_control_error_packet_timeout:			//= 0x0B
		//-------------------------------------------------------------------------------------------------------------//
		
		break;
		//-------------------------------------------------------------------------------------------------------------//
		case	STATUS_received_power_packet_timeout:			//= 0x0C
		//-------------------------------------------------------------------------------------------------------------//
				if(WPC_Function_Status.P9261_ReStart_End_Flag==TRUE)
				{
					P9261_RegMessage.Received_Power_Packet_Timeout_Status = TRUE;
				}
				else
				{
					P9261_RegMessage.Received_Power_Packet_Timeout_Status = FALSE;
				}
		break;
		//-------------------------------------------------------------------------------------------------------------//
		case	STATUS_tnext_timing_violation:					//= 0x0D
		//-------------------------------------------------------------------------------------------------------------//
		
		break;
		//-------------------------------------------------------------------------------------------------------------//
		case	STATUS_packet_defect_during_id_config:			//= 0x0E
		//-------------------------------------------------------------------------------------------------------------//
		
		break;
		//-------------------------------------------------------------------------------------------------------------//
		case	STATUS_lllegal_packet_during_power_transfer:	//= 0x0F
		//-------------------------------------------------------------------------------------------------------------//
		
		break;
		//-------------------------------------------------------------------------------------------------------------//
		case	STATUS_tx_extern_over_temperature_limit:		//= 0x10
		//-------------------------------------------------------------------------------------------------------------//
				P9261_RegMessage.Over_Temperature_Alarm_Status = TRUE;
		break;
		//-------------------------------------------------------------------------------------------------------------//
		case	STATUS_tx_over_voltage_limit:					//= 0x11
		//-------------------------------------------------------------------------------------------------------------//
		
		break;
		//-------------------------------------------------------------------------------------------------------------//
		case	STATUS_tx_die_temperature_over_limit:			//= 0x12
		//-------------------------------------------------------------------------------------------------------------//
				//over_temp_2 = 1;
		break;
		//-------------------------------------------------------------------------------------------------------------//
		case	STATUS_lllegal_packet_during_id_config:			//= 0x13
		//-------------------------------------------------------------------------------------------------------------//
		
		break;
		//-------------------------------------------------------------------------------------------------------------//
		case	STATUS_tx_under_voltage_limit:					//= 0x14
		//-------------------------------------------------------------------------------------------------------------//
				P9261_RegMessage.Tx_Under_Voltage_Limit_Status = TRUE;
		break;
		//-------------------------------------------------------------------------------------------------------------//
		case	STATUS_wireless_charger_off:					//= 0x15
		//-------------------------------------------------------------------------------------------------------------//
				P9261_RegMessage.Wireless_Charger_Off_Status = TRUE;
		break;
		//-------------------------------------------------------------------------------------------------------------//
		case	STATUS_peps_interrupted:						//= 0x16
		//-------------------------------------------------------------------------------------------------------------//
		
		break;
		//-------------------------------------------------------------------------------------------------------------//
		case	STATUS_standby:									//= 0x17
		//-------------------------------------------------------------------------------------------------------------//
		
		break;
		//-------------------------------------------------------------------------------------------------------------//
		case	STATUS_power_loss_fod_alarm:					//= 0x18
		//-------------------------------------------------------------------------------------------------------------//
				P9261_RegMessage.Power_Loss_Fod_Alarm_Status = TRUE;
		break;
		//-------------------------------------------------------------------------------------------------------------//
		case	STATUS_open_fod_alarm:							//= 0x19
		//-------------------------------------------------------------------------------------------------------------//
				if(WPC_Function_Status.OpenFOD_Detect_Time_Flag==FALSE)
				{
					WPC_Function_Status.OpenFOD_Detect_Time_Flag = TRUE;
				}
				//-------------------------------------------------------//
				if(FOD_Alarm_Wait_Time==CLEAR)
				{
					FOD_Alarm_Wait_Time = 1000;
					FOD_Alarm_Count++;
					if(FOD_Alarm_Count==6)
					{
						FOD_Alarm_Count = CLEAR;
						P9261_RegMessage.Open_Fod_Alarm_Status = TRUE;
					}
				}
		break;
		//-------------------------------------------------------------------------------------------------------------//
		case	STATUS_tx_overcurrent_alarm:					//= 0x1A
		//-------------------------------------------------------------------------------------------------------------//
		
		break;
		//-------------------------------------------------------------------------------------------------------------//
		case	STATUS_rx_jiggle_alarm:							//= 0x1B	
		//-------------------------------------------------------------------------------------------------------------//
				P9261_RegMessage.Rx_Jiggle_Alarm_Status = TRUE;
		break;
		//-------------------------------------------------------------------------------------------------------------//
		default:
		break;
	}
}
/***********************************************************************************************************************
* Function Name: Receive_P9261_Combined_Msg_Task
* Description  : This function is Receive_P9261_Combined_Msg_Task function.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void Receive_P9261_Combined_Msg_Task(unsigned char P9261_COMBINED)
{
	switch(P9261_COMBINED)
	{
		//-------------------------------------------------------------------------------------------------------------//
		case	COMBINED_over_temperature_threshold:	//= 0x00,
		//-------------------------------------------------------------------------------------------------------------//
				Clear_P9261_Combined_State_Flag(&P9261_CombinedMessage);
		break;
		//-------------------------------------------------------------------------------------------------------------//
		case	COMBINED_over_voltage_threshold:		//= 0x01,
		//-------------------------------------------------------------------------------------------------------------//
				Clear_P9261_Combined_State_Flag(&P9261_CombinedMessage);
		break;
		//-------------------------------------------------------------------------------------------------------------//
		case	COMBINED_over_power_from_rx:			//= 0x02,
		//-------------------------------------------------------------------------------------------------------------//
				Clear_P9261_Combined_State_Flag(&P9261_CombinedMessage);
		break;
		//-------------------------------------------------------------------------------------------------------------//
		case	COMBINED_open_fod:						//= 0x03,
		//-------------------------------------------------------------------------------------------------------------//
				Clear_P9261_Combined_State_Flag(&P9261_CombinedMessage);		
				if((COIL_1_Q_Message==CLEAR)||(COIL_2_Q_Message==CLEAR)||(COIL_3_Q_Message==CLEAR))
				{
					P9261_CombinedMessage.In_Open_FOD = FALSE;
				}
				else
				{
					P9261_CombinedMessage.In_Open_FOD = TRUE;
				}
		break;
		//-------------------------------------------------------------------------------------------------------------//
		case	COMBINED_real_fod:						//= 0x04,
		//-------------------------------------------------------------------------------------------------------------//
				Clear_P9261_Combined_State_Flag(&P9261_CombinedMessage);
				P9261_CombinedMessage.In_Ploss_FOD = TRUE;
		break;
		//-------------------------------------------------------------------------------------------------------------//
		case	COMBINED_charge_complete_from_rx:		//= 0x05,
		//-------------------------------------------------------------------------------------------------------------//
				Clear_P9261_Combined_State_Flag(&P9261_CombinedMessage);
				P9261_CombinedMessage.In_Charge_complete_From_Rx = TRUE;
		break;
		//-------------------------------------------------------------------------------------------------------------//
		case	COMBINED_peps_alarm:					//= 0x06,
		//-------------------------------------------------------------------------------------------------------------//
				Clear_P9261_Combined_State_Flag(&P9261_CombinedMessage);
		break;
		//-------------------------------------------------------------------------------------------------------------//
		case	COMBINED_can_message_on_off:			//= 0x07,
		//-------------------------------------------------------------------------------------------------------------//
				Clear_P9261_Combined_State_Flag(&P9261_CombinedMessage);
		break;
		//-------------------------------------------------------------------------------------------------------------//
		case	COMBINED_charging:						//= 0x08,
		//-------------------------------------------------------------------------------------------------------------//
				Clear_P9261_Combined_State_Flag(&P9261_CombinedMessage);
				P9261_CombinedMessage.In_Charging = TRUE;
		break;
		//-------------------------------------------------------------------------------------------------------------//
		case	COMBINED_standby:						//= 0x09,
		//-------------------------------------------------------------------------------------------------------------//
				Clear_P9261_Combined_State_Flag(&P9261_CombinedMessage);
				P9261_CombinedMessage.In_Standby = TRUE;
		break;
		//-------------------------------------------------------------------------------------------------------------//
		case	COMBINED_under_voltage_threshold:		//= 0x0A
		//-------------------------------------------------------------------------------------------------------------//
				Clear_P9261_Combined_State_Flag(&P9261_CombinedMessage);
		break;
		//-------------------------------------------------------------------------------------------------------------//
		default:
		break;
	}
}
/***********************************************************************************************************************
* Function Name: I2C_Commumication_Start
* Description  : This function is I2C_Commumication_Start function.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void I2C_Commumication_Start(void)
{
	P9261_Address[0] = 0x06;
	P9261_Address[1] = 0x90;
	R_IIC00_Master_Send(P9261_ADDRESS,&P9261_Address,2);// i2c - write //
}
/***********************************************************************************************************************
* Function Name: Read_P9261_Register_Message
* Description  : This function is Read_P9261_Register_Message function.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void Read_P9261_Register_Message(void)
{
	if(delay_count!=CLEAR)	{delay_count--;}
	else
	{
		switch(P9261_IIC_FLOW)
		{
			//---------------------------------------------------------------------------------------//
			case	0xD0:	// Read P9261 register 0x0690 //
			//---------------------------------------------------------------------------------------//
							P9261_Address[0] = 0x06;
							P9261_Address[1] = 0x90;
							R_IIC00_Master_Send(P9261_ADDRESS,&P9261_Address,2);// i2c - write //
							P9261_IIC_FLOW = 0xD1;
			break;
			//---------------------------------------------------------------------------------------//
			case	0xD1:
			//---------------------------------------------------------------------------------------//
							if(P9261_IIC_State.iic_send_end_flag==TRUE)
							{
								if(P9261_IIC_State.iic_receiver_end_flag==TRUE)
								{
									P9261_IIC_State.iic_send_end_flag = FALSE;
									P9261_IIC_State.iic_receiver_end_flag = FALSE;
									R_IIC00_Master_Receive(P9261_ADDRESS,&P9261_Message,1);// i2c - read //
									P9261_IIC_FLOW = 0xD2;
								}
							}
			break;
			//---------------------------------------------------------------------------------------//
			case	0xD2:	// P9261 register message update //
			//---------------------------------------------------------------------------------------//
							if(P9261_IIC_State.iic_receiver_end_flag==TRUE)
							{
								P9261_Reg_State_Update();
								P9261_IIC_FLOW = 0xD3;
							}
			break;
			//---------------------------------------------------------------------------------------//
			case	0xD3:	// Read P9261 register 0x0691 //
			//---------------------------------------------------------------------------------------//
							P9261_Address[0] = 0x06;
							P9261_Address[1] = 0x91;
							R_IIC00_Master_Send(P9261_ADDRESS,&P9261_Address,2);// i2c - write //
							P9261_IIC_FLOW = 0xD4;
			break;
			//---------------------------------------------------------------------------------------//
			case	0xD4:
			//---------------------------------------------------------------------------------------//
							if(P9261_IIC_State.iic_send_end_flag==TRUE)
							{
								if(P9261_IIC_State.iic_receiver_end_flag==TRUE)
								{
									P9261_IIC_State.iic_send_end_flag = FALSE;
									P9261_IIC_State.iic_receiver_end_flag = FALSE;
									R_IIC00_Master_Receive(P9261_ADDRESS,&P9261_Message,1);// i2c - read //
									P9261_IIC_FLOW = 0xD5;
								}
							}
			break;
			//---------------------------------------------------------------------------------------//
			case	0xD5:	// P9261 register message update //
			//---------------------------------------------------------------------------------------//
							if(P9261_IIC_State.iic_receiver_end_flag==TRUE)
							{
								P9261_Reg_State_Update();
								P9261_IIC_FLOW = 0xD6;
							}
			break;
			//---------------------------------------------------------------------------------------//
			case	0xD6:	// Read P9261 register 0x06C0 //
			//---------------------------------------------------------------------------------------//
							P9261_Address[0] = 0x06;
							P9261_Address[1] = 0xC0;
							R_IIC00_Master_Send(P9261_ADDRESS,&P9261_Address,2);// i2c - write //
							P9261_IIC_FLOW = 0xD7;
			break;
			//---------------------------------------------------------------------------------------//
			case	0xD7:
			//---------------------------------------------------------------------------------------//
							if(P9261_IIC_State.iic_send_end_flag==TRUE)
							{
								if(P9261_IIC_State.iic_receiver_end_flag==TRUE)
								{
									P9261_IIC_State.iic_send_end_flag = FALSE;
									P9261_IIC_State.iic_receiver_end_flag = FALSE;
									R_IIC00_Master_Receive(P9261_ADDRESS,&P9261_Message,1);// i2c - read //
									P9261_IIC_FLOW = 0xD8;
								}
							}		
			break;
			//---------------------------------------------------------------------------------------//
			case	0xD8:	// P9261 register message update //
			//---------------------------------------------------------------------------------------//
							if(P9261_IIC_State.iic_receiver_end_flag==TRUE)
							{
								P9261_Reg_State_Update();
								P9261_IIC_FLOW = 0xD0;
							}
			break;
			//---------------------------------------------------------------------------------------//
			default:
			break;
		}
	}
}
/***********************************************************************************************************************
* Function Name: P9261_Reg_State_Update
* Description  : This function is P9261_Reg_State_Update function.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void P9261_Reg_State_Update(void)
{
	if(P9261_IIC_FLOW==0xD2)
	{
		P9261_Reg_State.TX_STATE_MESSAGE = (P9261_Message&0x1F);
		P9261_TX_STATE_MSG = P9261_Reg_State.TX_STATE_MESSAGE;
	}
	else if(P9261_IIC_FLOW==0xD5)
	{
		P9261_Reg_State.TX_STATUS_MESSAGE = (P9261_Message&0x1F);
		P9261_TX_STATUS_MSG = P9261_Reg_State.TX_STATUS_MESSAGE;
	}
	else if(P9261_IIC_FLOW==0xD8)
	{
		P9261_Reg_State.COMBINED_MESSAGE = (P9261_Message&0x0F);
		P9261_COMBINED_MSG = P9261_Reg_State.COMBINED_MESSAGE;
	}
	else{}
}

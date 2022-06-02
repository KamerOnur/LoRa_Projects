#ifndef ANOLOG_CLOCK_H
#define ANOLOG_CLOCK_H

#define STEP_TIME_MAX   10
#define STEP_TIME       2

#define F_HEF4094_DAT	5 // ATMEGA328P Pin 9  //PORTD_5
#define F_HEF4094_CLK	2 // ATMEGA328P Pin 32 //PORTD_2
#define F_REF_12		0 // ATMEGA328P Pin 23 //PORTC_0
#define F_REF_DK		1 // ATMEGA328P Pin 24 //PORTC_1


#define R_HEF4094_DAT	4 // ATMEGA328P Pin 2  //PORTD 4
#define R_HEF4094_CLK	3 // ATMEGA328P Pin 1  //PORTD 3
#define R_REF_12		2 // ATMEGA328P Pin 25 //PORTC_2
#define R_REF_DK		3 // ATMEGA328P Pin 26 //PORTC_3

#define CW				1
#define CCW				0

#define ACTIVE			1
#define PASSIVE			0

enum {
	CLOCK_RESET,
	CLOCK_STARTUP,
	IN_THE_SENSOR,
	TEN_MIN_GO_BACK,
	FIND_REF,
	WAIT_REF,
	NORMAL_MODE,
	STEP_MODE,
	CLOCK_PARK,
};

struct Stepper_Clock {
	uint8_t  Clk_Port;
	uint8_t	 Clk_Pin;
	uint8_t  Data_Port;
	uint8_t	 Data_Pin;
	uint8_t  Ref_12_Port;
	uint8_t	 Ref_12_Pin;
	uint8_t  Ref_DK_Port;
	uint8_t	 Ref_DK_Pin;
	uint8_t  Dir;
	uint8_t  State;
	uint8_t  Phase;
	uint8_t  Act;
	uint16_t Ref_Wait_Time;
	uint8_t  Step_Time;
	uint8_t  Acceleration_Time;
	uint16_t Reel_Pos_dk;
	uint16_t Scale_Pos_dk;
	uint16_t Reel_Pos_stp;
	uint16_t Scale_Pos_stp;
};

void Stepper_Init(struct Stepper_Clock *Stp_Drv,
uint8_t Clk_Port,  uint8_t Clk_Pin,
uint8_t Data_Port,  uint8_t Data_Pin,
uint8_t Ref_12_Port,  uint8_t Ref_12_Pin,
uint8_t Ref_DK_Port, uint8_t Ref_DK_Pin);

void Stepper_Off(struct Stepper_Clock *Stp_Drv);

void Stepper_Next_Step(struct Stepper_Clock *Stp_Drv);
void Stepper_Back_Step(struct Stepper_Clock *Stp_Drv);

void Stepper_Next_DK(struct Stepper_Clock *Stp_Drv);
void Stepper_Back_DK(struct Stepper_Clock *Stp_Drv);

void Stepper_Clock_Process(struct Stepper_Clock *Stp_Drv);

void Stepper_Set_Reel_Pos_dk(struct Stepper_Clock *Stp_Drv, uint16_t m_reel_dk);

uint16_t Stepper_Get_Reel_Pos_dk(struct Stepper_Clock *Stp_Drv);
uint16_t Stepper_Get_Scale_Pos_dk(struct Stepper_Clock *Stp_Drv);
uint16_t Stepper_Get_Reel_Pos_stp(struct Stepper_Clock *Stp_Drv);
uint16_t Stepper_Get_Scale_Pos_stp(struct Stepper_Clock *Stp_Drv);

#endif // ANOLOG_CLOCK_H

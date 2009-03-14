//----------------------------------------------------------------------------------------------------
/**
* @file   Hal.h
* The header file for Hal.cpp, hardware abstract level function.
* @author Poenix
* @date   13:3:2007   10:28
*/
//----------------------------------------------------------------------------------------------------
#include <conio.h>



//----------------------------------------------------------------------------------------------------
/**
* @name SL811 port defines
* @{
*/

//----------------------------------------------------------------------------------------------------
/**
* SL811 address port 
*/
//----------------------------------------------------------------------------------------------------
#define SL811_ADDR_PORT		0x764
//----------------------------------------------------------------------------------------------------
/**
* SL811 data port
*/
//----------------------------------------------------------------------------------------------------
#define SL811_DATA_PORT		0x762

//----------------------------------------------------------------------------------------------------
/**
* SL811 mode ( host/slave ) select port
*/
//----------------------------------------------------------------------------------------------------
#define SL811_MODE_PORT     0x760

/** @}*/	// end of SL811 port defines
//----------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------
/**
* @name 8254 port defines
* @{
*/

/** 8254 timer 0 */
#define TIMER0_8254		0x380

/** 8254 timer 1 */
#define TIMER1_8254		0x382

/** 8254 timer 2 */
#define TIMER2_8254		0x384

/** 8254 control port */
#define CTL_8254		0x386


/** @}*/	// end of 8254 port defines
//----------------------------------------------------------------------------------------------------


//----------------------------------------------------------------------------------------------------
/**
* @name 8254 CMD defines
* @{
*/

/** select timer0 */
#define SC0_8254		0x00

/** select timer1 */
#define SC1_8254		0x40

/** select timer2 */
#define SC2_8254		0x80

/** read back */
#define	R_B_8254		0xC0

/** Counter latch command */
#define CLC_8254		0x00

/** least significant byte only */
#define LST_8254		0x10

/** most significant byte only */
#define MST_8254		0x20

/** least byte first, the most significant byte */
#define L2M_8254		0x30

/** mode 0, interrupt on terminal count */
#define IOTC_8254		0x00

/** Hardware retriggerable one-shot */
#define HRT_8254		0x02

/** mode 1, Rate generator */
#define	RGEN_8254		0x04

/** Square wave mode */
#define	SWAV_8254		0x06

/** Software triggered strobe */
#define	STSB_8254		0x08


/** Hardware triggered strobe */
#define	HTSB_8254		0x0A

/** Binary counter 16 bits */
#define	BCH_8254		0x00

/** Binary coded decimal(BCD) */
#define BCD_8254		0x01

/** @}*/
//----------------------------------------------------------------------------------------------------



//----------------------------------------------------------------------------------------------------
/**
* @name 8255 port defines
* @{
*/

/** 8255 port A */
#define PA_8255			0x388

/** 8255 port B */
#define PB_8255			0x38A

/** 8255 port C */
#define PC_8255			0x38C

/** 8255 control port */
#define CTL_8255		0x38E

/** @}*/
//----------------------------------------------------------------------------------------------------


//----------------------------------------------------------------------------------------------------
/**
* @name PIC 8259
* @{
*/

#define SLAVE_8259		0xA1


#define MASTER_8259		0x21

/*
OCW1	IRQ7	IRQ6	IRQ5	IRQ4	IRQ3	IRQ2	IRQ1	IRQ0
LPT1	FLOPPY	LPT2	COM1&3	COM2&4	PIC2	KeyB	8254 timer0
*/

// keyboard is opened for test, assume the IRQ is opened
#define MASK_8259		0xDF

/** @}*/
//----------------------------------------------------------------------------------------------------


//----------------------------------------------------------------------------------------------------
/**
* @name DAC7744 port defines
* @{
*/

#define DAC_CH1			0x390

#define DAC_CH2			0x394

#define DAC_CH3			0x392

#define DAC_CH4			0x396

// DAC 2, IC4

#define DAC_CH5			0x398

#define DAC_CH6			0x39C

#define DAC_CH7			0x39A

#define DAC_CH8			0x39E

/** @}*/
//----------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------
/**
* SL811 register read function
* @param[in] bRegAddr is the sl811's register address
* @return the value of the register
* @remarks none
* @note none
*/
//----------------------------------------------------------------------------------------------------
unsigned char SL811Read(unsigned char bRegAddr );


//----------------------------------------------------------------------------------------------------
/**
* SL811 register write function
* @param[in] bRegAddr sl811's register address
* @param[in] bData the data to write
* @return none
* @remarks none
* @note none
*/
//----------------------------------------------------------------------------------------------------
void SL811Write(unsigned char bRegAddr, unsigned char bData );


//----------------------------------------------------------------------------------------------------
/**
* SL811 buffer read
* @param[in] bRegAddr sl811's register address
* @param[in] pDes	  the destination of the read data
* @param[in] bNum     data num to read
* @return none
* @remarks none
* @note none
*/
//----------------------------------------------------------------------------------------------------
void SL811BufRead( unsigned char bRegAddr, unsigned char *pDes, unsigned char bNum );

//----------------------------------------------------------------------------------------------------
/**
* SL811 buffer write
* @param[in] bRegAddr sl811's register address
* @param[in] pOrg	  the written data's start address
* @param[in] bNum	  data num to write
* @return none
* @remarks none
* @note none
*/
//----------------------------------------------------------------------------------------------------
void SL811BufWrite( unsigned char bRegAddr, unsigned char *pOrg, unsigned char bNum );

//----------------------------------------------------------------------------------------------------
/**
* Word swap function
* @param[in] bWord the operation data 
* @return the word swapped
* @remarks This function swap the word's high and low byte
* @note none
* @deprecated For special reason this function may be remove in the future
*/
//----------------------------------------------------------------------------------------------------
unsigned short WordSwap(unsigned short bWord );

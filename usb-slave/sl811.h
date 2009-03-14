//----------------------------------------------------------------------------------------------------
/**
* @file sl811.h
* The header file for sl811.cpp
* @author Phoenix
* @date 13:3:2007   18:31
*/
//----------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------
/**
* @name 
* @{
*/
/** EP0 MaxPktSize, control EP */
#define EP0_LEN 			64
/** EP1 MaxPktSize, bulk in EP */
#define EP1_LEN 			64
/** EP2 MaxPktSize, bulk out EP */
#define EP2_LEN             64

#define cMemEnd             0xFF

/** Maximum of interfaces  */
#define MAXIFCNUM			1	

/** Time out (ms) */
#define TIME_OUT			25
/** @}*/
//----------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------
/**
* @name 
* @{
*/
// USB Control Registers
#define CtrlReg         	0x05	
#define IntEna         	 	0x06
#define USBAddress			0x07
#define IntStatus       	0x0D
#define cDATASet        	0x0E
#define cSOFcnt         	0x0F

// EP0 Endpoint Registers (set A)
#define EP0AControl    		0x00	
#define EP0AAddress    		0x01
#define EP0AXferLen    		0x02
#define EP0AStatus     		0x03
#define EP0ACounter    		0x04

// EP1 Endpoint Registers (set A)
#define EP1AControl    		0x10	
#define EP1AAddress    		0x11
#define EP1AXferLen    		0x12
#define EP1AStatus     		0x13
#define EP1ACounter    		0x14

// EP1 Endpoint Registers (set B)
#define EP1BControl			0x18
#define EP1BAddress			0x19
#define EP1BXferLen			0x1A
#define EP1BStatus			0x1B
#define EP1BCounter			0x1C

// EP2 Endpoint Registers (set A)
#define EP2AControl			0x20
#define EP2AAddress			0x21
#define EP2AXferLen			0x22
#define EP2AStatus			0x23
#define EP2ACounter			0x24

// EP2 Endpoint Registers (set B)
#define EP2BControl			0x28
#define EP2BAddress			0x29
#define EP2BXferLen			0x2A
#define EP2BStatus			0x2B
#define EP2BCounter			0x2C
/** @}*/
//----------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------
/**
* @name 
* @{
*/
/**
* EP0 buffer
*/
#define EP0A_Slave_Buf		0x40					// Start address for EP0 buffer

/**
* EP1 buffer
*/
#define EP1A_Slave_Buf      0x80

/**
* EP2 buffer
*/
#define EP2A_Slave_Buf      0xC0

/** @}*/
//----------------------------------------------------------------------------------------------------


//----------------------------------------------------------------------------------------------------
/**
* @name Interrupt register bit masks
* @{
*/
#define EP0_DONE			0x01
#define EP1_DONE			0x02
#define EP2_DONE			0x04
#define EP3_DONE			0x08
#define DMA_DONE			0x10
#define SOF_DONE			0x20
#define USB_RESET_DONE		0x40
/** @}*/
//----------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------
/**
* @name EPx status register bit masks
* @{
*/
#define EP_ACK				0x01
#define EP_ERROR			0x02
#define EP_TIMEOUT			0x04
#define EP_SEQUENCE			0x08
#define EP_SETUP			0x10
#define EP_OVERFLOW			0x20
#define EP_NAK				0x40
#define EP_STALL			0x80

/** @}*/
//----------------------------------------------------------------------------------------------------


//----------------------------------------------------------------------------------------------------
/**
* @name EPx control register bit masks
* @{
*/

#define DATA0_OUT    		0x03   	// (Data0 + OUT/SETUP + Enable + Arm)
#define DATA1_OUT    		0x43   	// (Data1 + OUT/SETUP + Enable + Arm)
#define DATA0_IN    		0x07   	// (Data0 + IN + Enable + Arm)
#define DATA1_IN    		0x47   	// (Data1 + IN + Enable + Arm)
#define DATAX	    		0x40   	// Data sequence bits
#define ENABLE_ISO    		0x10   	// Enable ISO on endpoint
#define SEND_STALL    		0x27   	// (STALL + IN + Enable + Arm)
#define DIRECTION 			0x04   	// OUT - '0', receive from host
// IN  - '1', transmit to host	

#define DATA0_IN_ENABLE		0x06   	// ( Data0 + IN + Enable )
#define DATA0_IN_DISABLE	0x04   	// ( Data0 + IN + Disable )

#define DATA1_IN_ENABLE     0x46
#define DATA1_IN_DISABLE    0x44

#define DATA0_OUT_ENABLE    0x02    // ( Data0 + OUT + Enable )
#define DATA0_OUT_DISABLE   0x00    // ( Data0 + OUT + Disable )
 
#define DATA1_OUT_ENABLE    0x42    // ( Data0 + OUT + Enable )
#define DATA1_OUT_DISABLE   0x40    // ( Data0 + OUT + Disable )
/** @}*/
//----------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------
/**
* @name USB specification 1.1 Chapter 9 definition
* @{
*/
#define STD_REQUEST			0x00
#define CLASS_REQUEST		0x01
#define VENDOR_REQUEST		0x02

#define RECIPIENT_DEV		0x00
#define RECIPIENT_IFC		0x01
#define RECIPIENT_ENP		0x02

#define GET_STATUS      	0x00																  
#define CLEAR_FEATURE   	0x01
#define SET_FEATURE    		0x03
#define SET_ADDRESS    		0x05
#define GET_DESCRIPTOR 		0x06
#define SET_DESCRIPTOR 		0x07
#define GET_CONFIG     		0x08
#define SET_CONFIG    		0x09
#define GET_INTERFACE  		0x0A
#define SET_INTERFACE  		0x0B
#define SYNCH_FRAME     	0x0C

#define DEVICE          	0x01
#define CONFIGURATION   	0x02
#define STRING          	0x03
#define INTERFACE       	0x04
#define ENDPOINT        	0x05

#define STDCLASS        	0x00
#define HIDCLASS        	0x03
#define HUBCLASS	 		0x09  
/** @}*/
//----------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------
/**
* @name Function prototypes
* @{
*/

//----------------------------------------------------------------------------------------------------
/**
* EP0 ( control endpoint ) IN arm  
* @param[in] bLen transfer length
* @param[in] bSeq data toggle sequence
* @return none
* @remarks none
* @note none
*/
//----------------------------------------------------------------------------------------------------
void EP0A_IN_ARM( unsigned char bLen,  unsigned char bSeq );

//----------------------------------------------------------------------------------------------------
/**
* EP0 ( control endpoint ) OUT arm
* @param[in] bLen transfer length
* @param[in] bSeq data toggle sequence
* @return none
* @remarks none
* @note In order to improve the speed in control transfer, at ep0, after setup packet, the slave device
*       receive one packet at most, so the bSeq is 1 by default. 
*/
//----------------------------------------------------------------------------------------------------
void EP0A_OUT_ARM( unsigned char bLen, unsigned char bSeq );

//----------------------------------------------------------------------------------------------------
/**
* EP1 ( bulk endpoint ) IN arm
* @param[in] bLen bytes num to send 
* @return none
* @remarks none
* @note Because data toggle, for purpose of improving speed, not use the bSeq parameter
*/
//----------------------------------------------------------------------------------------------------
void EP1A_IN_ARM( unsigned char bLen );

//----------------------------------------------------------------------------------------------------
/**
* EP2 ( bulk endpoint ) OUT arm
* @param[in] bLen bytes num to receive 
* @return none
* @remarks none
* @note Because data toggle, for purpose of improving speed, not use the bSeq parameter
*/
//----------------------------------------------------------------------------------------------------
void EP2A_OUT_ARM( unsigned char bLen );

//----------------------------------------------------------------------------------------------------
/**
* SL811 registers initialization
* @return none
* @remarks none
* @note This should be use after SL811Slave_Init()
* @see SL811Slave_Init()
*/
//----------------------------------------------------------------------------------------------------
void SL811_Init( void );

//----------------------------------------------------------------------------------------------------
/**
* SL811 slave mode initialization
* @return none
* @remarks none
* @note This function is related to the hardware design. 
*/
//----------------------------------------------------------------------------------------------------
void SL811Slave_Init( void );

//----------------------------------------------------------------------------------------------------
/**
* SL811 host mode initialization
* @return none
* @remarks none
* @note This function is related to the hardware design
*/
//----------------------------------------------------------------------------------------------------
void SL811Host_Init( void );

//----------------------------------------------------------------------------------------------------
/**
* SL811 memory test
* @return memory test errors
* @remarks none
* @note You'd better do this before your tasks.
*/
//----------------------------------------------------------------------------------------------------
unsigned short SL811MemTest( void );

//----------------------------------------------------------------------------------------------------
/**
* EP0 control transfer interrupt service routine
* @return none
* @remarks This function process the data transfer on default control pipe.Such as:
* - At the setup stage, it make PCs recgnize the USB device.
* - After the setup stage, we use it send cmds to the USB device.
* @note You'd do your best to make it run faster!
*/
//----------------------------------------------------------------------------------------------------
void EP0_ISR( void );


//----------------------------------------------------------------------------------------------------
/**
* EP1 bulk in interrupt service routine
* @return TRUE on success, or FALSE on failed
* @remarks none
* @note none
*/
//----------------------------------------------------------------------------------------------------
unsigned char EP1_IN_ISR( void );

//----------------------------------------------------------------------------------------------------
/**
* EP2 bulk out interrupt service routine
* @return TRUE on success, or FALSE on failed
* @remarks
* @note
*/
//----------------------------------------------------------------------------------------------------
unsigned char EP2_OUT_ISR( void );
/** @}*/
//----------------------------------------------------------------------------------------------------

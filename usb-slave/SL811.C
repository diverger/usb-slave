#include "defs.h"
#include "sl811.h"
#include "descp.h"
#include "hal.h"
#include <dos.h>
#include <stdio.h>
#include <conio.h>
#include <mem.h>
#include <bios.h>

//#define MAX_BUFFER_LEN		8192
#define DATAIN_BUFFER_LEN   0xFFFF
#define DATAOUT_BUFFER_LEN  0xFFFF

//----------------------------------
// SL811S Variables
//----------------------------------
INT8U	Slave_USBaddr;			// USB device address
INT8U	Slave_ConfigVal;		// Device configuration value
INT8U	Slave_RemoteWU;			// Device remote wakeup stats
INT8U	Slave_inEPstall;		// EP0 ~ EP7's IN stall status
INT8U	Slave_outEPstall;		// EP0 ~ EP7's OUT stall status
INT8U	Slave_IfcAlt[MAXIFCNUM];// 8 interface(Ep0~7) contain alternate setting value

//----------------------------------------------------------------------------------------------------
/**
* @name Global variables for EP0
* @{
*/
/** EP0 cmd  */
INT8U        EP0_CMD = 0;

INT8U        EP0_rq_len = 0;
INT16U       EP0_buffer_idx = 0;
INT8U        EP0_xfr_len = 0;
SetupPKG 	dReq;					// Setup token struct
INT16U		in_buffer_idx;			// EP0 IN data buffer tracking
INT16U		len_req;				// length of data for EP0

INT16U		sof_cnt;				// 1ms counter
INT16U        sofTimeOut;
INT8U        enumDone = 0;
INT8U        flipFlag = 0;

/** @}*/
//----------------------------------------------------------------------------------------------------

//----------------------------------------------------------
//for ep1
//--------------------------------------------------------------
INT8U        EP1_IN_CMD = 0;
INT32U       EP1_rq_len = 0;

INT16U        EP1_single_xfr = 0;
INT16U        EP1_buffer_idx = 0;
INT8U        EP1_xfr_len = 0;
//-------------------------------------------------
//for ep2
//-------------------------------------------------
INT8U        EP2_OUT_CMD = 0;
INT32U       EP2_rq_len = 0;

INT16U        EP2_single_xfr = 0;
INT16U        EP2_buffer_idx = 0;
INT8U        EP2_xfr_len = 0;

INT8U COMMONBUF[65];
//INT8U DATA_BUF[MAX_BUFFER_LEN];
INT8U DATA_IN_BUF[DATAIN_BUFFER_LEN];
INT8U DATA_OUT_BUF[DATAOUT_BUFFER_LEN];

//BYTE *pInBuf = DATA_BUF;
//BYTE *pOutBuf = DATA_BUF + DATAIN_BUFFER_LEN;
BYTE *pInBuf = DATA_IN_BUF;
BYTE *pOutBuf = DATA_OUT_BUF;


INT8U taskStart = 0;

BOOL 		BUS_POWERED;			// Bus powered device 

//----------------------------------------------------------------------------------------------------
/**
* Device descriptor
*/
//----------------------------------------------------------------------------------------------------
unsigned char Dev_Descp[] = 
{	
	DEV_LEN,			// bLength: Size of descriptor
	0x01,				// bDescriptorType: Device	
	0x10,0x01,			// bcdUSB: USB 1.1
	0x00,				// bDeviceClass:	none
	0x00,				// bDeviceSubClass: none
	0x00,				// bDeviceProtocol: none
	EP0_LEN,			// bMaxPacketSize0: 8/64 bytes
	0xFF,0xFF,			// idVendor: 0x04B4 - Cypress
	0x00,0x00,			// idProduct: 0x1111 - SL811S
	0x00,0x01,			// bcdDevice: version 1.0
	0x01,				// iManufacturer: Cypress
	0x02,				// iProduct: Cypress SL811S - HID Consumer Control
	0x00,				// iSerialNumber:	none. May effect the regnization on PC's multi-usb-port
	0x01				// bNumConfigurations: 1, this device has only one configuration
};

//----------------------------------------------------------------------------------------------------
/**
* Configuration descriptor
*/
//----------------------------------------------------------------------------------------------------
BYTE	Cfg_Descp[] = 
{	
	0x09,				// bLength: Size of descriptor
	0x02,				// bDescriptorType: Configuration	
	CFG_LEN,0x00,		// wTotalLength: Cfg+Ifc+Class+Ep = 32 bytes
	0x01,				// bNumInterfaces: 1 interface
	0x01,				// bConfigurationValue: 1
	0x00,				// iConfiguration: none
	0xA0,				// bmAttributes: bus-powered, remote-wakeup
	0x32,				// MaxPower: 100mA

	//---------------------------------------------------------------------------------------------------------------------
	// INTERFACE DESCRIPTOR
	//---------------------------------------------------------------------------------------------------------------------

	0x09,				// bLength: Size of descriptor
	0x04,				// bDescriptorType: Interface
	0x00,				// bInterfaceNumber: #0
	0x00,				// bAlternateSetting: #0
	0x02,				// bNumEndpoints: 2
	0xFF,				// bInterfaceClass: HID-class
	0x00,				// bInterfaceSubClass: none
	0xFF,				// bInterfaceProtocol: none
	0x00,				// iInterface: none


	//---------------------------------------------------------------------------------------------------------------------
	// HID-CLASS DESCRIPTOR
	//---------------------------------------------------------------------------------------------------------------------
	/*
	0x09,				// bLength: Size of descriptor
	0x21,				// bDescriptorType: HID Class
	0x00,0x01,			// bcdHID: HID Class version 1.1
	0x00,				// bCountryCode: none
	0x01,				// bNumDescriptors: 1 report descp
	0x22,				// bDescriptorType: report descriptor type
	REP_LEN,0x00,		// wDescriptorLength: 31 bytes

	*/

	//---------------------------------------------------------------------------------------------------------------------
	// ENDPOINT DESCRIPTOR
	//---------------------------------------------------------------------------------------------------------------------

	0x07,				// bLength: Size of descriptor
	0x05,				// bDescriptorType: Endpoint
	0x81,				// bEndpointAddress: IN, EP1
	0x02,				// bmAttributes: Bulk
	0x40,0x00,			// wMaxPacketSize: 64
	0x00,				// bInterval: no use for bulk and control endpoint
	//-------------------
	0x07,
	0x05,
	0x02,				// bEndpointAddress: OUT, EP2
	0x02,
	0x40,
	0x00,
	0x00
};

//----------------------------------------------------------------------------------------------------
/**
* Language string descriptor 
*/
//----------------------------------------------------------------------------------------------------
unsigned char LangString[] = {4,3,9,4};

//----------------------------------------------------------------------------------------------------
/**
* Manufacture string descriptor
*/
//----------------------------------------------------------------------------------------------------
unsigned char MfgString[]  = {12,3,'H',0,'a',0,'n',0,'d',0,'y',0};

//----------------------------------------------------------------------------------------------------
/**
* Production string descriptor
* note: For some special reason, should restrict the size to 64 bytes
*/
//----------------------------------------------------------------------------------------------------
unsigned char ProdString[] = {44,3,'H',0,'a',0,'n',0,'d',0,'y',0,' ',0,'T',0,'e',0,'s',0,'t',0,' ',0,'i',0,'n',0,'s',0,'t',0,'r',0,'u',0,'m',0,'e',0,'n',0,'t',0};

void EP0A_IN_ARM( INT8U bLen, INT8U bSeq )
{
	SL811Write( EP0AAddress, EP0A_Slave_Buf );		// this clause can be removed
	SL811Write( EP0AXferLen, bLen );
	if ( bSeq )
	{
		SL811Write( EP0AControl, DATA1_IN );
	}
	else
	{
		SL811Write( EP0AControl, DATA0_IN );
	}
}



void EP0A_OUT_ARM(INT8U bLen, INT8U bSeq /* = 1  */)
{
	SL811Write( EP0AAddress, EP0A_Slave_Buf );		// this clause can be removed
	SL811Write( EP0AXferLen, bLen );

	if ( bSeq )
	{
		SL811Write( EP0AControl, DATA1_OUT );
	}
	else
	{
		SL811Write( EP0AControl, DATA0_OUT );
	}
	
}


void EP1A_IN_ARM(INT8U bLen )
{
	SL811Write( EP1AAddress, EP1A_Slave_Buf );		// this clause can be removed
	SL811Write( EP1AXferLen, bLen );
}

void EP2A_OUT_ARM( INT8U bLen )
{
	SL811Write( EP2AAddress, EP2A_Slave_Buf );		// this clause can be removed
	SL811Write( EP2AXferLen, bLen );
}


void SL811_Init( void )
{
	INT8U i;

	taskStart = 0;
	sof_cnt = 0;
	EP0_CMD = 0;

	sofTimeOut = 0;
	enumDone = 0;

	BUS_POWERED = 0;						// define as a bus powered device
	Slave_USBaddr = 0;						// set to default USB address zero
	Slave_ConfigVal = 0;					// default device config value

	Slave_RemoteWU = 0;						// device remote wakeup support
	Slave_inEPstall = 0;					// EP0 ~ EP7's IN
	Slave_outEPstall = 0;					// EP0 ~ EP7's OUT

	for( i = 0;i< MAXIFCNUM; ++i )				// reset alternate setting
	{
		Slave_IfcAlt[i] = 0;
	}

	for( i = 0;i<EP0_LEN; ++i )				// clear EP0 Buffer
	{
		SL811Write( EP0A_Slave_Buf + i,0 );
	}
	for(i = 0;i < EP1_LEN; ++i )			// clear EP1 Buffer
	{
		SL811Write( EP1A_Slave_Buf + i, 0 );
	}

	for ( i = 0; i < EP2_LEN; ++i )
	{
		SL811Write( EP2A_Slave_Buf + i, 0 );
	}

	// set bit 7 to 0, change to slave mode, bit 6 is 0, no change the polarity, full speed
	SL811Write( cSOFcnt, 0x00 );

	// disable all interrupts
	SL811Write( IntEna, 0x00 );

	// USB device's origin address
	SL811Write( USBAddress, 0x00 );

	// EP0 ARM, wait data 
	SL811Write( EP0AAddress, EP0A_Slave_Buf );
	SL811Write( EP0AXferLen, EP0_LEN );
	SL811Write( EP0AControl, DATA0_OUT );

	// Enable EP1, response with NAK 
	SL811Write( EP1AAddress, EP1A_Slave_Buf );
	SL811Write( EP1AXferLen, EP1_LEN );
	SL811Write( EP1AControl, DATA0_IN_ENABLE );

	// Enable EP2, response with NAK
	SL811Write( EP2AAddress, EP2A_Slave_Buf );
	SL811Write( EP2AXferLen, EP2_LEN );
	SL811Write( EP2AControl, DATA0_OUT_ENABLE );

	// enable SOF, EP0, EP1, USB Reset interrupts
	SL811Write( IntEna, 0x67 );
	// clear all interrupts
	SL811Write( IntStatus, 0xFF );
	SL811Write( CtrlReg, 0x01 ); 
}

void SL811Slave_Init( void )
{
	outpw( SL811_MODE_PORT, 0x05 );

	while ( SL811MemTest() )
	{
		continue;
	}

	outpw( SL811_MODE_PORT, 0x01 );
	delay( 10 );
	outpw( SL811_MODE_PORT, 0x05 );
}


void SL811Host_Init( void )
{
	outpw( SL811_MODE_PORT, 0x00 );

	outpw( SL811_MODE_PORT, 0x04 );
	SL811Write( cSOFcnt, 0x80 );
	delay( 100 );
	outpw( SL811_MODE_PORT, 0x02 );
	delay( 100 );
}

INT16U SL811MemTest()
{
	INT16U errors = 0;
	INT8U i = 0;
	for( i = EP0A_Slave_Buf;i < cMemEnd; ++i)
	{
		SL811Write(i,i);
	}
	for(i = EP0A_Slave_Buf;i < cMemEnd; ++i)
	{
		if(i != SL811Read( i ))
		{
			++errors;
		}
		SL811Write(i,(INT8U)(~i));

		if( ( INT8U )(~i) != SL811Read( i ) )
		{
			++errors;
		}
	}


	for( i = EP0A_Slave_Buf, outpw( SL811_ADDR_PORT, EP0A_Slave_Buf ); i < cMemEnd; ++i)
	{
		outpw( SL811_DATA_PORT,i);
	}

	for( i = EP0A_Slave_Buf, outpw( SL811_ADDR_PORT, EP0A_Slave_Buf ); i < cMemEnd; ++i)
	{
		if( i != (inpw( SL811_DATA_PORT )&0xFF ) )
		{
			++errors;
		}
	}

	for( i = EP0A_Slave_Buf, outpw( SL811_ADDR_PORT,EP0A_Slave_Buf ); i<cMemEnd; ++i )
	{
		outpw( SL811_DATA_PORT, 0 );
	}

	return errors;
}


void EP0_ISR( void )
{

	INT8U status, byte_rx, len_xfr;

	INT8U req_type;

	// Clear EP0 interrupt flag
	SL811Write( IntStatus, EP0_DONE );

	// Read the handshake info about last transfer  
	status = SL811Read( EP0AStatus );

	// Read the bytes remain to receive, important for OUT token 
	byte_rx = SL811Read(EP0AXferLen) - SL811Read(EP0ACounter);



	//printf( "0X-", status );

	//puts( "EP0_ISR" );

	if ( status & EP_ACK )				// last pack ACK
	{	
		//puts("EP0_ACK");
		// the origin address of usb device is 0, when the host set a new address, 
		// set the new address, then clear this variable to 0
		if (Slave_USBaddr)				
		{  
			SL811Write(USBAddress,Slave_USBaddr);
			Slave_USBaddr = 0;  
		}

		
		if(status & EP_SETUP)
		{
			//puts("EP_SETUP");
			SL811BufRead(EP0A_Slave_Buf, (INT8U*)&dReq, byte_rx);    /* Read 8-byte command */
			len_req = dReq.wLength;

			req_type = (dReq.bmRequest&0x60)>>5;

			//cout << "REQ_TYPE: " << req_type << endl;
			//printf( "REQ_TYPE: %0X", req_type  );
			switch( req_type )
			{
			case STD_REQUEST:
				switch (dReq.bRequest)
				{
				case GET_DESCRIPTOR:
					switch ((INT8)(dReq.wValue>>8))
					{
					case DEVICE:
						SL811BufWrite(EP0A_Slave_Buf,(INT8U*)Dev_Descp,DEV_LEN);
						len_req = (len_req >= DEV_LEN) ? DEV_LEN:len_req;

						break;

					case CONFIGURATION:
						SL811BufWrite(EP0A_Slave_Buf,(INT8U*)Cfg_Descp,CFG_LEN);	// load Config Descp	
						len_req = (len_req >= CFG_LEN) ? CFG_LEN:len_req;


						break;	
					case STRING:
						switch(dReq.wValue & 0x0F)									// get string index
						{
						case 0x00: SL811BufWrite(EP0A_Slave_Buf,(INT8U*)LangString,LangString[0]);
							len_req = (len_req>=LangString[0]) ? LangString[0]:len_req;

							break;				
						case 0x01: SL811BufWrite(EP0A_Slave_Buf,(INT8U*)MfgString,MfgString[0]);
							len_req = (len_req>=MfgString[0]) ? MfgString[0]:len_req;

							break;		
						case 0x02: SL811BufWrite(EP0A_Slave_Buf,(INT8U*)ProdString,ProdString[0]);
							len_req = (len_req>=ProdString[0]) ? ProdString[0]:len_req;

							break;		
						}
						break;
					default:
						//gErrorCode = 0x0001;
						break;
					}


					len_xfr = (len_req >= EP0_LEN) ? EP0_LEN : (INT8U)len_req;
					EP0A_IN_ARM(len_xfr,1);
					break;

				case GET_STATUS:
					switch(dReq.bmRequest&0x03)							// check for recipients
					{
					case RECIPIENT_DEV:									// load current device status
						SL811Write(EP0A_Slave_Buf,(Slave_RemoteWU<<1)|BUS_POWERED);	
						break;
					case RECIPIENT_IFC:							
						SL811Write(EP0A_Slave_Buf,0);					// first byte = 0
						break;
					case RECIPIENT_ENP:							
						if((dReq.wIndex>>8) & 0x80)						// for IN direction endpoint
						{
							if(Slave_inEPstall & (0x01<<((dReq.wIndex>>8)&0x0F)))	
								SL811Write(EP0A_Slave_Buf,1);			// first byte = 1 (IN endpoint stall)		
							else
								SL811Write(EP0A_Slave_Buf,0);			// first byte = 0 (IN endpoint not stall)		
						}
						else									 		// for OUT direction endpoint
						{
							if(Slave_outEPstall & (0x01<<((dReq.wIndex>>8)&0x0F)))	
								SL811Write(EP0A_Slave_Buf,1);			// first byte = 1 (OUT endpoint stall)		
							else
								SL811Write(EP0A_Slave_Buf,0);			// first byte = 0 (OUT endpoint not stall)		
						}
						break;
					}
					SL811Write(EP0A_Slave_Buf+1,0);							// second byte = 0
					EP0A_IN_ARM(2,1);						// send 2 bytes data back to host

					break;

				case CLEAR_FEATURE:
				case SET_FEATURE:

					//puts("feature");

					switch(dReq.bmRequest&0x03)								// check for recipients,00,01,02
					{
					case RECIPIENT_DEV://recipient is device
						if((dReq.wValue>>8) == 1)						// feature selector = 1 (remote wakeup)
						{
							if(dReq.bRequest == SET_FEATURE)				// set remote wake up status	
								Slave_RemoteWU = 1;						
							if(dReq.bRequest == CLEAR_FEATURE)			// clear remote wake up status		
								Slave_RemoteWU = 0;						
							EP0A_IN_ARM(0,1);							// IN status stage, no data send
						}
						else
							SL811Write(EP0AControl,SEND_STALL);			// Stall unsupported requests,0 is stall					
						break;

					case RECIPIENT_ENP://recipient is endpoint
						if((dReq.wValue) == 0)						// feature selector = 0 (endpoint stall)
						{
							if((dReq.wIndex>>8) & 0x80)					// for IN direction endpoint
							{
								if(dReq.bRequest == SET_FEATURE)			// set endpoint stall (limit to 7 IN's data endpoint)
									Slave_inEPstall |=  (0x01<<((dReq.wIndex>>8)&0x0F)); 
								if(dReq.bRequest == CLEAR_FEATURE)		// clear endpoint stall (limit to 7 IN's data endpoint)
									Slave_inEPstall &=  ~(0x01<<((dReq.wIndex>>8)&0x0F)); 
							}
							else									 	// for OUT direction endpoint
							{
								if(dReq.bRequest == SET_FEATURE)			// set endpoint stall (limit to 7 OUT's data endpoint)
									Slave_outEPstall |=  (0x01<<((dReq.wIndex>>8)&0x0F)); 
								if(dReq.bRequest == CLEAR_FEATURE)		// clear endpoint stall (limit to 7 OUT's data endpoint)
									Slave_outEPstall &=  ~(0x01<<((dReq.wIndex>>8)&0x0F)); 
							}
							EP0A_IN_ARM(0,1);							// IN status stage
						}		
						else
							SL811Write(EP0AControl,SEND_STALL);			// Stall unsupported requests						
						break;

					default: 
						//puts("");
						SL811Write(EP0AControl,SEND_STALL);				// Stall all unsupported requests
						break;
					}

					break;

				case SET_ADDRESS:
					Slave_USBaddr = (INT8U)dReq.wValue;

					//EP0A_IN_Arm(EP0A_Slave_Buf,0,1);
					SL811Write(EP0AXferLen,0);
					SL811Write(EP0AControl,0x47);

					break;


				case GET_CONFIG:
					SL811Write(EP0A_Slave_Buf,Slave_ConfigVal);
					EP0A_IN_ARM(1,1);

					break;

				case GET_INTERFACE:
					SL811Write(EP0A_Slave_Buf,Slave_IfcAlt[dReq.wIndex]);// load current alternate setting

					EP0A_IN_ARM(1,1);									// send 1 INT8U data back to host


					break;

				case SET_CONFIG:
					Slave_ConfigVal = dReq.wValue;  /* fall through */
					//EP2_cmd = 0x47;
					//EP1_cmd = 0x43;

					// IN change to toggle after data completion
					EP1_IN_CMD = 0x07;
					EP2_OUT_CMD = 0x03;
					EP0A_IN_ARM(0,1);

					break;
					//case SET_DESCRIPTOR:
				case SET_INTERFACE:
					Slave_IfcAlt[dReq.wIndex] = dReq.wValue;			    // selected interface number
					EP0A_IN_ARM(0,1);
					//EP2_cmd = 0x47;
					//EP1_cmd = 0x43;

					// IN change to toggle after data completion
					EP1_IN_CMD = 0x07;
					EP2_OUT_CMD = 0x03;
					enumDone = 1;
					break;	
				default:
					SL811Write(EP0AControl,SEND_STALL);						// Stall all unsupported requests

					break;
				}				
				break;
			case VENDOR_REQUEST:

				//puts("Vendor request!");
				//printf( "Vendor: %0X", req_type  );
				//cout<< len_req <<endl;

				if ( len_req )
				{
					EP0_rq_len = len_req;					
					EP0_xfr_len = (EP0_rq_len >= EP0_LEN) ? EP0_LEN : (INT8U)EP0_rq_len;
					SL811Write(EP0AXferLen,EP0_xfr_len);				            // max length of transfer allowed
					EP0_CMD = 0x43;
					SL811Write(EP0AControl,EP0_CMD);	
				}
				else
				{
					// return 0 data pack
					SL811Write(EP0AXferLen,0);
					SL811Write(EP0AControl,0x47);
					flipFlag = 1;					
				}
				break;
			default:
				SL811Write(EP0AControl, 0x47);
				break;
			}
		}
		else if ( SL811Read(EP0AControl) & DIRECTION )
		{

			if ( flipFlag )
			{
				flipFlag = 0;
				taskStart = 1;
			}
			EP0A_OUT_ARM(EP0_LEN, 1);
			return;
		} 
		else if ( !byte_rx )
		{
			EP0A_OUT_ARM(EP0_LEN, 1);
			return;
		}
		else 
		{
			//byte_rx = SL811Read(EP0AXferLen) - SL811Read(EP1ACounter);
			if ( !(EP0_rq_len - EP0_xfr_len) || ( byte_rx != EP0_LEN ))
			{

				flipFlag = 1;
				
				SL811Write(EP0AXferLen,0);
				SL811Write(EP0AControl,0x47);
				SL811BufRead(EP0A_Slave_Buf,(INT8U *)COMMONBUF,byte_rx);
			}	
		}

	}	
}


INT8U EP1_IN_ISR( void )
{
	INT8U intr;
	INT8U result = 0;

	// the up driver return direct on 0 size bulk read, so we return directly
	//if ( !EP1_single_xfr )
	//{
	//	return TRUE;
	//}

	// recover the buffer index
	EP1_buffer_idx = 0;



	EP1_xfr_len = ( EP1_single_xfr >= EP1_LEN ) ? EP1_LEN : EP1_single_xfr;

	SL811BufWrite( EP1A_Slave_Buf, (INT8U *)pInBuf, EP1_xfr_len );

	// update the new buffer index
	EP1_buffer_idx += EP1_xfr_len;

	// data toggle
	// EP1_IN_CMD = ( EP1_IN_CMD & 0x40 ) ? 0x07 : 0x47;

	SL811Write( EP1AAddress, EP1A_Slave_Buf );
	SL811Write( EP1AXferLen, EP1_xfr_len );
	SL811Write( EP1AControl, EP1_IN_CMD );

	EP1_IN_CMD = ( EP1_IN_CMD & 0x40 ) ? 0x07 : 0x47;


	while ( TRUE )
	{
		while ( TRUE )
		{
			intr = SL811Read( IntStatus );

			if ( intr & EP1_DONE )				// data transfer complete
			{
				result = SL811Read( EP1AStatus );
				SL811Write( IntStatus, EP1_DONE );
				break;
			}
			else if ( intr & EP0_DONE )			// host send a new cmd
			{
				// resume the last no success command
				EP1_IN_CMD = ( EP1_IN_CMD & 0x40 ) ? 0x07 : 0x47;
				//SL811Write( EP1AAddress, EP1A_Slave_Buf );
				//SL811Write( EP1AXferLen, EP1_LEN );

				// cancel the last data xfr
				SL811Write( EP1AControl, EP1_IN_CMD & 0xFE );

				return FALSE;
			}
			else if ( intr & USB_RESET_DONE )	// USB device reseted by host
			{
				//cout << "Usb reset!" << endl;
				return FALSE;
			}


			//if ( _bios_keybrd( _KEYBRD_READY ) )					// check key
			//{
			//	return FALSE;
			//}
		}
		


		if ( result & EP_ACK )						// last transfer succeed
		{
			//puts( "OK!" );
			if ( !( EP1_single_xfr -= EP1_xfr_len ) )
			{
				break;
			}

			EP1_xfr_len = ( EP1_single_xfr >= EP1_LEN ) ? EP1_LEN : EP1_single_xfr;

			SL811BufWrite( EP1A_Slave_Buf, ( INT8U *)( pInBuf + EP1_buffer_idx ), EP1_xfr_len );
			// EP1_IN_CMD = ( EP1_IN_CMD & 0x40 ) ? 0x07 : 0x47;

			EP1_buffer_idx += EP1_xfr_len;

			SL811Write( EP1AAddress, EP1A_Slave_Buf );
			SL811Write( EP1AXferLen, EP1_xfr_len );
			SL811Write( EP1AControl, EP1_IN_CMD );

			// next time the data toggle
			EP1_IN_CMD = ( EP1_IN_CMD & 0x40 ) ? 0x07 : 0x47;
		}
		else
		{
			// retry
			// restore last toggle
			//puts( "Retry!!!" );
			EP1_IN_CMD = ( EP1_IN_CMD & 0x40 ) ? 0x07 : 0x47;
			SL811Write( EP1AAddress, EP1A_Slave_Buf );
			SL811Write( EP1AXferLen, EP1_xfr_len );
			SL811Write( EP1AControl, EP1_IN_CMD );
		}
	}

	SL811Write( EP1AControl, EP1_IN_CMD & 0xFE );
	return TRUE;
}

INT8U EP2_OUT_ISR( void )
{
	INT8U byte_rx;
	INT8U intr;
	INT8U result = 0;

	if ( !EP2_single_xfr )
	{
		return TRUE;
	}

	EP2_buffer_idx = 0;
	EP2_xfr_len = (EP2_single_xfr >= EP2_LEN) ? EP2_LEN : EP2_single_xfr;

	SL811Write( EP2AAddress, EP2A_Slave_Buf );
	SL811Write(EP2AXferLen,EP2_xfr_len );// max length of transfer allowed
	SL811Write(EP2AControl,EP2_OUT_CMD );
	EP2_OUT_CMD =  ( EP2_OUT_CMD & 0x40 )  ? 0x03 : 0x43;

	while (TRUE)
	{
		while ( TRUE )
		{
			intr = SL811Read(IntStatus);

			if ( intr & EP2_DONE )
			{
				SL811Write(IntStatus,EP2_DONE);
				result = SL811Read(EP2AStatus);
				break;
			}
			else if ( intr & EP0_DONE )
			{
				// resume the current command
				EP2_OUT_CMD =  (EP2_OUT_CMD & 0x40)  ? 0x03 : 0x43;

				// cancel the last data xfr
				SL811Write( EP2AControl, EP2_OUT_CMD & 0xFE );
				return FALSE;
			}			
			else if ( intr & USB_RESET_DONE )
			{
				return FALSE;
			}

			//if ( _bios_keybrd( _KEYBRD_READY ))
			//{
			//	return FALSE;
			//}
		}

		if ( result & EP_ACK )
		{
			//puts( "OK!" );
			byte_rx = SL811Read(EP2AXferLen) - SL811Read(EP2ACounter);
			SL811BufRead(EP2A_Slave_Buf,(INT8U *)(pOutBuf + EP2_buffer_idx),byte_rx);		

			/*
			for ( int i = 0; i < byte_rx; ++i)
			{
			putchar(DATA_OUT_BUF[i]);
			}

			getch();
			*/
			EP2_buffer_idx += byte_rx;

			if ( !(EP2_single_xfr -= byte_rx) || ( byte_rx != EP2_LEN ) )
			{
				break;
			}

			EP2_xfr_len = (EP2_single_xfr >= EP2_LEN) ? EP2_LEN : EP2_single_xfr;			
			// EP2_OUT_CMD =  (EP2_OUT_CMD & 0x40)  ? 0x03 : 0x43;
			SL811Write( EP2AAddress, EP2A_Slave_Buf );
			SL811Write( EP2AXferLen,EP2_xfr_len );// max length of transfer allowed
			SL811Write( EP2AControl,EP2_OUT_CMD );

			EP2_OUT_CMD =  (EP2_OUT_CMD & 0x40)  ? 0x03 : 0x43;
		}
		else 
		{
			puts( "Error!" );
		}
	}	

	SL811Write( EP2AControl, EP2_OUT_CMD & 0xFE );
	return TRUE;	
}

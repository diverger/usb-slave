// file hal.cpp
#include "defs.h"
#include <conio.h>
#include "HAL.H"


void SL811BufRead( unsigned char bRegAddr, unsigned char *pDes, unsigned char bNum )
{	
	
	unsigned char i;
	
	outpw( SL811_ADDR_PORT,bRegAddr );
	for ( i = 0; i < bNum; ++i)
	{
		*pDes++ = inpw(SL811_DATA_PORT)&0xFF;
	}
}

void SL811BufWrite( unsigned char bRegAddr, unsigned char *pOrg, unsigned char bNum )
{	

	if ( bNum == 0)
	{
		return;
	}
	outpw(SL811_ADDR_PORT,bRegAddr );
	while( bNum-- )
	{		
		outpw(SL811_DATA_PORT,*pOrg++);		
	}
}

//----------------------------------------------------------------------------------------------------
/**
* SL811 register read function
* @param[in] bRegAddr is the sl811's register address
* @return the value of the register
* @remarks none
* @note none
*/
//----------------------------------------------------------------------------------------------------
unsigned char SL811Read(unsigned char bRegAddr )
{  
	outpw( SL811_ADDR_PORT, bRegAddr );
	return( inpw(SL811_DATA_PORT)&0xFF );
}


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
void SL811Write(unsigned char bRegAddr, unsigned char bData )
{  
	outpw( SL811_ADDR_PORT, bRegAddr );
	outpw(SL811_DATA_PORT, bData );
}


unsigned short WordSwap(unsigned short bWord )
{
	return((( bWord & 0x00FF )<<8)|(( bWord &0xFF00 )>>8));
}

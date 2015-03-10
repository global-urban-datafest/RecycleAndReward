
#include <reg51.h>
#include "max7219.h"


unsigned char code Seg_test[8]={0x40,0x20,0x10,0x08,0x04,0x02,0x01,0x80};						 
unsigned char code Number_arr[10]={0x7e,0x30,0x6d,0x79,0x33,0x5b,0x5f,0x70,0x7f,0x7b};				 
unsigned char code Character_arr[53]={0x77,0,0x4e,0,0x4f,0x47,0,0x37,0,0,0,0x0e,0,0,0,				 
									  0x67,0x73,0,0,0,0x3e,0,0,0,0,0,0,0,0,0,0x08,0,0,0x1f,			 
									  0x0d,0x3d,0,0,0,0x17,0,0,0,0x06,0,0x15,0x1d,0,0,0x05,0,0,0x1c};

void DISPLAY_TEST(void)
{
	unsigned char i;
	for(i=0;i<=SNUM;i++) 
	{
		Write7219(i,DISPLAY_TEST_REGISTER,0x01);
	}
}
void DISPLAY_NORMAL(void)
{
	unsigned char i;
	for(i=0;i<=SNUM;i++) 
	{
		Write7219(i,DISPLAY_TEST_REGISTER,0x00);
	}
}
void SHUT_DOWN(void)
{
	unsigned char i;
	for(i=0;i<=SNUM;i++) 
	{
		Write7219(i,SHUT_DOWN_REGISTER,0x00);
	}
}
void RE_DISPLAY(void)
{
	unsigned char i;
	for(i=0;i<=SNUM;i++) 
	{
		Write7219(i,SHUT_DOWN_REGISTER,0x01);
	}
}
void OPEN_DECODE_MODE(void)
{
	unsigned char i;
	for(i=0;i<=SNUM;i++) 
	{
		Write7219(i,DECODE_MODE_REGISTER,DECODE_MODE_INIT);
	}
}
void CLOSE_DECODE_MODE(void)
{
	unsigned char i;
	for(i=0;i<=SNUM;i++) 
	{
		Write7219(i,DECODE_MODE_REGISTER,0x00);
	}
}
void SET_INTENSITY(unsigned char level)
{
	unsigned char i;
	for(i=0;i<=SNUM;i++) 
	{
		Write7219(i,INTENSITY_REGISTER,level);
	}
}
void CLEAR_DISPLAY(void)
{
	unsigned char i,j;
	for(i=0;i<=SNUM;i++) 
	{
		for(j=1;j<=8;j++)
		{
			Max7219_UserDecode_Display(i,j,' ',0);
		}
	}
}

void Init_Max7219(void)
{
	unsigned char i;
	for(i=1;i<=SNUM;i++)
	{
		Write7219(i,SHUT_DOWN_REGISTER,SHUT_DOWN_INIT);				
		Write7219(i,DISPLAY_TEST_REGISTER,DISPLAY_TEST_INIT);		
		Write7219(i,DECODE_MODE_REGISTER,DECODE_MODE_INIT);			
		Write7219(i,SCAN_LIMIT_REGISTER,SCAN_LIMIT_INIT);			
		Write7219(i,INTENSITY_REGISTER,INTENSITY_INIT);				
	}
}
void Write7219(unsigned char num,unsigned char address,unsigned char dat)
{
	unsigned char i,j;
	CS=0;					

	for(i=0;i<(SNUM-num);i++)
		for(j=0;j<16;j++)
		{
			CLK=0;			
			DI=0;
			CLK=1;
		}

	for (i=0;i<8;i++) 		
	{
		CLK=0;				
		if((address &0x80))	
		{
			DI=1;
		}
		else
		{
			DI=0;
		}

		address <<=1; 		
		CLK=1;				 
	}

	for (i=0;i<8;i++)
	{
		CLK=0;
		if((dat&0x80))
		DI=1;
		else
		DI=0;
		dat <<=1; 
		CLK=1;
	}
	for(i=0;i<(num-1);i++)
		for(j=0;j<16;j++)
		{
			CLK=0;			
			DI=0;
			CLK=1;
		}

	CS=1;					
}
void Init_system()
{
	CS=0;
	DI=0;
	CLK=0;
	CS=1;
	DI=1;
	CLK=1;
}
void Max7219_Decode_Display(unsigned char num,unsigned char adress,unsigned char c,unsigned char dot)
{
	OPEN_DECODE_MODE();
	
	if((dot&0x01)==0x01)	
	{
		c=c|0x80;
	}
	Write7219(num,adress,c);
}

void Max7219_UserDecode_Display(unsigned char num,unsigned char adress,unsigned char c,unsigned char dot)
{
	CLOSE_DECODE_MODE();
	
	if((dot&0x01)==0x01)							
	{
		c=c|0x80;
	}
	if(c>=0&&c<=9)								
	{
		Write7219(num,adress,Number_arr[c]);
	}
	if(c>=65&&c<=117)							
	{
		Write7219(num,adress,Character_arr[(c-65)]);
	}
	if(c==45)										
	{
		Write7219(num,adress,0x01);
	}
	if(c==32)										
	{
		Write7219(num,adress,0x00);
	} 
	if(c==61)									    
	{
		Write7219(num,adress,0x09);
	}
	if(c==46)									   
	{
		Write7219(num,adress,0x80);
	}
}

void Max7219_NoDecode_Display(unsigned char num,unsigned char adress,unsigned char c)
{
	Write7219(num,adress,c);
}
/***************************************END********************************************/

void Lcd_show_number(unsigned int weight)
{	
	unsigned int i,number = weight;
	unsigned char show_addr;
	unsigned char max_bit;

	for (i=6; i!=0; i--)
	{
		max_bit = number/(10^i);
		if(max_bit != 0) {
			show_addr++;
			Max7219_UserDecode_Display(1,show_addr,max_bit,0);		
			number = number%(10^i);
		}
	}
}


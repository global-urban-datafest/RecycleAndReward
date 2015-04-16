
#ifndef _MAX7219_H_
#define _MAX7219_H_

sbit CS = P1^2;
sbit DI = P1^0;
sbit CLK = P1^1;

#define SNUM 2									


extern unsigned char code Seg_test[8];		
extern unsigned char code Number_arr[10];		
extern unsigned char code Character_arr[53];	
												

#define DECODE_MODE_INIT			0xFF			
													
													
													
#define INTENSITY_INIT				0x03			
#define SCAN_LIMIT_INIT				0x04			
													
#define SHUT_DOWN_INIT				0x01		   	
#define DISPLAY_TEST_INIT			0x00			



#define DECODE_MODE_REGISTER 	0x09                
#define INTENSITY_REGISTER		0x0A                
#define SCAN_LIMIT_REGISTER 	0x0B                
#define SHUT_DOWN_REGISTER 		0x0C               
#define DISPLAY_TEST_REGISTER 	0x0F                


void DISPLAY_TEST(void);												
void DISPLAY_NORMAL(void);		  										
void SHUT_DOWN(void);													
void RE_DISPLAY(void);													
void OPEN_DECODE_MODE(void);											
void CLOSE_DECODE_MODE(void);											
void SET_INTENSITY(unsigned char level); 								
void CLEAR_DISPLAY(void);												


void Init_system();	   												
void Init_Max7219(void);												
void Write7219(unsigned char num,unsigned char address,unsigned char dat);
void Max7219_Decode_Display(unsigned char num,unsigned char adress,unsigned char c,unsigned char dot);		
																					   					
																										
void Max7219_UserDecode_Display(unsigned char num,unsigned char adress,unsigned char c,unsigned char dot);	
																					   					
																										
void Max7219_NoDecode_Display(unsigned char num,unsigned char adress,unsigned char c);

#endif
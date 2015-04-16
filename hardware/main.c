
#include <intrins.h>
#include <stdio.h>
#include <math.h>
#include "LPCREG.H"
#include "max7219.h"
#include<stdarg.h>  

#define uchar unsigned char
#define uint  unsigned int


#define MAIN_Fosc		18432000L

#define	STATUS_SLEEP	0
#define	STATUS_BARCODE_CHECK	1
#define	STATUS_WEIGHT_CHECK	2
#define	STATUS_GPRS_SEND	3
#define DOOR_CLOSE   0
#define DOOR_OPEN    1

#define  TL0_5MS_INI	0X25
#define  TH0_5MS_INI	0XE2

#define  TL1_5MS_INI	0X25
#define  TH1_5MS_INI	0XE2

#define A_X128 (128)
#define B_X32  (32)
#define A_X64  (64)

unsigned int idata g_ucGlobalState = 0;
unsigned int idata SystemTime = 0;
unsigned int idata weight_time = 0;
unsigned long prior_total_weight = 0;


sbit P_TXD1 = P2^6;
sbit BARCODE_TRIGGER = P2^7;
sbit ADSK = P2^2;  //ADSK
sbit ADDO = P2^3;  //ADDO
sbit DOORST = P0^2;  //door status
sbit DOORLS = P0^1;  //door limit switch

void	Tx1Send(uchar dat);
void    sendstring(unsigned char *string);
void    uart_printf(const char *fmt,...);
void    Lcd_show_number(unsigned int weight);
/************* GPRS **************/
#define PHASE_ATE0	2
#define PHASE_BEARER_CONFIG1	3
#define PHASE_BEARER_CONFIG2	4
#define PHASE_GPRS_OPEN	    14
#define PHASE_GPRS_QUERY	15
#define PHASE_HTTP_INIT	16
#define PHASE_HTTP_PARA1	17
#define PHASE_HTTP_PARA2	18
#define PHASE_HTTP_GET	28
#define PHASE_HTTP_READ	29
#define PHASE_HTTP_TERM	30
#define PHASE_GPRS_CLOSE	31

#define PHASE_CMGD	50
#define PHASE_CMGS_NUMBER	51
#define PHASE_CMGS_CONTENT	52

#define PHASE_NULL	127

#define RECEIVE_BUFFER_SIZE 16
#define SEND_BUFFER_SIZE 16

unsigned char code phone_number[11]="18618600000";
unsigned int code g_CollectorSn=10001;

unsigned char read_data_counter;
unsigned int idata ActionWaitTime;

unsigned char g_usPhase = 0;
char idata encode_data[SEND_BUFFER_SIZE];
char idata read_buffer[RECEIVE_BUFFER_SIZE];
char idata barcode_buffer[RECEIVE_BUFFER_SIZE];


extern void* memset(void* s, char ch, int n);
void sim900_http(void);
char com_send_reply(char* s, unsigned char s_len, unsigned char wait_time);
int strlen(char* lpString);

void Lcd_show_number(unsigned int num);

unsigned int read_current_weight_data;

char* mystrchr(char* s,char c)
{
  while(*s != '\0' && *s != c)
  {
    ++s;
  }
  return *s == c ?s:0;
}

char *mystrstr(const char *s1, const char *s2)
{
	int n;
	if (*s2)
	{
		while (*s1)
		{
			for (n=0; *(s1 + n) == *(s2 + n); n++)
			{
				if (!*(s2 + n + 1))
					return (char *)s1;
			}
			s1++;
		}
		return 0;
	}
	else
		return (char *)s1;
}

void t0(void) interrupt 0// using 2   
{
}

void timer0 (void) interrupt 1    
{
	TH0=TH0_5MS_INI;
	TL0=TL0_5MS_INI;

	if (SystemTime != 0)
	{
		SystemTime--;
	}

	if (ActionWaitTime != 0)
	{
		ActionWaitTime--;
	}

	if(weight_time != 0)
	{
	   weight_time--;
	}
}

void Init_Com1(void)
{
	PCON = 0x00;
	SCON = 0x50;  
	SM2  = 1;     

	AUXR |= 0x40;		
	AUXR &= 0xFE;		

	TMOD &= 0x0F;		
	TMOD |= 0x20;		
	
	TH1 = 0xFB;
	TL1 = 0xFB;

	TR1 = 1;
	AUXR1=0x00;  //0x80:p1  0x00:p3

}

void Init_Com2(void)
{
	SCON = 0x50;
	AUXR |= 0x40;		
	AUXR &= 0xFE;		

	TMOD &= 0x0F;	
	TMOD |= 0x20;		
	
	TH1 = 0xC4;//FB;
	TL1 = 0xC4;//FB;

	TR1 = 1;
	AUXR1 |= 0x80;

}

void uart_char(char mychar)
{	
	TI = 0;
	RI = 0;
	
	SBUF=mychar;
	while(!TI);				
}

void DelayMs(uint Ms)
{
	uint i,TempCyc;
	for(i=0;i<Ms;i++)
	{
		TempCyc = 250;
		while(TempCyc--);
	}
}

char com_send_reply(char* s, unsigned char s_len, unsigned char wait_time)
{
	unsigned char rec_count;
	unsigned char index;	

	sprintf(encode_data, s);
	for(index = 0; index<s_len; index++)		
	{
		TI=0;
  	    SBUF = encode_data[index];
		while(!TI);
	}


	if(wait_time > 0)
	{
		ActionWaitTime=wait_time*10;
		rec_count = 0;
		memset(read_buffer, 0 ,RECEIVE_BUFFER_SIZE);
		while(ActionWaitTime!=0)
		{
			if ( RI )
			{
				read_buffer[rec_count] = SBUF;			
				RI = 0;
				rec_count++;
	
				if(rec_count>1 && read_buffer[rec_count-1]=='\r' && read_buffer[rec_count-2]!='K')
				{
					rec_count = 0;
					memset(read_buffer, 0 ,RECEIVE_BUFFER_SIZE);
				}
	
				if(rec_count >= RECEIVE_BUFFER_SIZE)
				{
					rec_count = 0;
				}
			}
		}	
		memset(read_buffer+rec_count, 0, 1);
				
		ActionWaitTime = wait_time*10;
		if(mystrstr((char*)read_buffer, "OK"))
		{
			return 1;
		}
		if(mystrstr((char*)read_buffer, "> "))
		{
			return 1;
		}

		if(SystemTime==0)
		{
			return 1;
		}
		

	}

	return 0;
}

void sim900_http()
{
	unsigned int num;
	
	TL1 = 0XFB;
	TH1 = 0XFB;
	TR1 = 1;

	TI = 0;
	RI = 0;

	g_usPhase = PHASE_ATE0;

	while(g_usPhase!=PHASE_NULL)
	{
		if(g_usPhase==PHASE_ATE0)
	   {
	   		if(com_send_reply("ATE0\r\n", 6, 5))
				g_usPhase = PHASE_BEARER_CONFIG1;

		}
		else if(g_usPhase==PHASE_BEARER_CONFIG1)
	   {
			com_send_reply("AT+SAPBR=3,1,\"", 14, 0);

			com_send_reply("Contype\"", 8, 0);

	   		if(com_send_reply(",\"GPRS\"\r\n", 9, 5))
				g_usPhase = PHASE_BEARER_CONFIG2;

		}
		else if(g_usPhase==PHASE_BEARER_CONFIG2)
	   {
			com_send_reply("AT+SAPBR=3,1,\"", 14, 0);

			com_send_reply("APN\"", 4, 0);

	   		if(com_send_reply(",\"CMNET\"\r\n", 10, 5))
				g_usPhase = PHASE_GPRS_OPEN;

		}
		else if(g_usPhase==PHASE_GPRS_OPEN)
		{
	   		if(com_send_reply("AT+SAPBR=1,1\r\n", 14, 25))
				g_usPhase = PHASE_GPRS_QUERY;

	   }
		else if(g_usPhase==PHASE_GPRS_QUERY)
		{
	   		if(com_send_reply("AT+SAPBR=2,1\r\n", 14, 5))
				g_usPhase = PHASE_HTTP_INIT;

	   }
		else if(g_usPhase==PHASE_HTTP_INIT)
		{
	   		if(com_send_reply("AT+HTTPINIT\r\n", 13, 5))
				g_usPhase = PHASE_HTTP_PARA1;

	   }
		else if(g_usPhase==PHASE_HTTP_PARA1)
		{
			com_send_reply("AT+HTTPPARA=\"", 13, 0);

			com_send_reply("CID\"", 4, 0);

	   		if(com_send_reply(",1\r\n", 4, 5))
				g_usPhase = PHASE_HTTP_PARA2;

	   }
		else if(g_usPhase==PHASE_HTTP_PARA2)
		{
			com_send_reply("AT+HTTPPARA=\"", 13, 0);

			com_send_reply("URL\"", 4, 0);

			com_send_reply(",\"sz.lucki.cn:1", 15, 0);

			com_send_reply("8000/traceapp/u", 15, 0);
			
			com_send_reply("pload/?barcode=", 15, 0);

			sprintf(encode_data, "%s", "1000000002"/*barcode_buffer*/, 1);
			com_send_reply(encode_data, strlen(encode_data), 0);
			
			sprintf(encode_data, "&key=%s", "AAAAA", 1);
			com_send_reply(encode_data, strlen(encode_data), 0);
	
			num = read_current_weight_data;
			if(num>1000)
				num = 1000;
			
			sprintf(encode_data, "&weight=%d\"\r\n", read_current_weight_data);
			
	   		if(com_send_reply(encode_data, strlen(encode_data), 5))
				g_usPhase = PHASE_HTTP_GET;
			
	   }
		else if(g_usPhase==PHASE_HTTP_GET)
		{
			com_send_reply("AT+HTTPACTION", 13, 0);

	   		if(com_send_reply("=0\r\n", 4, 35))
				g_usPhase = PHASE_HTTP_READ;
				
	   }
		else if(g_usPhase==PHASE_HTTP_READ)
		{
	   		if(com_send_reply("AT+HTTPREAD\r\n", 13, 35))
				g_usPhase = PHASE_HTTP_TERM;

	   }
		else if(g_usPhase==PHASE_HTTP_TERM)
		{
	   		if(com_send_reply("AT+HTTPTERM\r\n", 13, 5))
				g_usPhase = PHASE_GPRS_CLOSE;

	   }
		else if(g_usPhase==PHASE_GPRS_CLOSE)
		{
	   		if(com_send_reply("AT+SAPBR=0,1\r\n", 14, 5))
				g_usPhase = PHASE_NULL;

	   }
   }
}

void barcode_check(void)
{
  char barcode_rec_count = 0;
  uart_printf("barcode_check\r\n");

TI = 0;
RI = 0;
ActionWaitTime=200;
			TL0 = TL0_5MS_INI;
			TH0 = TH0_5MS_INI;
			TR0 = 1;

barcode_rec_count = 0;
  memset(barcode_buffer, 0 ,RECEIVE_BUFFER_SIZE);
  while(ActionWaitTime!=0)
  {
    if ( RI )
    {
      barcode_buffer[barcode_rec_count] = SBUF;		
      RI = 0;
      barcode_rec_count++;
    }
  }	

  barcode_buffer[barcode_rec_count] = 0;
	
  uart_printf("barcode(%s)\r\n",barcode_buffer);

}

unsigned long ReadValue(unsigned char mode)
{
  unsigned long count;
  unsigned char i;
  ADDO = 1;
  DelayMs(4); 

  switch(mode)
  {
    case A_X128:i=25;break;
	case B_X32 :i=26;break;
	case A_X64 :i=27;break;
    default    :break;
  }

  for(;i;i--)
  {
    ADSK=1;
	ADSK=0;
  }

  ADSK = 0;	     
  count = 0; 
  while(ADDO);	 
  for(i=0;i<24;i++)
  {
    ADSK = 1;	 
	count = count << 1;	 
	ADSK = 0;
	if(ADDO)
	{
	  count++;
	}
  }
  ADSK = 1;
  ADSK = 0;
  if(count&0x800000)
  {
    count|=0xFF800000;
  }
  else
  {
    count&=0x007FFFFF;
  }
  return(count);
}

set_total_weight_value(void)
{
  unsigned long weight;
  weight = ReadValue(A_X128);
  prior_total_weight = weight/1000;
  uart_printf("the prior_total_weight is %lld\r\n",prior_total_weight);
}

unsigned int weight_check(void)
{
  unsigned long weight;
  unsigned long weight_offset;
  uart_printf("weight_check\r\n");
  weight = ReadValue(A_X128)/1000;
  uart_printf("the weighit(%lld)\r\n",weight);

  weight = weight-prior_total_weight;
  weight_offset = abs(weight);
  if(weight_offset < 5)
  {
    weight = 0;
  }
  uart_printf("the weighit1(%lld)\r\n",weight);
  CLEAR_DISPLAY();		
  DISPLAY_NORMAL();		
  DelayMs(10);
  Lcd_show_number((unsigned int)weight);
  weight = (weight * 25)/10;
  uart_printf("the weighit2 is %lld\r\n",weight);
  return weight;
}

main()   
{

	DOORST = 1;                    
	DOORLS = DOOR_OPEN;			  
	uart_printf("****** START ******\r\n");	

	Init_Max7219();			
	DISPLAY_TEST();		
	DelayMs(10);
	CLEAR_DISPLAY();		
	DISPLAY_NORMAL();	
	DelayMs(10);

	TMOD &= 0x21;//20;
	
	g_ucGlobalState = STATUS_SLEEP;


	while(1)
	{
		if(	SystemTime==0 )
		{
			g_ucGlobalState = STATUS_SLEEP;
			ADSK = 1;
			ADDO = 1;
				
	        uart_printf("SLEEP\r\n");	
			CLEAR_DISPLAY();		
			DISPLAY_NORMAL();		
			DelayMs(10);
		}
		//uart_printf("g_ucGlobalState(%d)\r\n",g_ucGlobalState);
		switch(g_ucGlobalState)
		{	
	      case STATUS_SLEEP:
		    ///if ( SystemTime==0 )
			{
			BARCODE_TRIGGER = 1;

			TMOD = 0x21;//20;

			IE=0;
			TR1 = 0;
			TCON=0X30;			// START T0

			WDT_CONTR=15;

			P0 = 0xFF;
			P1 = 0xFF;
			P2 = 0xFF;
			P3 = 0xFF;
			P4 = 0xFF;
			EX0=1;
			EA=1;

			PCON=0X02;  

			_nop_();
			_nop_();
			_nop_();

			EX0=0;

			PCON = 0x00;  
 	        uart_printf("WAKE\r\n");	
			IE=0;
			
			TL0=TL0_5MS_INI;
			TH0=TH0_5MS_INI;
			ET0=1;
			EA=1;
			set_total_weight_value();   //test the pro
			g_ucGlobalState = STATUS_WEIGHT_CHECK;
			SystemTime=5000;	//1s
			//TR0 = 1;
			//TR1 = 1;

			ADSK = 0;
			ADDO = 0;
			break;
			}

		  case STATUS_BARCODE_CHECK:
			{
			  if(DOORLS != DOOR_CLOSE)
			  {
			  	 g_ucGlobalState = STATUS_WEIGHT_CHECK;
				 uart_printf("wait door close\r\n");
				 break;
			  }

			  SystemTime=5000;	//1s

			  BARCODE_TRIGGER = 0;
			  DelayMs(1000);
			  BARCODE_TRIGGER = 1;

			  //Init_Com2();	
			  //barcode_check();
			  g_ucGlobalState = STATUS_WEIGHT_CHECK;
			  break;
			}

		  case STATUS_WEIGHT_CHECK:
			{
              weight_time = 1000;       
			  DOORST = 0;              
			  if(DOORLS != DOOR_CLOSE)
			  {
			     SystemTime=5000;	    
			  	 g_ucGlobalState = STATUS_WEIGHT_CHECK;
				 uart_printf("wait door close\r\n");
			     break;
			  }
			  uart_printf("start weight check!\r\n");
			  DelayMs(5000);
			  DOORST = 1;               
			  SystemTime=5000;	//1s
			  TL1=TL1_5MS_INI;
			  TH1=TH1_5MS_INI;
			  ET1=1;
			  EA=1;

			  DelayMs(3000);
			  SystemTime=5000;	//1s
			  read_current_weight_data = weight_check();

			  uart_printf("the DOORLS(%d),DOORST(%d)\r\n",DOORLS,DOORST);
			  g_ucGlobalState = STATUS_GPRS_SEND;
			  break;
			}

		  case STATUS_GPRS_SEND:
			{
			  BARCODE_TRIGGER = 1;
			  SystemTime=5000;	//1s

			  Init_Com1();	
			  sim900_http();
			  g_ucGlobalState = STATUS_SLEEP;
			  while(1);
			  break;
			}
	      default:
		     g_ucGlobalState = STATUS_SLEEP;
			 break;
		}

		DelayMs(10);

	}
} 

void    BitTime(void)	
{
	uint i;
	i = ((MAIN_Fosc / 100) * 8) / 140000L - 1;		
	while(--i);
}

void	Tx1Send(uchar dat)		//9600，N，8，1		发送一个字节
{
	uchar	i;
	EA = 0;
	P_TXD1 = 0;
	BitTime();
	for(i=0; i<8; i++)
	{
		if(dat & 1)		P_TXD1 = 1;
		else			P_TXD1 = 0;
		dat >>= 1;
		BitTime();
	}
	P_TXD1 = 1;
	EA = 1;
	BitTime();
	BitTime();
}

void sendstring(unsigned char *string)  
{  
    while(*string!='\0')
    {  
        Tx1Send(*string);  
        string++;  
    }  
}

void uart_printf(const char *fmt,...)  
{  
    va_list ap;  
    char xdata string[1024];
    va_start(ap,fmt);  
    vsprintf(string,fmt,ap);
    sendstring(string);  
    va_end(ap);  
}

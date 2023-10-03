/* DIGITAL CLOCK WORKING IN FOUR MODES
PRESS MODE BUTTON TO SWITCH VARIOUS MODES

**********MODE0 - 24 HOUR CLOCK *********
STATUS LED WILL GLOW CONTINUOUSLY
FOR SETTING PRESS RESET BUTTON TO SET TIME OF CLOCK 
FOR VALUE UPGRADATION PRESS UP BUTTON 
FOR CURSOR SHIFTING PRESS RESET BUTTON 
AFTER SETTING IS DONE PRESS START BUTTON TO START THE CLOCK
THE SET VALUE WILL BE IMPLEMENTED ON 12HOUR CLOCK ALSO
AM PM LED WILL NOT GLOW

******** MODE1 - STOP WATCH **********
STATUS LED WILL BLINK EVERY SECOND
PRESS START BUTTON TO START OR PAUSE THE STOP WATCH TIMER.
DISPLAY WILL BLINK WHEN STOP WATCH IS RESUME OR ZERO.
PRESS RESET BUTTON TO RESET THE STOP WATCH TO ZERO.

********MODE2 - TIMER ***********
STATUS LED WILL NOT GLOW
PRESS UP BUTTON FOR VALUE UPGRATION  AND RESET BUTTON FOR CURSOR SHIFTING.
HERE ONLY MINUTE AND SECOND CAN BE SET.
AFTER VALUE IS SET PRESS START BUTTON TO START THE TIMER 
TIMER CAN BE RESUME OR PAUSE BY START BUTTON
WHEN VALUE IS REACHED TO ZERO (DISPLAY WILL BLINK)
TO RESET THE TIMER PRESS STRAT BUTTON TO RESUME THEN PRESS RESET BUTTON TO RESET THE TIMER TO SET VALUE AGAIN

******** MODE3 - 12 HOUR CLOCK ******
STATUS LED WILL GLOW CONTINUOUSLY
FOR SETTING PRESS RESET BUTTON TO SET TIME OF CLOCK 
FOR VALUE UPGRADATION PRESS UP BUTTON 
FOR CURSOR SHIFTING PRESS RESET BUTTON 
AFTER SETTING IS DONE PRESS START BUTTON TO START THE CLOCK
THE SET VALUE WILL BE IMPLEMENTED ON 24HOUR CLOCK ALSO
*/

#include <P18f4580.h>

#define s1 PORTDbits.RD0 
#define s2 PORTDbits.RD1
#define s3 PORTDbits.RD2
#define s4 PORTDbits.RD3
#define mode_switch PORTBbits.RB0
#define start_button PORTBbits.RB1
#define reset_button PORTBbits.RB2
#define up_button PORTBbits.RB3
#define led PORTDbits.RD7
#define buzzer PORTDbits.RD6
#define am_led PORTDbits.RD4
#define pm_led PORTDbits.RD5


void delay(int a);
void timer0_init(void);
void time_update(void);
void display_init(void);
void time_display(unsigned int high , unsigned int low);
void digit_blink(unsigned int i , unsigned int high, unsigned int low);
void initialisation(void);
unsigned short hour_12 ;
unsigned short sec = 0;
unsigned short min = 0;
unsigned short hour = 0;
unsigned short t_min =0; // clock timer minute
unsigned short t_sec = 0; //clock timer second
unsigned short sw_min = 0, sw_sec = 0; //stop watch min , stop watch sec
short mode = 0;
short s_flag = 0;// start_button button flag //1 = start_button , 0 = stop
short r_flag =0; //reset_button button flag
short reset_flag = 0; //to reset the value of timer or setting is going on
short blink_flag =0; //to indicate its blink time //for blink function 

void main()
{
	int arr[] = {0xbf,0x86,0xdb,0xcf,0xe6,0xed,0xfd,0x87,0xff,0xef};//without dp{0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f};
	TRISC = 0x00;
	TRISD = 0x00;
	TRISA = 0x00;
	ADCON1 = 0x0f;
	TRISB = 0xff;
	
	INTCON2bits.INTEDG0 = 0; //falling edge interrupt
   timer0_init();
	display_init();
	initialisation();
	//am_led = 1;
	while(1)
	{
		if((mode==0 || mode ==3) && reset_flag); //do not update time during time setting
		else
			time_update();
/********* 24 HOUR DIGITAL CLOCK MODE ****************/
		if(mode == 0)
		{
			led = 1; //led will be continuously on 
			//time_display(hour,min);
			/****FOR TIME SETTING***/
			if(reset_button == 0) //to enter in setting mode
			{
				while(!reset_button);
				
				if(reset_flag)
				{
					r_flag = ~r_flag;
				}
				else
					reset_flag = ~reset_flag;		
			}
			
			if(up_button == 0 && reset_flag) //to change the value in setting mode
			{
				while(!up_button);
				if(r_flag) //minute setting
				{
					if(min<60)
						min++ ;
					else
						min = 0;
				}
				else  //hour setting
				{
					if(hour<23 && hour>=0)
					{
						hour++ ;
					}	
					else
						hour = 0;
				}
				
			}
			if(start_button == 0)//after setting press start button to set the value
			{
				while(!start_button);
				reset_flag = 0;
			}
			if(reset_flag)//to blink digit in setting mode
			{
				if(r_flag==0 && blink_flag)
					s1 = 1;
				if(r_flag)
					s1 = 1;			
				PORTC = ~(arr[hour/10]); 
				delay(100);
				s1 = 0;
				PORTC = 0xFF; //to clear the display
		
				if(r_flag==0 && blink_flag)
					s2 = 1;
				if(r_flag)
					s2 = 1;	
				PORTC = ~(arr[hour%10]);
				delay(100);
				s2 = 0;
				PORTC = 0xFF;
		
				if(r_flag && blink_flag)
					s3 = 1;
				if(r_flag==0)
					s3 = 1;		
				PORTC = ~(arr[min/10]);	
				delay(100);
				s3 = 0;
				PORTC = 0xFF;
			
				if(r_flag && blink_flag)
					s4 = 1;
				if(r_flag==0)
					s4 = 1;	
				PORTC = ~(arr[min%10]);
				delay(100);
				s4 = 0;
				PORTC = 0xFF;
			}
			else
			{
				time_display(hour,min);	
				
			}		
						
		}	
/*********** STOP WATCH MODE ***************/
		else if (mode == 1) // led will blink every second
		{
			
			if(start_button == 0) //stop watch pause / resume
			{
				while(!start_button);
				s_flag = ~s_flag;
			}
			if(reset_button == 0)// stop watch reset_button to zero
			{
				while(!reset_button);
				sw_sec = 0;
				sw_min = 0;
				s_flag = 0;
			}	
			if(s_flag == 0 )//blink during stop watch is resumed 
			{
				if(blink_flag)
				{
					time_display(sw_min,sw_sec);
				}
				else
				{
					display_init();
				}		
			}	
			else
			{
				time_display(sw_min,sw_sec);
			}	
		}
/*********** TIMER MODE **************/	
		else if(mode == 2)
		{
			
			/*sw_min and sw_sec will be displayed and decresed 
			t_min and t_sec will be stored for reset fuction
			when reset is pressed after start then value will be set original value*/	
			
			if(reset_button ==0 && s_flag == 0 ) 
			{
				
				while(!reset_button);
				if(reset_flag)
				{
					sw_min = t_min;
					sw_sec = t_sec;
					reset_flag = 0;
					r_flag = 0;
				}	
				if(r_flag < 1 )
				{
				//	r_flag++;
					r_flag = 3;
				}
				else
				{
					r_flag-- ;
					//r_flag = 0;
				}
							
			}	
			//value increement of particular digit during setting
			// setting can be done only when time is stop(by pressing reset button) ...timer can be resume by start button and stopped by reset button
			if(up_button == 0 && s_flag == 0 && reset_flag == 0 ) 
			{
				int temp = 0;
				
				while(!up_button);
				if(r_flag == 0) //minute tenth place digit
				{
					temp = t_min / 10;
					if(temp <9 )
					{
						temp++;
					}
					else 
					{
						temp = 0;
					}	
					t_min = (t_min%10) + (temp*10);
				}
				else if(r_flag == 1) //minute unit place digit
				{
					temp = t_min % 10;
					if(temp <9 )
					{
						temp++;
					}
					else 
					{
						temp = 0;
					}	
					t_min = ((t_min/10)*10) + temp;
				}
				else if(r_flag == 2) //second 10th place digit
				{
					temp = t_sec / 10;
					if(temp <9)
					{
						temp++;
					}
					else 
					{
						temp = 0;
					}	
					t_sec = (t_sec%10) + (temp*10);
				}
				else if(r_flag == 3) // second unit place digit
				{
					temp = t_sec % 10;
					if(temp <9 )
					{
						temp++;
					}
					else 
					{
						temp = 0;
					}	
					t_sec = ((t_sec/10)*10) + temp;
				}
				sw_min = t_min;
				sw_sec = t_sec; 				
			}
				
			 
			if (start_button == 0)//timer pause, resume, start
			{
				 while(!start_button);
				 s_flag = ~s_flag;
				 reset_flag = 1;
				 r_flag = 0;
			}
			if(sw_min == 0 && sw_sec == 0 && s_flag) //blink when time up
			{
				if(blink_flag )
				{
					time_display(sw_min,sw_sec);
				}	
			}	
			else if(!s_flag && !reset_flag)
			{
				digit_blink(r_flag, t_min, t_sec);
			}
			else	
				time_display(sw_min,sw_sec);	
		}	
/************** 12 HOUR CLOCK MODE ****************/
		else if(mode == 3)
		{
		
			led = 1; //led will be continuously on 
			//time_display(hour,min);
			/****FOR TIME SETTING***/
			if(reset_button == 0) //to enter in setting mode
			{
				while(!reset_button);
				
				if(reset_flag)// TO SHIFT CURSOR IN SETTING MODE
				{
					if(r_flag<2)
					r_flag++; 
					else
					r_flag = 0;
				}
				else
				{
					reset_flag = ~reset_flag;
					r_flag = 0;
				}			
			}
			
			if(up_button == 0 && reset_flag) //to change the value in setting mode
			{
				while(!up_button);
				if(r_flag == 1) //minute setting
				{
					if(min<60)
						min++ ;
					else
						min = 0;
				}
				else if(r_flag == 0)  //hour setting
				{
					if(hour_12<12 && hour_12>=0)
					{
						hour_12++ ;
					}	
					else
						hour_12 = 0;
				}
				else if(r_flag == 2)
				{
					am_led = ~am_led;
					pm_led = ~pm_led;
				}	
				
			}
			if(am_led ==1 )
			{
				hour = hour_12;
				if(hour_12 == 12)
				hour = 0;
			}	
			else if (pm_led == 1)
			{
				hour = hour_12 + 12;
				if(hour_12 == 12)
				hour = 12;
			}	
			
			if(start_button == 0)//after setting press start button to set the value
			{
				while(!start_button);
				reset_flag = 0;
			}
			if(reset_flag)//to blink digit in setting mode
			{
				if(r_flag==0 && blink_flag)
					s1 = 1;
				if(r_flag==1||r_flag == 2)
					s1 = 1;			
				PORTC = ~(arr[hour_12/10]); 
				delay(100);
				s1 = 0;
				PORTC = 0xFF; //to clear the display
		
				if(r_flag==0 && blink_flag)
					s2 = 1;
				if(r_flag==1||r_flag == 2)
					s2 = 1;	
				PORTC = ~(arr[hour_12%10]);
				delay(100);
				s2 = 0;
				PORTC = 0xFF;
		
				if(r_flag==1 && blink_flag)
					s3 = 1;
				if(r_flag==0||r_flag == 2)
					s3 = 1;		
				PORTC = ~(arr[min/10]);	
				delay(100);
				s3 = 0;
				PORTC = 0xFF;
			
				if(r_flag==1 && blink_flag)
					s4 = 1;
				if(r_flag==0||r_flag == 2)
					s4 = 1;	
				PORTC = ~(arr[min%10]);
				delay(100);
				s4 = 0;
				PORTC = 0xFF;
			}
			else
			{
				time_display(hour_12,min);	
			}		
						
		}	
	}	
}
void delay(int a)
{
	int i;
	for(i=0;i<a;i++);
		for(i=0;i<a;i++);
}
void initialisation()
{
		s_flag = 0;
		r_flag = 3;
		if(mode==0 || mode == 3)//clock mode
			r_flag =0;
			
		sw_min = 0;
		sw_sec = 0;
		if(mode == 2)
		{
			sw_min = t_min;
			sw_sec = t_sec;	
		}
		led = 0;
		buzzer = 0;
		reset_flag = 0;
		display_init();
		am_led = 0;
		pm_led = 0;	
}	
void display_init()
{
	s1 = 0;
	s2 = 0;
	s3 = 0;
	s4 = 0;
}

void digit_blink(unsigned int i , unsigned int high, unsigned int low)	
{
	int arr[] = {0xbf,0x86,0xdb,0xcf,0xe6,0xed,0xfd,0x87,0xff,0xef};//without dp{0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f};
	if(i == 0 )	
		{
			if( blink_flag)
			s1=1;
		}
		else
			s1=1;
			PORTC = ~(arr[high/10]); 
			delay(100);
			s1 = 0;
			PORTC = 0xFF; //to clear the display
		
		if(i == 1 )	
		{
			if( blink_flag)
			s2=1;
		}
		else	
			s2 = 1;
			PORTC = ~(arr[high%10]);
			delay(100);
			s2 = 0;
			PORTC = 0xFF;
		
		if(i == 2)	
		{
			if( blink_flag)
			s3=1;
		}
		else		
			s3 = 1;
			PORTC = ~(arr[low/10]);	
			delay(100);
			s3 = 0;
			PORTC = 0xFF;
		
		if(i == 3 )	
		{
			if( blink_flag)
			s4=1;
		}
		else	
			s4 = 1;
			PORTC = ~(arr[low%10]);
			delay(100);
			s4 = 0;
			PORTC = 0xFF;
}	
			
void time_display(unsigned int high , unsigned int low)
{
	int arr[] = {0xbf,0x86,0xdb,0xcf,0xe6,0xed,0xfd,0x87,0xff,0xef};//without dp{0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f};
		
			s1 = 1;			
			PORTC = ~(arr[high/10]); 
			delay(100);
			s1 = 0;
			PORTC = 0xFF; //to clear the display
	
			s2 = 1;	
			PORTC = ~(arr[high%10]);
			delay(100);
			s2 = 0;
			PORTC = 0xFF;
	
			s3 = 1;		
			PORTC = ~(arr[low/10]);	
			delay(100);
			s3 = 0;
			PORTC = 0xFF;
		
			s4 = 1;	
			PORTC = ~(arr[low%10]);
			delay(100);
			s4 = 0;
			PORTC = 0xFF;
							
}		
void timer0_init()
{
	T0CON = 0x07; //prescalar 256//16bit timer
	INTCON = 0xF0; //global, peripheral,ext0 interrupt enable and t0 oerflow interrpt enable
	TMR0H = 0xF0;//0xFE; //for 1 sec
	TMR0L = 0xBD;//0x79;
	T0CONbits.TMR0ON = 1; ///timer start_button
}	
void time_update()
{
	if(sec >= 60)
		{
			min++ ; 
			sec = 0;
		}	
		if(min >= 60)
		{
			hour++;
			min = 0;
		}	
		if(hour >= 24)
		{
			hour = 0;
			min = 0;
			sec = 0;
		
		}
		if(mode == 3)
		{
			if(hour >=12 )
			{					
				hour_12 = hour - 12;
				if(hour == 12)
					hour_12 = 12;
				pm_led = 1;
				am_led = 0;
			}
		//	else if(hour == 0)
		//	{
		//		hour_12 = 12;
		//		am_led = 1;
		//		pm_led = 0;
		//	}			
			else
			{
				hour_12 = hour;
				if(hour == 0)
					hour_12 = 12;				
				am_led = 1;
				pm_led = 0;
			}	
		}	
		if(mode ==1 && sw_sec ==60)
		{
			sw_min++;
			sw_sec = 0;
		}	
		if(mode == 2 && sw_sec == 0 && sw_min !=0)
		{
			sw_min--;
		}		
}
		
#pragma code ISR = 0x08
#pragma interrupt  ISR



void  ISR(void)
{
	
	if(INTCONbits.TMR0IF == 1 && INTCONbits.TMR0IE == 1)
	{
		TMR0H = 0xF0; //for 1 sec
		TMR0L = 0xBD;
		sec++;
		blink_flag = ~blink_flag;
		
		if(mode == 1)
		{
			led = ~led;
			if(s_flag)
			{
				sw_sec++;
			}	
		}
		if(mode == 2)
		{
			if(s_flag && (sw_sec != 0 || sw_min != 0))
			{
				sw_sec = sw_sec-1 ;
			}
			if(s_flag && sw_sec == 0 && sw_min ==0)
			{
				buzzer = ~buzzer;
				
			}		
		}		
		INTCONbits.TMR0IF = 0;
	}
	else if(INTCONbits.INT0IF == 1 && INTCONbits.INT0IE == 1 )
	{
		initialisation();
		if(mode<3)
		{
			mode++;
		}	
		else
		{
			mode = 0;
		}	
		
		INTCONbits.INT0IF =0;
	}	
	
		
	
}

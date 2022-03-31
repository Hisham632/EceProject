#define ADC_BASE               0xFF204000
//#define LED_BASE	           0xFF200000
#define SW_BASE               0xFF200040
#define JP1_BASE              0xFF200060
#define KEY_BASE              0xFF200050
#define HEX3_HEX0_BASE        0xFF200020
#define HEX5_HEX4_BASE        0xFF200030
#define TIMER_A9_BASE 0xFFFEC600
#define JP1_BASE              0xFF200060

//Define LED base
//volatile int* const led = (int *) LED_BASE;

//Define switch pointer
volatile int * SW_ptr = (int *)SW_BASE;

//Define GPIO pointer
volatile int * GP_ptr = (int *)JP1_BASE;

//Key pointer
volatile int * const key_ptr = (int*)KEY_BASE;

//LED 1 to 4 fromt the right
volatile int* const led = (int *) HEX3_HEX0_BASE;
 
//LED 5 to 6 from the right
volatile int* const led2 = (int *) HEX5_HEX4_BASE;


//Hex code
char hex_code[16] = {0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x67};

//Temperature variable
int outsideTemp = 0;
int insideTemp = 0;
int desiredTemp = 0;
int autoFlag = 0;
int reachedFlag = 0;
int listen = 0;


//Timer structure
typedef struct _a9_timer
{
    int load;
    int count;
    int control;
    int status; 
} a9_timer;
 
volatile a9_timer* const timer_1 = (a9_timer*)TIMER_A9_BASE;


typedef struct _ADC
{
unsigned int ch0 ;
unsigned int ch1 ;
} ADC ;

volatile ADC* const ADC_ptr = (ADC*)ADC_BASE;


typedef struct _GPIO{
unsigned int data ;
unsigned int control ;
} GPIO ;

volatile GPIO* const port_A = (GPIO*) JP1_BASE;



// initializes timer to 'interval'
void set_timer( int interval ){
 
    timer_1->load = interval;
}
 
 
void start_timer() // starts the timer counting, clears previous timeout flag
{
    //Set the status flag in Interrupt Status to 1 
    timer_1->status = 1; 
	
    //3 bc 0 1 1 --> (interrupt, auto, enable)
    //1<<8 --> shift 8 bits from 1
    timer_1->control = 3; 
}
 
 
void stop_timer() // stops timer
{
    //Set the control bit to 2
    timer_1->control = 2;
 
 
} 
 
 
 



//Read switches
int ReadSwitches(void)
{
    //Return the binary value of the switch currently flipped
    return *(SW_ptr);
 
}


int ReadKeys(void){
    //Return binary value of key pressed
    return *(key_ptr);
}

void DisplayTempHex(int one,int two)
{
 
    //Set the value of the LED pointer to the current hexcode based on current switch
    *(led) = hex_code[one] + (hex_code[two] << 8);
	//*(led) = (hex_code[s] << 16) + (hex_code[ts] << 24);
	//*(led2) = hex_code[m] + (hex_code[tm] << 8);
    
}


void DisplayTimeHex(int hr,int m1,int m2)
{
 
    //Set the value of the LED pointer to the current hexcode based on current switch
    *(led) = hex_code[hr] + (hex_code[m1] << 8) + (hex_code[m2] << 16);
	//*(led) = (hex_code[s] << 16) + (hex_code[ts] << 24);
    
}





void main(){
    //ADC stuff
    int ADCdata = 0;
    int potdata =0;
    port_A->control = 0xCFF;
    ADC_ptr->ch1 = 1;

    //Interval for 3 seconds
    int interval = 600000000;
    int started = 0;
    //int time = 0;
 
    //Set initial load of timer (1 milli sec --> 200)
    set_timer(interval);
 	
    
	int runTmFlag = 0;
    int current_button = 0;
    int previous_button = 0;

    int current_button2 = 0;
    int previous_button2 = 0;

    while(1){
		
		current_button = ReadKeys();
        //Starting mode
		if(ReadSwitches() == 1) //Channel 1
		{
            /*if((current_button != previous_button) && current_button == 1){
                desiredTemp += 1;
                int firstval = desiredTemp % 10;
                int secondval = (desiredTemp/10)%10;
                DisplayTempHex(firstval,secondval);
            }
            else if((current_button2 != previous_button2) && current_button2 == 1){
                desiredTemp -= 1;
                int firstval = desiredTemp % 10;
                int secondval = (desiredTemp/10)%10;
                DisplayTempHex(firstval,secondval);
            }*/

            ADCdata = ADC_ptr->ch0;
            if(ADCdata>>15){
                
                potdata = ADCdata & 0xFFF;
                if(potdata == 0){
                    port_A->data = 0b0;
					desiredTemp=16;
					
                }
                if(potdata < 410){
                    port_A->data = 0b1;
					desiredTemp=17;
                }
                if(potdata > 410*1+1 && potdata <410*2){
                    port_A->data = 0b11;
					desiredTemp=18;
                }
                if(potdata > 410*2+1 && potdata < 410*3){
                    port_A->data = 0b111;
					desiredTemp=19;
                }
                if(potdata > (410*3+1) && potdata < 410*4){
                    port_A->data = 0b1111;
					desiredTemp=20;
                }
                if(potdata > 410*4+1 && potdata < 410*5){
                    port_A->data = 0b11111;
					desiredTemp=21;
                }
                if(potdata > 410*5+1 && potdata < 410*6){
                    port_A->data = 0b111111;
					desiredTemp=22;
                }
                if(potdata > 410*6+1 && potdata < 410*7){
                    port_A->data = 0b1111111;
					desiredTemp=23;
                }
                if(potdata > 410*7+1 && potdata < 410*8){
                    port_A->data = 0b11111111;
					desiredTemp=24;
                }
                if(potdata > 410*8+1 && potdata < 410*9){
                    port_A->data = 0b111111111;
					desiredTemp=25;
                }
                if(potdata > 410*9+1 && potdata < 410*10){
                    port_A->data = 0b1111111111;
					desiredTemp=26;
                }
            }
            int firstval = desiredTemp % 10;
            int secondval = (desiredTemp/10)%10;
            DisplayTempHex(firstval,secondval);

        }
        else{ //Channel 0
			
            //Display current temperature
            int firstval = insideTemp % 10;
            int secondval = (insideTemp/10)%10;
            DisplayTempHex(firstval,secondval);
			
			if(runTmFlag != 1){
				start_timer();
				started = 1;
				runTmFlag = 1;
			}
			
            //Start the timer
            if(reachedFlag != 1)
            {
                
            }
            
			if((current_button != previous_button) && current_button == 1){
               runTmFlag = 0;
            }
            
            if(started == 1)
		    {
			
                //Get the current count
                //current_count = timer_1->count;
                
                //If status flag is 1, means timer has reached the end 
                if(timer_1->status == 1)
                {
                    //Increment time by 1 every 1ms
                    insideTemp=insideTemp+1;
                    
                    
                    
                    //Clear timeout flag once the timer is done (Set the status flag in Interrupt Status to 1)
                    timer_1->status = 1; 

                }
            
            }


            if(insideTemp == desiredTemp){
                stop_timer();
                reachedFlag = 1;
            }
		}

    previous_button = current_button;
    previous_button2 = current_button2;
    }


}
#include <mega128.h>
#include <delay.h>

#define TRIGGER PORTD.0

flash unsigned char seg_pat[10] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f};
flash unsigned char seg_on[4] = {0x08, 0x04, 0x02, 0x01};

unsigned char echo_st = 0;
unsigned char dsp_no = 0;
unsigned int dist = 0, cnt = 0;
unsigned int N1000, N100, N10, N1;

void Distance_out(void);

void main(void){

    //적외선 센서
    volatile unsigned char a=0;
    
    DDRE=0b00001000;
    DDRC=0XFF;
    PORTC=0XFF;

    DDRB = 0xFF;
    DDRD = 0b11111101;
    DDRG = 0xFF;
    PORTB = 0x0;
    PORTD = 0x0;
    TRIGGER = 0;
    
    TIMSK = 0b01000001;         
    TCCR0 = 0b00000111;
    ASSR = 0x0;
    TCNT0 = 176;
    
    TCCR2 = 0b00000000;
    TCNT2 = 138;     
    
    SREG = 0x80;
    
    while(1){  
    
        EICRA = 0b00001100;
        EIMSK = 0b00000010;
        echo_st = 0;
        cnt = 0;
        TRIGGER = 1;
        delay_us(15);
        TRIGGER = 0;
        
        while(echo_st != 2);
        dist = cnt;
        Distance_out();
        
        delay_ms(100);
        
        //적외선 센서
        a=PINE.2;  // 막으면 0111 1111 // 안막으면 1111 1111
          
        if(a==1)
            PORTC=0XFF;
        else  
           PORTC=0X00;
    } 
}


void Distance_out(void){

    int buf;
    int i;
    //DDRE=0xFF;
      
    if(dist <= 15){
        for(i=0;i<25;i++){ PORTE.3=1; delay_us(2400); PORTE.3=0; delay_ms(20); } // 90
    }
    else{
        for(i=0;i<25;i++){ PORTE.3=1; delay_us( 600); PORTE.3=0; delay_ms(20); } // -90도
    }
    N1000 = dist/1000;
    buf = dist%1000;
    
    N100 = buf/100;
    buf = buf%100;
    
    
    N10 = buf/10;
    N1 = buf%10;
}

interrupt [TIM0_OVF] void time0(void)
{
    unsigned char pat;
       
    TCNT0 = 176;    
    
    if(dsp_no == 0) pat = N1;
    else if(dsp_no == 1) pat = N10;
    else if(dsp_no == 2) pat = N100;
    else pat = N1000;
    
    PORTG = seg_on[dsp_no];
    PORTB = (seg_pat[pat] & 0x70) | (PORTB & 0x0F);
    PORTD = ((seg_pat[pat] & 0x0F) << 4) | (PORTD & 0x0F);
    
    dsp_no = (dsp_no + 1) % 4;
}

interrupt [TIM2_OVF] void time2(void)
{
    TCNT2 = 138;
    cnt++;
    if(cnt>300) echo_st = 2;
}

interrupt [EXT_INT1] void trigger(void)
{
    if(echo_st == 0){
        echo_st = 1;
        TCNT2 = 138;
        TCCR2 = 0b00000010;
        EICRA = 0b00001000;
    }
    else{
        echo_st = 2;
        TCCR2 = 0b00000000;
        EIMSK = 0x0;
    }
}
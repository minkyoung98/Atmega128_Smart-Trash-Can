# Smart Trash Can
## 목차
1. 만든 의도
2. 사용 부품
3. 코드 설명
4. 동작 결과
<br><br>
# 1. 만든의도

* 위생<br>
잔여물이 묻은 뚜껑을 손대지 않고 열 수 있다.

* 편의<br>
뚜껑을 열지 않고 내부의 양을 알 수 있다.
<br><br>
# 2. 사용 부품
2.1 부품 소개

|부품명 | 부품 | 설명 |
|----|----|----|
|초파 센서|<img src="https://user-images.githubusercontent.com/48309721/71659170-f0168a80-2d89-11ea-9bb4-be6faa9244c6.png" width="150"></img>| 물체와의 거리를 측정 하는 센서로, 사람의 손과의 거리를 측정한다.|
|적외선 센서|<img src="https://user-images.githubusercontent.com/48309721/71659539-54861980-2d8b-11ea-91dc-4abcc8978dc1.png" width="150"></img>|앞에 물체가 있는지를 감지하는 센서로, 쓰레기통이 가득 차는것을 감지한다.|
|발광 다이오드|<img src="https://user-images.githubusercontent.com/48309721/71659645-aaf35800-2d8b-11ea-9f40-077dfeb1e6a1.png" width="150"></img>|평소에는 파란 LED가 켜지고, 쓰레기가 가득 차면 주황색 LED가 켜진다.|
|서보 모터|<img src="https://user-images.githubusercontent.com/48309721/71659651-af1f7580-2d8b-11ea-8d44-4d1133ba1584.png" width="150"></img>|초음파 센서를 이용하여 사람의 손이 일정한 거리 안에 들어오면 모터를 돌려 뚜껑을 연다.|

2.2 부품 연결

<img src="https://user-images.githubusercontent.com/48309721/71659800-3f5dba80-2d8c-11ea-832b-00b04f870a81.png" width="200"></img>
<img src="https://user-images.githubusercontent.com/48309721/71659806-4684c880-2d8c-11ea-812b-5e83b886e9d1.png" width="200"></img>
<img src="https://user-images.githubusercontent.com/48309721/71659810-4ab0e600-2d8c-11ea-9142-233c4b8581bd.png" width="200"></img>
<img src="https://user-images.githubusercontent.com/48309721/71659817-4f759a00-2d8c-11ea-91a0-bc0c1e1d7d93.png" width="200"></img>

<br><br>
# 3. 코드 설명 
```c
void Distance_out(void){ //표시할 새 거리 측정값 

    int buf;
    int i;
      
    //서보모터  
    if(dist <= 15){ 
        //open
        for(i = 0; i < 25; i++){ PORTE.3=1; delay_us(2400); PORTE.3=0; delay_ms(60); } // 90도
        delay_ms(2000);
        //close 
        for(i = 0; i < 25; i++){ PORTE.3=1; delay_us(600); PORTE.3=0; delay_ms(60); } // -90도 
    }
  
    N1000 = dist/1000; //m 10자리 추출 
    buf = dist%1000;
    
    N100 = buf/100; //m 1자리 추출 
    buf = buf%100;
    
    N10 = buf/10; //cm 10자리 추출 
    N1 = buf%10; // cm 1자리 추출  
}

```
거리를 측정해서 서보모터를 작동시킨다. 
<br><br>
```c
interrupt [TIM0_OVF] void time0(void) //LED & 거리측정값 표시  
{ 
	//적외선 센서
    volatile unsigned char a=0;
    DDRE=0b00001000; //포트E.3
    

    a=PINE.2;  // 막으면 0111 1111 // 안막으면 1111 1111
    
    if(a==1){ //안찼을때 
    	PORTC=0xFF;
    	PORTF=0x01; //파란 LED ON 
    }
    else{ //찼을때 
        PORTC=0x00;
        PORTF=0x02; //주황 LED ON 
    }
          
    unsigned char pat;
       
    TCNT0 = 176;    
    
    // 표시할 위치의 표시 값 
    if(dsp_no == 0) pat = N1;
    else if(dsp_no == 1) pat = N10;
    else if(dsp_no == 2) pat = N100; 
    else pat = N1000;
     
    // 7세그먼트 표시 
    PORTG = seg_on[dsp_no]; //표시할 7세그먼트 ON 
    PORTB = (seg_pat[pat] & 0x70) | (PORTB & 0x0F);
    PORTD = ((seg_pat[pat] & 0x0F) << 4) | (PORTD & 0x0F);
    
    dsp_no = (dsp_no + 1) % 4; //표시할 세그먼트 번호 갱신 
}
```
적외선 센서를 이용하여 쓰레기의 양의 측정하여 쓰레기가 차지 않았으면 파란색 LED를 on하고 가득 차있으면 주황색 LED를 on한다.
<br>
<img src="https://user-images.githubusercontent.com/48309721/71664812-cc5d3f80-2d9d-11ea-898d-a19223445770.png" width="200"></img>
<img src="https://user-images.githubusercontent.com/48309721/71664814-cebf9980-2d9d-11ea-9c3b-95f9bedfe5f6.png" width="200"></img>

```c
interrupt [TIM2_OVF] void time2(void) //초음파 센서 거리 측정  
{
    TCNT2 = 138;
    cnt++;
    if(cnt>300) echo_st = 2; 
}
```
초음파센서를 이용하여 손이 다가오면 뚜껑을 열 수 있도록 하기위해 다가오는 손과의 거리를 측정한다.

<img src="https://user-images.githubusercontent.com/48309721/71664865-09293680-2d9e-11ea-943e-a6356d9b410c.png" width="170"></img>
<img src="https://user-images.githubusercontent.com/48309721/71664869-0c242700-2d9e-11ea-96a0-edc393588134.png" width="170"></img>
<img src="https://user-images.githubusercontent.com/48309721/71664871-0f1f1780-2d9e-11ea-8eb1-b953e2308222.png" width="170"></img>

# 4. 동작 결과


[![Everything Is AWESOME](https://user-images.githubusercontent.com/48309721/71665419-337bf380-2da0-11ea-903a-c17ad21b5270.JPG)](https://www.youtube.com/watch?v=EDEjoMPb_DU "클릭하여 YouTube로 이동")

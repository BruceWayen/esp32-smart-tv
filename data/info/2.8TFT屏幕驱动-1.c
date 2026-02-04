/********************金逸晨**************************
*****************GMT028-05 IPS  模块 2.8寸  8P SPI TFT FOR FOR AT89S52/STC12*************
*****AT89S52驱动**************************需设置引脚为推挽模式
***** P30----SCL   P31-----SDA			P32-----RST    P33-----DC    P34-----CS  P35-----BL
*********ST7789 控制器，需设置bl为推挽模式
默认数据格式位RGB565
***** BY:GU 


***********************************************/
#include "reg51.h"
#define LCD_DIS_CHINA_ROT  0


#define set_0   0x01
#define set_1   0x02
#define set_2   0x04
#define set_3   0x08    
#define set_4   0x10
#define set_5   0x20
#define set_6   0x40
#define set_7   0x80

#define clr_0   0xFE
#define clr_1   0xFD
#define clr_2   0xFB
#define clr_3   0xF7    
#define clr_4   0xEF
#define clr_5   0xDF
#define clr_6   0xBF
#define clr_7   0x7F


///////stc   IO   模式寄存器
sfr P1M1 = 0x91;    //PxM1.n,PxM0.n     =00--->Standard,    01--->push-pull
sfr P1M0 = 0x92;    //                  =10--->pure input,  11--->open drain
sfr P0M1 = 0x93;
sfr P0M0 = 0x94;
sfr P2M1 = 0x95;
sfr P2M0 = 0x96;
sfr P3M1 = 0xB1;
sfr P3M0 = 0xB2;
sfr P4M1 = 0xB3;
sfr P4M0 = 0xB4;
sfr P5M1 = 0xC9;
sfr P5M0 = 0xCA;
sfr P6M1 = 0xCB;
sfr P6M0 = 0xCC;
sfr P7M1 = 0xE1;
sfr P7M0 = 0xE2;

/**********引脚分配，连接LCD屏，更具实际情况修改*********/
     
#define TFT_SCK_0   	P3 &=clr_0    // 设置CS接口到P30
#define TFT_SCK_1   	P3|=set_0
#define TFT_SDA_0   	P3 &=clr_1      // 设置DC接口到P31
#define TFT_SDA_1   	P3|=set_1
#define TFT_RST_0  	P3 &=clr_2      // 设置WR接口到P32   清零
#define TFT_RST_1  	P3|=set_2       //置位
#define TFT_DC_0  	P3 &=clr_3      // 设置RD接口到P33
#define TFT_DC_1  	P3|=set_3
#define TFT_CS_0  	P3 &=clr_4       // 设置RST接口到P34
#define TFT_CS_1 	P3|=set_4
#define TFT_BL_0  	P3 &=clr_5      // 设置RST接口到P35
#define TFT_BL_1  	P3|=set_5

#define DIS_DIR  0
#define     RED          0XF800	  //红色
#define     GREEN        0X07E0	  //绿色
#define     BLUE         0X001F	  //蓝色
#define     WHITE        0XFFFF	  //白色
#define     BLACK        0X0000  //黑色

#define TFT_COLUMN_NUMBER 240
#define TFT_LINE_NUMBER 320
#define TFT_COLUMN_OFFSET 0

#define PIC_NUM 28800			//图片数据大小
unsigned char  *point;
code unsigned char  chines_word[   ][32]=   //汉字码
{
0x00,0x00,0xE4,0x3F,0x28,0x20,0x28,0x25,0x81,0x08,0x42,0x10,0x02,0x02,0x08,0x02,
0xE8,0x3F,0x04,0x02,0x07,0x07,0x84,0x0A,0x44,0x12,0x34,0x62,0x04,0x02,0x00,0x02,/*"深",0*/

0x88,0x20,0x88,0x24,0x88,0x24,0x88,0x24,0x88,0x24,0xBF,0x24,0x88,0x24,0x88,0x24,
0x88,0x24,0x88,0x24,0x88,0x24,0xB8,0x24,0x87,0x24,0x42,0x24,0x40,0x20,0x20,0x20,/*"圳",1*/

0x80,0x00,0x80,0x00,0x40,0x01,0x20,0x02,0x10,0x04,0x08,0x08,0xF4,0x17,0x83,0x60,
0x80,0x00,0xFC,0x1F,0x80,0x00,0x88,0x08,0x90,0x08,0x90,0x04,0xFF,0x7F,0x00,0x00,/*"金",2*/

0x80,0x00,0x82,0x00,0x84,0x0F,0x44,0x08,0x20,0x04,0xF0,0x3F,0x27,0x22,0x24,0x22,
0xE4,0x3F,0x04,0x05,0x84,0x0C,0x84,0x54,0x44,0x44,0x24,0x78,0x0A,0x00,0xF1,0x7F,/*"逸",3*/

0xF8,0x0F,0x08,0x08,0xF8,0x0F,0x08,0x08,0xF8,0x0F,0x00,0x00,0xFC,0x3F,0x04,0x00,
0xF4,0x1F,0x04,0x00,0xFC,0x7F,0x94,0x10,0x14,0x09,0x12,0x06,0x52,0x18,0x31,0x60,/*"晨",4*/

0x80,0x00,0x80,0x00,0x80,0x00,0xFC,0x1F,0x84,0x10,0x84,0x10,0x84,0x10,0xFC,0x1F,
0x84,0x10,0x84,0x10,0x84,0x10,0xFC,0x1F,0x84,0x50,0x80,0x40,0x80,0x40,0x00,0x7F,/*"电",5*/

0x00,0x00,0xFE,0x1F,0x00,0x08,0x00,0x04,0x00,0x02,0x80,0x01,0x80,0x00,0xFF,0x7F,
0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0xA0,0x00,0x40,0x00,/*"子",6*/
};



void Delay_us(unsigned int _us_time)
{       
  unsigned char x=0;
  for(;_us_time>0;_us_time--)
  {
    x++;x++;x++;x++;x++;x++;x++;x++;x++;x++;x++;x++;
	  x++;x++;x++;x++;x++;x++;x++;x++;x++;x++;x++;x++;
  }
}
void Delay_ms(unsigned int _ms_time)
  {
    unsigned int i,j;
    for(i=0;i<_ms_time;i++)
    {
    for(j=0;j<900;j++)
      {;}
    }
  }
  void IO_init(void)
{
	P3=0XFF;
	P3M0 =	0X20;
	P3M1 = 0X00;
	P0M0 =	0X00;
	P0M1 = 0X00;
	P0=0XFF;
	P1M0 =	0X00;
	P1M1 = 0X00;
	P1=0XFF;
}
/*************SPI配置函数*******************
SCL空闲时低电平，第一个上升沿采样
模拟SPI
******************************************/

/**************************SPI模块发送函数************************************************

 *************************************************************************/
void TFT_Send(unsigned  char byte)				//向液晶屏写一个8位数据
{
  
  unsigned char counter;
 
  for(counter=0;counter<8;counter++)
  { 
    TFT_SCK_0;		  
    if((byte&0x80)==0)
    {
      TFT_SDA_0;
    }
    else TFT_SDA_1;
    byte=byte<<1;	
    TFT_SCK_1;		
  }		
	TFT_SCK_0;
}

void TFT_SEND_CMD(unsigned char o_command)
  {
    TFT_DC_0;
    TFT_CS_0;
    TFT_Send(o_command);
    TFT_CS_1;
   
    //TFT_DC_1;
  }
  //向液晶屏写一个8位数据
void TFT_SEND_DATA(unsigned  int o_data)
  { 
    TFT_DC_1;
    TFT_CS_0;
    TFT_Send(o_data);
    TFT_CS_1;
    
   }
void TFT_SET_ADD(unsigned  int X,unsigned int Y,unsigned  int X_END,unsigned int Y_END)
{	

            //判断地址是否合法
    if((X_END>(TFT_COLUMN_NUMBER-1))|| (X>(TFT_COLUMN_NUMBER-1)) )      //横向向显示    //超出x地址范围
    {
        X_END = (TFT_COLUMN_NUMBER-1);
            //X = 0;
    }   
    if ((Y_END > (TFT_LINE_NUMBER-1) )||(Y > (TFT_LINE_NUMBER-1) ))       //超出Y范围
    {
            //Y = 0;                          //移动到第一行
            Y_END =  ( TFT_LINE_NUMBER-1);
                
    }
	TFT_SEND_CMD(0x2A);     //窗口地址横向                   
    TFT_SEND_DATA(X>>8);     
    TFT_SEND_DATA(X);//start column
	TFT_SEND_DATA(X_END>>8);     
    TFT_SEND_DATA(X_END);//start column
	TFT_SEND_CMD(0x2B);     //窗口地址纵向                   
    TFT_SEND_DATA(Y>>8);     
    TFT_SEND_DATA(Y);		//start row
    TFT_SEND_DATA(Y_END>>8);     
    TFT_SEND_DATA(Y_END);		//start row
    TFT_SEND_CMD(0x2C);     //写入数据   
}  
void TFT_Clear(unsigned int back_c)									//清除液晶数据，刷黑
  {
    unsigned int ROW=0,column=0;
	TFT_SET_ADD(0x0000,0x0000,TFT_COLUMN_NUMBER-1,TFT_LINE_NUMBER-1);//纵向
    for(ROW=0;ROW<TFT_LINE_NUMBER;ROW++)             //ROW loop
      { 
          for(column=0;column<TFT_COLUMN_NUMBER;column++)  //column loop
            {     
				TFT_SEND_DATA(back_c>>8);
				TFT_SEND_DATA(back_c);
            }
      }
  }

void TFT_Init(void)        
  {
	  
	TFT_RST_0;
	Delay_ms(100);
	TFT_RST_1;
	Delay_ms(120);
//-----------------------ST7789V Frame rate setting-----------------//
//************************************************
                TFT_SEND_CMD(0x3A);        //65k mode
                TFT_SEND_DATA(0x05);
                TFT_SEND_CMD(0xC5); 		//VCOM
                TFT_SEND_DATA(0x1A);
                TFT_SEND_CMD(0x36);                 // 屏幕显示方向设置
	  //调整显示方向  MY,MX,MV,ML,RGB,MH,X,X       row-order，column-order，row/column exchange，
	   #if (DIS_DIR == 0)                   //正常竖显
            TFT_SEND_DATA(0x00);
        #elif (DIS_DIR == 1)                   //旋转90度显示
            TFT_SEND_DATA(0xA0);
        #elif (DIS_DIR == 2)                   //旋转180度显示
            TFT_SEND_DATA(0xC0);
        #elif(DIS_DIR == 3)                   //旋转270度显示
            TFT_SEND_DATA(0x60);
        #endif      
                //-------------ST7789V Frame rate setting-----------//
                TFT_SEND_CMD(0xb2);		//Porch Setting
                TFT_SEND_DATA(0x05);
                TFT_SEND_DATA(0x05);
                TFT_SEND_DATA(0x00);
                TFT_SEND_DATA(0x33);
                TFT_SEND_DATA(0x33);

                TFT_SEND_CMD(0xb7);			//Gate Control
                TFT_SEND_DATA(0x05);			//12.2v   -10.43v
                //--------------ST7789V Power setting---------------//
                TFT_SEND_CMD(0xBB);//VCOM
                TFT_SEND_DATA(0x3F);

                TFT_SEND_CMD(0xC0); //Power control
                TFT_SEND_DATA(0x2c);

                TFT_SEND_CMD(0xC2);		//VDV and VRH Command Enable
                TFT_SEND_DATA(0x01);

                TFT_SEND_CMD(0xC3);			//VRH Set
                TFT_SEND_DATA(0x0F);		//4.3+( vcom+vcom offset+vdv)

                TFT_SEND_CMD(0xC4);			//VDV Set
                TFT_SEND_DATA(0x20);				//0v

                TFT_SEND_CMD(0xC6);				//Frame Rate Control in Normal Mode
                TFT_SEND_DATA(0X01);			//111Hz

                TFT_SEND_CMD(0xd0);				//Power Control 1
                TFT_SEND_DATA(0xa4);
                TFT_SEND_DATA(0xa1);

                TFT_SEND_CMD(0xE8);				//Power Control 1
                TFT_SEND_DATA(0x03);

                TFT_SEND_CMD(0xE9);				//Equalize time control
                TFT_SEND_DATA(0x09);
                TFT_SEND_DATA(0x09);
                TFT_SEND_DATA(0x08);
                //---------------ST7789V gamma setting-------------//
                TFT_SEND_CMD(0xE0); //Set Gamma
                TFT_SEND_DATA(0xD0);
                TFT_SEND_DATA(0x05);
                TFT_SEND_DATA(0x09);
                TFT_SEND_DATA(0x09);
                TFT_SEND_DATA(0x08);
                TFT_SEND_DATA(0x14);
                TFT_SEND_DATA(0x28);
                TFT_SEND_DATA(0x33);
                TFT_SEND_DATA(0x3F);
                TFT_SEND_DATA(0x07);
                TFT_SEND_DATA(0x13);
                TFT_SEND_DATA(0x14);
                TFT_SEND_DATA(0x28);
                TFT_SEND_DATA(0x30);
                 
                TFT_SEND_CMD(0XE1); //Set Gamma
                TFT_SEND_DATA(0xD0);
                TFT_SEND_DATA(0x05);
                TFT_SEND_DATA(0x09);
                TFT_SEND_DATA(0x09);
                TFT_SEND_DATA(0x08);
                TFT_SEND_DATA(0x03);
                TFT_SEND_DATA(0x24);
                TFT_SEND_DATA(0x32);
                TFT_SEND_DATA(0x32);
                TFT_SEND_DATA(0x3B);
                TFT_SEND_DATA(0x14);
                TFT_SEND_DATA(0x13);
                TFT_SEND_DATA(0x28);
                TFT_SEND_DATA(0x2F);

                TFT_SEND_CMD(0x20); 		//反显
	TFT_SEND_CMD(0x11); //Exit Sleep // 退出睡眠模式
	Delay_ms(120); 
	TFT_SEND_CMD(0x29); //Display on // 开显示
  }
void display_char16_16(unsigned int x,unsigned int y,unsigned long color,unsigned char word_serial_number)
{
   unsigned int column;
  unsigned char tm=0,temp=0,xxx=0;

   TFT_SEND_CMD(0x2a);    //Column address set
  TFT_SEND_DATA(x>>8);    //start column
  TFT_SEND_DATA(x);
  x=x+15;
  TFT_SEND_DATA(x>>8);    //end column
  TFT_SEND_DATA(x);

  TFT_SEND_CMD(0x2b);     //Row address set
  TFT_SEND_DATA(y>>8);    //start row
  TFT_SEND_DATA(y); 
  y=y+15;
  TFT_SEND_DATA(y>>8);    //end row
  TFT_SEND_DATA(y);
    TFT_SEND_CMD(0x2C);     //Memory write
    
    
  for(column=0;column<32;column++)  //column loop
          {
        temp=chines_word[  word_serial_number ][xxx];
        for(tm=0;tm<8;tm++)
        {
        if(temp&0x01)
          {
			  TFT_SEND_DATA(color>>8);
			   TFT_SEND_DATA(color);
          }
        else 
          {
			  TFT_SEND_DATA(0XFF);
			  TFT_SEND_DATA(0XFF);
          }
          temp>>=1;
        }
        xxx++;
          
      }
}

// void Picture_Display(const unsigned char *ptr_pic)
// {
    // unsigned long  number;
	// unsigned int Ppdata;
	// TFT_SET_ADD(20,20,139,140);
	// for(number=0;number<PIC_NUM;number++)	
          // {
			// Ppdata=*ptr_pic++;
            // TFT_SEND_DATA(Ppdata);
          // }
  // }
int main(void)
{ 
	IO_init();
	TFT_BL_1;
	TFT_Init();

	
	while(1)
	{	
	TFT_Clear(RED);
	Delay_ms(10);
	TFT_Clear(GREEN);
	Delay_ms(10);
	TFT_Clear(BLUE);
	Delay_ms(10);
	TFT_Clear(BLACK);
	// Picture_Display(&picture_tab[0]);
	// Delay_ms(10);
    display_char16_16(20,160,BLUE,0);
	display_char16_16(36,160,GREEN,1);
	display_char16_16(60,160,RED,2);
	display_char16_16(76,160,BLUE,3);
	display_char16_16(92,160,GREEN,4);
	display_char16_16(118,160,BLUE,5);
	display_char16_16(134,160,RED,6);
	Delay_ms(20);

	}
}












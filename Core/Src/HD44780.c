#include "HD44780.h"
#include "i2c.h"

extern I2C_HandleTypeDef hi2c1;

uint8_t portlcd = 0;



void LCD_Write_I2C_LCD(uint8_t to_send)
{
    static uint8_t bufer_i2c = 0;
    bufer_i2c = to_send;
    HAL_I2C_Master_Transmit(&hi2c1, (uint16_t)0x4E, &bufer_i2c, sizeof(bufer_i2c), 1000);
}

 void Priv_Send_Halfbyte(uint8_t c)
{
        c<<=4;
        ENABLE_SET();
        // Delay_us(50);
        HAL_Delay(1);
       
        LCD_Write_I2C_LCD(portlcd|c);
       
        ENABLE_RESET();
        // Delay_us(50);
        HAL_Delay(1);
}
void Priv_Send_Byte(uint8_t c, uint8_t mode)
{
        uint8_t hc=0;
       
        if (mode==0) { RS_RESET(); }
        else         { RS_SET();   }
        hc=c>>4;
       
        Priv_Send_Halfbyte(hc);
        Priv_Send_Halfbyte(c);
}

 void LCD_Init(void)
{
    uint8_t i=0;
    //Wybranie trybu czterobitowego dane wysylane cztery razy
    HAL_Delay(5);
    for(i=0;i<3;i++)
    {
        Priv_Send_Halfbyte(0x03);
        HAL_Delay(20);
    }
    //Wlaczenie trybu czterobitowego
    Priv_Send_Halfbyte(0x02);
    HAL_Delay(20);
    Priv_Send_Byte(HD44780_FUNCTION_SET | HD44780_FONT5x7 | HD44780_TWO_LINE | HD44780_4_BIT,0);
    HAL_Delay(1);
    Priv_Send_Byte(HD44780_DISPLAY_ONOFF | HD44780_DISPLAY_OFF,0);
    HAL_Delay(1);
    Priv_Send_Byte(HD44780_ENTRY_MODE | HD44780_EM_SHIFT_CURSOR | HD44780_EM_INCREMENT,0);
    HAL_Delay(1);
    Priv_Send_Byte(HD44780_DISPLAY_ONOFF | HD44780_DISPLAY_ON | HD44780_CURSOR_OFF | HD44780_CURSOR_NOBLINK,0);
    HAL_Delay(1);
    LED_SET();
    WRITE_SET();
    LCD_Clear();
}

void LCD_Clear(void)
{
    Priv_Send_Byte(HD44780_CLEAR, 0);
    HAL_Delay(100);
}

void LCD_Send_Char(char ch)
{
    Priv_Send_Byte(ch, 1);
}

void LCD_Send_String(char* str)
{
    uint8_t i=0;
    while(str[i] != 0)
    {
        Priv_Send_Byte(str[i],1);
        i++;
    }
}

void LCD_Send_Str_Pos(char* st, uint8_t x, uint8_t y)
{
    LCD_Set_Position(x, y);
    LCD_Send_String(st);
}

 void LCD_Set_Position(uint8_t x, uint8_t y)
{
    switch(y)
    {
       case 0:
            Priv_Send_Byte(x|0x80,0);
            HAL_Delay(1);
            break;
       case 1:
            Priv_Send_Byte((0x40+x)|0x80,0);
            HAL_Delay(1);
            break;
    }
}




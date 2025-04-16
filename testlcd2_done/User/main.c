#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"

#define LCD_RS GPIO_Pin_13
#define LCD_RW GPIO_Pin_14
#define LCD_EN GPIO_Pin_15
#define LCD_D4 GPIO_Pin_8
#define LCD_D5 GPIO_Pin_9
#define LCD_D6 GPIO_Pin_10
#define LCD_D7 GPIO_Pin_11

static __IO uint32_t usTicks;

void DelayInit(void);
void DelayUs(uint32_t us);
void DelayMs(uint32_t ms);

void GPIO_LCD_Config(void);
void LCD_Enable(void);
void LCD_Send4Bit(unsigned char Data);
void LCD_SendCommand(unsigned char command);
void LCD_Clear(void);
void LCD_Init(void);
void LCD_Gotoxy(unsigned char x, unsigned char y);
void LCD_PutChar(unsigned char Data);
void LCD_Puts(char *s);

int main(void)
{
    DelayInit();
    LCD_Init();

    LCD_Gotoxy(0, 0);
    LCD_Puts("Hello STM32");

    LCD_Gotoxy(0, 1);
    LCD_Puts("LCD 16x2 Test");

    while (1)
    {
        // Gi? n?i dung hi?n th?
    }
}

//========================= LCD ===========================

void GPIO_LCD_Config(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitTypeDef GPIO_LCD;
    GPIO_LCD.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_LCD.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_LCD.GPIO_Pin = LCD_D4 | LCD_D5 | LCD_D6 | LCD_D7;
    GPIO_Init(GPIOA, &GPIO_LCD);

    GPIO_LCD.GPIO_Pin = LCD_RS | LCD_RW | LCD_EN;
    GPIO_Init(GPIOB, &GPIO_LCD);

    // Reset t?t c? chân
    GPIO_ResetBits(GPIOA, LCD_D4 | LCD_D5 | LCD_D6 | LCD_D7);
    GPIO_ResetBits(GPIOB, LCD_RS | LCD_RW | LCD_EN);
}

void LCD_Enable(void)
{
    GPIO_SetBits(GPIOB, LCD_EN);
    DelayMs(1);
    GPIO_ResetBits(GPIOB, LCD_EN);
    DelayMs(1);
}

void LCD_Send4Bit(unsigned char Data)
{
    GPIO_WriteBit(GPIOA, LCD_D4, Data & 0x01);
    GPIO_WriteBit(GPIOA, LCD_D5, (Data >> 1) & 0x01);
    GPIO_WriteBit(GPIOA, LCD_D6, (Data >> 2) & 0x01);
    GPIO_WriteBit(GPIOA, LCD_D7, (Data >> 3) & 0x01);
}

void LCD_SendCommand(unsigned char command)
{
    GPIO_ResetBits(GPIOB, LCD_RS);  // RS = 0 for command
    LCD_Send4Bit(command >> 4);
    LCD_Enable();
    LCD_Send4Bit(command);
    LCD_Enable();
}

void LCD_Clear(void)
{
    LCD_SendCommand(0x01);
    DelayMs(2);
}

void LCD_Init(void)
{
    GPIO_LCD_Config();
    DelayMs(20);

    LCD_Send4Bit(0x03);
    LCD_Enable();
    DelayMs(5);

    LCD_Send4Bit(0x03);
    LCD_Enable();
    DelayUs(150);

    LCD_Send4Bit(0x03);
    LCD_Enable();
    DelayMs(5);

    LCD_Send4Bit(0x02); // Ch? d? 4-bit
    LCD_Enable();

    LCD_SendCommand(0x28); // Giao ti?p 4-bit, 2 dòng, font 5x8
    LCD_SendCommand(0x0C); // B?t hi?n th?, t?t con tr?
    LCD_SendCommand(0x06); // T? d?ng tang d?a ch?, không d?ch khung
    LCD_Clear();
}

void LCD_Gotoxy(unsigned char x, unsigned char y)
{
    unsigned char address;
    if (y == 0)
        address = 0x80 + x;
    else
        address = 0xC0 + x;
    LCD_SendCommand(address);
}

void LCD_PutChar(unsigned char Data)
{
    GPIO_SetBits(GPIOB, LCD_RS); // RS = 1 for data
    LCD_Send4Bit(Data >> 4);
    LCD_Enable();
    LCD_Send4Bit(Data);
    LCD_Enable();
    GPIO_ResetBits(GPIOB, LCD_RS);
}

void LCD_Puts(char *s)
{
    while (*s)
    {
        LCD_PutChar(*s);
        s++;
    }
}

//==================== Delay Functions =====================

void SysTick_Handler(void)
{
    if (usTicks != 0)
        usTicks--;
}

void DelayInit(void)
{
    SystemCoreClockUpdate();
    SysTick_Config(SystemCoreClock / 1000000); // 1us
}

void DelayUs(uint32_t us)
{
    usTicks = us;
    while (usTicks);
}

void DelayMs(uint32_t ms)
{
    while (ms--)
    {
        DelayUs(1000);
    }
}

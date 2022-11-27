#include "stm32f4xx.h"
#include "ssd1305.h"

void write_cmd(unsigned char);
void write_data(unsigned char);

void ssd1305_init()
{
  LCD_NSS_LO;
  // Hardware Reset of SSD1305
  LCD_RES_LO;
  for(u32 i=0x0007000;i>0;i--); //
  LCD_RES_HI;
 // write_cmd(SSD1305_CMD_SWRES);
  // Set Lower Column address
  write_cmd(SSD1305_CMD_SETLOWCOLADD + 0x00);
  // Set Higher Column Address
  write_cmd(SSD1305_CMD_SETHIGHCOLADD + 0x00);
  // Set Memory Addressing Mode
  write_cmd(SSD1305_CMD_SETMEMADDMODE);
  write_cmd(0x00); //Horizontal Addressing mode (0x00), vertical (0x01), Page (0x10)
  // Set Column Start and End Address
  write_cmd(SSD1305_CMD_SETCOLADD);
  write_cmd(0x00);
  write_cmd(127);
  // Set Page Address
  write_cmd(SSD1305_CMD_SETPAGEADD);
  write_cmd(0x00); // Start at Page 0
  write_cmd(0x07); // End at Page 7
  // Set Display Start Line (0x40 & (0-63 decimal)
  write_cmd(SSD1305_CMD_SETDISPSTART + 0x00);
  // Contrast Control
  write_cmd(SSD1305_CMD_SETCONTRAST);
  write_cmd(0x80); // 0 - 255
  // Brightness Control
  write_cmd(SSD1305_CMD_SETCBRIGHTNESS);
  write_cmd(0x80); // 0 - 255
  // Set Pulse Widths
  write_cmd(SSD1305_CMD_SETCLUT);
  write_cmd(0x3f); // 31-63
  write_cmd(0x3f); // 31-63
  write_cmd(0x3f); // 31-63
  // Segment Re-map
  write_cmd(SSD1305_CMD_SETSEGREMAP + 0x00); //No Remap
  //Switch Display to show RAM content
  write_cmd(SSD1305_CMD_ALLDISPON + 0x00); //0x01 turns all pixels on
  // Set Normal Display (Not inverse)
  write_cmd(SSD1305_CMD_NORMALINV + 0x00);
  // Set Multiplex Ratio
  write_cmd(SSD1305_CMD_SETMUXRATIO);
  write_cmd(0x3f); // Set to 64 decimal
  // Set DC-DC enable external Vc supply
  write_cmd(SSD1305_CMD_MASTERCONFIG);
  write_cmd(0x8e); //0x8f on 0x8e off
  // Display On
  write_cmd(SSD1305_CMD_DISPON);
  // Set COM Output scan direction
  write_cmd(SSD1305_CMD_SETSCANDIR + 0x08);  //remaped mode
  // Set Display Offset
  write_cmd(SSD1305_CMD_SETDISPOFFSET);
  write_cmd(0x00); //No vertical offset 0-63 decimal
  // Display Clock divide
  write_cmd(SSD1305_CMD_SETCLKDIV);
  write_cmd(0x00); //Set to 130Hz было А0
  // Set Area colour mode
  write_cmd(SSD1305_CMD_SETAREACOLRMODE);
  write_cmd(0x00); //Mono mode
  // Set Precharge Period
  write_cmd(SSD1305_CMD_SETPRECHARGE);
  write_cmd(0x28); //Default
  // COM Pins Hardware Configuration
  write_cmd(SSD1305_CMD_SETHWCONFIG);
  write_cmd(0x12); //Alt COM pins config
  // Set VCOMH
  write_cmd(SSD1305_CMD_VCOMLEVEL);
  write_cmd(0x34); // 0.605*12.5v = 7.56v, actual = 7.1v
//        write_cmd(0x1b); // 0.605*12.5v = 7.56v, actual = 7.1v
  LCD_NSS_HI;
}

/*
static void ssd1305_on()
{
  write_cmd(SSD1305_CMD_DISPON);
}

static void ssd1305_off()
{
  write_cmd(SSD1305_CMD_DISPOFF);
}
*/
static void SPI_SendByte(unsigned char byte)
{

     while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET);
//     LCD_NSS_LO;
     /* Send 16-bit word through the SPI1 peripheral */
     SPI_I2S_SendData(SPI1, byte);
     /* Loop while BUSY in communication or TX buffer not emplty
      */
     while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET);
//     LCD_NSS_HI;
}

static void write_cmd(unsigned char cmd)
{
  LCD_COMMAND;
  for(u32 i=0x070;i>0;i--){}//задержка для установления работы 0x010
  (void)SPI_SendByte(cmd);
}

void ssd1305_rst_first()
{
  LCD_NSS_LO; 
 for(u8 i=0x070;i>0;i--){}//задержка для установления работы 0x010
  write_cmd(SSD1305_CMD_SETDISPSTART); 
  write_cmd(0x00); 
  write_cmd(SSD1305_CMD_SETPAGEADD_PGMODE + 0);
  write_cmd(SSD1305_CMD_SETLOWCOLADD + 0);
  write_cmd(SSD1305_CMD_SETHIGHCOLADD + 0);
  
 // Set Normal Display (Not inverse)
  //write_cmd(SSD1305_CMD_NORMALINV + 0x00);

  LCD_DATA;

}
void ssd1305_rst_second()
{
  for(u8 i=0x070;i>0;i--){}//задержка для установления работы 0x010
  for(u8 i=0x070;i>0;i--){}//задержка для установления работы 0x010
  for(u8 i=0x070;i>0;i--){}//задержка для установления работы 0x010
   LCD_NSS_HI;
}



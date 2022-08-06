// --------------------------------------------------------------------------
// ST7735-library (implementation)
//
// This code should be portable and not PIC-specific.
// Hardware-specific implementations (e.g. #defines for 
// functions like spiwrite() or tft_dc_low()) are in hw.h
//
// Author: Bernhard Bablok
//
// The code is based on work from Gavin Lyons, see
// https://github.com/gavinlyonsrepo/pic_16F18346_projects
//
// https://github.com/bablokb/pic-st7735
// --------------------------------------------------------------------------

#include <stdint.h>
#include <stdlib.h>
#include "hw.h"
#include "ST7735_TFT.h"


// internal state (TODO: put into structure and pass as arg to allow
// more than a single display)
bool _wrap = true;
uint8_t _colstart = 0, _rowstart = 0, _tft_type, _rotation = 0, _xstart = 0, _ystart = 0;

// we keept this public
uint8_t tft_width = 128, tft_height = 160;

// because of code compatibilty
#define _width         tft_width
#define _height        tft_height

// Write an SPI command
void write_command(uint8_t cmd_){
  tft_dc_low();
  tft_cs_low();
  spiwrite(cmd_);
  tft_cs_high();
}

// Write SPI data
void write_data(uint8_t data_){
  tft_dc_high();
  tft_cs_low();
  spiwrite(data_);
  tft_cs_high();
}

#if defined TFT_ENABLE_GENERIC
void Bcmd(){
  write_command(ST7735_SWRESET);
  __delay_ms(50);
  write_command(ST7735_SLPOUT);
  __delay_ms(250);
  __delay_ms(250);
  write_command(ST7735_COLMOD);
  write_data(0x05);
  __delay_ms(10);
  write_command(ST7735_FRMCTR1);
  write_data(0x00);
  write_data(0x06);
  write_data(0x03);
  __delay_ms(10);
  write_command(ST7735_MADCTL);
  write_data(0x08);
  write_command(ST7735_DISSET5);
  write_data(0x15);
  write_data(0x02);
  write_command(ST7735_INVCTR);
  write_data(0x00);
  write_command(ST7735_PWCTR1);
  write_data(0x02);
  write_data(0x70);
  __delay_ms(10);
  write_command(ST7735_PWCTR2);
  write_data(0x05);
  write_command(ST7735_PWCTR3);
  write_data(0x01);
  write_data(0x02);
  write_command(ST7735_VMCTR1);
  write_data(0x3C);
  write_data(0x38);
  __delay_ms(10);
  write_command(ST7735_PWCTR6);
  write_data(0x11);
  write_data(0x15);
  write_command(ST7735_GMCTRP1);
  write_data(0x09); write_data(0x16); write_data(0x09); write_data(0x20);
  write_data(0x21); write_data(0x1B); write_data(0x13); write_data(0x19);
  write_data(0x17); write_data(0x15); write_data(0x1E); write_data(0x2B);
  write_data(0x04); write_data(0x05); write_data(0x02); write_data(0x0E);
  write_command(ST7735_GMCTRN1);
  write_data(0x0B); write_data(0x14); write_data(0x08); write_data(0x1E);
  write_data(0x22); write_data(0x1D); write_data(0x18); write_data(0x1E);
  write_data(0x1B); write_data(0x1A); write_data(0x24); write_data(0x2B);
  write_data(0x06); write_data(0x06); write_data(0x02); write_data(0x0F);
  __delay_ms(10);
  write_command(ST7735_CASET);
  write_data(0x00); write_data(0x02); write_data(0x08); write_data(0x81);
  write_command(ST7735_RASET);
  write_data(0x00); write_data(0x01); write_data(0x08); write_data(0xA0);
  write_command(ST7735_NORON);
  __delay_ms(10);
  write_command(ST7735_DISPON);
  __delay_ms(250);
  __delay_ms(250);
}
#endif

void Rcmd1(){
  write_command(ST7735_SWRESET);
  __delay_ms(150);
  write_command(ST7735_SLPOUT);
  __delay_ms(250);
  __delay_ms(250);
  write_command(ST7735_FRMCTR1);
  write_data(0x01);
  write_data(0x2C);
  write_data(0x2D);
  write_command(ST7735_FRMCTR2);
  write_data(0x01);
  write_data(0x2C);
  write_data(0x2D);
  write_command(ST7735_FRMCTR3);
  write_data(0x01); write_data(0x2C); write_data(0x2D);
  write_data(0x01); write_data(0x2C); write_data(0x2D);
  write_command(ST7735_INVCTR);
  write_data(0x07);
  write_command(ST7735_PWCTR1);
  write_data(0xA2);
  write_data(0x02);
  write_data(0x84);
  write_command(ST7735_PWCTR2);
  write_data(0xC5);
  write_command(ST7735_PWCTR3);
  write_data(0x0A);
  write_data(0x00);
  write_command(ST7735_PWCTR4);
  write_data(0x8A);
  write_data(0x2A);
  write_command(ST7735_PWCTR5);
  write_data(0x8A);
  write_data(0xEE);
  write_command(ST7735_VMCTR1);
  write_data(0x0E);
  write_command(ST7735_INVOFF);
  write_command(ST7735_MADCTL);
  write_data(0xC8);
  write_command(ST7735_COLMOD);
  write_data(0x05); 
}

#if defined(TFT_ENABLE_RED) || defined(TFT_ENABLE_BLACK)
void Rcmd2red(){
  write_command(ST7735_CASET);
  write_data(0x00); write_data(0x00);
  write_data(0x00); write_data(0x7F);
  write_command(ST7735_RASET);
  write_data(0x00); write_data(0x00);
  write_data(0x00); write_data(0x9F);
}
#endif

void Rcmd3(){
  write_command(ST7735_GMCTRP1);
  write_data(0x02); write_data(0x1C); write_data(0x07); write_data(0x12);
  write_data(0x37); write_data(0x32); write_data(0x29); write_data(0x2D);
  write_data(0x29); write_data(0x25); write_data(0x2B); write_data(0x39);
  write_data(0x00); write_data(0x01); write_data(0x03); write_data(0x10);
  write_command(ST7735_GMCTRN1);
  write_data(0x03); write_data(0x1D); write_data(0x07); write_data(0x06);
  write_data(0x2E); write_data(0x2C); write_data(0x29); write_data(0x2D);
  write_data(0x2E); write_data(0x2E); write_data(0x37); write_data(0x3F);
  write_data(0x00); write_data(0x00); write_data(0x02); write_data(0x10);
  write_command(ST7735_NORON);
  __delay_ms(10);
  write_command(ST7735_DISPON);
  __delay_ms(100);
}


/*
  SPI displays set an address window rectangle for blitting pixels
  Parameter1:  x0 Top left corner x coordinate
  Parameter2:  y0 Top left corner y coordinate
  Parameter3:  x1 Lower right corner x coordinate
  Parameter4:  y1 Lower right corner y coordinate
 https://en.wikipedia.org/wiki/Bit_blit
*/
void setAddrWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1){
  write_command(ST7735_CASET);
  write_data(0);
  write_data(x0 + _xstart);
  write_data(0);
  write_data(x1 + _xstart);
  write_command(ST7735_RASET);
  write_data(0);
  write_data(y0 + _ystart);
  write_data(0);
  write_data(y1 + _ystart);
  write_command(ST7735_RAMWR); // Write to RAM
}



void NormalDisplay(){
  write_command(ST7735_NORON);
}

void pushColors(uint16_t *colors, int len){
  tft_dc_high();
  tft_cs_low();

  spi_write_blocking(SPI_TFT_PORT, colors, len * 2);

  tft_cs_high() ;
}



// Function for Hardware Reset pin 
#if defined TFT_ENABLE_RESET
void TFT_ResetPIN() {
  tft_rst_high() ;
  __delay_ms(10);
  tft_rst_low();
  __delay_ms(10);
  tft_rst_high() ;
  __delay_ms(10);
}
#endif

// Init Black PCB version
#if defined TFT_ENABLE_BLACK
void TFT_BlackTab_Initialize(){
#if defined TFT_ENABLE_RESET
  TFT_ResetPIN();
#endif
  tft_dc_low();
  Rcmd1();
  Rcmd2red();
  Rcmd3();
  write_command(ST7735_MADCTL);
  write_data(0xC0);
  _tft_type = 1;
}
#endif

// Generic PCB init function
#if defined TFT_ENABLE_GENERIC
void TFT_ST7735B_Initialize(){
#if defined TFT_ENABLE_RESET
  TFT_ResetPIN();
#endif
  tft_dc_low();
  Bcmd();
  _tft_type = 2;
}
#endif

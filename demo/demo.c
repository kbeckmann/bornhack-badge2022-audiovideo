// --------------------------------------------------------------------------
// Demo program for the ST7735-library
//
// Author: Bernhard Bablok
//
// The code is based on work from Gavin Lyons, see
// https://github.com/gavinlyonsrepo/pic_16F18346_projects
//
// https://github.com/bablokb/pico-st7735
// --------------------------------------------------------------------------
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hw.h"
#include "tst_funcs.h"
#include "ST7735_TFT.h"

#include "color.h"
#include "color_invert.h"

// ---------------------------------------------------------------------------
// hardware-specific intialization
// SPI_* constants from CMakeLists.txt or user.h

void init_hw() {
  stdio_init_all();
  spi_init(SPI_PORT, 40000000);                // SPI with 40Mhz
  gpio_set_function(SPI_RX, GPIO_FUNC_SPI);
  gpio_set_function(SPI_SCK,GPIO_FUNC_SPI);
  gpio_set_function(SPI_TX, GPIO_FUNC_SPI);

  gpio_init(SPI_TFT_CS);
  gpio_set_dir(SPI_TFT_CS, GPIO_OUT);
  gpio_put(SPI_TFT_CS, 1);                        // Chip select is active-low

  gpio_init(SPI_TFT_DC);
  gpio_set_dir(SPI_TFT_DC, GPIO_OUT);
  gpio_put(SPI_TFT_DC, 0);                        // Chip select is active-low

  gpio_init(SPI_TFT_RST);
  gpio_set_dir(SPI_TFT_RST, GPIO_OUT);
  gpio_put(SPI_TFT_RST, 0);

  gpio_init(13);
  gpio_set_dir(13, GPIO_OUT);
  gpio_put(13, 1);
}


// ---------------------------------------------------------------------------
// main loop

static void draw_buffer(uint16_t *buf, int x, int y, int width, int height)
{
  setAddrWindow(x, y, x + width - 1, y + height - 1);
  pushColors(buf, width * height);
}

int main() {
  init_hw();
  TFT_BlackTab_Initialize();

  while(1) {
    draw_buffer((uint16_t *) color_rgb, 0, 0, 128, 128);
    draw_buffer((uint16_t *) color_invert_rgb, 0, 0, 128, 128);
  }

  while(1);
  return 0;
}

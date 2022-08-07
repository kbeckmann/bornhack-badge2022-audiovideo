// --------------------------------------------------------------------------
// Video and AM transmitter for the Bornhack Badge 2022.
// Based on the Demo program for the ST7735-library
//
// Author: Konrad Beckmann
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
#include "ST7735_TFT.h"

#include "pico/multicore.h"

#include "radio.pio.h"

typedef struct {
  uint32_t length;
  uint32_t fps;
  uint8_t  data[];
} video_data_header_t;



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


#define SAMPRATE 24000
#define FPS      25
uint16_t audio_buf[SAMPRATE / FPS / 2];
int audio_idx;
volatile bool audio_lock;

// Write `period` to the input shift register
void pio_radio_set_period(PIO pio, uint sm, uint32_t period) {
    pio_sm_set_enabled(pio, sm, false);
    pio_sm_put_blocking(pio, sm, period);
    pio_sm_exec(pio, sm, pio_encode_pull(false, false));
    pio_sm_exec(pio, sm, pio_encode_out(pio_isr, 32));
    pio_sm_set_enabled(pio, sm, true);
}

// Write `level` to TX FIFO. State machine will copy this into X.
void pio_radio_set_level(PIO pio, uint sm, uint32_t level) {
    pio_sm_put_blocking(pio, sm, level);
}



static void audio_core(void)
{
  PIO pio = pio0;

  uint offset = pio_add_program(pio, &radio_program);
  uint sm = pio_claim_unused_sm(pio, true);
  radio_program_init(pio, sm, offset, 11);

  // pio_radio_set_period(pio, sm, (1u << 16) - 1);
  pio_radio_set_period(pio, sm, (1u << 8) - 1);
  // pio_radio_set_period(pio, sm, 8);

  // gpio_set_drive_strength(11, GPIO_DRIVE_STRENGTH_2MA);
  // gpio_set_drive_strength(11, GPIO_DRIVE_STRENGTH_4MA); // default
  // gpio_set_drive_strength(11, GPIO_DRIVE_STRENGTH_8MA);
  gpio_set_drive_strength(11, GPIO_DRIVE_STRENGTH_12MA);


  int8_t *audio_data = (int8_t *) 0x10800000;
  int samples = 2177011;
  int i = 0;


  while(1) {
    if (audio_idx == 0) {
      audio_lock = true;
      memcpy(audio_buf, &audio_data[i], sizeof(audio_buf));
      audio_lock = false;
    }
    uint8_t sample = audio_data[i];
    i = (i + 1) % samples;
    audio_idx = (audio_idx + 1) % sizeof(audio_buf);

    pio_radio_set_level(pio, sm, sample);

    sleep_us(1000000 / SAMPRATE);
  }

}

// ---------------------------------------------------------------------------
// main loop

static void draw_buffer(const uint8_t *buf, int x, int y, int width, int height)
{
  setAddrWindow(x, y, x + width - 1, y + height - 1);
  pushColors(buf, width * height * 2);
}

int main() {

	set_sys_clock_khz(144000, true);

  multicore_launch_core1(audio_core);


  gpio_init(PICO_DEFAULT_LED_PIN);
  gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);





#if 0
  while (1) {

  }
#endif

  init_hw();
  TFT_BlackTab_Initialize();

  video_data_header_t *video_data_header = (video_data_header_t *) 0x10010000;

  int frame = 0;
  int width = 129;
  int height = 161;
  int frame_size_bytes = width * height * 2;
  int frames = video_data_header->length / frame_size_bytes;
  int fps = video_data_header->fps;
  int one_over_fps = 1000 / fps;

  uint8_t *video_data = video_data_header->data;

  // Wait for the audio core to load its data
  while (audio_lock == false) {

  }

  while(1) {
    uint64_t t0 = to_us_since_boot(get_absolute_time());

    // Ensure that audio core has loaded its data
    while (audio_lock == true) {

    }

    draw_buffer(&video_data[frame * frame_size_bytes], 0, 0, width, height);

    // Wait for the audio core to load its data
    while (audio_lock == false) {

    }
    frame = (frame + 1) % frames;
  }

  while(1);
  return 0;
}

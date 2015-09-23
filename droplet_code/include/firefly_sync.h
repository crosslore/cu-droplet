#pragma once

#include <avr/io.h>
#include "droplet_init.h"

#define FFSYNC_FULL_PERIOD_MS		2333
#define FFSYNC_REFR_PERIOD_MS		221
#define FFSYNC_TRANSMIT_DELAY_MS	16

#define FFSYNC_MS_CONVERSION_FACTOR 7.8125

#define FFSYNC_FULL_PERIOD			(uint16_t)(FFSYNC_FULL_PERIOD_MS*FFSYNC_MS_CONVERSION_FACTOR)
#define FFSYNC_REFR_PERIOD			(uint16_t)(FFSYNC_REFR_PERIOD_MS*FFSYNC_MS_CONVERSION_FACTOR)
#define FFSYNC_TRANSMIT_DELAY		(uint16_t)(FFSYNC_TRANSMIT_DELAY_MS*FFSYNC_MS_CONVERSION_FACTOR)

#define FFSYNC_MAX_DEVIATION		(((uint16_t)(FFSYNC_FULL_PERIOD_MS/182))+1)

#define FIREFLY_SYNC_B 2.75
#define FIREFLY_SYNC_EPS 0.25
#define FIREFLY_SYNC_ALPHA exp(FIREFLY_SYNC_B*FIREFLY_SYNC_EPS)
#define FIREFLY_SYNC_BETA ((FIREFLY_SYNC_ALPHA - 1) / (exp(FIREFLY_SYNC_B) - 1))

void firefly_sync_init();

inline void update_firefly_counter()
{
	uint16_t the_count = TCE0.CNT;
	if(the_count>FFSYNC_REFR_PERIOD)
	TCE0.CNT =  (uint16_t)(fmin(FIREFLY_SYNC_ALPHA * the_count/FFSYNC_FULL_PERIOD + FIREFLY_SYNC_BETA, 1.) * ((float)FFSYNC_FULL_PERIOD));
}

<<<<<<< HEAD
volatile uint32_t lightStart;
=======
uint8_t sync_blink_r, sync_blink_g, sync_blink_b;
uint8_t sync_def_r, sync_def_g, sync_def_b;
void set_sync_blink_default(uint8_t r, uint8_t g, uint8_t b);
void set_sync_blink_color(uint8_t r, uint8_t g, uint8_t b);
void enable_sync_blink(uint16_t phase_offset_ms);
void disable_sync_blink();
>>>>>>> 514cdc8daee1822f543b65000feef87a4419a489

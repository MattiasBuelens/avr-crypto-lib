/* main-echo-test.c */
/*
    This file is part of the AVR-Crypto-Lib.
    Copyright (C) 2008  Daniel Otte (daniel.otte@rub.de)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
/*
 * CubeHash test-suit
 *
*/

#include "config.h"
#include "uart_i.h"
#include "debug.h"

#include "echo.h"
#include "cli.h"
#include "hfal_echo.h"
#include "shavs.h"
#include "nessie_hash_test.h"
#include "performance_test.h"
#include "hfal-nessie.h"
#include "hfal-performance.h"
#include "hfal-test.h"

#include <stdint.h>
#include <string.h>
#include <stdlib.h>

char* algo_name = "CubeHash";


const hfdesc_t* algolist[] PROGMEM = {
	(hfdesc_t*)&echo224_desc,
	(hfdesc_t*)&echo256_desc,
	NULL
};

/*****************************************************************************
 *  additional validation-functions											 *
 *****************************************************************************/
/* IntermediateKAT1_256.txt */
uint8_t intermediate_data[] PROGMEM = {
	0xDB, 0x11, 0xF6, 0x09, 0xBA, 0xBA, 0x7B, 0x0C,
	0xA6, 0x34, 0x92, 0x6B, 0x1D, 0xD5, 0x39, 0xC8,
	0xCB, 0xAD, 0xA2, 0x49, 0x67, 0xD7, 0xAD, 0xD4,
	0xD9, 0x87, 0x6F, 0x77, 0xC2, 0xD8, 0x0C, 0x0F,
	0x4D, 0xCE, 0xFB, 0xD7, 0x12, 0x15, 0x48, 0x37,
	0x35, 0x82, 0x70, 0x5C, 0xCA, 0x24, 0x95, 0xBD,
	0x2A, 0x43, 0x71, 0x6F, 0xE6, 0x4E, 0xD2, 0x6D,
	0x05, 0x9C, 0xFB, 0x56, 0x6B, 0x33, 0x64, 0xBD,
	0x49, 0xEE, 0x07, 0x17, 0xBD, 0xD9, 0x81, 0x0D,
	0xD1, 0x4D, 0x8F, 0xAD, 0x80, 0xDB, 0xBD, 0xC4,
	0xCA, 0xFB, 0x37, 0xCC, 0x60, 0xFB, 0x0F, 0xE2,
	0xA8, 0x0F, 0xB4, 0x54, 0x1B, 0x8C, 0xA9, 0xD5,
	0x9D, 0xCE, 0x45, 0x77, 0x38, 0xA9, 0xD3, 0xD8,
	0xF6, 0x41, 0xAF, 0x8C, 0x3F, 0xD6, 0xDA, 0x16,
	0x2D, 0xC1, 0x6F, 0xC0, 0x1A, 0xAC, 0x52, 0x7A,
	0x4A, 0x02, 0x55, 0xB4, 0xD2, 0x31, 0xC0, 0xBE,
	0x50, 0xF4, 0x4F, 0x0D, 0xB0, 0xB7, 0x13, 0xAF,
	0x03, 0xD9, 0x68, 0xFE, 0x7F, 0x0F, 0x61, 0xED,
	0x08, 0x24, 0xC5, 0x5C, 0x4B, 0x52, 0x65, 0x54,
	0x8F, 0xEB, 0xD6, 0xAA, 0xD5, 0xC5, 0xEE, 0xDF,
	0x63, 0xEF, 0xE7, 0x93, 0x48, 0x9C, 0x39, 0xB8,
	0xFD, 0x29, 0xD1, 0x04, 0xCE
	};

void echo256_interm(void){
	echo_small_ctx_t ctx;
	uint8_t data[1384/8];
	uint8_t hash[32];
	echo256_init(&ctx);
	memcpy_P(data, intermediate_data, 173);
	cli_putstr_P(PSTR("\r\ninit done "));
	echo_small_lastBlock(&ctx, data, 1384);
	cli_putstr_P(PSTR("\r\nlastblock done "));
	echo256_ctx2hash(hash, &ctx);
	cli_putstr_P(PSTR("\r\nhash = "));
	cli_hexdump(hash, 32);
}

void echo256_test0(void){
	echo_small_ctx_t ctx;
	uint8_t hash[32];
	echo256_init(&ctx);
	cli_putstr_P(PSTR("\r\ninit done "));
	echo_small_lastBlock(&ctx, NULL, 0);
	cli_putstr_P(PSTR("\r\nlastblock done "));
	echo256_ctx2hash(hash, &ctx);
	cli_putstr_P(PSTR("\r\nhash = "));
	cli_hexdump(hash, 32);
}

void performance_echo(void){
	hfal_performance_multiple(algolist);
}

void testrun_nessie_echo(void){
	hfal_nessie_multiple(algolist);
}
/*****************************************************************************
 *  main																	 *
 *****************************************************************************/

const char nessie_str[]      PROGMEM = "nessie";
const char test256_str[]     PROGMEM = "test256";
const char interm_str[]      PROGMEM = "interm";
const char performance_str[] PROGMEM = "performance";
const char echo_str[]        PROGMEM = "echo";
const char shavs_list_str[]  PROGMEM = "shavs_list";
const char shavs_set_str[]   PROGMEM = "shavs_set";
const char shavs_test1_str[] PROGMEM = "shavs_test1";
const char shavs_test3_str[] PROGMEM = "shavs_test3";

cmdlist_entry_t cmdlist[] PROGMEM = {
	{ nessie_str,                NULL, testrun_nessie_echo         },
	{ interm_str,                NULL, echo256_interm              },
	{ test256_str,               NULL, echo256_test0               },
	{ performance_str,           NULL, performance_echo            },
	{ shavs_list_str,            NULL, shavs_listalgos             },
	{ shavs_set_str,         (void*)1, (void_fpt)shavs_setalgo     },
	{ shavs_test1_str,           NULL, shavs_test1                 },
	{ shavs_test3_str,           NULL, shavs_test3                 },
	{ echo_str,              (void*)1, (void_fpt)echo_ctrl         },
	{ NULL,                      NULL, NULL                        }
};

int main (void){
	DEBUG_INIT();

	cli_rx = (cli_rx_fpt)uart0_getc;
	cli_tx = (cli_tx_fpt)uart0_putc;
	shavs_algolist=(hfdesc_t**)algolist;
	shavs_algo=(hfdesc_t*)&echo256_desc;
	for(;;){
		cli_putstr_P(PSTR("\r\n\r\nCrypto-VS ("));
		cli_putstr(algo_name);
		cli_putstr_P(PSTR("; "));
		cli_putstr(__DATE__);
		cli_putstr_P(PSTR(" "));
		cli_putstr(__TIME__);
		cli_putstr_P(PSTR(")\r\nloaded and running\r\n"));

		cmd_interface(cmdlist);
	}
}

/* uart_ni.h */
/*
    This file is part of the AVR-uart_ni.
    Copyright (C) 2006-2015 Daniel Otte (bg@nerilex.org)

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
/**
 * \file     uart_ni.h
 * \email    bg@nerilex.org
 * \author   Daniel Otte
 * \date     2009-07-24
 * \license  GPLv3 or later
 * \ingroup  uart_ni
 * \brief    declaration for non-interrupt uart
 */

#ifndef UART_NI_H_
#define UART_NI_H_

#include "config.h"
#include <stdint.h>

#if UART0_NI

/**
 * \brief initialize uart0.
 * This function initializes the first uart according to the parameter specified
 * in config.h .
 */
void uart0_init(void);

/**
 * \brief send data through uart0.
 * This function sends data through the first uart
 * (the data size is defined in config.h).
 * \param c data to send
 */
void uart0_putc(uint16_t c);

/**
 * \brief read data from uart0.
 * This function reads data from the first uart
 * (the data size is defined in config.h).
 * \return data received by uart0
 */
uint16_t uart0_getc(void);

/**
 * \brief checks if data is available.
 *
 * This function checks the state of the input buffer of uart0 and
 * returns if data is available or not.
 * \return zero if no data is available else a value different from zero is returned
 */
uint8_t uart0_dataavail(void);

#define uart0_flush()

#endif /* UART0_NI */

#if UART1_NI
/**
 * \brief initialize uart1.
 * This function initializes the second uart according to the parameter specifyed
 * in config.h .
 */
void uart1_init(void);

/**
 * \brief send data through uart1.
 * This function sends data through the second uart
 * (the data size is defined in config.h).
 * \param c data to send
 */
void uart1_putc(uint16_t c);

/**
 * \brief read data from uart1.
 * This function reads data from the second uart
 * (the data size is defined in config.h).
 * \return data received by uart1
 */
uint16_t uart1_getc(void);

/**
 * \brief checks if data is available.
 * This function checks the state of the input buffer of uart1 and
 * returns if data is available or not.
 * \return zero if no data is available else a value different from zero is returned
 */
uint8_t uart1_dataavail(void);

#define uart1_flush()

#endif /* UART1_NI */

#endif /* UART_NI_H_ */

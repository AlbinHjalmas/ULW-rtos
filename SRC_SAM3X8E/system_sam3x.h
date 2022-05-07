/**
 * \file
 *
 * \brief Provides the low-level initialization functions that called 
 * on chip startup.
 *
 * Copyright (c) 2011-2012 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */

#ifndef SYSTEM_SAM3X_H_INCLUDED
#define SYSTEM_SAM3X_H_INCLUDED

/* @cond 0 */
/**INDENT-OFF**/
#ifdef __cplusplus
extern "C" {
#endif
/**INDENT-ON**/
/* @endcond */

#include <stdint.h>
#include "core_cm3.h"
  
/* ************************************************************************** */
/*   ELECTRICAL DEFINITIONS FOR SAM3X8E */
/* ************************************************************************** */

/* Device characteristics */
#define CHIP_FREQ_SLCK_RC_MIN           (20000UL)
#define CHIP_FREQ_SLCK_RC               (32000UL)
#define CHIP_FREQ_SLCK_RC_MAX           (44000UL)
#define CHIP_FREQ_MAINCK_RC_4MHZ        (4000000UL)
#define CHIP_FREQ_MAINCK_RC_8MHZ        (8000000UL)
#define CHIP_FREQ_MAINCK_RC_12MHZ       (12000000UL)
#define CHIP_FREQ_CPU_MAX               (84000000UL)
#define CHIP_FREQ_XTAL_32K              (32768UL)
#define CHIP_FREQ_XTAL_12M              (12000000UL)

/* Embedded Flash Write Wait State */
#define CHIP_FLASH_WRITE_WAIT_STATE     (6U)

/* Embedded Flash Read Wait State (VDDCORE set at 1.8V) */
#define CHIP_FREQ_FWS_0                 (19000000UL) /**< \brief Maximum operating frequency when FWS is 0 */
#define CHIP_FREQ_FWS_1                 (50000000UL) /**< \brief Maximum operating frequency when FWS is 1 */
#define CHIP_FREQ_FWS_2                 (64000000UL) /**< \brief Maximum operating frequency when FWS is 2 */
#define CHIP_FREQ_FWS_3                 (80000000UL) /**< \brief Maximum operating frequency when FWS is 3 */
#define CHIP_FREQ_FWS_4                 (90000000UL) /**< \brief Maximum operating frequency when FWS is 4 */
  
 
#define CKGR_MOR_MOSCRCF_4_MHz   (0x0u << 4)
#define CKGR_MOR_MOSCRCF_8_MHz   (0x1u << 4)
#define CKGR_MOR_MOSCRCF_12_MHz   (0x2u << 4)

  
extern uint32_t SystemCoreClock;	/* System Clock Frequency (Core Clock) */

/**
 * @brief Setup the microcontroller system.
 * Initialize the System and update the SystemCoreClock variable.
 */
void SystemInit(void);

/**
 * @brief Updates the SystemCoreClock with current core Clock 
 * retrieved from cpu registers.
 */
void SystemCoreClockUpdate(void);

/** 
 * Initialize flash.
 */
void system_init_flash(uint32_t ul_clk);



#endif /* SYSTEM_SAM3X_H_INCLUDED */

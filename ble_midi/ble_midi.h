/**
 * Copyright (c) 2015 - 2020, Nordic Semiconductor ASA
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 *
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
 
 /**
 Copyright 2021, Konstantin Vysotsky
 */
 

#ifndef NRF_SDH_BLE_MIDI_H__
#define NRF_SDH_BLE_MIDI_H__

#include "app_util.h"
#include "ble.h"
#include "nrf_section_iter.h"
#include "sdk_config.h"
#include "sdk_errors.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**@brief   Macro for defining a ble_midi instance.
 *
 * @param   _name   Name of the instance.
 * @hideinitializer
 */
#define BLE_MIDI_DEF(_name)                                                                          \
static ble_midi_t _name;                                                                             \
NRF_SDH_BLE_OBSERVER(_name ## _obs,                                                                  \
                     BLE_MIDI_BLE_OBSERVER_PRIO,                                                     \
                     ble_midi_on_ble_evt, &_name)
										 
#define MIDI_UUID_BASE        {0x00, 0xC7, 0XC4, 0X4E, 0XE3, 0X6C, 0X51, 0XA7, \
                               0X33, 0X4B, 0XE8, 0XED, 0X00, 0X00, 0XB8, 0X03 }
#define MIDI_UUID_SERVICE     0x0E5A
#define MIDI_UUID_CHAR		   	0xE5DB
#define MIDI_UUID_CHAR_BASE 	{0xF3, 0X6B, 0X10, 0X9D, 0X66, 0XF2, 0XA9, 0XA1, \
															0X12, 0X41, 0X68, 0X38, 0XDB, 0XE5, 0X72, 0X77 }

// Forward declaration of the ble_midi_t type.
typedef struct ble_midi_s ble_midi_t;
																	 
typedef void (*ble_midi_io_write_handler_t) (uint16_t conn_handle, ble_midi_t * p_midi, const uint8_t * message, size_t len);
																	 
/** @brief MIDI Service init structure. This structure contains all options and data needed for
 *        initialization of the service.*/
typedef struct
{
    ble_midi_io_write_handler_t io_write_handler;  /**< Event handler to be called when the MIDI IO Characteristic is written. */
} ble_midi_init_t;

/**@brief MIDI Button Service structure. This structure contains various status information for the service. */
struct ble_midi_s
{
    uint16_t                    service_handle;      /**< Handle of MIDI Service (as provided by the BLE stack). */
    ble_gatts_char_handles_t    io_char_handles;     /**< Handles related to the LED Characteristic. */
    uint8_t                     uuid_type;           /**< UUID type for the LED Button Service. */
    ble_midi_io_write_handler_t io_write_handler;    /**< Event handler to be called when the LED Characteristic is written. */
};											


/**@brief Function for initializing the MIDI Service.
 *
 * @param[out] p_midi     MIDI Service structure. This structure must be supplied by
 *                        the application. It is initialized by this function and will later
 *                        be used to identify this particular service instance.
 * @param[in] p_midi_init  Information needed to initialize the service.
 *
 * @retval NRF_SUCCESS If the service was initialized successfully. Otherwise, an error code is returned.
 */
uint32_t ble_midi_init(ble_midi_t * p_midi, const ble_midi_init_t * p_midi_init);


/**@brief Function for handling the application's BLE stack events.
 *
 * @details This function handles all events from the BLE stack that are of interest to the MIDI Service.
 *
 * @param[in] p_ble_evt  Event received from the BLE stack.
 * @param[in] p_context  MIDI Service structure.
 */
void ble_midi_on_ble_evt(ble_evt_t const * p_ble_evt, void * p_context);


/**@brief Function for sending a MIDI message notification.
 *
 ' @param[in] conn_handle   Handle of the peripheral connection to which the MIDI message notification will be sent.
 * @param[in] p_midi 				MIDI Service structure.
 * @param[in] packet 				BLE-MIDI packet.
 * @param[in] len           Length of the MIDI message.
 *
 * @retval NRF_SUCCESS If the notification was sent successfully. Otherwise, an error code is returned.
 */
uint32_t ble_midi_send_packet(uint16_t conn_handle, ble_midi_t * p_midi, uint8_t * packet, uint16_t len);

#ifdef __cplusplus
}
#endif

#endif // BLE_LBS_H__

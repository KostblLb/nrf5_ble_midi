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
 
#include "sdk_common.h"
#include "ble_midi.h"
#include "ble_srv_common.h"
#include "nrf_log.h"
#include "app_error.h"


/**@brief Function for handling the Write event.
 *
 * @param[in] p_midi     MIDI Service structure.
 * @param[in] p_ble_evt  Event received from the BLE stack.
 */
static void on_write(ble_midi_t * p_midi, ble_evt_t const * p_ble_evt)
{
    ble_gatts_evt_write_t const * p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;

		NRF_LOG_INFO("WRITE some data...");
		NRF_LOG_HEXDUMP_INFO(p_evt_write->data, p_evt_write->len);
    if (   (p_evt_write->handle == p_midi->io_char_handles.value_handle)
        && (p_evt_write->len > 0)
        && (p_midi->io_write_handler!= NULL))
    {
        p_midi->io_write_handler(p_ble_evt->evt.gap_evt.conn_handle, p_midi, &(p_evt_write->data[0]), p_evt_write->len);
    }
}


void ble_midi_on_ble_evt(ble_evt_t const * p_ble_evt, void * p_context)
{
    ble_midi_t * p_midi = (ble_midi_t *)p_context;
	  NRF_LOG_INFO("received EVENT %d", p_ble_evt->header.evt_id );

    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GATTS_EVT_WRITE:
            on_write(p_midi, p_ble_evt);
            break;

        default:
            // No implementation needed.
            break;
    }
}


uint8_t MIDI_IO_CHAR_INIT_VALUE = 0x00;
uint32_t ble_midi_init(ble_midi_t * p_midi, const ble_midi_init_t * p_midi_init)
{
    uint32_t              err_code;
    ble_uuid_t            ble_uuid;
    ble_add_char_params_t add_char_params;

    // Initialize service structure.
    p_midi->io_write_handler = p_midi_init->io_write_handler;

    // Add service.
    ble_uuid128_t service_base_uuid = {MIDI_UUID_BASE};
    err_code = sd_ble_uuid_vs_add(&service_base_uuid, &p_midi->uuid_type);
    VERIFY_SUCCESS(err_code);

    ble_uuid.type = p_midi->uuid_type;
    ble_uuid.uuid = MIDI_UUID_SERVICE;

    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &ble_uuid, &p_midi->service_handle);
    VERIFY_SUCCESS(err_code);
		

    // Add Button characteristic.
    memset(&add_char_params, 0, sizeof(add_char_params));
    add_char_params.uuid              = MIDI_UUID_CHAR;
 // add_char_params.uuid_type         set later
    add_char_params.max_len           = 32 * sizeof(uint8_t); // ?
		add_char_params.is_var_len				= true;
    add_char_params.init_len          = sizeof(MIDI_IO_CHAR_INIT_VALUE);
		add_char_params.p_init_value 			= &MIDI_IO_CHAR_INIT_VALUE; // always keep or tmp value is ok?
		add_char_params.char_props.write   = 1;
		add_char_params.char_props.write_wo_resp = 1;
    add_char_params.char_props.read   = 1;
    add_char_params.char_props.notify = 1;

    add_char_params.write_access      = SEC_OPEN;
    add_char_params.read_access       = SEC_OPEN;
    add_char_params.cccd_write_access = SEC_OPEN;
		
    ble_uuid128_t io_char_base_uuid = {MIDI_UUID_CHAR_BASE};
    err_code = sd_ble_uuid_vs_add(&io_char_base_uuid, &add_char_params.uuid_type);
    VERIFY_SUCCESS(err_code);

    return characteristic_add(p_midi->service_handle,
                                  &add_char_params,
                                  &p_midi->io_char_handles);
}


uint32_t ble_midi_send_packet(uint16_t conn_handle, ble_midi_t * p_midi, uint8_t * packet, uint16_t len)
{
    ble_gatts_hvx_params_t params;

    memset(&params, 0, sizeof(params));
    params.type   = BLE_GATT_HVX_NOTIFICATION;
    params.handle = p_midi->io_char_handles.value_handle;
    params.p_data = packet;
    params.p_len  = &len;
	
		return sd_ble_gatts_hvx(conn_handle, &params);
}

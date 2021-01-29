 /**
 Copyright 2021, Konstantin Vysotsky
 */

#ifndef __BLE_MIDI_MESSAGE_H__
#define __BLE_MIDI_MESSAGE_H__

#include <stdint.h>
#include <stdlib.h>
#include <string.h>           
#define MIDI_MAX_MESSAGES_PER_PACKET 120
#define MIDI_MAX_DATA_PER_MESSAGE 3
#define MIDI_MAX_PACKET_SIZE (MIDI_MAX_MESSAGES_PER_PACKET * (MIDI_MAX_DATA_PER_MESSAGE + 1))

#ifdef __cplusplus
extern "C" {
#endif

/** @brief single midi event message
*/
typedef struct {
	uint8_t  timestamp_low;
	uint8_t  p_data[MIDI_MAX_DATA_PER_MESSAGE]; // status byte + midi data
	uint8_t  len;
} ble_midi_message_t;

/** @brief midi message manager, accumulates messages and sends them
*/
typedef struct {
	uint16_t timestamp;
	ble_midi_message_t messages[MIDI_MAX_MESSAGES_PER_PACKET];
	uint16_t messages_len;
} ble_midi_session_t;

/** @brief creates session used to collect messages in a buffer ant then send using BLE
*/
ble_midi_session_t create_ble_midi_session(void);

/**
	@param message, including status byte
*/
void post_ble_midi_message(ble_midi_session_t* session, uint8_t* message, uint8_t len);

/** @brief writes a BLE-MIDI packet to a buffer
		@returns written data length
*/
uint16_t get_ble_midi_packet(ble_midi_session_t* session, uint8_t* buffer);

/** @brief resets the session to create another packet
*/
void flush_ble_midi_session(ble_midi_session_t* session);

/** @brief checks if the session has any messages to send;
*/
int has_ble_midi_messages(ble_midi_session_t* session);

#ifdef __cplusplus
}
#endif

#endif

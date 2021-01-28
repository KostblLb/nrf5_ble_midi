#include "ble_midi_message.h"
#include "nrf_log.h"

static int is_status_byte(uint8_t byte) ;

ble_midi_session_t create_ble_midi_session() {
	ble_midi_session_t session;
	session.timestamp = 0;
	session.messages_len = 0;
	memset(session.messages, 0, sizeof(ble_midi_message_t) * MIDI_MAX_MESSAGES_PER_PACKET);
	return session;
}

void post_ble_midi_message(ble_midi_session_t* session, uint8_t* message, uint8_t len) {
	NRF_LOG_INFO("Posting MIDI message:");
	NRF_LOG_HEXDUMP_INFO(message, len);
	session->timestamp += 0;											// for now assume all messages come without latency

	ble_midi_message_t ble_message;
	ble_message.len = len;
	memcpy(ble_message.p_data, message, len); // for now assume all messages have a status byte
	ble_message.timestamp_low = 0x7F & session->timestamp;
	
	memcpy(&session->messages[session->messages_len], &ble_message, sizeof(ble_midi_message_t));
	session->messages_len++;
}

uint16_t get_ble_midi_packet(ble_midi_session_t* session, uint8_t* buffer) {
	NRF_LOG_INFO("Getting MIDI packet, summary:");
	NRF_LOG_INFO("messages len:\t%d;\n", session->messages_len);
	
	buffer[0] = (1 << 7) | (session->timestamp >> 7); // that's the packet's header
	int i = 1;
	for (int msg_idx = 0; msg_idx < session->messages_len; msg_idx++) {
		ble_midi_message_t msg = session->messages[msg_idx];
		NRF_LOG_INFO("message [%d] (len: %d):", msg_idx, msg.len);
		NRF_LOG_HEXDUMP_INFO(msg.p_data, msg.len);
		buffer[i] = 1 << 7 | msg.timestamp_low;
		i++;
		
		for (int msg_data_idx = 0; msg_data_idx < msg.len; msg_data_idx++) {
			buffer[i] = is_status_byte(msg.p_data[msg_data_idx]) ? (1 << 7 | msg.p_data[msg_data_idx]) : msg.p_data[msg_data_idx];
			i++;
		}
	}
	
	NRF_LOG_INFO("MIDI packet:");
	NRF_LOG_HEXDUMP_INFO(buffer, i);
	return i;
}

void flush_ble_midi_session(ble_midi_session_t* session) {
	memset(session->messages, 0, sizeof(ble_midi_message_t) * MIDI_MAX_MESSAGES_PER_PACKET);
	session->messages_len = 0;
}

static int is_status_byte(uint8_t byte) {
	return true;
}

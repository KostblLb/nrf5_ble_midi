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

void post_ble_midi_message(ble_midi_session_t* session, uint8_t* message, uint8_t len, uint64_t posting_time) {
	if (!session->messages_len) {
		session->timestamp = 0x1FFF & posting_time; // keep the last 13 bits as the packet timestamp
	}

	ble_midi_message_t ble_message;
	ble_message.len = len;
	memcpy(ble_message.p_data, message, len);
	ble_message.timestamp_low = 0x7F & posting_time;
	
	memcpy(&session->messages[session->messages_len], &ble_message, sizeof(ble_midi_message_t));
	session->messages_len++;
}

uint16_t get_ble_midi_packet(ble_midi_session_t* session, uint8_t* buffer) {
	buffer[0] = (1 << 7) | (session->timestamp >> 7); // that's the packet's header
	int i = 1;
	for (int msg_idx = 0; msg_idx < session->messages_len; msg_idx++) {
		ble_midi_message_t msg = session->messages[msg_idx];
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

int has_ble_midi_messages(ble_midi_session_t* session) {
	return session->messages_len;
}

static int is_status_byte(uint8_t byte) {
	return true;
}

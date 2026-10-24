#include <parser.h>
#include <controls.h>

/*
 * Synth communication protocol
 * Packet format:
 * +--------+--------+----------+-----------+---------+---------+---------+
 * | HEADER | MODULE | VOICE_ID | PARAMETER | INDEX   | VALUE_H | VALUE_L |
 * | 1 byte | 1 byte | 1 byte   | 1 byte    | 1 byte  | 1 byte  | 1 byte  |
 * +--------+--------+----------+-----------+---------+---------+---------+
 *
 * HEADER = 0xAA
 *
 * VALUE is transmitted MSB first:
 * value = (VALUE_H << 8) | VALUE_L
*/

void protocol_parse_byte(uint8_t byte){
	static parser_state_t state = WAIT_HEADER;
	static uint8_t buffer[PACKET_BYTES];
	static uint8_t idx = 0;

	switch(state){
		case WAIT_HEADER:
			if(byte==0xAA){
				idx=0;
				state=READ_PACKET;
			}
			break;

		case READ_PACKET:
			buffer[idx]=byte;
			idx++;
			if(idx==PACKET_BYTES){
				packet_t packet;
				packet.module = buffer[0];
				packet.voice_id = buffer[1];
				packet.parameter = buffer[2];
				packet.index = buffer[3];
				packet.value = ((uint16_t)buffer[4] << 8) | buffer[5];
				update_controls(&packet);
				state=WAIT_HEADER;
			}
			break;
	}
}

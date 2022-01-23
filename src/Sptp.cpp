/**
 * Copyright (c) 2022 Florian Frankenberger
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include<Sptp.h>

Sptp::Sptp(int baudRate) {
    Serial.begin(baudRate);
}

void Sptp::process() {
    while (Serial.available()) {
        uint8_t read_byte = Serial.read();
        
        if (!this->in_frame) {
            if (read_byte == SPTP_FRAME_START) {
                this->in_frame = true;
                this->frame_pos = 0;
            }
        } else {
            this->buffer[this->frame_pos++] = read_byte;

            //header completely read?
            if (this->frame_pos == HEADER_SIZE) {
                this->payload_len = (this->buffer[FRAME_POS_PAYLOAD_LEN_HIGH] << 8) | (this->buffer[FRAME_POS_PAYLOAD_LEN_LOW]);
                this->payload_crc = (this->buffer[FRAME_POS_CRC_BYTE_4] << 24) 
                    | (this->buffer[FRAME_POS_CRC_BYTE_3] << 16)
                    | (this->buffer[FRAME_POS_CRC_BYTE_2] << 8)
                    | (this->buffer[FRAME_POS_CRC_BYTE_1] << 0);
            }

            if (this->frame_pos > HEADER_SIZE && this->frame_pos - HEADER_SIZE >= this->payload_len) {
                this->in_frame = false;
                this->processFrame();
            }
        }
    }
}

void Sptp::processFrame() {
    uint8_t *data = this->buffer + HEADER_SIZE;
    uint32_t crc = CRC32.crc32(data, this->payload_len);

    if (crc != this->payload_crc) {
        return;
    }

    if (this->callback != NULL) {
        this->callback(this->buffer[FRAME_POS_COMMAND], data, this->payload_len);
    }
}

void Sptp::sendCommand(uint8_t command, uint8_t *argv, uint16_t argc) {
    Serial.write(SPTP_FRAME_START);
    Serial.write(command);
    Serial.write((argc >> 8) & 0xFF);
    Serial.write(argc & 0xFF);
    this->writeCrc(argv, argc);
    Serial.write(argv, argc);
    Serial.flush();
}

void Sptp::setCommandCallback(commandCallbackFunction callback) {
    this->callback = callback;
}

void Sptp::writeCrc(uint8_t *argv, uint16_t argc) {
    uint32_t crc = CRC32.crc32(argv, argc);
    Serial.write((crc >> 24) & 0xFF);
    Serial.write((crc >> 16) & 0xFF);
    Serial.write((crc >> 8) & 0xFF);
    Serial.write(crc & 0xFF);
}
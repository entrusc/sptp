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
#ifndef SPTP_H_
#define SPTP_H_

#include <Arduino.h>
#include <FastCRC.h>

#define SPTP_FRAME_START 0b01010101
#define MAX_FRAME_LEN 65536

// relative to the frame start byte
#define FRAME_POS_COMMAND           0x00
#define FRAME_POS_PAYLOAD_LEN_HIGH  0x01
#define FRAME_POS_PAYLOAD_LEN_LOW   0x02
#define FRAME_POS_CRC_BYTE_4        0x03
#define FRAME_POS_CRC_BYTE_3        0x04
#define FRAME_POS_CRC_BYTE_2        0x05
#define FRAME_POS_CRC_BYTE_1        0x06

#define HEADER_SIZE 7

class Sptp {
    typedef void (*commandCallbackFunction)(uint8_t command, uint8_t *argv, uint16_t argc);
    commandCallbackFunction callback;
    FastCRC32 CRC32;

    uint8_t buffer[MAX_FRAME_LEN];
    bool in_frame = false;
    uint16_t frame_pos = 0;
    uint16_t payload_len = 0;
    uint32_t payload_crc = 0;

    void processFrame();
    void writeCrc(uint8_t *argv, uint16_t argc);

    public:
        Sptp(int baudRate);

        void process();
        void sendCommand(uint8_t command, uint8_t *argv, uint16_t argc);
        void setCommandCallback(commandCallbackFunction callback);
};

#endif
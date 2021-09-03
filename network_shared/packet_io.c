
/**********************************************************************************************
*
*   raylib_networking_smaple * a sample network game using raylib and enet
*
*   LICENSE: ZLIB
*
*   Copyright (c) 2021 Jeffery Myers
*
*   Permission is hereby granted, free of charge, to any person obtaining a copy
*   of this software and associated documentation files (the "Software"), to deal
*   in the Software without restriction, including without limitation the rights
*   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
*   copies of the Software, and to permit persons to whom the Software is
*   furnished to do so, subject to the following conditions:
*
*   The above copyright notice and this permission notice shall be included in all
*   copies or substantial portions of the Software.
*
*   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
*   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
*   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
*   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
*   SOFTWARE.
*
**********************************************************************************************/

// common data handling functions shared by the server and the client

#include "packet_io.h"

uint8_t ReadByte(ENetPacket* packet, size_t* offset)
{
    // make sure we have not gone past the end of the data we were sent
    if (*offset > packet->dataLength)
        return 0;

    // cast the data to a byte so we can increment it in 1 byte chunks
    uint8_t* ptr = (uint8_t*)packet->data;

    // get the byte at the current offset
    uint8_t data = ptr[(*offset)];

    // move the offset over 1 byte for the next read
    *offset = *offset + 1;

    return data;
}

int16_t ReadShort(ENetPacket* packet, size_t* offset)
{
    // make sure we have not gone past the end of the data we were sent
    if (*offset > packet->dataLength)
        return 0;

    // cast the data to a byte at the offset
    uint8_t* data = (uint8_t*)packet->data;
    data += (*offset);

    // move the offset over 2 bytes for the next read
    *offset = (*offset) + 2;

    // cast the data pointer to a short and return a copy in host byte order
    return ntohs(*(int16_t*)data);
}

int32_t ReadInt(ENetPacket* packet, size_t* offset)
{
    // make sure we have not gone past the end of the data we were sent
    if (*offset > packet->dataLength)
        return 0;

    // cast the data to a byte at the offset
    uint8_t* data = (uint8_t*)packet->data;
    data += (*offset);

    // move the offset over 2 bytes for the next read
    *offset = (*offset) + 4;

    // cast the data pointer to a short and return a copy in host byte order
    return ntohl(*(int32_t*)data);
}

float ReadFloat(ENetPacket* packet, size_t* offset)
{
    // make sure we have not gone past the end of the data we were sent
    if (*offset > packet->dataLength)
        return 0;

    // cast the data to a byte at the offset
    uint8_t* data = (uint8_t*)packet->data;
    data += (*offset);

    // move the offset over 2 bytes for the next read
    *offset = (*offset) + 4;

    // cast the data pointer to a short and return a copy in host byte order
    return ntohf(*(uint32_t*)data);
}

int64_t ReadInt64(ENetPacket* packet, size_t* offset)
{
    // make sure we have not gone past the end of the data we were sent
    if (*offset > packet->dataLength)
        return 0;

    // cast the data to a byte at the offset
    uint8_t* data = (uint8_t*)packet->data;
    data += (*offset);

    // move the offset over 2 bytes for the next read
    *offset = (*offset) + 8;

    // cast the data pointer to a short and return a copy in host byte order
    return ntohll(*(int64_t*)data);
}

bool WriteByte(uint8_t value, ENetPacket* packet, size_t* offset)
{
    // make sure the buffer in the packet is large enough to hold the data
    if (*offset + 1 > packet->dataLength)
        return false;

    // get a pointer at the offset in the data
    uint8_t* data = (uint8_t*)packet->data;
    data += (*offset);

    // write the data and update the offset
    *data = value;
    *offset = *offset + 1;
    return true;
}

bool WriteShort(int16_t value, ENetPacket* packet, size_t* offset)
{
    // make sure the buffer in the packet is large enough to hold the data
    if (*offset + 2 > packet->dataLength)
        return false;

    // get a pointer at the offset in the data
    uint8_t* data = (uint8_t*)packet->data;
    data += (*offset);

    // write the data and update the offset
    *((int16_t*)data) = htons(value);
    *offset = *offset + 2;
    return true;
}

bool WriteInt(int32_t value, ENetPacket* packet, size_t* offset)
{
    // make sure the buffer in the packet is large enough to hold the data
    if (*offset + 4 > packet->dataLength)
        return false;

    // get a pointer at the offset in the data
    uint8_t* data = (uint8_t*)packet->data;
    data += (*offset);

    // write the data and update the offset
    *((int32_t*)data) = htonl(value);
    *offset = *offset + 4;
    return true;
}

bool WriteFloat(float value, ENetPacket* packet, size_t* offset)
{
    // make sure the buffer in the packet is large enough to hold the data
    if (*offset + 4 > packet->dataLength)
        return false;

    // get a pointer at the offset in the data
    uint8_t* data = (uint8_t*)packet->data;
    data += (*offset);

    // write the data and update the offset
    *((uint32_t*)data) = htonf(value);
    *offset = *offset + 4;
    return true;
}

bool WriteInt64(int64_t value, ENetPacket* packet, size_t* offset)
{
    // make sure the buffer in the packet is large enough to hold the data
    if (*offset + 8 > packet->dataLength)
        return false;

    // get a pointer at the offset in the data
    uint8_t* data = (uint8_t*)packet->data;
    data += (*offset);

    // write the data and update the offset
    *((int64_t*)data) = htonll(value);
    *offset = *offset + 8;
    return true;
}
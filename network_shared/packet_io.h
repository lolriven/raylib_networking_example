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

#pragma once

#include <stdint.h>
#include "enet.h"

/// <summary>
/// Read one value out of a packet, from an offset, and update that offset to the next location to read from
/// </summary>
/// <param name="packet">The packet to read from</param>
/// <param name="offset">A pointer to an offset that is updated, this should be passed to other read functions so they read from the correct place</param>
/// <returns>The value read</returns>
uint8_t ReadByte(ENetPacket* packet, size_t* offset);
int16_t ReadShort(ENetPacket* packet, size_t* offset);
int32_t ReadInt(ENetPacket* packet, size_t* offset);
float ReadFloat(ENetPacket* packet, size_t* offset);
int64_t ReadInt64(ENetPacket* packet, size_t* offset);


/// <summary>
/// writes one byte out of a packet, from an offset, and update that offset to the next location to read from
/// </summary>
/// <param name="value">The value to be written from</param>
/// <param name="packet">The packet to write to</param>
/// <param name="offset">A pointer to an offset that is updated, this should be passed to other write functions so they write to the correct place</param>
/// <returns>True if the packet was large enough to hold the value</returns>
bool WriteByte(uint8_t value, ENetPacket* packet, size_t* offset);
bool WriteShort(int16_t value, ENetPacket* packet, size_t* offset);
bool WriteInt(int32_t value, ENetPacket* packet, size_t* offset);
bool WriteFloat(float value, ENetPacket* packet, size_t* offset);
bool WriteInt64(int64_t value, ENetPacket* packet, size_t* offset);

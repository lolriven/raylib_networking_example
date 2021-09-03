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

// server code

// include the network layer from enet (https://github.com/zpl-c/enet)
#define ENET_IMPLEMENTATION
#include "enet.h"

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

// max number of players
#define MAX_CLIENTS 8

// All the different commands that can be sent over the network
typedef enum
{
    // Server -> Client, You have been accepted. Contains the id for the client player to use
    AcceptPlayer = 1,

    // Server -> Client, Add a new player to your simulation, contains the ID of the player and a position
    AddPlayer = 2,

    // Server -> Client, Remove a player from your simulation, contains the ID of the player to remove
    RemovePlayer = 3,

    // Server -> Client, Update a player's position in the simulation, contains the ID of the player and a position
    UpdatePlayer = 4,

    // Client -> Server, Provide an updated location for the client's player, contains the postion to update
    UpdateInput = 5,
}NetworkCommands;


// the info we are tracking about each player in the game
typedef struct
{
    // is this player slot active
    bool Active;

    // have they sent us a valid position yet?
    bool ValidPosition;

    // the network connection they use
    ENetPeer* Peer;

    // the last known location in X and Y
    int16_t X;
    int16_t Y;
}PlayerInfo;


// The list of all possible players
// this is the server state of the game that represents the current game state
// this is what server code would check to see where all the players are and what they are doing
PlayerInfo Players[MAX_CLIENTS] = { 0 };

// Utility functions to read data out of a packet
// Optimally this would go into a library that was shared by the client and the server

/// <summary>
/// Read one byte out of a packet, from an offset, and update that offset to the next location to read from
/// </summary>
/// <param name="packet">The packet to read from</param>
/// <param name="offset">A pointer to an offset that is updated, this should be passed to other read functions so they read from the correct place</param>
/// <returns>The byte read</returns>
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

/// <summary>
/// Read a signed short from the network packet
/// Note that this assumes the packet is in the host's byte ordering
/// In reality read/write code should use ntohs and htons to convert from network byte order to host byte order, so both big endian and little endian machines can play together
/// </summary>
/// <param name="packet">The packet to read from<</param>
/// <param name="offset">A pointer to an offset that is updated, this should be passed to other read functions so they read from the correct place</param>
/// <returns>The signed short that is read</returns>
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

    // cast the data pointer to a short and return a copy
    return *(int16_t*)data;
}

// finds the player slot that goes with the player connection
// the peer has the void* ENetPeer::data that can be used to store arbitary application data
// but that involves managing structure pointers so it is kept out of this example
int GetPlayerId(ENetPeer* peer)
{
    // find the slot that matches the pointer
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (Players[i].Active && Players[i].Peer == peer)
            return i;
    }
    return -1;
}

// sends a packet over the network to every active player, except the one specified (usually the sender)
// senders know what they sent so you can choose to not send them data they already know.
// in a truly authoritive server you'd send back an acceptance message to all client input so they know it wasn't rejected.
void SendToAllBut(ENetPacket* packet, int exceptPlayerId)
{
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (!Players[i].Active || i == exceptPlayerId)
            continue;

        enet_peer_send(Players[i].Peer, 0, packet);
    }
}

// the main server loop
void main()
{
    printf("Startup\n");

    // set up networking
    if (enet_initialize() != 0)
        return;

    printf("Initialized\n");

    // network servers must 'listen' on an interface and a port
    // this code sets up enet to listen on any available interface and using our port
    // the client must use the same port as the server and know the address of the server
    ENetAddress address = { 0 };
    address.host = ENET_HOST_ANY;
    address.port = 4545;

    // create the server host
    ENetHost* server = enet_host_create(&address, MAX_CLIENTS, 1, 0, 0);

    if (server == NULL)
        return;

    printf("Created\n");

    // the server will run forever. If we wanted a way to stop it, we'd set run to false using some code
    bool run = true;

    while (run)
    {
        ENetEvent event = { 0 };

        // see if there are any inbound network events, wait up to 1000ms before returning.
        // if the server also did game logic, this timeout should be lowered
        if (enet_host_service(server, &event, 1000) > 0)
        {
            // see what kind of event we have
            switch (event.type)
            {

            // a new client is trying to connect
            case ENET_EVENT_TYPE_CONNECT:
            {
                printf("Player Connected\n");

                // find an empty slot, or disconnect them if we are full
                int playerId = 0;
                for (; playerId < MAX_CLIENTS; playerId++)
                {
                    if (!Players[playerId].Active)
                        break;
                }

                // we are full
                if (playerId == MAX_CLIENTS)
                {
                    // I said good day SIR!
                    enet_peer_disconnect(event.peer, 0);
                    break;
                }

                // player is good, don't give away the slot
                Players[playerId].Active = true;

                // but don't send out an update to everyone until they give us a good position
                Players[playerId].ValidPosition = false;
                Players[playerId].Peer = event.peer;

                // pack up a message to send back to the client to tell them they have been accepted as a player
                uint8_t buffer[2] = { 0 };
                buffer[0] = (uint8_t)AcceptPlayer;  // command for the client
                buffer[1] = (uint8_t)playerId;      // the player ID so they know who they are

                // copy the buffer into an enet packet (TODO : add write functions to go directly to a packet)
                ENetPacket* packet = enet_packet_create(buffer, 2, ENET_PACKET_FLAG_RELIABLE);
                // send the data to the user
                enet_peer_send(event.peer, 0, packet);

                // We have to tell the new client about all the other players that are already on the server
                // so send them an add message for all existing active players.
                for (int i = 0; i < MAX_CLIENTS; i++)
                {
                    // only people who are valid and not the new player
                    if (i == playerId || !Players[i].ValidPosition)
                        continue;

                    // pack up an add player message with the ID and the last known position
                    uint8_t addBuffer[6] = { 0 };
                    buffer[0] = (uint8_t)AddPlayer;
                    buffer[1] = (uint8_t)i;
                    *(int16_t*)(buffer + 2) = (int16_t)Players[i].X;
                    *(int16_t*)(buffer + 4) = (int16_t)Players[i].Y;

                    // Optimally we'd also send other info like name, color, and other static player info.

                    // copy and send the message
                    packet = enet_packet_create(buffer, 6, ENET_PACKET_FLAG_RELIABLE);
                    enet_peer_send(event.peer, 0, packet);

                    // NOTE enet_host_service will handle releasing send packets when the network system has finally sent them,
                    // you don't have to destroy them
                }
                break;
            }

            // someone sent us data
            case ENET_EVENT_TYPE_RECEIVE:
            {
                // find the player who sent the data
                // we don't need them to send us what ID they are, we know who they are by the peer
                // we want to trust the client as little as possible so that people can't cheat/hack
                // if we blindly accepted a player ID, a client could send you updates for someone else :(

                int playerId = GetPlayerId(event.peer);
                if (playerId == -1)
                {
                    // they are not one of our peeple, boot them
                    enet_peer_disconnect(event.peer, 0);
                    break;
                }

                // keep track of how far into the message we are
                size_t offset = 0;

                // read off the command the client wants us to process
                NetworkCommands command = ReadByte(event.packet, &offset);

                // we only accept one message from clients for now, so make sure this is what it is
                if (command == UpdateInput)
                {
                    // update the location data with the new info
                    Players[playerId].X = ReadShort(event.packet, &offset);
                    Players[playerId].Y = ReadShort(event.packet, &offset);

                    // lets tell everyone about this new location
                    NetworkCommands outboundCommand = UpdatePlayer;

                    // if they are new, send this update as an add player instead of an update
                    if (!Players[playerId].ValidPosition)
                        outboundCommand = AddPlayer;

                    // the player has sent us a position, they can be part of future regular updates
                    Players[playerId].ValidPosition = true;

                    // pack up the update message with command, player and position
                    uint8_t buffer[6] = { 0 };
                    buffer[0] = (uint8_t)outboundCommand;
                    buffer[1] = (uint8_t)playerId;
                    *(int16_t*)(buffer + 2) = (int16_t)Players[playerId].X;
                    *(int16_t*)(buffer + 4) = (int16_t)Players[playerId].Y;

                    // Copy and send the data to everyone but the player who sent it  (TODO : add write functions to go directly to a packet)
                    ENetPacket* packet = enet_packet_create(buffer, 6, ENET_PACKET_FLAG_RELIABLE);
                    SendToAllBut(packet, playerId);

                    // NOTE enet_host_service will handle releasing send packets when the network system has finally sent them,
                    // you don't have to destroy them
                }

                // tell enet that it can recycle the inbound packet
                enet_packet_destroy(event.packet);
                break;
            }
            case ENET_EVENT_TYPE_DISCONNECT_TIMEOUT:
            case ENET_EVENT_TYPE_DISCONNECT:
            {
                // a player was disconnected
                printf("Player Disconnected\n");
                
                // find them if they are a real player
                int playerId = GetPlayerId(event.peer);
                if (playerId == -1)
                    break;

                // mark them as inactive and clear the peer pointer
                Players[playerId].Active = false;
                Players[playerId].Peer = NULL;

                // Tell everyone that someone left
                uint8_t buffer[2] = { 0 };
                buffer[0] = (uint8_t)RemovePlayer;
                buffer[1] = (uint8_t)playerId;

                // Copy and send the data to everyone but the player who sent it  (TODO : add write functions to go directly to a packet)
                ENetPacket* packet = enet_packet_create(buffer, 2, ENET_PACKET_FLAG_RELIABLE);
                SendToAllBut(packet, -1);

                // NOTE enet_host_service will handle releasing send packets when the network system has finally sent them,
                   // you don't have to destroy them

                break;
            }

            case ENET_EVENT_TYPE_NONE:
                break;
            }
        }
    }

    // cleanup
    enet_host_destroy(server);
    enet_deinitialize();
}
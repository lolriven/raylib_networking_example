# Raylib Networking Example
A simple example of how to do networking using raylib and enet.

Raylib can be found at https://github.com/raysan5/raylib, but is a subnet module for this repository
Enet can be found at https://github.com/zpl-c/enet but is also included in this repository

## About
This is a simple client/server networking demo that allows up to 8 players to connect to a server and move boxes around a fixed size area. It is written in Pure C using Raylib for graphics and window setup and the ZPL-C version of enet for networking.

When a client is started it will attempt to connect to the server (on localhost by default). Once conncected it will spawn a player with a peset color that the client can move around with the arrow keys. Different colored player objects for other clients will be shown in the window, updating with the respective client. Each client maintains a local simulation state that represents the gameplay state that it is aware of. The server also maintains a state of the last known positon of each connected player.

This example is not a robust networking system and is only intended as a simple example of how to setup a basic client/server system. Game networking is a very complex subject with many subtle nuances. Different network setups are required for different kinds of games. For action games a good place to start is the information released by Valve regarding how the game Half Life handled networking.
https://developer.valvesoftware.com/wiki/Source_Multiplayer_Networking
https://developer.valvesoftware.com/wiki/Lag_compensation
https://developer.valvesoftware.com/wiki/Latency_Compensating_Methods_in_Client/Server_In-game_Protocol_Design_and_Optimization

## Building the Example
This example uses premake.

1) Download Premake 5 for your platform https://premake.github.io/download
2) Init raylib as a submodule (in the folder raylib)
3) Run premake for your platform (A batch file for Visual Studio 2019 is included)
4) Build the client and the server

## Code Overview

### Server
The server is entirely contained within the server.c file. It is very simple and just runs a loop looking for network events. When a player connects, disconnects or sends data, the server responds to the event, updates an internal player list, and sends out required updates to other players.

### Client
The client is broken up into 3 files
main.c
networking.h
networking.c

#### main.c
The main file is where the normal raylib window is setup, input is checked and the game is drawn. Every frame the input is checked, the player is updated and the field is drawn with all players on it.

Due to conflicts between raylib and windows.h, it is not possible to include networking in the same source files as raylib. For this reason the gameplay and networking systems are put into a seperate file and accessed via an interface header.

#### networking.h
This is the interface between the network gameplay system and the main game application. It exists to keep raylib and windows files seperate. It contains defintions of all the functions and constants that are needed by the main game to run the game. Because raymath.h does not conflict with windows.h, the networking.h file includes raymath in order to use raylib structures, such as Vector2

#### networking.c
This is the implementation file for the network gameplay system. It uses enet to create a client connection to the server and keep the local simulation up to date. It sends out the local player's position 20 times a second using a server tick clock. This prevents the network from being overloaded with updates with every drawn frame and different update rates for players with different frame rates.

## Network Commands
All network iformation is sent as commands. Commands are encoded into the network packet as a single byte, allowing up to 255 different commands. The command tells the receiving system what kind of data will be in the packet and what the requested action is.

## Packet Data
In this example network data is packaged up in the native format for the sending computer. This means that computers with different byte ordering (https://en.wikipedia.org/wiki/Endianness) can not communicate with each other. A real game would encode all data into Network Byte Order on send and decode on receive.



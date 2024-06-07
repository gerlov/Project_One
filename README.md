# Labyrinth of the Minotaur (SDL, SDL-net, C-lang) 

is a network-based multiplayer game in classic 2D maze style, where players navigate through a labyrinth to find the exit and escape before the Minotaur catches them. 

Developed by students from KTH Royal Institute of Technology as part of the course CM1008 VT24 Project Course in Application Development (60476).

Team: @gerlov, @Majwt, @BaronBalt, @CJPob, @juliazubko 

https://github.com/gerlov/Project_One  


https://github.com/juliazubko/Labyrinth-of-the-Minotaur/assets/102211232/4077d547-77d0-4cb4-adc8-15e58fc25c84



## 1. Features 

- **Dynamically Generated Mazes:** Each game round features a uniquely created maze layout.
- **Spawn Area:** The game utilizes an intelligent mechanism to strategically place humans and the hunter at different starting points on the map, balancing the game dynamics.
- **Randomly Assigned Roles:** Players are randomly assigned various roles that influence their tasks and objectives within the game. Humans should aim to escape the maze, and hunter focus on preventing escapes by capturing characters within a specific range.
- **Main Menu:**  Allows users to smoothly navigate through game settings and options, provide a server IP for connection, join a lobby/waiting area, control volume, and more.
- **Power-Ups:** The game board includes randomly placed power-ups that provide players with various advantages for a limited time:
    - Red Coin: Increases speed for 10 seconds.
    - Green Crystal: Renders the player invisible to others for 10 seconds.
    - Skull (Hunter-specific): Neutralizes all players who are not invisible at the moment it is activated (instant kill).
    - Map (Human-specific): Displays the entire maze and the player's position for 10 seconds while simultaneously making the player invisible.
- **Escape portal:** Allows non-hunter players to instantly win the game by teleporting out of the maze when they enter it.
- **Controls:** Players can navigate in the game using either the ASDW keys or the arrow keys. The menu is operable with the mouse. For hunters, pressing the spacebar allows eliminating humans within the "kill reach area."
  


## 2. Code Structure and System Architecture  

The game features a modular structure, where functions such as network communication and game logic are divided into separate modules. 

Each module is independent and focuses on specific tasks, which minimizes dependencies between modules.

**Lib Directory:** the directory containing all shared libraries that manage the logic for various game components such as character handling, collision detection, etc, along with associated resources.

**Assets Subfolder:**  stores game resources like images and sounds, organized into subdirectories based on their use.

**Inc Subolder:** header files where functions and data structures are declared.

**Src Subfolder:**  source code for shared libraries managing the game logic.

Additionally, there are separate directories for the **server** and **client** , each with its own Makefile to build the applications. 

**A main Makefile** at the root of the project structure coordinates the build process for both the client and server, simplifying and enabling the game to be launched from a single point.

![Skärmbild 2024-06-07 140819](https://github.com/juliazubko/Labyrinth-of-the-Minotaur/assets/102211232/3eaba715-25fb-4ffd-9dc4-2ea92cd8226d)    



## 3. System Requirements
The game can be compiled and run on Windows or MacOS.

The SDL2 library is required, including SDL2_image, SDL2_ttf, SDL2_net, and SDL2_mixer.

## 4. Network Configuration
The system is designed to let users enter a server address through a user interface. No other IP or port configuration is required by the user to play the game, as the client handles connections dynamically by automatically opening a UDP socket on a port assigned by the system. This allows users to start and play the game immediately without any manual network settings. It is also possible to run the server and several clients on the same machine. 

## 5. User Manual

### Compilation
Run `make` (macOS) or `mingw32-make.exe` (Windows) in the top folder’s Makefile to compile both the server and client simultaneously (this  executes the Makefiles within the “Server/” and “Client/” directories which contain commands to compile the files; instructions are also provided to compile the client and server separately by running make server and make client). 
Run `make clean`(macOS) or  `mingw32-make clean` (Windows) to clean both the server and client. 

### Starting the Game

**Server:**
The server is launched by running the file ./Server/Server.out. (Windows: `mingw32-make.exe runserver`). No window will display by default, but this can be changed by setting #define SHOW_WINDOW 1 in Server/src/main.c. With this setting, the server will render the labyrinth and the game data it receives from the clients. The server will terminate itself after all clients have exited.

**Client:**
The client is started by running the file ./Client/Client.out (Windows: `mingw32-make.exe runclient`). Upon launch, the game's main menu is displayed. To join a game, click on “Join/Start Game” and enter the IP address of the server. This connects the player to the server and displays them in a lobby. The first player to join is considered the “host”. To ready up, players press the space bar. If there are more than one client and all are ready, the host starts the game by pressing enter. 



## APPENDIX: Tutorial on the C, make and SDL basics. Prepared by KTH teaching staff.
**source: https://canvas.kth.se/courses/46290**

**To install C and Make (and msys2 for Windows):** https://github.com/Makerspace-KTH/c_programing_intro

**To install SDL (Note: msys2 is required for Windows):** https://github.com/Makerspace-KTH/sdl_hello

For Windows, if you prefer to use PowerShell (instead of msys2’s terminal):
You will need to set the PATH variable in Windows. Do this in the advanced system settings under environment variables. 
Select the PATH variable, edit it, and add the path to gcc.exe, mingw32-make.exe, etc. (e.g., C:\msys64\mingw64\bin).




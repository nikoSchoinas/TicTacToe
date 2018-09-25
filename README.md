# TicTacToe
The programs have developed for Windows operating systems through Visual Studio 2013. They use Douglas E. Comer’s APIs that are provided in the cnai-api folder. The program runs locally and stimulates a TCP communication between a client and a server.  

-TicTacToeS is an executable file that runs in cmd with the above syntax: TicTacToeS 2000 
You need to be at the same directory with the exe file. It’s the server that communicates with the 2 players above TCP. The source code is in TicTacToe.c file and there are some basic comments for its understanding. The server handles specific messages to ensure the right communication between the two entities (players). Messages such as: game’s board, Wrong, Fault, 0 (in case of a wrong move), TimeIsOver. Those messages are already known to TicTacToeC program and they indicate specific movements. The server notifies the player if a wrong move is occured and it returns the round at the beginning.

-TicTacToeC is an executable that runs in cmd with the syntax: TicTacToeC
It takes some information about IP and port from tictactoe.config file and then connects to the server. The commented code is at TicTacToeC.c file. Every player that executes TicTacToeC has 15 seconds to make a move. The program prints messages to console to inform the user about the game’s state.

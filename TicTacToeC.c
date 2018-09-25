/* TicTacToeC.c */

#include <stdlib.h>
#include <stdio.h>
#include <cnaiapi.h>
#include <time.h>
#include <string.h>

#define BUFFSIZE		256

int recvln(connection, char *, int);
int readln(char *, int);
static void DisplayBoard(char Board[3][3]);				/* This function displays TicTacToe board on screen*/
int CheckForWinner(char Board[3][3], int player);		/* This function checks for winner				   */

/*-----------------------------------------------------------------------
*
* Program: TicTacToeC
* Purpose: contact a TicTacToeServer that allows 2 clients to play TicTacToe game.
* Usage:   TicTacToeC
*
*-----------------------------------------------------------------------
*/
int main(int argc, char *argv[])
{
	computer	comp;							/* Variable for IP connection			*/
	connection	conn;							/* Variable for port connection			*/
	char buff[BUFFSIZE];						/* Array for receiving/sending data from/to TicTacToe server*/
	int len;									/* Lenght of received data				*/
	int i = 0;                                  /* Loop counter                         */
	int player = 0;                             /* Player number - 1 or 2               */
	int choice = 0;                             /* Player's choice						*/
	int row = 0;                                /* Row index for a square				*/
	int column = 0;                             /* Column index for a square			*/
	int winner = 0;                             /* The winning player                   */
	char board[3][3];							/* TicTacToe board                      */
	int playerID, enemyID;						/* Variables for player identification  */
	char IP[32];								/* This array stores server's IP address*/
	int port;									/* This variable stores connection's port*/
	FILE *fp;									/* Pointer to file						*/
	time_t start, stop;							/* Variables for counting time			*/		
	double diff;								/* Substraction of time variables       */
	boolean guard = TRUE;						/* A boolean variable					*/

	/*file reading.*/
	fp = fopen("tictactoe.conf", "r");
	if (fp == NULL) {
		fprintf(stderr, "Failed to open configuration file %s", "tictactoe.conf");
		exit(1);
	}
	while(!feof(fp)) {
		fscanf(fp, "%s%d", IP, &port);
	}

	/* convert the compname to binary form comp */
	comp = cname_to_comp(IP);
	if (comp == -1)
		exit(1);

	/* make a connection to the chatserver */
	conn = make_contact(comp, port);
	if (conn < 0)
		exit(1);
	
	/*Wait for second player to connect*/
	(void)recv(conn, buff, BUFFSIZE, 0);
	playerID = atoi(buff);
	enemyID = (playerID == 1) ? 2 : 1;
	if (playerID == 1)
		printf("Please Wait For the Second Player.\n");
	printf("playerID = %d\n", playerID);

	/*Client receives message "Connection with TicTacToeS Server Established" from server */
	(void)recv(conn, buff, BUFFSIZE, 0);
	printf("%s\n", buff);
	
	/*Client receives board from server*/
	(void)recv(conn, buff, BUFFSIZE, 0);
	strcpy(board, buff);
	DisplayBoard(board);


	for (i = 0; i < 9 && winner == 0; i++)
	{
		/*check for winner at the begging of every round */
		winner = CheckForWinner(board,player);

		/*player number in relation to 'i' */
		player = i % 2 + 1;
		if (winner != 0)
			break;
		

		if (player == playerID)
		{
			printf("Please enter the number of the square where you want to place your %c: ", (player == 1) ? 'X' : 'O');
			time(&start);
			scanf("%d", &choice);
			time(&stop);
			diff = difftime(stop, start);
			if (diff <= 15)			 /*player has 15 seconds to make his move  */
			{		 
				row = --choice / 3;  /* Get row index of tictactoe board square */
				column = choice % 3; /* Get column index of tictactoe board square */
			}
			else {
				printf("Time is Over\n");
				sprintf(buff, "TimeIsOver");
				send(conn, buff, BUFFSIZE, 0);/*sends message "OUT OF TIME" to server via buff array*/
				send_eof(conn);
				guard = FALSE;
				i = 10; /* helps to finish 'for' iteration*/
				//break;
			}
			if (guard)
			{
				sprintf(buff, "%d", ++choice);
				send(conn, buff, BUFFSIZE, 0); /* Send player's choice to server */
			}
		}
		else
		{
			printf("\nWaiting for enemy's move...\n");
			len = recv(conn, buff, BUFFSIZE, 0); /* Receive peer's selection */
			if (strcmp(buff, "TimeIsOver") == 0)
			{
				printf("Enemy's time is over.\n");
				(void)send_eof(conn);
				i = 10;
				guard = FALSE;
			}
			else
			{
				choice = atoi(buff);
				if (choice == 0)
					printf("Enemy made wrong choice\n");
				else
					printf("Enemy's choice: %d\n", choice);
			}
		}
		if (guard)
		{
			(void)recv(conn, buff, BUFFSIZE, 0);
			if (strcmp(buff, "Wrong") == 0) /*If player 1 make wrong move*/
			{
				i = i - 1;	/*player 1 plays again. Round is repeated*/
				(void)recv(conn, buff, BUFFSIZE, 0); /*Player receives correct board (before the mistake) from server*/
				strcpy(board, buff);
				if (player == playerID)
					printf("This move is not accepted!\n");
			}
			else if (strcmp(buff, "Fault") == 0)	/*If player 2 make wrong move*/
			{
				i = i - 2; /*player 1 plays again. Round is repeated*/
				(void)recv(conn, buff, BUFFSIZE, 0); /*Player receives correct board (before the mistake) from server*/
				strcpy(board, buff);
				if (player == playerID)
					printf("This move is not accepted!\n");
			}
			else
			{
				strcpy(board, buff);  
			}
			DisplayBoard(board);
			/*check for winner at the end of every round*/
			winner = CheckForWinner(board,player);

		}
	}

		if (guard)
		{
			/* Display result message */
			if (winner == 0)
			{
				printf("It's is a draw\n");
				(void)send_eof(conn);
			}
			else if (winner == playerID)
			{
				printf("You are the winner!!!\n");
				(void)send_eof(conn);
			}
			else {
				printf("GAME OVER. You loose.\n");
				(void)send_eof(conn);
			}

			(void)send_eof(conn);
		}
		return 0;
	}

static void DisplayBoard(char Board[3][3]){
	int row, column;
	printf("\n\n");
	for (row = 0; row<3; row++){
		if (row != 0) printf("---+---+---\n");
		for (column = 0; column<3; column++){
			if (column != 0) printf("|");
			printf(" %c ", Board[row][column]);
		}
		printf("\n");
	}
}
int CheckForWinner(char Board[3][3], int player){
	int winner,line;
	winner = 0;
	/*cross checking for matching*/
	if ((Board[0][0] == Board[1][1] && Board[0][0] == Board[2][2]) || (Board[0][2] == Board[1][1] && Board[0][2] == Board[2][0]))
		winner = player;
	else
		/*line and row checking for matching*/
		for (line = 0; line <= 2; line++)
		{
			if ((Board[line][0] == Board[line][1] && Board[line][0] == Board[line][2]) || (Board[0][line] == Board[1][line] && Board[0][line] == Board[2][line]))
				winner = player;
		}
	return(winner);
}


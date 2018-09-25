/* TicTacToeS.c */

#include <stdlib.h>
#include <stdio.h>
#include <cnaiapi.h>

#define BUFFSIZE		256


int recvln(connection, char *, int);
int readln(char *, int);
static void DisplayBoard(char Board[3][3]);  /* This function displays TicTacToe board on screen*/

/*-----------------------------------------------------------------------
*
* Program: TicTacToeS
* Purpose: wait for a connection from two clients & allow users to play TicTacToe
* Usage:   TicTacToeS 20000
*
*-----------------------------------------------------------------------
*/
int main(int argc, char *argv[])
{
	connection	conn1, conn2;		/* Connection variables.One for each client*/		
	int	len;						/* Lenght of received data				*/
	char buff[BUFFSIZE];			/* Array for receiving/sending data from/to TicTacToe clients*/
	char board[3][3] = {            /* Tic Tac Toe Board                    */
		{ '1', '2', '3' },          /* Initial values are reference numbers used to select a vacant square for a turn.*/
		{ '4', '5', '6' },          
		{ '7', '8', '9' }          
	};
	int firstPlayer = 1;			/* Variable for identify 1st player */
	int secondPlayer = 2;			/* Variable for identify 2nd player */
	int row, column;				/* Row or column index for a square */
	int choice;						/* Player's choice					*/		
	int i = 0;						/* Loop counter                     */
	boolean guard = TRUE;			/* A boolean variable		    	*/
	char correctBoard[3][3];		/* This Board stores the correct moves in case of a player make a wrong move*/

	if (argc != 2)
	{
		(void)fprintf(stderr, "usage: %s <appnum>\n", argv[0]);
		exit(1);
	}

	(void)printf("TicTacToe Server Waiting For First Player.\n");

	/* wait for a connection from first TicTacToe client */
	conn1 = await_contact((appnum)atoi(argv[1]));
	if (conn1 < 0)
		exit(1);

	/*Send the identification number to client 1 */
	sprintf(buff, "%d", firstPlayer);
	(void)send(conn1, buff, BUFFSIZE, 0);

	/* wait for a connection from second TicTacToe client */
	conn2 = await_contact((appnum)atoi(argv[1]));
	if (conn2 < 0)
		exit(1);
	
	/*Send the identification number to client 1 */
	sprintf(buff, "%d", secondPlayer);
	(void)send(conn2, buff, BUFFSIZE, 0);

	/*Send the above message to clients*/
	sprintf(buff, "Connection with TicTacToe Server Established.\n");
	(void)send(conn1, buff, BUFFSIZE, 0);
	(void)send(conn2, buff, BUFFSIZE, 0);

	/*TicTacToe board*/
	strcpy(buff, board);
	strcpy(correctBoard, board);
	(void)send(conn1, buff, BUFFSIZE, 0);
	(void)send(conn2, buff, BUFFSIZE, 0);

	
	while (i < 9)
	{
		if (len = recv(conn1, buff, BUFFSIZE, 0) > 0) {

			if (strcmp(buff, "TimeIsOver") == 0)   /* check if firstPlayer made his move on time*/
			{
				send_eof(conn1);
				(void)send(conn2, buff, BUFFSIZE, 0);
				send_eof(conn2);
				i = 10;
				guard = FALSE;
			}
			else
			{

				choice = atoi(buff);
				row = --choice / 3; /* Get row index of square */
				column = choice % 3; /* Get column index of square */
				if (choice >= 0 && choice < 9 && board[row][column] <= '9') /*check for wrong move*/
				{
					(void)send(conn2, buff, BUFFSIZE, 0);
					printf("firstPlayer chose square at row = %d column =%d\n", row, column);
					board[row][column] = 'X';/* Insert player symbol */
					strcpy(buff, board);
					(void)send(conn1, buff, BUFFSIZE, 0);
					(void)send(conn2, buff, BUFFSIZE, 0);
					DisplayBoard(board);
				}
				else					/*move was wrong*/
				{
					sprintf(buff, "0");
					(void)send(conn2, buff, BUFFSIZE, 0);
					sprintf(buff, "Wrong");
					(void)send(conn2, buff, BUFFSIZE, 0);
					(void)send(conn1, buff, BUFFSIZE, 0);
					strcpy(board, correctBoard);
					strcpy(buff, correctBoard);
					(void)send(conn2, buff, BUFFSIZE, 0);
					(void)send(conn1, buff, BUFFSIZE, 0);
					continue;
				}
			}
		}
		if (!guard)
			continue;
		
		if (len = recv(conn2, buff, BUFFSIZE, 0) > 0)
		{
			if (strcmp(buff, "TimeIsOver") == 0)				/* check if firstPlayer made his move on time*/
			{
				(void)send(conn1, buff, BUFFSIZE, 0);
				send_eof(conn1);
				send_eof(conn2);
				i = 10;
				guard = FALSE;
			}
			else
			{
				choice = atoi(buff);
				row = --choice / 3; /* Get row index of tictactoe board square */
				column = choice % 3; /*Get column index of tictactoe board square */
				if (choice >= 0 && choice < 9 && board[row][column] <= '9') /*check for wrong move*/
				{
					(void)send(conn1, buff, BUFFSIZE, 0);
					board[row][column] = 'O';							/* Insert player symbol */
					printf("secondPlayer chose square at row = %d column =%d\n", row, column);
					strcpy(buff, board);
					(void)send(conn1, buff, BUFFSIZE, 0);				/*send board to clients*/
					(void)send(conn2, buff, BUFFSIZE, 0);
					DisplayBoard(board);
					strcpy(correctBoard, board); /*save the last correct board after secondPlayer choice*/
				}
				else							/*move was wrong*/
				{
					sprintf(buff, "0");
					(void)send(conn1, buff, BUFFSIZE, 0);
					sprintf(buff, "Fault");
					(void)send(conn1, buff, BUFFSIZE, 0);
					(void)send(conn2, buff, BUFFSIZE, 0);
					i = i - 2;
					strcpy(board, correctBoard);
					strcpy(buff, correctBoard);
					(void)send(conn1, buff, BUFFSIZE, 0);
					(void)send(conn2, buff, BUFFSIZE, 0);
				}
			}
		}
		i = i + 2;
	}

	/* iteration ends when EOF found on stdin or chat connection */
	(void)printf("\nTicTacToeS Server Connection Closed.\n\n");
	if (guard) {
		send_eof(conn1);
		send_eof(conn2);
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

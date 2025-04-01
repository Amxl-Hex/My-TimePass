#include<stdio.h>
#include<stdlib.h>
#include<termios.h>
#include<time.h>
#include<unistd.h>

#define WIDTH 20
#define HEIGHT 20

int gameOver = 0;
int headX = WIDTH/2, headY = HEIGHT/2;
int tailX[100]={0}, tailY[100]={0}, tail_length = 0;
int fruitX = 0, fruitY = 0, score = 0;
enum direction {STOP=0, UP, DOWN, RIGHT, LEFT} dir;

//enable and disable raw mode
void enableRawMode()
{
	struct termios terminal;
	tcgetattr(STDIN_FILENO, &terminal);
	terminal.c_lflag &= ~(ICANON|ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &terminal);
}
void disableRawMode()
{
	struct termios terminal;
	tcgetattr(STDIN_FILENO, &terminal);
	terminal.c_lflag |= (ICANON|ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &terminal);
}

//draw the top and bottom border when called from draw() function.
void topbottom_border()
{
	for(int i=0; i<WIDTH+2; i++)
		printf("0");
	printf("\n");
}
//displaying the game logic
void draw()
{
	system("clear");
	topbottom_border(); //printing the top border
	//drawing the snake with updated values
	for(int i=0; i<HEIGHT; i++)
	{
		for(int j=0; j<WIDTH; j++)
		{
			if(j==0)
				printf("0"); //print the left border line by line
			if(i==headY && j==headX)
				printf("S"); //snake head
			else if(i==fruitY && j==fruitX)
				printf("F"); //fruit
			else
			{
				int tail_true = 0; //flag to know if there is snake segment in the corresponding (x,y) space
				for(int k=0; k<tail_length; k++)
				{
					if(tailX[k]==j && tailY[k]==i)
					{
						printf("o"); //snake segments
						tail_true = 1; //flag updated
					}
				}
				if(!tail_true) //if no snake segments then...
					printf(" "); //prints void space ie. there is no segment of snake nor any fruit present
			}
			if(j==WIDTH-1)
				printf("0"); //prints the right border line by line.
		}
		printf("\n");
	}
	topbottom_border(); //printing the bottom border
	printf("Score: %d\n", score);
}

//to get input from the user
int key_input()
{
	struct timeval tv = { 0L, 0L };
   	fd_set fds;
    	FD_ZERO(&fds);
    	FD_SET(STDIN_FILENO, &fds);
    	return select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv);
}
char get_key()
{
	char c;
	read(STDIN_FILENO, &c, 1);
	return c;
}

//deciding the direction depending on the input
void input()
{
	if(key_input())
	{
		switch(get_key())
		{
			case 'w': dir = UP; break;
			case 's': dir = DOWN; break;
			case 'a': dir = LEFT; break;
			case 'd': dir = RIGHT; break;
			case 'x': dir = gameOver = 1; break;
		}
	}
}

//game logic
void logic()
{
	//movement of the snake
	int prevX = tailX[0], prevY = tailY[0];
	int prev2X, prev2Y;
	tailX[0] = headX;
	tailY[0] = headY;
	for(int i=1; i<tail_length; i++)
	{
		prev2X = tailX[i];
		prev2Y = tailY[i];
		tailX[i] = prevX;
		tailY[i] = prevY;
		prevX = prev2X;
		prevY = prev2Y;
	}
	switch(dir)
	{
		case UP: headY--; break;
		case DOWN: headY++; break;
		case LEFT: headX--; break;
		case RIGHT: headX++; break;
		default: break;
	}

	//wrapping around the boundaries
	if(headX >= WIDTH)
		headX = 0;
	else if(headX < 0)
		headX = WIDTH - 1;
	if(headY >= HEIGHT)
		headY = 0;
	else if(headY < 0)
		headY = HEIGHT - 1;

	//check for if snake has eaten itself
	for(int i=0; i<tail_length; i++)
	{
		if(headX == tailX[i] && headY == tailY[i])
			gameOver = 1;
	}

	//fruit is eaten or not
	if(headX == fruitX && headY == fruitY)
	{
		score+=10;
		tail_length++;
		fruitX = rand()%WIDTH;
		fruitY = rand()%HEIGHT;
	}
}


int main()
{
	srand(time(0));
	enableRawMode();
	while(!gameOver)
	{
		draw();
		input();
		logic();
		usleep(100000);
	}
	disableRawMode();
	printf("\nGAME OVER!\nSCORE : %d\n", score);
	return 0;
}

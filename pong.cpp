#include <iostream>
#include <windows.h>
#include <conio.h>
#include <time.h>

#include<conio.h>
#include<dos.h>
#include<stdlib.h>
#include<string.h>

#define SCREEN_WIDTH 90
#define SCREEN_HEIGHT 26
#define WIN_WIDTH 70
#define MENU_WIDTH 20
#define GAP_SIZE 7
#define PIPE_DIF 45

using namespace std;

HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
COORD CursorPosition;

void gotoxy(int x, int y){
	COORD c;
	c.X = x;
	c.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
}

void setcursor(bool visible, DWORD size) 
{
	if(size == 0)
		size = 20;	
	
	CONSOLE_CURSOR_INFO lpCursor;	
	lpCursor.bVisible = visible;
	lpCursor.dwSize = size;
	SetConsoleCursorInfo(console,&lpCursor);
}

void hideCursor(){
	CONSOLE_CURSOR_INFO cursor;
	cursor.dwSize = 100;
	cursor.bVisible = false;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursor);
}

void drawBorder(){ 
	
	for(int i=0; i<SCREEN_WIDTH; i++){
		gotoxy(i,0); cout<<"_";
		gotoxy(i,SCREEN_HEIGHT); cout<<"_";
	}
	
	for(int i=0; i<SCREEN_HEIGHT; i++){
		gotoxy(0,i); cout<<"|";
		gotoxy(SCREEN_WIDTH,i); cout<<"|";
	}
	for(int i=0; i<SCREEN_HEIGHT; i++){
		gotoxy(WIN_WIDTH,i); cout<<"|";
	}
}

int life;
int score = 0;
int const screenHeight = 20;
int const screenWidth = 30;
int map[screenHeight][screenWidth];
char heart[8][18] = {"..#####...#####..",
					 ".#######.#######.",
					 ".###############.",
					 "..#############..",
					 "....#########....",
					 "......#####......",
					 "........#........"};
					 
bool decre_life;

void updateScore(){
	gotoxy(WIN_WIDTH + 7, 5);cout<<"Score: "<<score<<endl;
}

class Paddle{
	public:
		int x;
		int y;
		int speed;
		char dir;
		int delay;
		int count_delay;
		
	void draw(){
		for(int i = 0; i < 9; i++)
			map[y][x+i] = 1;
	}
	
	void move(){
		if(count_delay == delay){
			if(dir == 'L' && x-speed > 0){
				x -= speed;
			}else if(dir == 'R' && x+speed < screenWidth-9){
				x += speed;
			}
			count_delay = 0;
		}
		count_delay++;
		if(decre_life){
			dir = 'S';
			x = screenWidth/2-5;
			y = screenHeight - screenHeight/7-1;
			decre_life = false;
		}
	}
};

class Ball{
	public:
		int x;
		int y;
		int speed;
		int dir;
		
	void draw(){
		map[y][x] = 5;
	}
	
	void move(){
		if(dir == 0 && !collision(x-speed, y-speed)){
			x -= speed;
			y -= speed;
		}else if(dir == 1 && !collision(x+speed, y-speed)){
			x += speed;
			y -= speed;
		}else if(dir == 2 && !collision(x-speed, y+speed)){
			x -= speed;
			y += speed;
		}else if(dir == 3 && !collision(x+speed, y+speed)){
			x += speed;
			y += speed;
		}
	}
	
	bool collision(int fx, int fy){
		if(map[fy][x] == 8){
			decre_life = true;
			x = screenWidth/2-1;
			y = screenHeight - screenHeight/7-3;
			dir = 4;
			life--;
		}else if(map[fy][fx] != 0 || map[y][fx] != 0 || map[fy][x] != 0 || map[fy][fx] == 2 || map[y][fx] == 2 || map[fy][x] == 2){
			if(map[fy][fx] == 2){ heart[fy-2][fx-6] = '.'; score++;
			updateScore();}
			if(map[y][fx] == 2){ heart[y-2][fx-6] = '.'; score++;
			updateScore();}
			if(map[fy][x] == 2){ heart[fy-2][x-6] = '.'; score++;
			updateScore();}
			
			if(map[y][fx] != 0) bounce(fx,y);
			else if(map[fy][x] != 0) bounce(x,fy);
			else if(map[fy][fx] != 0) bounce(fx,fy);

			return true;
		}
		
		return false;
	}
	
	void bounce(int fx, int fy){
		if(dir == 0){
			if(fx < x) dir = 1;
			else if(fy < y) dir = 2;
			else if(fx < x && fy < y) dir = 0;
		}else if(dir == 1){
			if(fx > x) dir = 0;
			else if(fy < y) dir = 3;
			else if(fx > x && fy < y) dir = 1;
		}else if(dir == 2){
			if(fx < x) dir = 3;
			else if(fy > y) dir = 0;
			else if(fx < x && fy > y) dir = 2;
		}else if(dir == 3){
			if(fx > x) dir = 2;
			else if(fy > y) dir = 1;
			else if(fx > x && fy > y) dir = 3;
		}
	}	
};

void brick(){
	for(int i = 0; i < 7; i++){
		for(int j = 0; j < 17; j++){
			if(heart[i][j] == '#') map[i+2][j+6] = 2;
		}
	}
}

Paddle paddle;
Ball ball;

void setup(){
	srand(time(NULL));
	decre_life = false;
	life = 5;
	paddle.x = screenWidth/2-5;
	paddle.y = screenHeight - screenHeight/7-1;
	paddle.speed = 1;
	paddle.delay = 1;
	ball.x = screenWidth/2;
	ball.y = screenHeight - screenHeight/7-2;
	ball.speed = 1;
	ball.dir = rand()%4;
}

void wall(){
	for(int i = 0; i < screenHeight; i++){
		for(int j = 0; j < screenWidth; j++){
			if(j == 0 || j == screenWidth-1) map[i][j] = 9;
			else if(i == 0) map[i][j] = 7;
			else if(i == screenHeight-1) map[i][j] = 8;
			else map[i][j] = 0;
		}
	}
}

void layout(){
	wall();
	paddle.draw();
	ball.draw();
	brick();
}

void display(){
	gotoxy(2,1); cout << "LIFE: " << life;
	
	// 9 = side wall
	// 7 = top wall
	// 8 = score wall
	// 1 = paddle
	// 5 = ball
	// 2 = bricks
	// 0 = blanks
	
	for(int i = 0; i < screenHeight; i++){
		for(int j = 0; j < screenWidth; j++){
			gotoxy(j+2, i+3);
			if(map[i][j] == 9) cout << char(219);
			if(map[i][j] == 1) cout << char(219);
			if(map[i][j] == 2) cout << char(233);
			if(map[i][j] == 7) cout << char(219);
			if(map[i][j] == 8) cout << char(240);
			if(map[i][j] == 5) cout << char(254);
			if(map[i][j] == 0) cout << char(32);
		}
	}
}

void input(){
	if(kbhit()){
		switch(getch()){
			case 75:
				paddle.dir = 'L';
				break;
			case 77:
				paddle.dir = 'R';
				break;
		}
		if(ball.dir == 4) ball.dir = rand()%2;
	}
}

void movements(){
	paddle.move();
	ball.move();
}

void gameOver(){
	system("cls");
	cout<<endl;
	cout<<"\t\t--------------------------"<<endl;
	cout<<"\t\t-------- Game Over -------"<<endl;
	cout<<"\t\t--------------------------"<<endl<<endl;
	cout<<"\t\tYour score is: " << score <<endl<<endl;
	cout<<"\t\tPress any key to go back to menu.";
	getch();
}

void startGame(){
	
	system("cls"); 
	drawBorder();
	updateScore();
	
	score = 0;
	
	gotoxy(WIN_WIDTH + 5, 2);cout<<"Master Blaster";
	gotoxy(WIN_WIDTH + 6, 4);cout<<"----------";
	gotoxy(WIN_WIDTH + 6, 6);cout<<"----------";
	gotoxy(WIN_WIDTH + 7, 12);cout<<"Control ";
	gotoxy(WIN_WIDTH + 7, 13);cout<<"-------- ";
	gotoxy(WIN_WIDTH + 2, 14);cout<<"    Arrow keys";
	
	gotoxy(10, 5);cout<<"Press any key to start";
	getch();
	gotoxy(10, 5);cout<<"                      ";
	
	hideCursor();
	setup();
	while(life > 0){
		display();
		layout();
		input();
		movements();
	}
	gameOver();
}

void instructions(){
	
	system("cls");
	cout<<"Instructions";
	cout<<"\n----------------";
	cout<<"\n Use arrow keys - left and right - to move the platform";
	cout<<"\n\nPress any key to go back to menu";
	getch();
}


int main(){	
	setcursor(0,0); 
	srand( (unsigned)time(NULL)); 
	do{
		system("cls");
		gotoxy(10,5); cout<<" -------------------------- "; 
		gotoxy(10,6); cout<<" |     Master Blaster     | "; 
		gotoxy(10,7); cout<<" -------------------------- ";
		gotoxy(10,9); cout<<"1. Start Game";
		gotoxy(10,10); cout<<"2. Instructions";	 
		gotoxy(10,11); cout<<"3. Quit";
		gotoxy(10,13); cout<<"Select option: ";
		char op = getche();
		
		if( op=='1') startGame();
		else if( op=='2') instructions();
		else if( op=='3') exit(0);
		
	}while(1);
	
	return 0;
	
	
}

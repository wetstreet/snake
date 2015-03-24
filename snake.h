#include <windows.h>
#include <mmsystem.h>
#include <stdio.h>
#include <time.h>
#include <conio.h>

#define RIGHT 0
#define DOWN 1
#define LEFT 2
#define UP 3
#define GAME_MENU 0
#define GAME_INIT 1
#define GAME_PRINT 2
#define GAME_RUN 3
#define GAME_OVER 4
#define SPEED_NORMAL 200
#define SPEED_FAST 150
#define SPEED_VERY_FAST 100

struct node{
	int x;
	int y;
	struct node *next;
};
struct node snake[200];

HANDLE hwnd;
FILE *fp;
int dir;
int prev_dir;
int len;
int delay;
int alive;
int peax,peay;
int bean_x,bean_y;
int score;
int record;
int eating;
int restart=1;
int GAME_STATE=GAME_MENU;
int pea_exist;
int bean_exist;
int bean_active;
int through_wall;
int going_through;
char choice;

void Gotoxy(int x,int y);
void InitGame();
void PrintWall();
void PrintSnakeHead();
void PrintSnakeBody();
void RunGame();
void MoveSnake();
void AddSnake();
void CleanSnake();
void PrintPea();
void PrintBean();
void PrintInfo();
void SetSize();
void LoadConfig();
void CheckRecord();
void HideCursor();
void MyShowCursor();
void SetColor(int i);
void Redraw(int c);
void GetDir();
void GetStickInput();
void DeleteSnake(int n);
void CheckCollision();
void GameOver();
void GameMenu();
int GetRand(int min,int max);

//----------------------------------------------------------------------

void gotoxy(int x,int y)
{
	COORD loc={x,y};
	SetConsoleCursorPosition(hwnd,loc);
}

void SetSize()
{
	system("mode con cols=62 lines=30");
}

int GetRand(int min,int max)
{
	srand(time(NULL));
	return min+rand()%(max-min+1);
}

void LoadConfig()
{
	if((fp=fopen("C:\\Windows\\Temp\\snake.log","r+"))==NULL)
	{
		fp=fopen("C:\\Windows\\Temp\\snake.log","w+");
		fputc('0',fp);
	}
	else
	{
		fscanf(fp,"%d",&record);
	}
	fclose(fp);
	gotoxy(48,3);
	SetColor(record);
	printf("Record:%3d",record);
	PrintInfo();
}

void PrintInfo()
{
	SetColor(score);
	gotoxy(48,6);
	printf("Score:%3d",score);
	SetConsoleTextAttribute(hwnd,15);
	gotoxy(48,9);
	printf("Length:%2d",len);
	//游戏异常退出时使游戏画面不受影响
	gotoxy(0,24);
}

void PrintPea()
{
	int i,fake;
	if(!pea_exist)
	{
		fake=0;
		peax=GetRand(1,20)*2;
		peay=GetRand(1,20);
		for(i=0;i<len;i++)
			if(peax==snake[i].x && peay==snake[i].y)
				fake=1;
		if(peax==bean_x && peay==bean_y)
			fake=1;
		if(!fake)
		{
			pea_exist=1;
			gotoxy(peax,peay);
			SetConsoleTextAttribute(hwnd,15);
			printf("◎");
		}
	}
}

void PrintBean()
{
	int i,fake;
	if(bean_active)
	{
		if(!bean_exist)
		{
			fake=0;
			bean_x=GetRand(1,20)*2;
			bean_y=GetRand(1,20);
			for(i=0;i<len;i++)
				if(bean_x==snake[i].x && bean_y==snake[i].y)
					fake=1;
			if(peax==bean_x && peay==bean_y)
				fake=1;
			if(!fake)
			{
				bean_exist=1;
				bean_active=0;
				gotoxy(bean_x,bean_y);
				SetConsoleTextAttribute(hwnd,15);
				printf("¤");
			}
		}
	}
}

void PrintWall()
{
	int i;
	SetConsoleTextAttribute(hwnd,240);
    gotoxy(0,0);
	for(i=0;i<22;i++)printf("  ");
	for(i=0;i<22;i++)
	{
        gotoxy(0,i);
        printf("  ");
	    gotoxy(42,i);
        printf("  ");
	}
    gotoxy(0,21);
	for(i=0;i<22;i++)printf("  ");
	//使边界更平滑
	SetConsoleTextAttribute(hwnd,15);
	for(i=1;i<21;i++)
	{
        gotoxy(2,i);
        printf("  ");
	    gotoxy(40,i);
        printf("  ");
	}
}

void CheckRecord()
{
	if(score>record)
	{
		gotoxy(48,18);
		fclose(fp);
		fp=fopen("C:\\Windows\\Temp\\snake.log","w+");
		fprintf(fp,"%d",score);
		fclose(fp);
		record=score;
		gotoxy(48,3);
		SetColor(record);
		printf("Record:%3d",score);
		gotoxy(48,12);
		SetConsoleTextAttribute(hwnd,15);
		printf("New Record!");
	}
}

void InitGame()
{
		alive=1;
		len=1;
		restart=1;
		score=0;
		dir=0;
		pea_exist=0;
		bean_exist=0;
		snake[0].x=11*2;
		snake[0].y=11;
		snake[0].next=NULL;
		system("cls");
		LoadConfig();
		HideCursor();
}

void HideCursor()
{
	CONSOLE_CURSOR_INFO cursor_info={1,0};
	SetConsoleCursorInfo(hwnd,&cursor_info);
}

void MyShowCursor()
{
	CONSOLE_CURSOR_INFO cursor_info={1,1};
	SetConsoleCursorInfo(hwnd,&cursor_info);
}

void SetColor(int i)
{
	if(i>=0 && i<10)
		SetConsoleTextAttribute(hwnd,15);
	else if(i>=10 && i<100)
		SetConsoleTextAttribute(hwnd,FOREGROUND_INTENSITY|FOREGROUND_GREEN);
	else if(i>=100 && i<1000)
		SetConsoleTextAttribute(hwnd,FOREGROUND_INTENSITY|FOREGROUND_BLUE);
	else if(i>=1000 && i<3500)
		SetConsoleTextAttribute(hwnd,FOREGROUND_INTENSITY|FOREGROUND_RED);
	else if(i>=3500)
		SetConsoleTextAttribute(hwnd,FOREGROUND_INTENSITY|FOREGROUND_RED|FOREGROUND_GREEN);
}

void GetDir()
{
	if(kbhit())
	{
		getch();
		switch(getch())
		{
		case 72:
			if(dir!=DOWN)dir=UP;
			break;
		case 75:
			if(dir!=RIGHT)dir=LEFT;
			break;
		case 77:
			if(dir!=LEFT)dir=RIGHT;
			break;
		case 80:
			if(dir!=UP)dir=DOWN;
			break;
		}
	}
}

void DeleteSnake(int n)
{
	int i;
	for(i=0;i<n;i++)
	{
		gotoxy(snake[len-1].x,snake[len-1].y);
		SetConsoleTextAttribute(hwnd,FOREGROUND_INTENSITY);
		printf("  ");
		snake[len-2].next=NULL;
		len--;
	}
}
//检测是否吃到豆子或特殊豆子
void CheckCollision()
{
	if(snake[0].x==peax && snake[0].y==peay && pea_exist==1)
	{
		score+=len;
		CheckRecord();
		AddSnake();
		pea_exist=0;
		if(len>30)
			if(GetRand(1,100)<10)
				bean_active=1;
	}
	if(snake[0].x==bean_x && snake[0].y==bean_y && bean_exist==1)
	{
		DeleteSnake(10);
		bean_exist=0;
		bean_active=0;
	}
}

void GameOver()
{
	gotoxy(48,15);
	printf("Game Over!");
	gotoxy(0,24);
	//读取输入缓冲区内残留的一个字符以正确完成输入
	getchar();
	printf("Enter 'r' to restart:\n");
	MyShowCursor();
	scanf("%c",&choice);
	if(choice=='r' || choice=='R')
		restart=1;
	else
		restart=0;
}

void GameMenu()
{
	int i;
	system("cls");
	printf("enter 1 to choose normal speed\n");
	printf("enter 2 to choose fast speed\n");
	printf("enter 3 to choose very fast speed\n");
	scanf("%d",&i);
	if(i==1)
		delay=SPEED_NORMAL;
	else if(i==2)
		delay=SPEED_FAST;
	else if(i==3)
		delay=SPEED_VERY_FAST;
	printf("enter 1 to choose normal mode\n");
	printf("enter 2 to choose crossing-wall mode\n");
	scanf("%d",&i);
	if(i==1)
		through_wall=0;
	else if(i==2)
		through_wall=1;
}

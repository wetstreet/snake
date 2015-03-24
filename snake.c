#include "snake.h"

int main()
{
	SetConsoleTitleA("贪吃蛇");
	SetSize();
	hwnd=GetStdHandle(STD_OUTPUT_HANDLE);
	//游戏主循环
	do
	{
		switch(GAME_STATE)
		{
			case GAME_MENU:
				GameMenu();
				GAME_STATE=GAME_INIT;
				break;
			case GAME_INIT:
				InitGame();
				GAME_STATE=GAME_PRINT;
				break;
			case GAME_PRINT:
				PrintWall();
				PrintSnakeHead();
				PrintPea();
				GAME_STATE=GAME_RUN;
				break;
			case GAME_RUN:
				RunGame();
				GAME_STATE=GAME_OVER;
				break;
			case GAME_OVER:
				GameOver();
				GAME_STATE=GAME_MENU;
				break;
		}
	}while(restart);
}
//游戏主要运行逻辑部分
void RunGame()
{
	while(alive)
	{
		Sleep(delay);
		GetDir();
		CleanSnake();
		MoveSnake();
		PrintSnakeHead();
		PrintSnakeBody();
		PrintPea();
		PrintBean();
		CheckCollision();
		PrintInfo();
	}
}
//把蛇尾从屏幕上清除
void CleanSnake()
{
	int i;
	//若蛇正在进食则不清除蛇尾，相当于增加一格蛇尾
	if(eating==0)
	{
		//非穿墙模式时，若蛇将要撞墙则不清除蛇尾
		if(through_wall==0)
		{
			switch(dir)
			{
			case UP:
				if(snake[0].y-1==0)
					return;
			case LEFT:
				if(snake[0].x-2==0)
					return;
			case RIGHT:
				if(snake[0].x+2==44)
					return;
			case DOWN:
				if(snake[0].y+1==22)
					return;
			}
		}
		//若蛇将要撞到自己身上也不清除蛇尾
		switch(dir)
		{
		case UP:
			for(i=1;i<len-1;i++)
				if(snake[i].x==snake[0].x&&snake[i].y==snake[0].y-1)
				{
					alive=0;
					return;
				}
			break;
		case LEFT:
			for(i=1;i<len-1;i++)
				if(snake[i].x==snake[0].x-2&&snake[i].y==snake[0].y)
				{
					alive=0;
					return;
				}
			break;
		case DOWN:
			for(i=1;i<len-1;i++)
				if(snake[i].x==snake[0].x&&snake[i].y==snake[0].y+1)
				{
					alive=0;
					return;
				}
			break;
		case RIGHT:
			for(i=1;i<len-1;i++)
				if(snake[i].x==snake[0].x+2&&snake[i].y==snake[0].y)
				{
					alive=0;
					return;
				}
			break;
		}
		SetConsoleTextAttribute(hwnd,15);
		gotoxy(snake[len-1].x,snake[len-1].y);
		printf(" ");
	}
}

void PrintSnakeHead()
{
	if(alive)
	{
		SetConsoleTextAttribute(hwnd,15);
		SetColor(score);
		switch(dir)
		{
		case UP:
			gotoxy(snake[0].x,snake[0].y);
			printf("↑");
			break;
		case LEFT:
			gotoxy(snake[0].x,snake[0].y);
			printf("←");
			break;
		case DOWN:
			gotoxy(snake[0].x,snake[0].y);
			printf("↓");
			break;
		case RIGHT:
			gotoxy(snake[0].x,snake[0].y);
			printf("→");
			break;
		}
	}
}

void PrintSnakeBody()
{
	if(alive)
	{
		if(snake[0].next!=NULL)
		{
			switch(dir)
			{
			case UP:
				switch(prev_dir)
				{
				case UP:
					gotoxy(snake[1].x,snake[1].y);
					printf("║");
					break;
				case LEFT:
					gotoxy(snake[1].x,snake[1].y);
					printf("╚");
					break;
				case RIGHT:
					gotoxy(snake[1].x,snake[1].y);
					printf("╝");
					break;
				}
				break;
			case LEFT:
				switch(prev_dir)
				{
				case UP:
					gotoxy(snake[1].x,snake[1].y);
					printf("╗");
					break;
				case LEFT:
					gotoxy(snake[1].x,snake[1].y);
					printf("═");
					break;
				case DOWN:
					gotoxy(snake[1].x,snake[1].y);
					printf("╝");
					break;
				}
				break;
			case DOWN:
				switch(prev_dir)
				{
				case LEFT:
					gotoxy(snake[1].x,snake[1].y);
					printf("╔");
					break;
				case DOWN:
					gotoxy(snake[1].x,snake[1].y);
					printf("║");
					break;
				case RIGHT:
					gotoxy(snake[1].x,snake[1].y);
					printf("╗");
					break;
				}
				break;
			case RIGHT:
				switch(prev_dir)
				{
				case UP:
					gotoxy(snake[1].x,snake[1].y);
					printf("╔");
					break;
				case DOWN:
					gotoxy(snake[1].x,snake[1].y);
					printf("╚");
					break;
				case RIGHT:
					gotoxy(snake[1].x,snake[1].y);
					printf("═");
					break;
				}
				break;
			}
		}
		prev_dir=dir;
	}
}

void AddSnake()
{
	snake[len-1].next=&snake[len];
	snake[len].x=snake[len-1].x;
	snake[len].y=snake[len-1].y;
	eating=1;
	snake[len].next=NULL;
	len++;
}

void MoveSnake()
{
	int i;
	if(alive)
	{
		switch(dir)
		{
		case UP:
			//将每格蛇身的坐标等于它上一格的坐标，如果蛇在进食则最后一格坐标不动
			for(i=len-eating;i>0;i--)
			{
				snake[i].x=snake[i-1].x;
				snake[i].y=snake[i-1].y;
				eating=0;
			}
			if(snake[0].y-1==0)
			{
				if(through_wall)
				{
					snake[0].y=20;
					going_through=1;
				}
				else
				{
				alive=0;
				break;
				}
			}
			if(!going_through)
				snake[0].y--;
			else
				going_through=0;
			break;
		case LEFT:
			for(i=len-eating;i>0;i--)
			{
				snake[i].x=snake[i-1].x;
				snake[i].y=snake[i-1].y;
				eating=0;
			}
			if(snake[0].x-2==0)
			{
				if(through_wall)
				{
					snake[0].x=40;
					going_through=1;
				}
				else
				{
				alive=0;
				break;
				}
			}
			if(!going_through)
				snake[0].x-=2;
			else
				going_through=0;
			break;
		case RIGHT:
			for(i=len-eating;i>0;i--)
			{
				snake[i].x=snake[i-1].x;
				snake[i].y=snake[i-1].y;
				eating=0;
			}
			if(snake[0].x+2==42)
			{
				if(through_wall)
				{
					snake[0].x=2;
					going_through=1;
				}
				else
				{
				alive=0;
				break;
				}
			}
			if(!going_through)
				snake[0].x+=2;
			else
				going_through=0;
			break;
		case DOWN:
			for(i=len-eating;i>0;i--)
			{
				snake[i].x=snake[i-1].x;
				snake[i].y=snake[i-1].y;
				eating=0;
			}
			if(snake[0].y+1==21)
			{
				if(through_wall)
				{
					snake[0].y=1;
					going_through=1;
				}
				else
				{
				alive=0;
				break;
				}
			}
			if(!going_through)
				snake[0].y++;
			else
				going_through=0;
			break;
		}
	}
}

#include <graphics.h>
#include <iostream>
#include <stdlib.h>
#include <string>
#include <conio.h>

using namespace std;

//控制键 上、下、左、右 控制方向，'q' 退出
#define KEY_UP 'w'   //char 'a'
#define KEY_LEFT 'a'
#define KEY_RIGHT 'd'
#define KEY_DOWN 's'
#define KEY_QUIT 'q'
struct _POS
{
	int x;
	int y;
};

#define isValid(next_pos)   ((next_pos.x>=0) && (next_pos.x<LINE) && (next_pos.y>=0) && (next_pos.y <COLUMN))

enum _DIRECTION
{
	UP,
    DOWN,
	LEFT,
	RIGHT,
};

enum _PROPS
{
	WALL,
	FLOOR,
	BOX_DES,
	MAN,
	BOX,
	HIT,
};


#define RATIO 61
#define SCREEN_WIDTH 960
#define SCREEN_HEIGHT 768
#define LINE 9
#define COLUMN 12
#define START_X 100
#define START_Y 150

IMAGE images[6];
struct _POS man ={5,5};
IMAGE bg_img;
/*游戏地图*/
int map[LINE][COLUMN] = {
{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
{ 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0 },
{ 0, 1, 4, 1, 0, 2, 1, 0, 2, 1, 0, 0 },
{ 0, 1, 0, 1, 0, 1, 0, 0, 1, 1, 1, 0 },
{ 0, 1, 0, 2, 0, 1, 1, 4, 1, 1, 1, 0 },
{ 0, 1, 1, 1, 0, 3, 1, 1, 1, 4, 1, 0 },
{ 0, 1, 2, 1, 1, 4, 1, 1, 1, 1, 1, 0 },
{ 0, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0 },
{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
};


void map_init(void);

void gameControl(enum _DIRECTION direct);
bool isGameOver();
void gameOverScene(IMAGE *bg);
int main(void){
	bool quit = false;
	map_init();       //初始化地图

	do {
		if(_kbhit()){ //玩家按键
			char ch = _getch();
			if(ch == KEY_UP){
				gameControl(UP);
			}else if(ch == KEY_DOWN){
				gameControl(DOWN);
			}else if(ch == KEY_LEFT){
				gameControl(LEFT);
			}else if(ch == KEY_RIGHT){
				gameControl(RIGHT);
			}else if(ch == KEY_QUIT){
				quit = true;
			}
			if(isGameOver()){
				Sleep(1000);
				gameOverScene(&bg_img);
				quit = true;
			}
		}
		Sleep(100);
	}while(quit==false); //!quit
	system("pause");
	closegraph();
	return 0;
}


void map_init(void)
{
	initgraph(SCREEN_WIDTH, SCREEN_HEIGHT);
	loadimage(&bg_img, _T("blackground.bmp"), SCREEN_WIDTH,
	SCREEN_HEIGHT, true);
	putimage(0, 0, &bg_img);
	//加载道具图标
	loadimage(&images[WALL],    _T("wall.bmp"), RATIO, RATIO, true);
	loadimage(&images[FLOOR],   _T("floor.bmp"), RATIO, RATIO, true);
	loadimage(&images[BOX_DES], _T("des.bmp"), RATIO, RATIO, true);
	loadimage(&images[MAN],     _T("man.bmp"), RATIO, RATIO, true);
	loadimage(&images[BOX],     _T("box.bmp"), RATIO, RATIO, true);
	loadimage(&images[HIT],     _T("box.bmp"), RATIO, RATIO, true);
	for(int i = 0; i< LINE; i++){
		for(int j = 0; j < COLUMN; j++){
		putimage(START_X+j*RATIO, START_Y+i*RATIO,
		&images[map[i][j]]);
		}
	}
}

void changeMap(struct _POS *next, enum _PROPS pos)
{
	//if( map[next->x][next->y] != BOX_DES )
		map[next->x][next->y] = pos;
	putimage(START_X + next->y * RATIO, START_Y + next->x * RATIO, &images[pos]);
}

/**********************************************
*实现游戏四个方向（上、下、左、右）的控制
* 输入：
* direct - 人前进方向
* 输出： 无
**********************************************/
void gameControl(enum _DIRECTION direct)
{
	struct _POS next_pos = man;
	struct _POS next_next_pos = man;
	switch(direct){
		case UP:
		next_pos.x--;
		next_next_pos.x-=2;
		break;
		case DOWN:
		next_pos.x++;
		next_next_pos.x+=2;
		break;
		case LEFT:
		next_pos.y--;
		next_next_pos.y-=2;
		break;
		case RIGHT:
		next_pos.y++;
		next_next_pos.y+=2;
		break;
	}
	//宏展开 next_pos.x>=0 && next_pos.x<LINE && next_pos.y>=0 &&
	//next_pos.y <COLUMN
	if( isValid(next_pos) && map[next_pos.x][next_pos.y] == FLOOR ){//人的前方是地板
		if(map[man.x][man.y]==BOX_DES){
			changeMap(&next_pos, MAN); //小人前进一格
			changeMap(&man, BOX_DES);
			man = next_pos;			
		}else{
			changeMap(&next_pos, MAN); //小人前进一格
			changeMap(&man, FLOOR);
			man = next_pos;		
		}

	}else if(isValid(next_next_pos) && map[next_pos.x][next_pos.y] == BOX){
		//人的前方是箱子
	    //两种情况，箱子前面是地板或者是箱子目的地
		if( map[next_next_pos.x][next_next_pos.y] == FLOOR){
			changeMap(&next_next_pos, BOX);
			changeMap(&next_pos, MAN); //小人前进一格
			changeMap(&man, FLOOR);
			man = next_pos;
		}else if(map[next_next_pos.x][next_next_pos.y] == BOX_DES){
			changeMap(&next_next_pos, HIT);
			changeMap(&next_pos, MAN); //小人前进一格
			changeMap(&man, FLOOR);
			man = next_pos;
		}
	}else if( isValid(next_pos) && map[next_pos.x][next_pos.y] == BOX_DES ){
		putimage(START_X +next_pos.y * RATIO, START_Y + next_pos.x * RATIO, &images[MAN]);
		changeMap(&man, MAN);
		changeMap(&man, FLOOR);
		man = next_pos;
	}
}

/**********************************************
*判断游戏是否结束，如果不存在任何一个箱子目的地，就代表游戏结束
*输入： 无
*返回值：
* true - 游戏结束 false - 游戏继续
**********************************************/
bool isGameOver(){
	for(int i = 0; i< LINE; i++){
		for(int j = 0; j < COLUMN; j++){
		if(map[i][j] == BOX_DES) return false;
		}
	}
	return true;
}

/**********************************************
*游戏结束场景，在玩家通关后显示
*输入：
* bg - 背景图片变量的指针
*返回值： 无
**********************************************/
void gameOverScene(IMAGE *bg){
	putimage(0, 0, bg);
	settextcolor(WHITE);
	RECT rec = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
	settextstyle(20, 0, _T("宋体"));
	drawtext(_T("恭喜您~ \n 您终于成为了一个合格的搬箱子老司机！"),
	&rec, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
}
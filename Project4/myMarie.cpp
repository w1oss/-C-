#include <stdio.h>
#include <graphics.h>
#include <iostream>
#include <conio.h>
#include <string.h>
#include <time.h>
#include <windows.h>
#pragma comment(lib,"Winmm.lib")
//ʵ�ֵ���ʱ��
clock_t Start_time;
clock_t End_time;
char str[100];
int time_tt;
//int atend;
//��������Ե�ͼ��ֹ��
#define KEY_DOWN(vKey) ((GetAsyncKeyState(vKey)&0x8000)?1:0)
#define BLOCKS 100
// ����PNGͼ��ȥ͸������
IMAGE background,castle;
void drawAlpha(IMAGE* picture, int  picture_x, int picture_y) //xΪ����ͼƬ��X���꣬yΪY����
{

	// ������ʼ��
	DWORD* dst = GetImageBuffer();    // GetImageBuffer()���������ڻ�ȡ��ͼ�豸���Դ�ָ�룬EASYX�Դ�
	DWORD* draw = GetImageBuffer();
	DWORD* src = GetImageBuffer(picture); //��ȡpicture���Դ�ָ��
	int picture_width = picture->getwidth(); //��ȡpicture�Ŀ�ȣ�EASYX�Դ�
	int picture_height = picture->getheight(); //��ȡpicture�ĸ߶ȣ�EASYX�Դ�
	int graphWidth = getwidth();       //��ȡ��ͼ���Ŀ�ȣ�EASYX�Դ�
	int graphHeight = getheight();     //��ȡ��ͼ���ĸ߶ȣ�EASYX�Դ�
	int dstX = 0;    //���Դ������صĽǱ�
	// ʵ��͸����ͼ ��ʽ�� Cp=��p*FP+(1-��p)*BP �� ��Ҷ˹���������е���ɫ�ĸ��ʼ���
	for (int iy = 0; iy < picture_height; iy++)
	{
		for (int ix = 0; ix < picture_width; ix++)
		{
			int srcX = ix + iy * picture_width; //���Դ������صĽǱ�
			int sa = ((src[srcX] & 0xff000000) >> 24); //0xAArrggbb;AA��͸����
			int sr = ((src[srcX] & 0xff0000) >> 16); //��ȡRGB���R
			int sg = ((src[srcX] & 0xff00) >> 8);   //G
			int sb = src[srcX] & 0xff;              //B
			if (ix >= 0 && ix <= graphWidth && iy >= 0 && iy <= graphHeight && dstX <= graphWidth * graphHeight)
			{
				dstX = (ix + picture_x) + (iy + picture_y) * graphWidth; //���Դ������صĽǱ�
				int dr = ((dst[dstX] & 0xff0000) >> 16);
				int dg = ((dst[dstX] & 0xff00) >> 8);
				int db = dst[dstX] & 0xff;
				draw[dstX] = ((sr * sa / 255 + dr * (255 - sa) / 255) << 16)  //��ʽ�� Cp=��p*FP+(1-��p)*BP  �� ��p=sa/255 , FP=sr , BP=dr
					| ((sg * sa / 255 + dg * (255 - sa) / 255) << 8)         //��p=sa/255 , FP=sg , BP=dg
					| (sb * sa / 255 + db * (255 - sa) / 255);              //��p=sa/255 , FP=sb , BP=db
			}
		}
	}
}
//int intheend=0;
int blo_cnt;
IMAGE beginBk, gameOver;
IMAGE humans[8], f_block[4], grass, a_block[4], cloud1, cloud2, tree;//������������̬����1����2����1����2��������
IMAGE enemy[10], pillar[3];   //��ͬ�ĵ����о�:�ڹ꣬С����
int gamelevel = 1;//������Ϸ�ؿ���
int life_num = 3;//����ֵ��
int width = 900, high = 600;
int maps[18][120];//ÿ��������ռ���Ϊ10*10
int maps2[18][120];//�����ȼ��ĵ�ͼ��
int left_i = 2;
int right_i = 0;
int lift, lift_high, low_high; //Ϊ1������״̬��Ϊ0���ڲ���״̬��Ϊ-1�����½�״̬��
//��¼����ĺ������꣬ʵʱ���µ�ͼ״̬-��
bool boolenemymove[14];
int isMovemap = 0;
int speed_control;
int human_cnt;
int enemySpeed[14];
int leap[9] = { 49,36,25,16,9,4,1,1,1 };
int cntleap = 0;
int onthewallss;
int isCameraMove = 1;//�ӽǹ̶�,��Ϊ0��ʱ����human.x���ڵ���width/2������ͻᱣ���ƶ���
//������´ﵽwidth/2��λ�õ�ʱ�����ĺ�����Ͳ����ˣ�
//��������һ��changeX��ȷ����������width/2��ʱ��ismovemap++,���ǣ�human.x���ֲ��䣻
int changeX = 0;
int isonthewall = 0;
int onthewall = 0;
int tothewall = 0;
int moneyx, moneyy;
int isonthewalls = 0;
int onthewalls = 0;
int last_time;
int ddd = 0;
int tt[14] = { 0 }, tt1[14] = { 0 };
int onit;
int leap_fre;
int right;
int movecnttop, movecntright, movecntleft;
typedef struct Mapsx
{
	double x;
	double y;
	int i;
	int j;
	int isblank;//�ж����������Ƿ�Ϊ�յģ�
	int iscolumn;
	int iswall;
	int type;
}sss;
sss maps_xy[18][120];
typedef struct sRole
{
	double x, y;
	int speed;
	int leap;
	int alive;
	int victory;//���ܵ��˸���
	int arrow;//�������⣻
}Role;
Role human;//������human����ṹ�壻
//�˵�Ԫ�أ�
IMAGE beginning;
IMAGE munuBackground, menuBlack, beginGame;
typedef struct Enemy
{
	int x, y;
	int alive;
	int arrow;//���˳���
	int speed;//���˵��ٶ����ã�
	int cnt; //ȷ����һ�ֵ��ˣ�
	int mapsi, mapsj;
	int died;
}Enes;
Enes ene[20];//���ݵ�ͼ�������ֵĳ�ʼλ���趨�Լ���x��y��

//���ڿ��е�ʯͷ������Ƚ϶࣬��������Ԫ�ص�,���Բ�ȡ�ṹ����ʽ����������
typedef struct Stone
{
	double x, y;
	int alive;//������������Ϳ���ײ��ʯͷ�ˣ�ʯͷ���飻
	int mapsi, mapsj;
	int type;
	int appear;
	int leap;
}stone;
stone blo[BLOCKS]; //����ʯͷ�������ӣ����ʺŵ������ӣ���ͨʯͷ
int score;
int isright;
int mapss, money
;//��Һ͵�ͼ��ת���� ѧ����ôʵ�ֶ���Ч����
//��Ϸ�˵����֣���ɣ�
void bkmusic()
{
	mciSendString(_T("open E:\\"), NULL, 0, NULL);
	mciSendString(_T("play bkmusic repeat"), NULL, 0, NULL);
}
void jumpmusic()
{
	mciSendString(_T("open C:\\Users\\Lenovo\\Desktop\\���������ز�\\�������Ծ.mp3"), NULL, 0, NULL);
	mciSendString(_T("play jpmusic"), NULL, 0, NULL);
}
void login()//��¼�˵�������
{
	loadimage(&beginBk, _T("C:\\Users\\Lenovo\\Desktop\\���������ز�\\menubk.jpg"));
	putimage(50, 80, &beginBk);
	settextstyle(30, 0, _T("΢���ź�"));
	settextcolor(RGB(255, 255, 0));
	rectangle(300, 80, 550, 120);
	outtextxy(310, 85, _T("1.��Ϸ��ʼ"));
	rectangle(300, 140, 550, 180);
	outtextxy(310, 145, _T("2.��������"));
	rectangle(300, 200, 550, 240);
	outtextxy(310, 205, _T("3.�˳���Ϸ"));
	bool flag = true;
	while (flag)
	{
		if (MouseHit())//�ж��Ƿ��������Ϣ
		{
			MOUSEMSG msg = GetMouseMsg();
			if (msg.uMsg == WM_LBUTTONDOWN)
			{
				if ((msg.x >= 300 && msg.x <= 550) && (msg.y >= 80 && msg.y <= 120))
				{
					//������Ϸ��ʼ������
					flag = false;
				}
				else if ((msg.x >= 300 && msg.x <= 550) && (msg.y >= 140 && msg.y <= 180))
				{
					//�����ֲ��ţ�
					bkmusic();
				}
				else if ((msg.x >= 300 && msg.x <= 550) && (msg.y >= 200 && msg.y <= 240))
				{
					exit(0);//�˳���Ϸ
				}
				else {
					flag = flag;
				}
			}
		}
	}
}
//��Ϸ�������֣�
void gameover()
{
	loadimage(&gameOver, _T("C:\\Users\\Lenovo\\Desktop\\���������ز�\\gameover.png"), 400, 300);
	setbkcolor(BLACK);
	cleardevice();
	putimage(30, 30, &gameOver);
	settextstyle(30, 0, _T("΢���ź�"));
	settextcolor(RGB(255, 255, 0));
	rectangle(330, 80, 550, 120);
	outtextxy(340, 85, _T("��Ϸ������"));
	rectangle(330, 140, 550, 180);
	settextcolor(YELLOW);
	sprintf_s(str, "���ĵ÷�Ϊ�� % d ", score);
	outtextxy(340, 145, _T(str));
}
//��ʼ�����������λ�õȲ�����//��ʼ��ͼ���ˣ����ˣ�ש�顣
//��������ͼ�ĳ�ʼ��,�涨�ĸ�λ�÷�ʲô
//ʵ����������ש��,big��ײ��������������ʺţ�����Ģ����Ģ��ҲҪ�ƶ���(һ�������������Ծ��ʱ��)
void isMoveMap()
{
	if (isCameraMove == 1 && human.x >= width / 2)
	{
		isCameraMove = 0;
		isMovemap += 1;//����ͼ����ʾ�����1��
	}
	else if (isCameraMove == 0 && changeX >= 99.6 && changeX <= 100.4)
	{
		isMovemap++;
		changeX = 0;
	}
	else if (human.x < width / 2)
	{
		isCameraMove = 1;
		changeX = 0;
	}
}
void isMoveHuman(int a)
{
	if (human.x < width / 2 && human.x >= 0)
	{
		if (a == 2)
		{
			human.x += 0.3;
		}
	}
	else
	{
		int j, i;
		//human.x���ֲ��䣻
		changeX += 2;
		if (changeX % 10 == 0)
		{
			for (i = 0; i < 18; i++)
			{
				for (j = 0; j < 120; j++)
				{
					maps_xy[i][j].x -= 3;//����ȷ��ש���x����λ�ã�
				}
			}
			for (int k = 0; k < 14; k++)
			{
				if (ene[k].speed != 0 && ene[k].x>-30 && ene[k].alive==1)
				{
					ene[k].x -= 3;
				}
			}
		}
	}
}
void init()
{
	//��ʼ����ͼ��
	int i, j;
	//���ò�ͬlevel�ĵ�ͼ��ʽ
	if (gamelevel == 1)
	{   //1�ǵ�ש��2��С�ݣ�3����ľ��4���ʺ�ש�飬5����ͨש�飬6���ƶ�1��7���ƶ�2��8�Ƕ���1,12��Χ����ǽ��ש��
		//9��������10�ǳ�����20������£�11�ǹ���
		int maps1[18][120] = {//��ʼ����ͼ
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,0,0,0,0,0,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,5,5,5,5,5,5,0,5,5,5,4,0,0,0,0,0,0,0,0,0,4,0,0,0,0,0,0,5,5,5,0,0,0,5,4,4,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,4,0,0,0,0,0,0,0,0,0,0,0,4,0,0,0,0,5,4,5,4,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,4,5,0,0,0,0,0,0,0,0,0,0,0,0,5,0,0,5,5,0,0,0,4,0,4,0,4,0,0,5,0,0,0,0,0,0,0,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			2,2,0,0,0,0,0,0,0,0,0,0,11,0,11,0,2,2,0,0,0,0,0,0,0,8,33,0,0,9,0,0,2,11,0,9,33,0,11,0,0,0,0,0,0,2,2,2,0,0,0,0,0,11,0,0,0,11,0,0,0,11,11,0,0,0,11,0,2,2,2,11,11,0,0,0,0,11,0,0,11,0,0,0,0,0,0,0,0,0,0,8,33,0,0,11,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,0,0,0,0,0,0,0,0,
			1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,33,33,1,1,1,1,0,0,1,1,1,1,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,1,1,1,
			1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,33,33,1,1,1,1,0,0,1,1,1,1,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,1,1,1
		};
		for (i = 0; i < 18; i++)
		{
			for (j = 0; j < 120; j++)
			{
				maps[i][j] = maps1[i][j];
				maps_xy[i][j].x = j * 30;
				maps_xy[i][j].y = i * 30;
				maps_xy[i][j].i = i;
				maps_xy[i][j].j = j;
				maps_xy[i][j].type = maps1[i][j];
				maps_xy[i][j].iswall = 0;
				maps_xy[i][j].iscolumn = 0;
				maps_xy[i][j].isblank = 0;
			}
		}
	}
	//���
	loadimage(&humans[0], _T("C:\\Users\\Lenovo\\Desktop\\���������ز�\\����.png"), 30, 30);
	loadimage(&humans[1], _T("C:\\Users\\Lenovo\\Desktop\\���������ز�\\������2.png"), 30, 30);
	loadimage(&humans[2], _T("C:\\Users\\Lenovo\\Desktop\\���������ز�\\������1.png"), 30, 30);
	loadimage(&humans[3], _T("C:\\Users\\Lenovo\\Desktop\\���������ز�\\������2.png"), 30, 30);
	loadimage(&humans[4], _T("C:\\Users\\Lenovo\\Desktop\\���������ز�\\big����.png"), 60, 60);
	loadimage(&humans[5], _T("C:\\Users\\Lenovo\\Desktop\\���������ز�\\��Ծ.png"), 30, 30);
	loadimage(&humans[6], _T("C:\\Users\\Lenovo\\Desktop\\���������ز�\\����������.png"), 30, 30);

	//����ש�飺
	loadimage(&a_block[0], _T("C:\\Users\\Lenovo\\Desktop\\���������ز�\\��ײ�鷽��.png"), 30, 30);
	loadimage(&a_block[1], _T("C:\\Users\\Lenovo\\Desktop\\���������ز�\\���ط���.png"), 30, 30);
	loadimage(&a_block[2], _T("C:\\Users\\Lenovo\\Desktop\\���������ز�\\������.png"), 30, 30);
	loadimage(&f_block[0], _T("C:\\Users\\Lenovo\\Desktop\\���������ز�\\�ݵ�.png"), 30, 30);
	loadimage(&f_block[1], _T("C:\\Users\\Lenovo\\Desktop\\���������ز�\\��ש.png"), 30, 30);
	loadimage(&f_block[2], _T("C:\\Users\\Lenovo\\Desktop\\���������ز�\\��ש.png"), 30, 30);

	//���ˣ�
	loadimage(&enemy[0], _T("C:\\Users\\Lenovo\\Desktop\\���������ز�\\�ڹ�.png"), 30, 30);
	loadimage(&enemy[1], _T("C:\\Users\\Lenovo\\Desktop\\���������ز�\\С����.png"), 30, 30);
	loadimage(&enemy[2], _T("C:\\Users\\Lenovo\\Desktop\\���������ز�\\С����2.png"), 30, 30);
	loadimage(&enemy[3], _T("C:\\Users\\Lenovo\\Desktop\\���������ز�\\С����3.png"), 30, 30);
	//����װ��
	loadimage(&grass, _T("C:\\Users\\Lenovo\\Desktop\\���������ز�\\��.png"), 30, 30);
	loadimage(&cloud1, _T("C:\\Users\\Lenovo\\Desktop\\���������ز�\\�Ʋ�1.png"), 90, 60);
	loadimage(&cloud2, _T("C:\\Users\\Lenovo\\Desktop\\���������ز�\\�Ʋ�.png"), 90, 60);
	loadimage(&tree, _T("C:\\Users\\Lenovo\\Desktop\\���������ز�\\��.png"), 80, 100);

	//����
	loadimage(&pillar[0], _T("C:\\Users\\Lenovo\\Desktop\\���������ز�\\����.png"), 60, 120);
	loadimage(&pillar[1], _T("C:\\Users\\Lenovo\\Desktop\\���������ز�\\����.png"), 60, 100);
	loadimage(&pillar[2], _T("C:\\Users\\Lenovo\\Desktop\\���������ز�\\����.png"), 60, 80);
	loadimage(&background, _T("C:\\Users\\Lenovo\\Desktop\\���������ز�\\bk.jpg"), 900, 540, 1);
	loadimage(&castle, _T("C:\\Users\\Lenovo\\Desktop\\���������ز�\\�����Ǳ�.png"), 150, 200);
	human.x = 30;
	human.y = high - 90;
	human.alive = 6;
	human.speed = 0.5;
	int k = 0, k2 = 0;
	for (i = 0; i < 14; i++)
	{
		boolenemymove[i] = true;
	}
	for (j = 0; j < 120; j++)
	{
		for (i = 0; i < 18; i++)
		{
			if (maps_xy[i][j].type == 11)
			{
				ene[k].speed = 0;
				ene[k].mapsi = i;
				ene[k].mapsj = j;
				ene[k].x = -80;
				ene[k].died = 0;
				ene[k].alive = 1;//����Ϊ1������ʾ����Ϊ1������ʾ��
				ene[k++].y = i * 30;
			}
			else if (maps_xy[i][j].type == 12)
			{
				maps_xy[i][j].iswall = 1;
			}
			else if (maps_xy[i][j].type >= 8 && maps_xy[i][j].type <= 10)
			{
				maps_xy[i][j].iscolumn = 1;
			}
			else if (maps_xy[i][j].type == 0 && i >= 16)
			{
				maps_xy[i][j].isblank = 1;
			}
		}
	}//��ʼ������ש���λ��
	//blo_cnt = k2;
}
void updateWithInput()//���ݼ������룬����δʵ�֣�����ͬʱ���������ַ�
{
	//BeginBatchDraw();
	putimage(human.x, human.y, &humans[left_i]);
	int i;
	bool flag = true;
	int kk = 0;
	//if (intheend == 0)
	//{
		if (GetAsyncKeyState('A') & 0x8000 && human.x>=10)
		{
			if (movecntleft == 3)
			{
				if (left_i == 2)
					human_cnt = 2;
				else
					human_cnt = 3;
				left_i++;
				if (left_i == 4)
				{
					left_i = 2;
				}
				human.x -= 0.3;
				kk = 1;
				movecntleft = 0;
				isright = 1;
			}
			movecntleft++;
		}
		if (GetAsyncKeyState('D') & 0x8000)
		{
			if (movecntright == 3)
			{
				if (right_i == 0)
					human_cnt = 0;
				else
					human_cnt = 1;
				right_i++;
				if (right_i == 2)
				{
					right_i = 0;
				}
				isMoveHuman(2);
				kk = 1;
				movecntright = 0;
				right = 1;
				isright = 0;
			}
			movecntright++;
		}
		if (GetAsyncKeyState('W') & 0x8000)
		{
			if (movecnttop == 8)
			{
				if (leap_fre <= 2)
				{
					lift = 1;
					human_cnt = 5;
					leap_fre++;
					//ÿ��һ����һ����Ч��
					jumpmusic();
				}
				kk = 1;
				movecnttop = 0;
			}
			movecnttop++;
		}
		if (kk == 1)
		{
			for (i = 0; i < blo_cnt; i++) {
				if (human.x >= width / 2 - 10 && blo[i].appear == 1)
				{
					blo[i].x -= 0.5;
				}
			}
		}
	//}
	putimage(human.x, human.y, &humans[human_cnt]);
	//EndBatchDraw();
}
//����������������ң���huamn.x++��
//ʵ�ֵ��˵ĳ��֡� //�趨�ض�λ�ó��֣����ҿ�ʼ���ߣ��������У����⣬��������
void enemyMove()
{
	int i;
	for (i = 0; i < 14; i++)
	{
		if (ene[i].mapsj <= isMovemap + 30 && boolenemymove[i] == true)
		{
			boolenemymove[i] = false;
			if (i == 0)
				ene[i].x = (ene[i].mapsj * 30) % 900;
			else {
				ene[i].x = 900;
			}
		}
		if (ene[i].x - human.x <= width / 2 && ene[i].x > -30)
		{
			ene[i].speed = 5;
		}
	}
	for (i = 0; i < 14; i++)
	{
		if (ene[i].speed != 0 && ene[i].alive == 1 && ene[i].x>-30)
		{
			maps_xy[ene[i].mapsi][ene[i].mapsj].type = 0;
			if (enemySpeed[i] == 30)
			{
				enemySpeed[i] = 0;
				ene[i].x = ene[i].x - 3;
			}
			enemySpeed[i]++;
		}
		else if (ene[i].x <= -30 && ene[i].x >= -60 && ene[i].alive == 1)
		{
			ene[i].alive = 0;
		}
	}
}
//�ѵ�ͼ��������ʾ�ڽ�����
void show()
{
	//BeginBatchDraw();
	putimage(0, 0, &background);
	int i, j;
	if (isMovemap+15 < 120)
	{
		for (i = 0; i < 18; i++)
		{
			for (j = isMovemap-3; j < 30 + isMovemap; j++)
			{

				int k;
				k = j - isMovemap;
				if (maps_xy[i][j].type == 1)
				{
					putimage(maps_xy[i][j].x, maps_xy[i][j].y, &f_block[1]);
					//drawAlpha(&f_block[1], maps_xy[i][j].x, maps_xy[i][j].y);
				}
				else if (maps_xy[i][j].type == 2)
				{
					//putimage(maps_xy[i][j].x, maps_xy[i][j].y, &grass);
					drawAlpha(&grass, maps_xy[i][j].x, maps_xy[i][j].y);
				}
				else if (maps_xy[i][j].type == 3)
				{
					//putimage(maps_xy[i][j].x, maps_xy[i][j].y, &tree);
					drawAlpha(&tree, maps_xy[i][j].x, maps_xy[i][j].y);
				}
				else if (maps_xy[i][j].type == 4)
				{
					putimage(maps_xy[i][j].x, maps_xy[i][j].y, &a_block[1]);
				}
				else if (maps_xy[i][j].type == 5)
				{
					putimage(maps_xy[i][j].x, maps_xy[i][j].y, &a_block[0]);
				}
				else if (maps_xy[i][j].type == 6)
				{
					//putimage(maps_xy[i][j].x, maps_xy[i][j].y, &cloud1);

					drawAlpha(&cloud1, maps_xy[i][j].x, maps_xy[i][j].y);
				}
				else if (maps_xy[i][j].type == 7)
				{
					//putimage(maps_xy[i][j].x, maps_xy[i][j].y, &cloud2);

					drawAlpha(&cloud2, maps_xy[i][j].x, maps_xy[i][j].y);
				}
				else if (maps_xy[i][j].type == 8)
				{
					putimage(maps_xy[i][j].x, maps_xy[i][j].y, &pillar[2]);
				}
				else if (maps_xy[i][j].type == 9)
				{
					putimage(maps_xy[i][j].x, maps_xy[i][j].y, &pillar[1]);
				}
				else if (maps_xy[i][j].type == 10)
				{
					putimage(maps_xy[i][j].x, maps_xy[i][j].y, &pillar[0]);
				}
				else if (maps_xy[i][j].type == 12)
				{
					putimage(maps_xy[i][j].x, maps_xy[i][j].y, &a_block[2]);
				}
			}
		}
		for (i = 0; i < 14; i++)
		{
			if (ene[i].speed != 0 && ene[i].alive == 1)
			{
				drawAlpha(&enemy[0], ene[i].x, ene[i].y);
				//putimage(ene[i].x, ene[i].y, &enemy[0]);
			}
		}
		//drawAlpha(&humans[human_cnt], human.x, human.y);
		putimage(human.x, human.y, &humans[human_cnt]);
		if (tothewall == 1)
		{
			isonthewall++;
			if (isonthewall == 40)
			{
				human.y += 60;
				tothewall = 0;
				isonthewall = 0;
			}
		}
	}
	else {
		//drawAlpha(&castle, width - 150, high - 150);
		ddd++;
		if (ddd >= 6)
		{
			isMovemap = 120;
		}
		putimage(width - 200, high - 260, &castle);
		for (i = 0; i < 18; i++)
		{
			for (j = 105; j < 120; j++)
			{
				if (maps_xy[i][j].type == 1)
				{
					putimage(maps_xy[i][j].x, maps_xy[i][j].y, &f_block[1]);
				}
				else if (maps_xy[i][j].type == 2)
				{
					putimage(maps_xy[i][j].x, maps_xy[i][j].y, &grass);
				}
				else if (maps_xy[i][j].type == 3)
				{
					putimage(maps_xy[i][j].x, maps_xy[i][j].y, &tree);
				}
				else if (maps_xy[i][j].type == 4)
				{
					putimage(maps_xy[i][j].x, maps_xy[i][j].y, &a_block[1]);
				}
				else if (maps_xy[i][j].type == 5)
				{
					putimage(maps_xy[i][j].x, maps_xy[i][j].y, &a_block[0]);
				}
				else if (maps_xy[i][j].type == 6)
				{
					putimage(maps_xy[i][j].x, maps_xy[i][j].y, &cloud1);
				}
				else if (maps_xy[i][j].type == 7)
				{
					putimage(maps_xy[i][j].x, maps_xy[i][j].y, &cloud2);
				}
				else if (maps_xy[i][j].type == 8)
				{
					putimage(maps_xy[i][j].x, maps_xy[i][j].y, &pillar[2]);
				}
				else if (maps_xy[i][j].type == 9)
				{
					putimage(maps_xy[i][j].x, maps_xy[i][j].y, &pillar[1]);
				}
				else if (maps_xy[i][j].type == 10)
				{
					putimage(maps_xy[i][j].x, maps_xy[i][j].y, &pillar[0]);
				}
				else if (maps_xy[i][j].type == 12)
				{
					putimage(maps_xy[i][j].x, maps_xy[i][j].y, &a_block[2]);
				}
			}
		}
		for (i = 0; i < 14; i++)
		{
			if (ene[i].speed != 0 && ene[i].alive == 1)
			{
				//drawAlpha(&enemy[0], ene[i].x, ene[i].y);
				putimage(ene[i].x, ene[i].y, &enemy[0]);
			}
			else if (ene[i].died >= 1 && ene[i].died <= 20)
			{
				//drawAlpha(&enemy[0], ene[i].x, ene[i].y);
				ene[i].died += 1;
			}
		}
		putimage(human.x, human.y, &humans[human_cnt]);
		if (tothewall == 1)
		{
			isonthewall++;
			if (isonthewall == 40)
			{
				human.y += 60;
				tothewall = 0;
				isonthewall = 0;
			}
		}
	}
	End_time = clock();
	time_tt = (int)((End_time - Start_time) / CLOCKS_PER_SEC);
	settextcolor(BLACK);
	sprintf_s(str, "����ֵ�� %d ", human.alive / 2);
	settextcolor(BLACK);
	outtextxy(100, 20, _T(str));

	sprintf_s(str, "����ʱ�� %d s", 300 - time_tt);
	settextcolor(BLACK);
	outtextxy(700, 20, _T(str));

	sprintf_s(str, "��ң� %d ", score);
	settextcolor(BLACK);
	outtextxy(500, 20, _T(str));
	if (money == 1)
	{
		if (last_time <=120)
		{
			IMAGE money;
			loadimage(&money, _T("C:\\Users\\Lenovo\\Desktop\\���������ز�\\money.png"), 30, 30);
			drawAlpha(&money, moneyx, moneyy-30);
			last_time++;
		}
		else if (last_time > 120)
		{
			money = 0;
			score += 10;
			last_time = 0;
		}
	}
	//EndBatchDraw();
}
void updateWithoutInput()
{
	//4��5��ש�飡
	//���ж�����ֵ��ʧ�ȵ����⣻
	int i, j, m;
	for (i = 0; i < 14; i++)
	{
		if (tt[i] == 1)
		{
			tt1[i]++;
		}
		if (tt1[i] == 60)
		{
			tt[i] = 0;
		}
	}
	for (m = 0; m < 14; m++)
	{
		if (ene[m].speed != 0 && ene[m].alive == 1)
		{
			if (tt[m] == 0 && ene[m].x <= human.x + 30 && ene[m].x >= human.x && human.y >= ene[m].y - 2 && human.y <= ene[m].y + 2)
			{
				tt[m] = 1;
				human.alive -= 1;
				//��ʱ�������������ٶȣ��½�
				if (human.alive == 0)
				{
					for (i = 0; i < 10; i++)
					{
						human.y += 0.9;
						FlushBatchDraw();
					}
					gameover();
					system("pause");
					isMovemap = 100;
				}
			}
			if (ene[m].x <= human.x + 30 && ene[m].x >= human.x && human.y<= ene[m].y && human.y>=ene[m].y-60)
			{
				ene[m].alive = 0;
				ene[m].died = 1;
				//ene[m].y += 120;
			}
		}//�������û��ש��͵���������һ���˻�enemy����ɣ�
		for (i = 0; i < 18; i++)
		{
			for (j = 0; j < 120; j++)
			{
				if (maps_xy[i][j].isblank == 1)
				{
					if (ene[m].x - maps_xy[i][j].x <= 2 && ene[m].x - maps_xy[i][j].x >= 0)
					{
						int lm;
						ene[m].alive = 0;
						ene[m].y += 120;
					}
					if (human.x > maps_xy[i][j].x+5 && human.y >= maps_xy[i][j].y && human.x < maps_xy[i][j].x+25)
					{
						int lm;
						human.alive -= 1;
						human.y -= 30;
						FlushBatchDraw();
						if (human.alive == 0)
						{
							human.y += 120;
							isMovemap = 140;
						}
						else {
							human.x -= 90;
							human.y = high - 90;
							/*settextcolor(BLACK);
							sprintf_s(str, "���Ѷ�ʧһ������,����������");
							settextcolor(BLACK);
							outtextxy(300, 150, _T(str));
							*/
						}
					}
				}
				//�ж���ǰ���Ƿ����ϰ�����Ӻ�ǽ��,��ɣ�
				if (human.x + 30 >= maps_xy[i][j].x && human.x + 30 <= maps_xy[i][j].x+2 && (maps_xy[i][j].iswall == 1 || maps_xy[i][j].iscolumn == 1) && human.y >= maps_xy[i][j].y-2)
				{
					human.x -= 5;//��ʱû�취������ǰ�ߣ�
				}
				else if (human.x >= maps_xy[i][j].x + 60 && human.x <= maps_xy[i][j].x + 62 && maps_xy[i][j].iscolumn == 1 && human.y >= maps_xy[i][j].y-2 && human_cnt >= 2 && human_cnt <= 3)
				{
					human.x += 5;
				}
				else if (human.x >= maps_xy[i][j].x + 30 && maps_xy[i][j].iswall == 1 && human.y >= maps_xy[i][j].y-2 && human_cnt >= 2 && human_cnt <= 3)
				{
					human.x += 5;
				}
				else if ((maps_xy[i][j].type == 4 || maps_xy[i][j].type == 5) && human.x >= maps_xy[i][j].x + 30 && human.x <= maps_xy[i][j].x + 32 && human.y >= maps_xy[i][j].y - 2 && human.y <= maps_xy[i][j].y + 2)
				{
					human.x += 5;
				}
				else if ((maps_xy[i][j].type == 4 || maps_xy[i][j].type == 5) && human.x + 30 <= maps_xy[i][j].x + 1 && human.x + 30 >= maps_xy[i][j].x - 1 && human.y >= maps_xy[i][j].y - 2 && human.y <= maps_xy[i][j].y + 2)
				{
					human.x -= 5;
				}
			}
		}
	}
	int first = 0;
	int second = 0;
	speed_control++;
	if (speed_control == 20)
	{
		if (lift == 1 && cntleap <= 8)
		{
			if (right == 1 && human.x <= width / 2 - 3)
			{
				human.x += 2;
			}
			for (int j = 0; j <= cntleap * 5; j++)
			{
				int i;
			}
			human.y -= leap[cntleap];
			cntleap++;
		}
		else if (lift == 1 && cntleap == 9 || cntleap >= 0 && lift != 0)
		{
			if (right == 1 && human.x <= width / 2 - 3)
			{
				human.x += 2;
			}
			for (int j = 0; j <= cntleap * 3; j++)
			{
				int i;
			}
			lift = -1;
			if (cntleap < 9)
				human.y += leap[cntleap];
			cntleap--;
		}
		else if (lift == -1 && cntleap == -1 || cntleap == -1)
		{
			right = 0;
			if (isright == 1)
				cntleap = -1;
			else
				cntleap = 0;
			lift = 0;
			leap_fre = 0;
		}
		else if (lift == 0)
		{
			human_cnt = human_cnt;
		}
		speed_control = 0;
		for (i = 0; i < 18; i++)
		{
			for (j = 0; j < 120; j++)
			{
				//���������й��ܻ㼯��
				//4��5�ɹ���
				if (maps_xy[i][j].type == 4 || maps_xy[i][j].type == 5)
				{   //����ש�飺��ס���������������
					if (human.y >= maps_xy[i][j].y + 35 && human.y - leap[cntleap] <= maps_xy[i][j].y + 30 && cntleap <= 8 && human.x >= maps_xy[i][j].x-30 && human.x <= maps_xy[i][j].x + 30 && lift == 1 && tothewall == 0)
					{
						human.y = maps_xy[i][j].y + 30;
						lift = 0;
						cntleap = -1;
						tothewall = 1;
						if (maps_xy[i][j].type == 4)
						{
							moneyx = maps_xy[i][j].x;
							moneyy = maps_xy[i][j].y;
							//չʾ��Ҳ����ܵ÷ּ�һ�ӣ�
							money = 1;
						}
					}	//��ש���Ϸ��������վ�����沢������Ծ:
					if (lift==-1)
					{
							if (human.y + 30 <= maps_xy[i][j].y && human.y >= maps_xy[i][j].y - 50 && (human.x >= maps_xy[i][j].x - 30 && human.x <= maps_xy[i][j].x + 30 && isright==0 || human.x >= maps_xy[i][j].x && human.x <= maps_xy[i][j].x+30 && isright==1))
							{
								if (first == 0)
								{
									cntleap = -1;
									lift = 0;
									human.y = maps_xy[i][j].y - 30;
									first = 1;
								}
								else if (second == 0 && first==1)
								{
									cntleap = -1;
									lift = 0;
									human.y = maps_xy[i][j].y - 30;
									second = 1;
								}
							}
						//������ש���Ϸ���
					}
				}
				else if (maps_xy[i][j].iscolumn == 1 && lift == -1)//�������ӻ�����ǽ��������жϣ�
				{
					if (human.y + 30 <= maps_xy[i][j].y && human.y >= maps_xy[i][j].y - 50)
					{
						if (human.x + 30 >= maps_xy[i][j].x && human.x <= maps_xy[i][j].x + 60 && isright == 0 || human.x <= maps_xy[i][j].x + 62 && human.x >= maps_xy[i][j].x && isright == 1)
						{
							human.y = maps_xy[i][j].y - 30;
							cntleap = -1;
							onthewalls = 1;
						}
					}
					/*else if (human.x + 30 < maps_xy[i][j].x || human.x > maps_xy[i][j].x + 30)
					{
						maps_xy[i][j].iscolumn = 0;
					}*/
				}
				else if (maps_xy[i][j].iswall == 1 && lift == -1)
				{
					if ((human.x + 30 >= maps_xy[i][j].x && human.x <= maps_xy[i][j].x+3 && isright == 0 || human.x >= maps_xy[i][j].x && human.x <= maps_xy[i][j].x + 3 && isright == 1) && human.y + 30 <= maps_xy[i][j].y && human.y >= maps_xy[i][j].y - 50)
					{
						//����������
						human.y = maps_xy[i][j].y - 30;
						cntleap = -1;
						onthewallss += 1;
						//���ж��Ƿ���������Ծ�������->����ı�lift��cntleap��ֵ
					}
				}
				if ((human.x > maps_xy[i][j].x + 60 && human.x<maps_xy[i][j].x + 65 && isright==0 || human.x + 30 <= maps_xy[i][j].x && human.x>=maps_xy[i][j].x - 40 && isright==1) && maps_xy[i][j].iscolumn == 1 && human.y + 30 >= maps_xy[i][j].y && onthewalls >= 1)
				{
					human.y = high - 90;//��ҪԤ����һ���������ģ��������ϰ���ȵ�
					cntleap = -1;
					onthewalls = 0;
					if (isright == 1)
					{
						human.x -= 4;
					}
					else{
						human.x += 4;
					}
				}
				//�ж����䣡
				if (tothewall == 0 && onthewalls==0 && i <= 16 && human.x >= maps_xy[i][j - 1].x && human.x <= maps_xy[i][j - 1].x + 32 && human.y >= maps_xy[i][j - 1].y - 1 && human.y <= maps_xy[i][j - 1].y + 1 && maps_xy[i + 1][j - 1].type != 1 && lift == 0)
				{
					if (maps_xy[i + 1][j-1].type == 0 || maps_xy[i+ 1][j-1].type < 4 && maps_xy[i + 1][j-1].type>1 || maps_xy[i + 1][j-1].type > 5 && maps_xy[i + 1][j-1].type <= 7 || maps_xy[i + 1][j-1].type == 11 )
					{

						if (maps_xy[i + 1][j].type >= 4 && maps_xy[i + 1][j].type <= 5 && human.x + 30 > maps_xy[i][j].x)
						{
							i = i;
						}
						else {
							//human.x += 4;
							if (isright == 1 && human.x +30 <= maps_xy[i][j].x)
							{
								human.x -= 4;
							}
							else if(isright==0 && human.x>= maps_xy[i][j].x+30)
							{
								human.x += 4;
							}
							human.y += 30;
						}
					}
				}
			}
		}
	}
}
int main()
{
	//��������ķ�ʽ��ʵ�ֳ�������£�
	initgraph(900, 540);
	width = 900;
	high = 540;
	setbkcolor(WHITE);
	cleardevice();
	login();
	init();
	setbkcolor(WHITE);
	cleardevice();
	Start_time = clock();
	BeginBatchDraw();
	while (isMovemap+10 < 120)
	{
		isMoveMap();
		updateWithInput();
		updateWithoutInput();
		enemyMove();
		setbkcolor(WHITE);
		cleardevice();
		show();
		FlushBatchDraw();
	}
	int i;
	for (i = 0; i < 20; i++)
	{
		gameover();
		FlushBatchDraw();
		Sleep(500);
	}
	_kbhit();
	Sleep(500);
	EndBatchDraw();
	closegraph();
	return 0;
}
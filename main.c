/***** Maze Program for MieruEMB System v1.1                    *******/
/***** Coding by Takunology and Nyabingo                        *******/
/**********************************************************************/
volatile char *e_vram = (char*)0x900000;
volatile int  *e_time = (int *)0x80010c;
volatile char *e_gp1  = (char*)0x8001f0;
volatile char *e_gp2  = (char*)0x8001f1;
volatile char *e_sw1  = (char*)0x8001fc;
volatile char *e_sw2  = (char*)0x8001fd;
volatile char *e_sw3  = (char*)0x8001fe;
volatile char *e_gin  = (char*)0x8001ff;

#include "cfont_ex.h" // include character & number font
#include "image.h" //include images

/**********************************************************************/

#ifdef QEMU
    #define LCD_HEIGHT  200
    #define LCD_WIDTH   320
#else
    #define LCD_HEIGHT  128
    #define LCD_WIDTH   128
#endif

/**********************************************************************/

void mylib_msleep(unsigned int tm);

void mylib_transition(const unsigned int data[][128], int isblackout)
{
    int i, j, k;

    for(i = 0; i < 128 / 8; i++)
	{
		for(j = 0; j < 128; j++)
		{
			for(k = 0; k < 120; k += 8)
			{
				if(isblackout == 0) e_vram[j + i*LCD_WIDTH + k*LCD_WIDTH] =  data[i+k][j];
				else if(isblackout == 1) e_vram[j + i*LCD_WIDTH + k*LCD_WIDTH] = 0;
			}
		}

		mylib_msleep(30);
	}
}

/**********************************************************************/
void mylib_putpic(int x, int y, int width, int height,  
                  const unsigned int data[][width]){
    int i,j;

    for(i=0; i<height; i++)
        for(j=0; j<width; j++)
            e_vram[(x+j) + (y+i)*LCD_WIDTH] =  data[i][j];
}

/**********************************************************************/

void mylib_putupper(int x, int y, char c, int color){ //Big Char
    int i, j;
    
    for(i = 0; i < 20; i++){
        for(j = 0; j < 11; j++){
            if(e_upper[(int)(c-'A')][i][j]) 
                e_vram[(x+j)+(y+i)*LCD_WIDTH] = color;
        }
    }
}

/**********************************************************************/

void mylib_putlower(int x, int y, char c, int color){ //Small Char
    int i, j;
    
    for(i = 0; i < 20; i++){
        for(j = 0; j < 11; j++){
            if(e_lower[(int)(c-'a')][i][j]) 
                e_vram[(x+j)+(y+i)*LCD_WIDTH] = color;
        }
    }
}

/**********************************************************************/

void mylib_putsymbol(int x, int y, char c, int color){
    int i, j;
    
    for(i = 0; i < 20; i++){
        for(j = 0; j < 11; j++){
			if(c == '.')
			{
			    if(e_symbol[0][i][j]) 
					e_vram[(x+j)+(y+i)*LCD_WIDTH] = color;
			}
			else if(c == ',')
			{
			    if(e_symbol[1][i][j]) 
					e_vram[(x+j)+(y+i)*LCD_WIDTH] = color;
			}
        }
    }
}

/**********************************************************************/

void mylib_putnum(int x, int y, int num, int color){
    int i, j;
    
    for(i=0; i<20; i++){
        for(j=0; j<11; j++){
            if(e_number[num][i][j]) 
				e_vram[(x+j)+(y+i)*LCD_WIDTH] = color;
        }
    }
}

/**********************************************************************/

void mylib_putnum_ex(int x, int y, int num_ex, int color){

	int digit = 0; //桁数
	int tmp = num_ex;

	while(tmp != 0){ //桁数の算出
		tmp = tmp / 10;
		++digit;
	}

	int num = 0;
    int i, j, k;
    
	for(k = 0; k < digit; k++)
	{	
		switch(digit - k)
		{
			case 3:
				num = num_ex / 100;
				break;
			case 2:
				num = (num_ex - (num_ex / 100 * 100) - (num_ex % 10)) / 10;
				break;
			case 1:
				num = num_ex - (num_ex / 10 * 10);
				break;
		}

		for(i = 0; i < 20; i++)
		{
			for(j = 0; j < 11; j++)
			{
				if(e_number[num][i][j]) 
					e_vram[(x+j)+(y+i)*LCD_WIDTH] = color;
			}
		}
		x += 10;
	}

	if(digit == 0)
	{	
		x += 20;
		for(i = 0; i < 20; i++)
		{
			for(j = 0; j < 11; j++)
			{
				if(e_number[0][i][j]) 
					e_vram[(x+j)+(y+i)*LCD_WIDTH] = color;
			}
		}
	}
}

/**********************************************************************/

void mylib_time_clear(int color)
{
	int i, j;

    for(i = 0; i < 128; i++){
        for(j = 0; j < 128; j++){
			if(94 < j && j < 125 && 2 < i && i < 21)
			{
				e_vram[j + i * LCD_WIDTH] = color;
			}
        }
    }
}

/**********************************************************************/

int ctoi(const char c){
  switch(c){
    case '0': return 0;
    case '1': return 1;
    case '2': return 2;
    case '3': return 3;
    case '4': return 4;
    case '5': return 5;
    case '6': return 6;
    case '7': return 7;
    case '8': return 8;
    case '9': return 9;
    default : return -1;
  }
}

/**********************************************************************/

void DOText(int x, int y, char str[], int length, int color, int interval)
{
	int CurrentX = x;
	int CurrentY = y;

	int i;

	for(i = 0; i < length; i++)
	{
		if('a' <= str[i] && str[i] <= 'z')
		{
			mylib_putlower(CurrentX, CurrentY, str[i], color);
		}
		else if('A' <= str[i] && str[i] <= 'Z')
		{
			mylib_putupper(CurrentX, CurrentY, str[i], color);
		}
		else if('0' <= str[i] && str[i] <= '9')
		{
			mylib_putnum(CurrentX, CurrentY, ctoi(str[i]), color);
		}
		else if(',' == str[i] || str[i] == '.')
		{
			mylib_putsymbol(CurrentX, CurrentY, str[i], color);
		}
		else if(' ' == str[i])
		{
			CurrentX += 5;
			continue;
		}
		else if('\n' == str[i])
		{
			CurrentX = x;
			CurrentY += 20;
			continue;
		}
		else
		{
			//DO NOTHING
		}

		CurrentX += 10;
		if(interval != 0) mylib_msleep(interval);
	}
}

/**********************************************************************/

int playerpos[2] = {121, 121};

void mylib_move_player(int dir)
{
	e_vram[playerpos[0]     + playerpos[1]     * LCD_WIDTH] = 7;
	e_vram[(playerpos[0]+1) + playerpos[1]     * LCD_WIDTH] = 7;
	e_vram[playerpos[0]     + (playerpos[1]+1) * LCD_WIDTH] = 7;
	e_vram[(playerpos[0]+1) + (playerpos[1]+1) * LCD_WIDTH] = 7;
	
	if(dir == 1) playerpos[0] += 2;
	else if(dir == 3) playerpos[0] -= 2;
	else if(dir == 4) playerpos[1] -= 2;
	else if(dir == 2) playerpos[1] += 2;
	
	e_vram[playerpos[0]     + playerpos[1]     * LCD_WIDTH] = 4;
	e_vram[(playerpos[0]+1) + playerpos[1]     * LCD_WIDTH] = 4;
	e_vram[playerpos[0]     + (playerpos[1]+1) * LCD_WIDTH] = 4;
	e_vram[(playerpos[0]+1) + (playerpos[1]+1) * LCD_WIDTH] = 4;
}

/**********************************************************************/

int enemypos[77][2] = {
	{101, 123},

	{119, 107},

	{91, 93},
	{89, 105},
	{87, 93},

	{107, 89},
	{105, 87},
	{107, 85},
	{109, 87},

	{65, 107},
	{59, 105},
	{61, 99},
	{67, 101},

	{93, 65},
	{93, 67},

	{71, 65},
	{73, 65},

	{73, 43},
	{73, 45},

	{93, 45},
	{95, 45},

	{87, 59},
	{89, 59},

	{79, 59},
	{79, 61},

	{77, 51},
	{79, 51},

	{87, 49},
	{87, 51},

	{9, 117},
	{17, 119},
	{9, 121},

	{31, 97},
	{25, 99},
	{23, 93},
	{29, 91},

	{37, 69},
	{33, 69},
	{33, 65},
	{37, 65},

	{9, 69},
	{9, 65},
	{5, 71},
	{5, 67},
	{5, 63},

	{17, 47},
	{17, 45},
	{17, 43},
	{17, 41},
	{17, 39},
	{17, 37},
	{17, 35},
	{17, 33},
	{17, 31},
	{17, 29},
	{9, 47},
	{9, 45},
	{9, 43},
	{9, 41},
	{9, 39},
	{9, 37},
	{9, 35},
	{9, 33},
	{9, 31},
	{9, 29},

	{49, 37},
	{51, 37},
	{41, 33},
	{41, 35},
	{43, 25},
	{45, 25},
	{53, 27},
	{53, 29},

	{89, 9},
	{85, 9},
	{85, 5},
	{89, 5}
};

void mylib_move_enemy(int pos[2], int enemynum, int count, int amp, int dir)
{
	if(count % (5 * 2 * amp) < (5 * amp))
	{
		if(count % 5 == 0)
		{
			e_vram[pos[0]     + pos[1]      * LCD_WIDTH] = 7;
			e_vram[(pos[0]+1) + pos[1]      * LCD_WIDTH] = 7;
			e_vram[pos[0]     + (pos[1]+1)  * LCD_WIDTH] = 7;
			e_vram[(pos[0]+1) + (pos[1]+1)  * LCD_WIDTH] = 7;

			if(dir == 1) pos[0] += 2;
			if(dir == 3) pos[0] -= 2;
			if(dir == 4) pos[1] -= 2;
			if(dir == 2) pos[1] += 2;

			e_vram[pos[0]     + pos[1]      * LCD_WIDTH] = 1;
			e_vram[(pos[0]+1) + pos[1]      * LCD_WIDTH] = 1;
			e_vram[pos[0]     + (pos[1]+1)  * LCD_WIDTH] = 1;
			e_vram[(pos[0]+1) + (pos[1]+1)  * LCD_WIDTH] = 1;
		}
	}
	else
	{
		if(count % 5 == 0)
		{
			e_vram[pos[0]     + pos[1]      * LCD_WIDTH] = 7;
			e_vram[(pos[0]+1) + pos[1]      * LCD_WIDTH] = 7;
			e_vram[pos[0]     + (pos[1]+1)  * LCD_WIDTH] = 7;
			e_vram[(pos[0]+1) + (pos[1]+1)  * LCD_WIDTH] = 7;
			
			if(dir == 3) pos[0] += 2;
			if(dir == 1) pos[0] -= 2;
			if(dir == 2) pos[1] -= 2;
			if(dir == 4) pos[1] += 2;
			
			e_vram[pos[0]     + pos[1]      * LCD_WIDTH] = 1;
			e_vram[(pos[0]+1) + pos[1]      * LCD_WIDTH] = 1;
			e_vram[pos[0]     + (pos[1]+1)  * LCD_WIDTH] = 1;
			e_vram[(pos[0]+1) + (pos[1]+1)  * LCD_WIDTH] = 1;
		}
	}
}

/**********************************************************************/

void mylib_putexplode(int x, int y, const unsigned int data[4][20][20])
{
    int i,j,k;

	for(k = 0; k < 5; k++)
	{
		for(i = 0; i < 20; i++)
		{
			for(j = 0; j < 20; j++)
			{
				if(k == 0) e_vram[(x+j) + (y+i)*LCD_WIDTH] = 0;
				else e_vram[(x+j) + (y+i)*LCD_WIDTH] =  data[k - 1][i][j];
			}
		}

		mylib_msleep(60);
	}

	for(k = 0; k < 4; k++)
	{
		for(i = 0; i < 20; i++)
		{
			for(j = 0; j < 20; j++)
			{
				if(k == 3)  e_vram[(x+j) + (y+i)*LCD_WIDTH] = 0;
				else e_vram[(x+j) + (y+i)*LCD_WIDTH] =  data[2 - k][i][j];
				
			}
		}

		mylib_msleep(60);
	}
}

/**********************************************************************/

void mylib_putster(int x, int y, const unsigned int data[4][16][16], int count, int delayflame)
{
    int i,j,k;

	for(k = 0; k < 4; k++)
	{
		for(i = 0; i < 16; i++)
		{
			for(j = 0; j < 16; j++)
			{
				if(count / 12 % 4 == k) e_vram[(x+j) + (y+i)*LCD_WIDTH] =  data[(k + delayflame) % 4][i][j];
			}
		}
	}
}

/**********************************************************************/

#ifdef QEMU
extern void io_stihlt();
#endif
void mylib_msleep(unsigned int tm){
#ifdef QEMU
    extern int timer_count;
    timer_count = 0;
    while(timer_count <= (tm / 10)) {
        io_stihlt();
    }
#else
    unsigned int end = (unsigned int) *e_time + tm;
    while (*e_time < end);
#endif
}

/**********************************************************************/

void mylib_clear(int color){
    int i;
    for(i=0; i<LCD_HEIGHT*LCD_WIDTH; i++) e_vram[i] = color;
}

/**********************************************************************/

//0 -> タイトル待機画面
//1 -> 何か入力したら
//2 -> 迷路スタート

int main(){
    
	//初期化
    mylib_transition(pic_data[0], 0);

	int state = 0;
	int count = 0;
	int length = 0;
	int time = 0;
	
	char Press_Any_Key[] = "Press Any Key";

	int enemypos_tmp[77][2];

	int i, j;
	for(i = 0; i < 77; i++)
	{
		for(j = 0; j < 2; j++) enemypos_tmp[i][j] = enemypos[i][j];
	}

    while(1){

		//0 -> タイトル待機画面

		if(state == 0)
		{
			length = sizeof(Press_Any_Key) / sizeof(char);
			count = 0;
		}
		
		while(state == 0)
		{
			mylib_msleep(20);

			count++;
			
			if(count % 40 < 20) DOText(5, 100, Press_Any_Key, length, 7, 0);
			else DOText(5, 100, Press_Any_Key, length, 0, 0);
			
			if(*e_sw1==0 || *e_sw2==0 || *e_sw3==0) state = 1;
		}

		//1 -> 何か入力したら

		if(state == 1)
		{
			count = 0;

			int i;

			for(i = 0; i < 20; i++)
			{
				mylib_msleep(20);

				count++;
			
				if(count % 4 < 2) DOText(5, 100, Press_Any_Key, length, 7, 0);
				else DOText(5, 100, Press_Any_Key, length, 0, 0);
			}

			state = 2;

			mylib_msleep(300);
		}

		//2 -> 迷路スタート

		if(state == 2)
		{
			count = 0;
			time = 300;

			mylib_transition(pic_data[1], 0);

			e_vram[playerpos[0]     + playerpos[1]     * LCD_WIDTH] = 4;
			e_vram[(playerpos[0]+1) + playerpos[1]     * LCD_WIDTH] = 4;
			e_vram[playerpos[0]     + (playerpos[1]+1) * LCD_WIDTH] = 4;
			e_vram[(playerpos[0]+1) + (playerpos[1]+1) * LCD_WIDTH] = 4;

			mylib_putnum_ex(94, 3, time, 0);
		}

		int j;
		int ArraySize = sizeof enemypos_tmp / sizeof enemypos_tmp[0];
		
		while(state == 2)
		{
			//入力受け付け

			mylib_msleep(50);

			if(*e_sw1 == 0 && playerpos[0] >= 0 && pic_data[1][playerpos[1]][playerpos[0]-2] != 0) //left
			{
				mylib_move_player(3);
			}
			else if(*e_sw3 == 0 && playerpos[0] <= 128 && pic_data[1][playerpos[1]][playerpos[0]+2] != 0) //right
			{
				mylib_move_player(1);
			}
		    else if(*e_sw2 == 0 && playerpos[1] >= 0 && pic_data[1][playerpos[1]-2][playerpos[0]] != 0) //up
			{
				mylib_move_player(4);
			}

			mylib_msleep(10);
			
		    if(*e_sw1 == 0 && *e_sw3 == 0 && playerpos[1] <= 128 && pic_data[1][playerpos[1]+2][playerpos[0]] != 0) //down
			{
				mylib_move_player(2);
			}
			
			count++;
			
			//カウントダウン

			if(count % 20 == 0)
			{
				time--;
				mylib_time_clear(7);
				mylib_putnum_ex(94, 3, time, 0);
			}

			//敵の動き

			mylib_move_enemy(enemypos_tmp[0], 0, count, 21, 3);

			mylib_move_enemy(enemypos_tmp[1], 1, count, 4, 2);

			mylib_move_enemy(enemypos_tmp[2], 2, count, 8, 2);
			mylib_move_enemy(enemypos_tmp[3], 3, count, 8, 4);
			mylib_move_enemy(enemypos_tmp[4], 4, count, 8, 2);

			mylib_move_enemy(enemypos_tmp[5], 5, count, 4, 4);
			mylib_move_enemy(enemypos_tmp[6], 6, count, 4, 1);
			mylib_move_enemy(enemypos_tmp[7], 7, count, 4, 2);
			mylib_move_enemy(enemypos_tmp[8], 8, count, 4, 3);

			mylib_move_enemy(enemypos_tmp[9 ], 9 , count, 4, 3);
			mylib_move_enemy(enemypos_tmp[10], 10, count, 4, 4);
			mylib_move_enemy(enemypos_tmp[11], 11, count, 4, 1);
			mylib_move_enemy(enemypos_tmp[12], 12, count, 4, 2);

			mylib_move_enemy(enemypos_tmp[13], 13, count, 11, 3);
			mylib_move_enemy(enemypos_tmp[14], 14, count, 11, 3);

			mylib_move_enemy(enemypos_tmp[15], 15, count, 11, 4);
			mylib_move_enemy(enemypos_tmp[16], 16, count, 11, 4);

			mylib_move_enemy(enemypos_tmp[17], 17, count, 11, 1);
			mylib_move_enemy(enemypos_tmp[18], 18, count, 11, 1);

			mylib_move_enemy(enemypos_tmp[19], 19, count, 11, 2);
			mylib_move_enemy(enemypos_tmp[20], 20, count, 11, 2);

			mylib_move_enemy(enemypos_tmp[21], 21, count, 6, 4);
			mylib_move_enemy(enemypos_tmp[22], 22, count, 6, 4);

			mylib_move_enemy(enemypos_tmp[23], 23, count, 6, 1);
			mylib_move_enemy(enemypos_tmp[24], 24, count, 6, 1);

			mylib_move_enemy(enemypos_tmp[25], 25, count, 6, 2);
			mylib_move_enemy(enemypos_tmp[26], 26, count, 6, 2);

			mylib_move_enemy(enemypos_tmp[27], 27, count, 6, 3);
			mylib_move_enemy(enemypos_tmp[28], 28, count, 6, 3);

			mylib_move_enemy(enemypos_tmp[29], 28, count, 6, 1);
			mylib_move_enemy(enemypos_tmp[30], 29, count, 6, 3);
			mylib_move_enemy(enemypos_tmp[31], 31, count, 6, 1);

			mylib_move_enemy(enemypos_tmp[32], 32, count, 4, 4);
			mylib_move_enemy(enemypos_tmp[33], 33, count, 4, 1);
			mylib_move_enemy(enemypos_tmp[34], 34, count, 4, 2);
			mylib_move_enemy(enemypos_tmp[35], 35, count, 4, 3);

			mylib_move_enemy(enemypos_tmp[36], 36, count, 4, 4);
			mylib_move_enemy(enemypos_tmp[37], 37, count, 4, 1);
			mylib_move_enemy(enemypos_tmp[38], 38, count, 4, 2);
			mylib_move_enemy(enemypos_tmp[39], 39, count, 4, 3);

			mylib_move_enemy(enemypos_tmp[40], 40, count, 4, 3);
			mylib_move_enemy(enemypos_tmp[41], 41, count, 4, 3);
			mylib_move_enemy(enemypos_tmp[42], 42, count, 4, 1);
			mylib_move_enemy(enemypos_tmp[43], 43, count, 4, 1);
			mylib_move_enemy(enemypos_tmp[44], 44, count, 4, 1);

			mylib_move_enemy(enemypos_tmp[45], 45, count, 4, 3);
			mylib_move_enemy(enemypos_tmp[46], 46, count, 4, 3);
			mylib_move_enemy(enemypos_tmp[47], 47, count, 4, 3);
			mylib_move_enemy(enemypos_tmp[48], 48, count, 4, 3);
			mylib_move_enemy(enemypos_tmp[49], 49, count, 4, 3);
			mylib_move_enemy(enemypos_tmp[50], 50, count, 4, 3);
			mylib_move_enemy(enemypos_tmp[51], 51, count, 4, 3);
			mylib_move_enemy(enemypos_tmp[52], 52, count, 4, 3);
			mylib_move_enemy(enemypos_tmp[53], 53, count, 4, 3);
			mylib_move_enemy(enemypos_tmp[54], 54, count, 4, 3);
			mylib_move_enemy(enemypos_tmp[55], 55, count, 4, 3);
			mylib_move_enemy(enemypos_tmp[56], 56, count, 4, 3);
			mylib_move_enemy(enemypos_tmp[57], 57, count, 4, 3);
			mylib_move_enemy(enemypos_tmp[58], 58, count, 4, 3);
			mylib_move_enemy(enemypos_tmp[59], 59, count, 4, 3);
			mylib_move_enemy(enemypos_tmp[60], 60, count, 4, 3);
			mylib_move_enemy(enemypos_tmp[61], 61, count, 4, 3);
			mylib_move_enemy(enemypos_tmp[62], 62, count, 4, 3);
			mylib_move_enemy(enemypos_tmp[63], 63, count, 4, 3);
			mylib_move_enemy(enemypos_tmp[64], 64, count, 4, 3);

			mylib_move_enemy(enemypos_tmp[65], 65, count, 8, 4);
			mylib_move_enemy(enemypos_tmp[66], 66, count, 8, 4);
			mylib_move_enemy(enemypos_tmp[67], 67, count, 8, 1);
			mylib_move_enemy(enemypos_tmp[68], 68, count, 8, 1);
			mylib_move_enemy(enemypos_tmp[69], 69, count, 8, 2);
			mylib_move_enemy(enemypos_tmp[70], 70, count, 8, 2);
			mylib_move_enemy(enemypos_tmp[71], 71, count, 8, 3);
			mylib_move_enemy(enemypos_tmp[72], 72, count, 8, 3);

			mylib_move_enemy(enemypos_tmp[73], 73, count, 4, 3);
			mylib_move_enemy(enemypos_tmp[74], 74, count, 4, 4);
			mylib_move_enemy(enemypos_tmp[75], 75, count, 4, 1);
			mylib_move_enemy(enemypos_tmp[76], 76, count, 4, 2);

			if(time == 0) state = 3;

			for(j = 0; j < ArraySize; j++)
			{
				if(playerpos[0] == enemypos_tmp[j][0] && playerpos[1] == enemypos_tmp[j][1]) state = 3;
			}

			if(playerpos[0] == 3 && playerpos[1] == 7) state = 4;
		}
		
		//3 -> ゲームオーバー

		if(state == 3)
		{
			mylib_clear(0);
			mylib_putexplode(playerpos[0] - 10, playerpos[1] - 10, explode);
			
			mylib_msleep(250);

			char YOUDIED[] = "YOU DIED";
			length = sizeof(YOUDIED) / sizeof(char);
			DOText(26, 64 - 10, YOUDIED, length, 1, 50);
		}

		while(state == 3)
		{
			mylib_msleep(20);

			if(*e_sw1==0 || *e_sw2==0 || *e_sw3==0) 
			{
				state = 0;
				playerpos[0] = 121;
				playerpos[1] = 121;
				int i, j;
				for(i = 0; i < 77; i++)
				{
					for(j = 0; j < 2; j++) enemypos_tmp[i][j] = enemypos[i][j];
				}

				mylib_transition(pic_data[0], 0);
			}
		}

		//4 -> ゲームクリア

		if(state == 4)
		{
			mylib_transition(pic_data[0], 1);

			count = 0;

			char CLEAR[] = "CLEAR";
			length = sizeof(CLEAR) / sizeof(char);
			DOText(39, 64 - 10, CLEAR, length, 3, 50);
		}

		while(state == 4)
		{
			mylib_putster(17, 18, ster, count, 0);
			mylib_putster(79, 6, ster, count, 3);
			mylib_putster(6, 68, ster, count, 5);
			mylib_putster(106, 60, ster, count, 5);
			mylib_putster(34, 101, ster, count, 4);
			mylib_putster(97, 94, ster, count, 2);
			
			mylib_msleep(5);
			count++;

			if(*e_sw1==0 || *e_sw2==0 || *e_sw3==0) 
			{
				state = 0;
				playerpos[0] = 121;
				playerpos[1] = 121;
				int i, j;
				for(i = 0; i < 77; i++)
				{
					for(j = 0; j < 2; j++) enemypos_tmp[i][j] = enemypos[i][j];
				}

				mylib_transition(pic_data[0], 0);
			}
		}
    }
}
/**********************************************************************/

#include <gb/gb.h>
#include <gb/drawing.h>
#include <stdio.h>
#include <rand.h>
#include "logo_map.c"
#include "logo_data.c"
#include "chunkSprites.c"

//game vairables
UWORD seed;
UINT16 score=0;
UINT8 curScroll;
UINT8 numBlocks=1;
UINT8 maxBlocks=10;
UINT8 lives = 3;
UINT8 paused;

struct chunk {
  UINT8 x;
  UINT8 y;
  UINT8 scale;
  UINT8 vis;
};

struct chunk chunks[30];

BOOLEAN collision(UINT8 objaX, UINT8 objaY, UINT8 objaW, UINT8 objaH, UINT8 objbX, UINT8 objbY, UINT8 objbW, UINT8 objbH)
{
	return objaX < objbX + objbW && objaX + objaW > objbX && objaY < objbY + objbH && objaY + objaH > objbY;
}

void performantDelay(UINT8 numloops)
{
	UINT8 ii;
for(ii = 0; ii < numloops; ii++)
{
wait_vbl_done();
}
}
UINT8 i;
void fadeOut()
{
	for(i=0;i<4;i++)
	{
switch(i)
{
case 0: BGP_REG = 0xE4; break;
case 1: BGP_REG = 0xF9; break;
case 2: BGP_REG = 0xFE; break;
case 3: BGP_REG = 0xFF; break;
}
performantDelay(10);
	}
}

void moveScrollBar()
{
	move_sprite(4+maxBlocks, curScroll - ((GRAPHICS_WIDTH/2) - 20)+8, GRAPHICS_HEIGHT+8-3);//scrollLeft
	move_sprite(5+maxBlocks, curScroll - ((GRAPHICS_WIDTH/2) - 20)+8+8, GRAPHICS_HEIGHT+8-3);//scrollMid1
	move_sprite(6+maxBlocks, curScroll - ((GRAPHICS_WIDTH/2) - 20)+8+8+8, GRAPHICS_HEIGHT+8-3);//scrollMid2
	move_sprite(7+maxBlocks, curScroll - ((GRAPHICS_WIDTH/2) - 20)+8+8+8+8, GRAPHICS_HEIGHT+8-3);//scrollMid3
	move_sprite(8+maxBlocks, curScroll - ((GRAPHICS_WIDTH/2) - 20)+8+8+8+8+8, GRAPHICS_HEIGHT+8-3);//scrollRight
}

void moveScrollLeft()
{
	if(curScroll >= 60)
	{
  moveScrollBar();
  curScroll--;
	}
}

UINT8 randG(UINT8 min, UINT8 max)
{
	INT8 varRand = min + rand() % (max+1 - min);
	if(varRand < min)
	return min;
	if(varRand > max)
	return max - randG(min, max / 2);
	return varRand;
}

void goodbeep()
{
NR10_REG = 0x40;
NR11_REG = 0x40;
NR12_REG = 0x73;
NR13_REG = 0x00;
NR14_REG = 0xC3;
}

void badbeep()
{
NR10_REG = 0x7F;
NR11_REG = 0x40;
NR12_REG = 0x73;
NR13_REG = 0x00;
NR14_REG = 0xC8;
}

void startBeep()
{

	NR10_REG = 0x4F;
NR11_REG = 0x4F;
NR12_REG = 0x75;
NR13_REG = 0x0F;
NR14_REG = 0xCE;
}

void endbeep()//work on it
{
NR10_REG = 0x79;
NR11_REG = 0x40;
NR12_REG = 0x73;
NR13_REG = 0x00;
NR14_REG = 0xC3;
}

void moveScrollRight()
{
	if(curScroll <= GRAPHICS_WIDTH+21)
	{
 moveScrollBar();
  curScroll++;
	}
}
UBYTE key;
void buttonCmds()
{
	key = joypad();
	if(!paused)
	{
	if(key & J_B)
	{
		curScroll = 60;
		moveScrollBar();
	}
	if(key & J_A)
	{
		curScroll = GRAPHICS_WIDTH+20;
		moveScrollBar();
	}
	if(key & J_DOWN)
	{
		curScroll = 120;
		moveScrollBar();
	}
	if(key & J_LEFT)
		moveScrollLeft();
	if(key & J_RIGHT)
		moveScrollRight();
	}
	if(key & J_SELECT)
		{
				if(BGP_REG == 0xE4)
				BGP_REG = 0x1B;
				else
				BGP_REG = 0xE4;
				performantDelay(15);
		}
		if(key & J_START)
		{
			if(paused == 1)
			paused = 0;
			else
			paused = 1;
			performantDelay(15);
		}
}
UINT8 b;
void gameLogic()
{
delay(10);
if(!paused)
	for(b = 0; b < numBlocks; b++)
	{
		if(key & J_UP)
		chunks[b].y+=2;
		else
		chunks[b].y++;
		if(chunks[b].y > 156)
		{
			if(chunks[b].vis == 1)
			{
			lives--;
			if(lives < 3)
			move_sprite(2, 0, 0);
			if(lives < 2)
			move_sprite(1, 0, 0);
			if(lives < 1)
			move_sprite(0, 0, 0);
			
			badbeep();
			if(lives==0)
			endbeep();
			}

		
		chunks[b].y = 0;
		chunks[b].x = randG(7, 161);
			chunks[b].vis = 1;
		
		}
		if(chunks[b].vis)
		{
		move_sprite(3+b, chunks[b].x, chunks[b].y);
		for(i=0;i<numBlocks;i++)//chunk collision detection
		{
			if(i!=b)
			if(collision(chunks[b].x, chunks[b].y, chunks[b].scale, chunks[b].scale, chunks[i].x, chunks[i].y, chunks[i].scale, chunks[i].scale))
			chunks[i].vis = 0;
		}

		//scrollColDet
		if(collision(chunks[b].x, chunks[b].y, chunks[b].scale, chunks[b].scale, curScroll - ((GRAPHICS_WIDTH/2) - 20)+8, GRAPHICS_HEIGHT+9, 40, 3))
		{
			chunks[b].vis = 0;
			chunks[b].x = randG(7, 161);
			chunks[b].y = randG(0, 155);
			move_sprite(3+b, 0, 0);
			score++;
                if(numBlocks < maxBlocks){
                    if(score/10 > 2)
                      {  numBlocks = round(score/10);}
                    else{
                        if(score > 2)
                          {  numBlocks = 2;}
                        else{
                            numBlocks = 1;}}}
			goodbeep();
		}
		}
	}
	
}

void registerSprites()
{
set_sprite_data(0, 5, TileLabel);
set_sprite_tile(0,4);//heart 1
move_sprite(0, 10, 18);

set_sprite_tile(1,4);//heart 2
move_sprite(1, 18, 18);

set_sprite_tile(2,4);//heart 3
move_sprite(2, 18+8, 18);

//defineChunks
for(i = 0; i < maxBlocks; i++)
{
    set_sprite_tile(3+i,0);//chunk
    move_sprite(3+i, 10*i, 0);//randomize X for game
}

//define scrollbar 3 piece
set_sprite_tile(4+maxBlocks,1);//scrollLeft
set_sprite_tile(5+maxBlocks,2);//scrollMid1
set_sprite_tile(6+maxBlocks,2);//scrollMid2
set_sprite_tile(7+maxBlocks,2);//scrollMid3
set_sprite_tile(8+maxBlocks,3);//scrollRight
SHOW_SPRITES;
}


UINT8 gameRun;
void gameHook()
{
	HIDE_BKG;
	registerSprites();
	curScroll = 120;
	moveScrollBar();
	for(i=0;i<maxBlocks;i++)//randomize chunks
	{
		chunks[i].x = randG(7, 161);
		chunks[i].y = randG(0, 150);
		chunks[i].scale = 6;
		chunks[i].vis = 0;
	}
	for(i=0;i<3;i++)//fade in
	{
switch(i)
{
	case 0: BGP_REG = 0xFE; break;
	case 1: BGP_REG = 0xF9; break;
	case 2: BGP_REG = 0xE4; break;
}
performantDelay(10);
	}
while(gameRun)
{
if(lives != 0)
{
buttonCmds();
gameLogic();
}
else
{
HIDE_SPRITES;
 color(BLACK,DKGREY,SOLID);
line(0,0,0,0);
    color(BLACK,DKGREY,SOLID);
    box(0,0,GRAPHICS_WIDTH-1,65,M_FILL);
	color(WHITE,DKGREY,SOLID);
	gotogxy(5, 1);
	gprint("GAME OVER!");
	gotogxy(1, 5);
	gprint("PRESS A TO");
	gotogxy(8, 6);
	gprint("PLAY AGAIN!");
	gotogxy(1, 10);
	color(DKGREY,WHITE,SOLID);
	gprint("MATCH SCORE:");
	gotogxy(1, 11);
	gprintf("%d", score);
	gotogxy(1, 14);
	gprint("MADE BY:");
	gotogxy(0, 15);
	gprint("Nicholas Hryckewycz");
	gotogxy(0, 16);
	gprint("2023");
	waitpad(J_A);
	color(BLACK,BLACK,SOLID);
    box(0,0,GRAPHICS_WIDTH,GRAPHICS_HEIGHT,M_FILL);
	reset();
}
}
}

void main()
{
	NR52_REG = 0x80;
	NR50_REG = 0x77;
	NR51_REG = 0xFF;
set_bkg_data(0, 237, logo_data);
set_bkg_data(0xED, 10, font_data);
set_bkg_tiles(0,0,20,18, logo_map);
SHOW_BKG;
DISPLAY_ON;

waitpad(J_A);


seed = LY_REG;
 seed |= (UWORD)DIV_REG << 8;
initrand(seed);
startBeep();
fadeOut();
//start game dev here
gameRun = 1;
gameHook();
}
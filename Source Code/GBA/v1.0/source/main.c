#include <gba_video.h>
#include <gba_interrupt.h>
#include <gba_systemcalls.h>
#include <gba_input.h>
#include <stdlib.h>

#define KEY_ANY    0x03FF

#define FB_WIDTH  240
#define FB_HEIGHT 160

int lives = 3,
scrollWidth = 80,
scrollPosX = 0,
numberOfBlocks = 1,
maxBlocks = 20,
godLeft = 0,
scrollPercentage = 30,
score = 0,
blocksX[100],
blocksY[100],
blockScale[100],
blockR[100],
blockG[100],
blockB[100];

bool blocksVis[100],
gamePaused = false,
blockGod = false;

bool collision(int objaX, int objaY, int objaW, int objaH, int objbX, int objbY, int objbW, int objbH)
{
	return objaX < objbX + objbW && objaX + objaW > objbX && objaY < objbY + objbH && objaY + objaH > objbY;
}

void drawRect(int x, int y, int width, int height, u8 r, u8 g, u8 b){
	int w; int h;
	for(w = 0; w < width; w++)
		for(h = 0; h < height; h++)
			((u16*)MODE3_FB)[(y+h)*SCREEN_WIDTH+(x+w)]= ((r & 0x1F) | (g & 0x1F) << 5 | (b & 0x1f) << 10);
}

void resetBlock(int block)
{
	bool flag = block == 1 && rand() % 30 + 1 == 20;
	blockR[block] = flag ? 0 : rand() % 255 + 30;
	blockG[block] = flag ? 0 : rand() % 255 + 30;
	blockB[block] = flag ? 0 : rand() % 255 + 30;
	blockScale[block] = rand() % 6 + 4;
	blocksX[block] = rand() % (FB_WIDTH - blockScale[block]) + 1;
	blocksY[block] = -60;
	if(flag)
		blockGod = true;
}

void drawShaders()
{
	for (int i = 0; i < numberOfBlocks; i++)
	{
		if (blockGod == true && i == 1  && blockR[i] == 0 && blockG[i] == 0 && blockB[i] == 0)
			drawRect(blocksX[i] -2, blocksY[i] - 2, blockScale[i] + 4, blockScale[i] + 4, 255, 255, 255);
		if (blocksVis[i] == true)
			drawRect(blocksX[i], blocksY[i], blockScale[i], blockScale[i], blockR[i], blockG[i], blockB[i]);
	}

	bool flag = godLeft == 0;
	drawRect(scrollPosX, FB_HEIGHT - 20, scrollWidth, 10, flag ? 41 : 211, flag ? 41 : 175, flag ? 41 : 56);

	for (int i = 0; i != lives; i++)
	{
		drawRect(4 + (i * 12), 4, 10, 10, 0, 255, 0);
	}

	if (gamePaused)
	{
		drawRect((FB_WIDTH / 2) - 20, (FB_HEIGHT / 2) - 20, 20, 40, 255, 255, 255);
		drawRect((FB_WIDTH / 2) + 20, (FB_HEIGHT / 2) - 20, 20, 40, 255, 255, 255);
	}
}

void gameLoop()
{
	if (gamePaused != true)
	{
		if (godLeft > 0)
		{
			scrollPercentage = 60; 
			godLeft--; 
			if (godLeft == 0)
			{
				for (int i = 0; i < 100; i++)
				{
					blocksVis[i] = false;
				}
				blockGod = false;
			}
		}
		else if (godLeft == 0) { scrollPercentage = 35; }
		scrollWidth = FB_WIDTH / 100 * scrollPercentage;

		if (lives == 0)
		{
			godLeft = 0;
			score = 0;
			numberOfBlocks = 1;
			lives = 3;
			for (int i = 0; i < 100; i++)
			{
				blocksVis[i] = false;
			}
		}
		for (int i = 0; i < numberOfBlocks; i++)
		{
			for (int b = 0; b < numberOfBlocks; b++)
			{
				if (collision(blocksX[i], blocksY[i], blockScale[i], blockScale[i], blocksX[b], blocksY[b], blockScale[b], blockScale[b]) && b != i)
				{
					if (blocksVis[i] == blocksVis[b])
					{
						blocksVis[i] = false;
						resetBlock(i);
						blocksVis[b] = false;
						blocksY[b] = rand() % (FB_HEIGHT - blockScale[i]) + 1;
					}
				}
			}
			if (blocksY[i] > -60)
			{
				if (blocksY[i] < FB_HEIGHT)
				{
					bool flag = godLeft > 0;
				
					blocksY[i] = blocksY[i] + (flag ? 2 : 1);
				}
				else
				{
					resetBlock(i);
					if (blocksVis[i] == true) { lives--; score = 0; }
				}
				if (collision(blocksX[i], blocksY[i], blockScale[i], blockScale[i], scrollPosX, FB_HEIGHT - 20, scrollWidth, 10))
				{
					if (blocksVis[i] == true)
					{
						if (i == 1 && blockGod == true) { godLeft = 400; }
						score++;
						if (score >= 500 && lives <= 5)
						{
							score = 0;
							if(lives <= 5)
							lives++;
						}
						resetBlock(i);
						if (numberOfBlocks < maxBlocks) { numberOfBlocks++; }
					}
				}
			}
			if (blocksY[i] == -60)
			{
				blocksY[i] = -59;
				blocksX[i] = rand() % (FB_WIDTH - blockScale[i]) + 1;
				blocksVis[i] = true;
			}
		}
	}
}

void buttonMonitoring()
{
	scanKeys();
    u32 key_states = ~REG_KEYINPUT & KEY_ANY;
	if (gamePaused != true)
	{
		if (key_states & KEY_LEFT)//LKEY_L
		{
			if (scrollPosX >= 8)
			{
				scrollPosX = scrollPosX - 8;
			}
			else if (scrollPosX >= 1)
			{
				scrollPosX = scrollPosX - 1;
			}
		}
		if (key_states & KEY_RIGHT)
		{
			if (scrollPosX <= FB_WIDTH - (scrollWidth + 8))
			{
				scrollPosX = scrollPosX + 8;
			}
			else if (scrollPosX <= FB_WIDTH - (scrollWidth + 1))
			{
				scrollPosX = scrollPosX + 1;
			}
		}
		if (key_states & KEY_L)
		{
			scrollPosX = 0;
		}
		if (key_states & KEY_R)
		{
			scrollPosX = FB_WIDTH - scrollWidth;
		}
	}
	if (key_states & KEY_START)
	{
		gamePaused = !gamePaused;
	}
}


int main()
{
	SetMode(MODE_3 | BG2_ON);
	irqInit();
	irqEnable(IRQ_VBLANK);
	
	scrollPosX = (FB_WIDTH / 2) - (scrollWidth / 2);

	for (int i = 0; i < 100; i++)
	{
		blocksY[i] = rand() % (FB_HEIGHT - 36) + 1;
		blocksX[i] = rand() % (FB_WIDTH - 36) + 1;
	}
	
	while(1)
	{
		drawRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, 0);
		VBlankIntrWait();
		buttonMonitoring();
		VBlankIntrWait();
		drawShaders();
		gameLoop();
		VBlankIntrWait();
	}
	return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <switch.h>


#define FB_WIDTH  1280
#define FB_HEIGHT 720

int lives = 3,
scrollWidth = 400,
scrollPosX = 0,
numberOfBlocks = 1,
blocksX[100],
blocksY[100],
blockScale[100],
blockR[100],
blockG[100],
blockB[100];

bool blocksVis[100],
gamePaused = false;

bool collision(int objaX, int objaY, int objaW, int objaH, int objbX, int objbY, int objbW, int objbH)
{
	return objaX < objbX + objbW && objaX + objaW > objbX && objaY < objbY + objbH && objaY + objaH > objbY;
}

void drawRect(u32 x, u32 y, u32 width, u32 height, u32* framebuf, u32 maxwidth, u32 maxheight, int r, int g, int b) {
	u32 pos, actualx = x + width, actualy = y + height, curX;
	while (y < actualy) {
		y++;
		if (y >= maxheight)
			break;
		for (curX = x; curX < actualx; curX++) {
			if (x >= maxwidth)
				break;
			pos = y * maxwidth + curX;
			framebuf[pos] = RGBA8_MAXALPHA(r, g, b);
		}
	}
}

void resetBlock(int block)
{
	blockR[block] = rand() % 255 + 1;
	blockG[block] = rand() % 255 + 1;
	blockB[block] = rand() % 255 + 1;
	blockScale[block] = rand() % 36 + 26;
	blocksX[block] = rand() % (FB_WIDTH - blockScale[block]) + 1;
	blocksY[block] = 0;
}

void drawShaders(u32* frame, u32 maxwidth, u32 maxheight)
{
	drawRect(0, 0, maxwidth, maxheight, frame, maxwidth, maxheight, 62, 62, 62);
	for (int i = 0; i < numberOfBlocks; i++)
	{
		if (blocksVis[i] == true)
			drawRect(blocksX[i], blocksY[i], blockScale[i], blockScale[i], frame, maxwidth, maxheight, blockR[i], blockG[i], blockB[i]);
	}
	drawRect(10, 10, 40, 40, frame, maxwidth, maxheight, 41, 41, 41);
	drawRect(60, 10, 40, 40, frame, maxwidth, maxheight, 41, 41, 41);
	drawRect(110, 10, 40, 40, frame, maxwidth, maxheight, 41, 41, 41);
	drawRect(scrollPosX, maxheight - 50, scrollWidth, 40, frame, maxwidth, maxheight, 41, 41, 41);
	if (lives == 3) {
		drawRect(12, 12, 36, 36, frame, maxwidth, maxheight, 0, 164, 35);
		drawRect(62, 12, 36, 36, frame, maxwidth, maxheight, 0, 164, 35);
		drawRect(112, 12, 36, 36, frame, maxwidth, maxheight, 0, 164, 35);
	}
	else if (lives == 2)
	{
		drawRect(12, 12, 36, 36, frame, maxwidth, maxheight, 0, 164, 35);
		drawRect(62, 12, 36, 36, frame, maxwidth, maxheight, 0, 164, 35);
		drawRect(112, 12, 36, 36, frame, maxwidth, maxheight, 195, 0, 0);
	}
	else if (lives == 1)
	{
		drawRect(12, 12, 36, 36, frame, maxwidth, maxheight, 0, 164, 35);
		drawRect(62, 12, 36, 36, frame, maxwidth, maxheight, 195, 0, 0);
		drawRect(112, 12, 36, 36, frame, maxwidth, maxheight, 195, 0, 0);
	}
	else if (lives == 0)
	{
		drawRect(12, 12, 36, 36, frame, maxwidth, maxheight, 195, 0, 0);
		drawRect(62, 12, 36, 36, frame, maxwidth, maxheight, 195, 0, 0);
		drawRect(112, 12, 36, 36, frame, maxwidth, maxheight, 195, 0, 0);
	}
	if (gamePaused)
	{
		drawRect((maxwidth / 2) - 20, (maxheight / 2) - 20, 20, 40, frame, maxwidth, maxheight, 255, 255, 255);
		drawRect((maxwidth / 2) + 20, (maxheight / 2) - 20, 20, 40, frame, maxwidth, maxheight, 255, 255, 255);
	}
}

int main(int argc, char* argv[])
{
	NWindow* win = nwindowGetDefault();

	Framebuffer fb;
	framebufferCreate(&fb, win, FB_WIDTH, FB_HEIGHT, PIXEL_FORMAT_RGBA_8888, 2);
	framebufferMakeLinear(&fb);

	scrollPosX = (FB_WIDTH / 2) - (scrollWidth / 2);

	for (int i = 0; i < 100; i++)
	{
		blocksY[i] = rand() % (FB_HEIGHT - 36) + 1;
		blocksX[i] = rand() % (FB_WIDTH - 36) + 1;
	}

	while (appletMainLoop())
	{
		hidScanInput();

		u64 kDown = hidKeysHeld(CONTROLLER_P1_AUTO), jDown = hidKeysDown(CONTROLLER_P1_AUTO);
		if (gamePaused != true) {
			if (lives == 0) 
			{ 
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
						blocksY[i] = rand() % (FB_HEIGHT - blockScale[i]) + 1;
						blocksVis[i] = false;
						resetBlock(i);
					}
				}
				if (blocksY[i] > 0)
				{
					if (blocksY[i] < FB_HEIGHT)
					{
						blocksY[i]++;
					}
					else
					{
						resetBlock(i);
						if (blocksVis[i] == true) { lives--; }
					}
					if (collision(blocksX[i], blocksY[i], blockScale[i], blockScale[i], scrollPosX, FB_HEIGHT - 50, scrollWidth, 40))
					{
						if (blocksVis[i] == true)
						{
							resetBlock(i);
							if (numberOfBlocks < 20) { numberOfBlocks++; }
						}
					}
				}
				if (blocksY[i] == 0)
				{
					blocksY[i] = 1;
					blocksX[i] = rand() % (FB_WIDTH - blockScale[i]) + 1;
					blocksVis[i] = true;
				}
			}

			if (kDown & KEY_L && scrollPosX >= 8)
			{
				scrollPosX = scrollPosX - 8;
			}
			if (kDown & KEY_R && scrollPosX <= FB_WIDTH - scrollWidth)
			{
				scrollPosX = scrollPosX + 8;
			}
			if (kDown & KEY_ZL)
			{
				scrollPosX = 0;
			}
			if (kDown & KEY_ZR)
			{
				scrollPosX = FB_WIDTH - scrollWidth;
			}
		}
		if (jDown & KEY_PLUS)
		{
			gamePaused = !gamePaused;
		}
		u32 stride;
		u32* framebuf = (u32*)framebufferBegin(&fb, &stride);

		drawShaders(framebuf, FB_WIDTH, FB_HEIGHT);

		framebufferEnd(&fb);
	}

	framebufferClose(&fb);
	return 0;
}
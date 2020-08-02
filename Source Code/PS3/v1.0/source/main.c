#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <math.h>
#include <sysmodule/sysmodule.h>
#include <tiny3d.h>
#include <libfont.h>
#include "pad.h"
#include "rsxutil.h"

int FB_WIDTH = 0, FB_HEIGHT = 0;

void screenRes()
{
	videoState state;
	videoResolution resolution;
	if (videoGetState(0, 0, &state) == 0 &&
		videoGetResolution(state.displayMode.resolution, &resolution) == 0)
	{
		if (resolution.height == 1080)
		{
			FB_WIDTH = resolution.width / 100 * 45;
			FB_HEIGHT = resolution.height / 100 * 55;
		}
		else if (resolution.height == 720)
		{
			FB_WIDTH = resolution.width / 100 * 72;
			FB_HEIGHT = resolution.height / 100 * 72;
		}
		else if (resolution.height == 480)
		{
			FB_WIDTH = resolution.width / 100 * 120;
			FB_HEIGHT = resolution.height / 100 * 120;
		}
	}
}

int lives = 3,
scrollWidth = 300,
scrollPosX = 0,
numberOfBlocks = 1,
maxBlocks = 30,
blocksX[100],
blocksY[100],
blockScale[100];

float blockR[100],
blockG[100],
blockB[100];

bool blocksVis[100],
gamePaused = false;

bool collision(int objaX, int objaY, int objaW, int objaH, int objbX, int objbY, int objbW, int objbH)
{
	return objaX < objbX + objbW && objaX + objaW > objbX && objaY < objbY + objbH && objaY + objaH > objbY;
}

void drawShader(float x, float y, float Width, float Height, float r, float g, float b)
{
	tiny3d_SetPolygon(TINY3D_QUADS);
	tiny3d_VertexPos(x, y, 1);
	tiny3d_VertexFcolor(r, g, b, 1.0f);
	tiny3d_VertexPos(x + Width, y, 1);
	tiny3d_VertexPos(x + Width, y + Height, 1);
	tiny3d_VertexPos(x, y + Height, 1);
	tiny3d_End();
}

void drawScene()
{
	tiny3d_Project2D();

	drawShader(0, 0, FB_WIDTH, FB_HEIGHT, 0.255f, 0.255f, 0.255f);
	int i;
	for (i = 0; i < numberOfBlocks; i++)
	{
		if (blocksVis[i] == true)
			drawShader(blocksX[i], blocksY[i], blockScale[i], blockScale[i], blockR[i] / 255, blockG[i] / 255, blockB[i] / 255);
	}
	drawShader(10, 10, 40, 40, 0.16f, 0.16f, 0.16f);
	drawShader(60, 10, 40, 40, 0.16f, 0.16f, 0.16f);
	drawShader(110, 10, 40, 40, 0.16f, 0.16f, 0.16f);
	drawShader(scrollPosX, FB_HEIGHT - 50, scrollWidth, 40, 0.16f, 0.16f, 0.16f);
	if (lives == 3) {
		drawShader(12, 12, 36, 36, 0.0f, 0.64f, 0.14f);
		drawShader(62, 12, 36, 36, 0.0f, 0.64f, 0.14f);
		drawShader(112, 12, 36, 36, 0.0f, 0.64f, 0.14f);
	}
	else if (lives == 2)
	{
		drawShader(12, 12, 36, 36, 0.0f, 0.64f, 0.14f);
		drawShader(62, 12, 36, 36, 0.0f, 0.64f, 0.14f);
		drawShader(112, 12, 36, 36, 0.76f, 0.0f, 0.0f);
	}
	else if (lives == 1)
	{
		drawShader(12, 12, 36, 36, 0.0f, 0.64f, 0.14f);
		drawShader(62, 12, 36, 36, 0.76f, 0.0f, 0.0f);
		drawShader(112, 12, 36, 36, 0.76f, 0.0f, 0.0f);
	}
	else if (lives == 0)
	{
		drawShader(12, 12, 36, 36, 0.76f, 0.0f, 0.0f);
		drawShader(62, 12, 36, 36, 0.76f, 0.0f, 0.0f);
		drawShader(112, 12, 36, 36, 0.76f, 0.0f, 0.0f);
	}
	if (gamePaused)
	{
		drawShader((FB_WIDTH / 2) - 20, (FB_HEIGHT / 2) - 20, 20, 40, 1.0f, 1.0f, 1.0f);
		drawShader((FB_WIDTH / 2) + 20, (FB_HEIGHT / 2) - 20, 20, 40, 1.0f, 1.0f, 1.0f);
	}
}

void resetBlock(int block)
{
	blockR[block] = rand() % 255 + 1;
	blockG[block] = rand() % 255 + 1;
	blockB[block] = rand() % 255 + 1;
	blockScale[block] = rand() % 28 + 20;
	blocksX[block] = rand() % (FB_WIDTH - blockScale[block]) + 1;
	blocksY[block] = -60;
}

void exiting()
{
}

s32 main(s32 argc, const char* argv[])
{
	tiny3d_Init(1 * 1);
	ioPadInit(7);
	atexit(exiting);
	screenRes();
	scrollPosX = (FB_WIDTH / 2) - (scrollWidth / 2);

	int l;
	for (l = 0; l < 100; l++)
	{
		blocksY[l] = rand() % (FB_HEIGHT - 36) + 1;
		blocksX[l] = rand() % (FB_WIDTH - 36) + 1;
	}
	while (1) {
		static long frame_count = 0;
		ps3pad_read();
		if (gamePaused != true) {
			if (lives == 0)
			{
				numberOfBlocks = 1;
				lives = 3;
				int i;
				for (i = 0; i < 100; i++)
				{
					blocksVis[i] = false;
				}
			}
			int i;
			for (i = 0; i < numberOfBlocks; i++)
			{
				int b;
				for (b = 0; b < numberOfBlocks; b++)
				{
					if (collision(blocksX[i], blocksY[i], blockScale[i], blockScale[i], blocksX[b], blocksY[b], blockScale[b], blockScale[b]) && b != i || blocksY[i] == blocksY[b] && b != i)
					{
						if (blocksVis[i] == blocksVis[b]) {
							blocksVis[i] = false;
							resetBlock(i);
							blocksVis[b] = false;
							blocksY[b] = rand() % 200 + 100;
						}
					}
				}
				if (blocksY[i] > -60)
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
			if (old_pad & BUTTON_L1)
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
			if (old_pad & BUTTON_R1)
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
			if (old_pad & BUTTON_L2)
			{
				scrollPosX = 0;
			}
			if (old_pad & BUTTON_R2)
			{
				scrollPosX = FB_WIDTH - scrollWidth;
			}
		}
		if (new_pad & BUTTON_START)
		{
			gamePaused = !gamePaused;
		}

		tiny3d_Clear(0xff000000, TINY3D_CLEAR_ALL);
		drawScene();
		tiny3d_Flip();
		frame_count++;
	}
	return 0;
}
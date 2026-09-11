
//#include <SDL.h>
#include "N64.h"
#include <stdio.h>
#include <string.h>

#include "DoomRPG.h"
#include "DoomCanvas.h"
#include "Entity.h"
#include "EntityDef.h"
#include "Game.h"
#include "Player.h"
#include "Render.h"
#include "SDL_Video.h"

#define FIXED_VERSION 1

Render_t* Render_init(Render_t* render, DoomRPG_t* doomRpg)
{
	testsub("Render_init");

	if (render == NULL)
	{
		render = SDL_malloc(sizeof(Render_t));
		if (render == NULL) {
			return NULL;
		}
	}
	SDL_memset(render, 0, sizeof(Render_t));

	//resourceAsStream.Init(&renderClass->mapFile, doomRPGClass, 1);
	render->doomRpg = doomRpg;
	render->skipStretch = 0;
	render->unk4 = 0;
	render->skipCull = 0;
	render->skipBSP = 0;
	render->skipLines = 0;
	render->unk5 = 0;
	render->skipSprites = 0;
	render->skipViewNudge = 0;
	render->ioBufferPos = 0;
	render->lines = NULL;
	render->nodes = NULL;
	render->mapSprites = NULL;
	render->mapCameraSpawnIndex = 0;
	render->floorColor = NULL;
	render->ceilingColor = NULL;
	render->ceilingTex = 0;
	render->floorTex = 0;
	render->columnScale = NULL;
	render->animFrameTime = 0;
	render->mapStringsIDs = NULL;
	render->mapStringCount = 0;

	return render;
}

void Render_setup(Render_t* render, SDL_Rect* windowRect)
{
	testsub("Render_setup");

	boolean memError = false;
	render->screenWidth = windowRect->w;
	render->screenHeight = windowRect->h;
	render->screenX = windowRect->x;
	render->screenY = windowRect->y;
	if ((windowRect->h & 1) != 0) {
		render->screenHeight = windowRect->h - 1;
	}
	render->halfScreenWidth = render->screenWidth / 2;
	render->halfScreenHeight = render->screenHeight / 2;
	render->fracHalfScreenWidth = (render->halfScreenWidth << FRACBITS) - 0x8000;
	render->fracHalfScreenHeight = (render->halfScreenHeight << FRACBITS) - 0x8000;

#if 0
	printf("render->screenWidth %d\n", render->screenWidth);
	printf("render->screenHeight %d\n", render->screenHeight);
	printf("render->screenX %d\n", render->screenX);
	printf("render->screenY %d\n", render->screenY);
	printf("render->halfScreenWidth %d\n", render->halfScreenWidth);
	printf("render->halfScreenHeight %d\n", render->halfScreenHeight);
	printf("render->fracHalfScreenWidth %d\n", render->fracHalfScreenWidth);
	printf("render->fracHalfScreenHeight %d\n", render->fracHalfScreenHeight);
#endif

	SDL_free(render->ceilingColor);
	render->ceilingColor = SDL_malloc(render->screenWidth * sizeof(short));
	if (render->ceilingColor == NULL) { memError = true; }

	SDL_free(render->floorColor);
	render->floorColor = SDL_malloc(render->screenWidth * sizeof(short));
	if (render->floorColor == NULL) { memError = true; }

	SDL_free(render->columnScale);
	render->columnScale = SDL_malloc(render->screenWidth * sizeof(int));
	if (render->columnScale == NULL) { memError = true; }

	if (memError) {
		//DoomRPG_setErrorID(render->doomRpg, 2);
		DoomRPG_Error("Render: Insufficient memory for allocation");
	}
}

void Render_freeRuntime(Render_t* render) {

	int i;
	testsub("Render_freeRuntime");

	SDL_free(render->mediaTexels);
	render->mediaTexels = NULL;

	SDL_free(render->shapeData);
	render->shapeData = NULL;

	SDL_free(render->mapSprites);
	render->mapSprites = NULL;
	render->numMapSprites = 0;

	SDL_free(render->nodes);
	render->nodes = NULL;

	SDL_free(render->lines);
	render->lines = NULL;
	render->linesLength = 0;

	SDL_free(render->tileEvents);
	render->tileEvents = NULL;

	SDL_free(render->mapByteCode);
	render->mapByteCode = NULL;

	for (i = 0; i < render->mapStringCount; i++) {
		SDL_free(render->mapStringsIDs[i]);
	}
	SDL_free(render->mapStringsIDs);
	render->mapStringCount = 0;
	render->mapStringsIDs = NULL;

	SDL_free(render->mediaBitShapeOffsets);
	render->mediaBitShapeOffsets = NULL;

	SDL_free(render->mediaTexelOffsets);
	render->mediaTexelOffsets = NULL;

	SDL_free(render->mediaTexturesIds);
	render->mediaTexturesIds = NULL;

	SDL_free(render->mediaSpriteIds);
	render->mediaSpriteIds = NULL;

	SDL_free(render->mapTextureTexels);
	render->mapTextureTexels = NULL;

	SDL_free(render->mapSpriteTexels);
	render->mapSpriteTexels = NULL;
}

void Render_free(Render_t* render, boolean freePtr)
{
	Render_freeRuntime(render);
	SDL_free(render->ceilingColor);
	SDL_free(render->floorColor);
	SDL_free(render->columnScale);
	SDL_free(render->mediaPalettes);

	SDL_free(render->framebuffer);
	render->framebuffer = NULL;

	if (render->piDIB) {
		SDL_DestroyTexture(render->piDIB);
		render->piDIB = NULL;
	}

	//resourceAsStream.Free(&render->mapFile, 0);
	if (freePtr) {
		SDL_free(render);
	}
}

int Render_startup(Render_t* render)
{
	FILE* fp = NULL;
	byte* fData;
	int w, h;
	int i;
	testsub("Render_Startup");

	fData = DoomRPG_fileOpenRead(render->doomRpg, "sintable.bin");
	//SDL_memmove(render->sinTable, fData, sizeof(render->sinTable));
	for(i = 0; i < 256; i++) {
		render->sinTable[i] = GetLong(fData + i*sizeof(long));
		printf("i=%i sin=%i", i, render->sinTable[i]);
		testsub(" val");
	}

	testsub("sin end");

	SDL_free(fData);
	// for (i = 0; i < 256; i++) {
	// 	render->sinTable[i] = SDL_SwapLE32(render->sinTable[i]);
	// }
	render->clipRect.x = render->doomRpg->doomCanvas->displayRect.x;
	render->clipRect.y = render->doomRpg->doomCanvas->displayRect.y;
	render->clipRect.w = render->doomRpg->doomCanvas->displayRect.w;
	render->clipRect.h = render->doomRpg->doomCanvas->displayRect.h;

	w = sdlVideo.rendererW;
	h = sdlVideo.rendererH;
	render->piDIB = SDL_CreateTexture(sdlVideo.renderer,
		SDL_PIXELFORMAT_RGB565, SDL_TEXTUREACCESS_STREAMING, w, h);

	render->pitch = (((w * SDL_BYTESPERPIXEL(SDL_PIXELFORMAT_RGB565)) + 3) & ~3);
	render->framebuffer = SDL_calloc(1, render->pitch * h);

	testsub("Render_startup1");
	//memset(render->framebuffer, 0xff, render->pitch * h);// test

	Render_loadPalettes(render);

		testsub("Render_Startup end");
	return 1;
}

void Render_loadPalettes(Render_t* render)
{
	byte* fData;
	int dataPos = 0, i;
	short color;
	int red, green, blue;

	render->paletteMemory = DoomRPG_freeMemory();

	fData = DoomRPG_fileOpenRead(render->doomRpg, "palettes.bin");

	SDL_free(render->mediaPalettes);

	render->mediaPalettesLength = DoomRPG_intAtNext(fData, &dataPos); // / 2;
	render->mediaPalettes = (short*)SDL_malloc(render->mediaPalettesLength * sizeof(short));
	if (render->mediaPalettes == NULL) {
		DoomRPG_Error("Render_loadPalettes: Insufficient memory for allocation");
	}

	//printf("render->mediaPalettesLength %d\n", render->mediaPalettesLength);

	for (i = 0; i < render->mediaPalettesLength; i++)
	{
		color = DoomRPG_shortAtNext(fData, &dataPos);

		blue = (color >> 11) & 0x1f;    // (color << 16) >> 27;
		blue = (blue << 3) | (blue >> 2);

		green = (color >> 5) & 0x3f;    // (color << 21) >> 26;
		green = (green << 2) | (green >> 4);

		red = (color & 0x1f);
		red = (red << 3) | (red >> 2);
		render->mediaPalettes[i] = (Uint16)Render_make565RGB(render, blue, green, red);
	}

	SDL_free(fData);

	render->paletteMemory = DoomRPG_freeMemory() - render->paletteMemory;
	//printf("paletteMemory %d\n", render->paletteMemory);
}

unsigned int Render_make565RGB(Render_t* render, int blue, int green, int red)
{
	return ((red>>3) << 11) | ((green>>3) << 6) | ((blue>>3)<<1) | 1;
}

unsigned short Render_RGB888_To_RGB565(Render_t* render, int rgb)
{
	return (unsigned short)Render_make565RGB(render, rgb & 0xff, (rgb >> 8) & 0xff, (rgb >> 16) & 0xff);
}

void Render_setGrayPalettes(Render_t* render)
{
	Uint16* mediaPalettes, color, grayColor, * mediaPlanes; //short
	int i, j;

	for (i = 0; i < render->mediaPalettesLength; i++) {
		mediaPalettes = render->mediaPalettes;
		color = (mediaPalettes[i]) ;
		//grayColor = (((color & 0xf800) >> 10) + ((color >> 5) & 0x3f) + ((color & 0x1f) << 1)) / 3; //RGB
		grayColor = (((color >> 11) + ((color >> 6) & 0x1f) + ((color & 0x1f))) / 3); //RGB
		//mediaPalettes[i] = ((grayColor >> 1) << 11) | (grayColor << 5) | (grayColor >> 1);
		//mediaPalettes[i] = ((grayColor >> 1) << 11) | ((grayColor>>1) << 6) | (grayColor >> 1) | 1;

		mediaPalettes[i] = ((grayColor) << 11) | ((grayColor) << 6) | ((grayColor)<<1) | 1;
		//breserk color??
		//mediaPalettes[i] = ((grayColor << 10) & 0xf800) | ((grayColor << 6)&0x3f) | ((grayColor & 0x1f )>>1) | 1;
	}

	for (i = 0; i < render->planeTexturesCnt; i++)
	{
		for (j = 0; j < (64 * 64); j++) {
			mediaPlanes = &render->mediaPlanes[i][j];
			color = (mediaPlanes[0]);
			//grayColor = (((color & 0xf800) >> 10) + ((color >> 5) & 0x3f) + ((color & 0x1f) << 1)) / 3; //RGB
			grayColor = (((color) >> 11) + ((color >> 6) & 0x1f) + ((color & 0x1f))) / 3; //RGB
			//mediaPlanes[0] = ((grayColor >> 1) << 11) | (grayColor << 5) | (grayColor >> 1);
			//mediaPlanes[0] = ((grayColor >> 1) << 11) | ((grayColor>>1) << 6) | (grayColor >> 1) | 1;

			mediaPlanes[0] = ((grayColor) << 11) | ((grayColor) << 6) | ((grayColor)<<1) | 1;
			//breserk color??
			//mediaPlanes[0] = ((grayColor << 10) & 0xf800) | ((grayColor << 6)&0x3f) | ((grayColor & 0x1f)>>1)| 1;
		}
	}

	color = (render->floorColor[0]);
	//grayColor = (((color & 0xf800) >> 10) + ((color >> 5) & 0x3f) + ((color & 0x1f) << 1)) / 3; //RGB
	grayColor = (((color) >> 11) + ((color >> 6) & 0x1f) + ((color >> 1 )& 0x1f)) / 3; //RGB
	//render->floorColor[0] = ((grayColor >> 1) << 11) | (grayColor << 5) | (grayColor >> 1);
	render->floorColor[0] = ((grayColor) << 11) | ((grayColor) << 6) | (grayColor << 1) | 1;
	//breserk color??
	//render->floorColor[0] = ((grayColor << 10) & 0xf800) | ((grayColor << 6)&0x3f) | ((grayColor & 0x1f)>>1)| 1;

	color = (render->ceilingColor[0]);
	grayColor = (((color & 0xf800) >> 11) + ((color >> 6) & 0x1f) + ((color & 0x1f) << 1)) / 3; //RGB
	//render->ceilingColor[0] = ((grayColor >> 1) << 11) | (grayColor << 5) | (grayColor >> 1);
	render->ceilingColor[0] = ((grayColor ) << 11) | ((grayColor) << 6) | (grayColor << 1) | 1;
	//breserk color??
	//render->ceilingColor[0] = ((grayColor << 10) & 0xf800) | ((grayColor << 6)&0x3f) | ((grayColor & 0x1f)>>1) | 1;

}


boolean Render_loadMappings(Render_t* render)
{
	byte* fData;
	int dataPos, i;
	int texelsCnt, bitShapeCnt;

	printf("---Render_loadMappings---\n");

	render->mappingMemory = DoomRPG_freeMemory();

	dataPos = 0;
	fData = DoomRPG_fileOpenRead(render->doomRpg, "mappings.bin");

	texelsCnt = DoomRPG_intAtNext(fData, &dataPos) * 2;
	bitShapeCnt = DoomRPG_intAtNext(fData, &dataPos) * 2;
	render->textureCnt = DoomRPG_intAtNext(fData, &dataPos);
	render->spriteCnt = DoomRPG_intAtNext(fData, &dataPos);

	printf("texelscnt=%x\n bitshapecnt=%x\n texturecnt=%x\n spritecnt=%x\n",
		texelsCnt, bitShapeCnt, render->textureCnt, render->spriteCnt);
	SDL_free(render->mediaTexelOffsets);
	render->mediaTexelOffsets = SDL_malloc(texelsCnt * sizeof(int));
	if (render->mediaTexelOffsets == NULL) { return false; }

	SDL_free(render->mediaBitShapeOffsets);
	render->mediaBitShapeOffsets = SDL_malloc(bitShapeCnt * sizeof(int));
	if (render->mediaBitShapeOffsets == NULL) { return false; }

	SDL_free(render->mediaTexturesIds);
	render->mediaTexturesIds = SDL_malloc(render->textureCnt * sizeof(short));
	if (render->mediaTexturesIds == NULL) { return false; }

	SDL_free(render->mediaSpriteIds);
	render->mediaSpriteIds = SDL_malloc(render->spriteCnt * sizeof(short));
	if (render->mediaSpriteIds == NULL) { return false; }

	for (i = 0; i < texelsCnt; i++) {
		render->mediaTexelOffsets[i] = DoomRPG_intAtNext(fData, &dataPos);
		// sprintf(test, "i=%i mediaTexelOffsets=%x", i, render->mediaTexelOffsets[i]);
		// testsub(test);
	}

	for (i = 0; i < bitShapeCnt; i++) {
		render->mediaBitShapeOffsets[i] = DoomRPG_intAtNext(fData, &dataPos);
		// sprintf(test, "i=%i mediaBitShapeOffsets=%x", i, render->mediaBitShapeOffsets[i]);
		// testsub(test);
	}

	for (i = 0; i < render->textureCnt; i++) {
		render->mediaTexturesIds[i] = DoomRPG_shortAtNext(fData, &dataPos);
	}

	for (i = 0; i < render->spriteCnt; i++) {
		render->mediaSpriteIds[i] = DoomRPG_shortAtNext(fData, &dataPos);
	}

	SDL_free(fData);

	render->mappingMemory = DoomRPG_freeMemory() - render->mappingMemory;
	//printf("render->mappingMemory %d\n", render->mappingMemory);

	return true;
}

boolean Render_beginLoadMap(Render_t* render, int mapNameID)
{
	byte* ioBuffer;
	byte r, g, b;

	if (Render_loadMappings(render))
	{
		render->mapMemory = DoomRPG_freeMemory();

		printf("---Render_beginLoadMap---\n");
		render->mapNameID = mapNameID;

		render->ioBufferPos = 0;
		if (render->mapNameID <= MAP_MENU) {
			render->ioBuffer = DoomRPG_fileOpenRead(render->doomRpg, "menu.bsp"); //menu.bsp
			strncpy(render->mapName, "intro.bsp", 10);
		}
		else {
			render->ioBuffer = DoomRPG_fileOpenRead(render->doomRpg, render->doomRpg->game->mapFiles[render->mapNameID - 1]);
			strncpy(render->mapName, render->doomRpg->game->mapFiles[render->mapNameID - 1], strlen(render->doomRpg->game->mapFiles[render->mapNameID - 1]));
		}

		ioBuffer = render->ioBuffer;

		#ifdef BREW
		strncpy(render->mapName, ioBuffer, MAPNAMESTRLEN);
		render->ioBufferPos = MAPNAMESTRLEN;
		#endif

		r = DoomRPG_byteAtNext(ioBuffer, &render->ioBufferPos);
		g = DoomRPG_byteAtNext(ioBuffer, &render->ioBufferPos);
		b = DoomRPG_byteAtNext(ioBuffer, &render->ioBufferPos);
		render->floorColor[0] = Render_make565RGB(render, b, g, r);

		// printf("floorcolor=%x", render->floorColor[0]);
		// testsub("-");

		r = DoomRPG_byteAtNext(ioBuffer, &render->ioBufferPos);
		g = DoomRPG_byteAtNext(ioBuffer, &render->ioBufferPos);
		b = DoomRPG_byteAtNext(ioBuffer, &render->ioBufferPos);
		render->ceilingColor[0] = Render_make565RGB(render, b, g, r);
		// printf("ceillingcolor=%x", render->ceilingColor[0]);
		// testsub("-");
		#ifdef BREW
		render->floorTex = DoomRPG_byteAtNext(ioBuffer, &render->ioBufferPos);
		render->ceilingTex = DoomRPG_byteAtNext(ioBuffer, &render->ioBufferPos);
		#endif

		r = DoomRPG_byteAtNext(ioBuffer, &render->ioBufferPos);
		g = DoomRPG_byteAtNext(ioBuffer, &render->ioBufferPos);
		b = DoomRPG_byteAtNext(ioBuffer, &render->ioBufferPos);
		render->introColor = Render_make565RGB(render, b, g, r); //0xFF000000 | (r << 16) | (g << 8) | b;

		render->loadMapID = DoomRPG_byteAtNext(ioBuffer, &render->ioBufferPos);
		render->mapSpawnIndex = DoomRPG_shortAtNext(ioBuffer, &render->ioBufferPos);
		render->mapSpawnDir = DoomRPG_byteAtNext(ioBuffer, &render->ioBufferPos);

		#ifdef BREW
		render->mapCameraSpawnIndex = DoomRPG_shortAtNext(ioBuffer, &render->ioBufferPos);
		#endif
		SDL_memset(render->mapFlags, 0, sizeof(render->mapFlags));

		//NEW J2ME



		//printf("render->ioBufferPos %d\n", render->ioBufferPos);
		render->mapMemory = DoomRPG_freeMemory() - render->mapMemory;
		//printf("render->mapMemory %d\n", render->mapMemory);

		return true;
	}

	return false;
}

boolean Render_beginLoadMapData(Render_t* render)
{
	short* mediaPlanes;
	byte* ioBuffer;
	Node_t* node;
	Line_t* line;
	Sprite_t* mapSprite;
	int mem, i, j, arg1, arg2, numByteCodes, numStrings;
	//int x, y, pos, sprId, evnt, strSize, flags, textId, indx;
	int x, y, pos, sprId, evnt, flags, textId, indx;
	Uint16 strSize;
	int texOffset, palOffset;

	printf("---Render_beginLoadMapData---\n");
	testsub("Render_beginLoadMapData");
	mem = DoomRPG_freeMemory();
	ioBuffer = render->ioBuffer;

	SDL_free(render->mapTextureTexels);
	render->mapTextureTexels = SDL_malloc(256 * sizeof(int));

	SDL_free(render->mapSpriteTexels);
	render->mapSpriteTexels = SDL_malloc(1024 * sizeof(int));

	// Read Nodes
	DoomCanvas_updateLoadingBar(render->doomRpg->doomCanvas);

	render->nodeMem = DoomRPG_freeMemory();
	render->nodesLength = DoomRPG_shortAtNext(ioBuffer, &render->ioBufferPos);
	//printf("nodesLength %d\n", render->nodesLength);


	SDL_free(render->nodes);
	render->nodes = SDL_malloc(render->nodesLength * sizeof(Node_t));

	for (i = 0; i < render->nodesLength; i++) {
		node = &render->nodes[i];
		node->x1 = DoomRPG_shiftCoordAt(ioBuffer, &render->ioBufferPos);
		node->y1 = DoomRPG_shiftCoordAt(ioBuffer, &render->ioBufferPos);
		node->x2 = DoomRPG_shiftCoordAt(ioBuffer, &render->ioBufferPos);
		node->y2 = DoomRPG_shiftCoordAt(ioBuffer, &render->ioBufferPos);


		arg1 = DoomRPG_byteAtNext(ioBuffer, &render->ioBufferPos);
		node->args1 = DoomRPG_shiftCoordAt(ioBuffer, &render->ioBufferPos) | (arg1 << 16);
		#ifdef BREW
		arg1 = DoomRPG_shortAtNext(ioBuffer, &render->ioBufferPos);
		#endif

		arg2 = DoomRPG_shortAtNext(ioBuffer, &render->ioBufferPos);
		node->args2 = arg1 | arg2 << 16;
		node->sprites = NULL;
	}
	render->nodeMem = DoomRPG_freeMemory() - render->nodeMem;
	//printf("nodeMem %d\n", render->nodeMem);
	//printf("render->ioBufferPos %d\n", render->ioBufferPos);
	testsub("Render_beginLoadMapData1");
	// Read Lines
	DoomCanvas_updateLoadingBar(render->doomRpg->doomCanvas);

	render->lineMem = DoomRPG_freeMemory();
	render->linesLength = DoomRPG_shortAtNext(ioBuffer, &render->ioBufferPos);
	render->mapTextureTexelsCount = 0;
	//printf("linesLength %d\n", render->linesLength);

	SDL_free(render->lines);
	render->lines = SDL_calloc(render->linesLength, sizeof(Line_t));
	printf("nodelength=%x pos=%x\n", render->linesLength, render->ioBufferPos);
	testsub("--test value2--");

	for (i = 0; i < render->linesLength; i++) {
		line = &render->lines[i];
		line->vert1.x = DoomRPG_shiftCoordAt(ioBuffer, &render->ioBufferPos);
		line->vert1.y = DoomRPG_shiftCoordAt(ioBuffer, &render->ioBufferPos);
		line->vert2.x = DoomRPG_shiftCoordAt(ioBuffer, &render->ioBufferPos);
		line->vert2.y = DoomRPG_shiftCoordAt(ioBuffer, &render->ioBufferPos);
		line->texture = DoomRPG_shortAtNext(ioBuffer, &render->ioBufferPos);
		line->flags = DoomRPG_intAtNext(ioBuffer, &render->ioBufferPos);

		// printf("x1=%x x2=%x\n x2=%x y2=%x\n tex=%x flag=%x\n",
		// 	line->vert1.x, line->vert1.y,
		// 	line->vert2.x, line->vert2.y,
		// 	line->texture, line->flags
		// );
		// testsub("--test value2--");

		if ((line->flags & 32768) == 0) {
			line->vert1.z = 0;
			line->vert2.z = MAX(ABS(line->vert2.x - line->vert1.x), ABS(line->vert2.y - line->vert1.y));
		}
		else {
			line->vert1.z = MAX(ABS(line->vert2.x - line->vert1.x), ABS(line->vert2.y - line->vert1.y));
			line->vert2.z = 0;
		}

		if ((line->flags & 512) != 0) {
			if ((line->flags & 8) != 0) {
				line->vert1.x += 3;
				line->vert2.x += 3;
			}
			else if ((line->flags & 16) != 0) {
				line->vert1.x -= 3;
				line->vert2.x -= 3;
			}
		}
		else if ((line->flags & 256) != 0) {
			if ((line->flags & 8) != 0) {
				line->vert1.y += 3;
				line->vert2.y += 3;
			}
			else if ((line->flags & 16) != 0) {
				line->vert1.y -= 3;
				line->vert2.y -= 3;
			}
		}

		if (line->texture == 7) {
			x = (line->vert1.x + ((line->vert2.x - line->vert1.x) / 2)) >> 6;
			y = (line->vert1.y + ((line->vert2.y - line->vert1.y) / 2)) >> 6;
			pos = (y * 32) + x;
			render->mapFlags[pos] |= BIT_AM_ENTRANCE;
		}

		Render_addMapTextures(render, line->texture);
		if (line->texture == 34) {
			Render_addMapTextures(render, 92);
		}
		else if (line->texture == 33) {
			Render_addMapTextures(render, 91);
		}
		else if (line->texture == 9) {
			Render_addMapTextures(render, 10);
		}
		else if (line->texture == 10) {
			Render_addMapTextures(render, 9);
		}
	}



	Render_addMapTextures(render, render->floorTex);
	Render_addMapTextures(render, render->ceilingTex);
	render->lineMem = DoomRPG_freeMemory() - render->lineMem;
	//printf("lineMem %d\n", render->lineMem);
	testsub("Render_beginLoadMapData2");
	// Read Sprites
	DoomCanvas_updateLoadingBar(render->doomRpg->doomCanvas);

	render->spriteMem = DoomRPG_freeMemory();
	render->numMapSprites = DoomRPG_shortAtNext(ioBuffer, &render->ioBufferPos);
	render->numSprites = render->numMapSprites + MAX_CUSTOM_SPRITES + MAX_DROP_SPRITES;
	render->mapSpriteTexelsCount = 0;
	//printf("numMapSprites %d\n", render->numMapSprites);

	printf("mapspritecount=%x pos=%x\n",  render->numMapSprites, render->ioBufferPos);
	testsub("--test value2L--");

	SDL_free(render->mapSprites);
	testsub("Render_beginLoadMapData3");
	render->mapSprites = SDL_calloc(render->numSprites, sizeof(Sprite_t));
	for (i = 0; i < render->numMapSprites; i++) {

		printf("i=%i mapspritecount=%i pos=%i\n",i,render->numMapSprites, render->ioBufferPos);
		//testsub("loop");
		mapSprite = &render->mapSprites[i];
		//testsub("loop2");
		mapSprite->x = DoomRPG_shiftCoordAt(ioBuffer, &render->ioBufferPos);
		mapSprite->y = DoomRPG_shiftCoordAt(ioBuffer, &render->ioBufferPos);
		mapSprite->info = DoomRPG_byteAtNext(ioBuffer, &render->ioBufferPos);

		if ((mapSprite->info == 136 || mapSprite->info == 137) || mapSprite->info == 144) {
			mapSprite->renderMode = 7;
		}
		if ((mapSprite->info != 136 && mapSprite->info != 137) && mapSprite->info != 144) {
			mapSprite->renderMode = 0;
		}
		//testsub("loop3");
		mapSprite->info |= DoomRPG_shortAtNext(ioBuffer, &render->ioBufferPos) << 16;
		mapSprite->node = NULL;
		mapSprite->ent = NULL;
		mapSprite->viewNext = NULL;
		mapSprite->nodeNext = NULL;

		if ((mapSprite->info & 0x800000) != 0) {
			if ((mapSprite->info & 0x100000) != 0) {
				mapSprite->y++;
			}
			else if ((mapSprite->info & 0x80000) != 0) {
				mapSprite->y--;
			}
			else if ((mapSprite->info & 0x200000) != 0) {
				mapSprite->x++;
			}
			else if ((mapSprite->info & 0x400000) != 0) {
				mapSprite->x--;
			}
		}
		//testsub("loop4");
		Render_relinkSprite(render, mapSprite);
		//testsub("loop5");
		sprId = mapSprite->info & 511;
		if (sprId >= 82 && sprId <= 90 && (sprId & 1) == 0) {
			mapSprite->info |= 0x4000000;
			Render_addMapSprites(render, sprId - 1);
		}
		else if (!(mapSprite->info & 0x20000000)) {
			if ((mapSprite->info & 0x40000)) {
				Render_addMapTextures(render, sprId);
			}
			else {
				Render_addMapSprites(render, sprId);
			}
		}
		//testsub("loopend");
	}

	for (j = 0; j < MAX_CUSTOM_SPRITES; j++) {
		render->customSprites[j] = &render->mapSprites[i++];
	}
	render->firstDropSprite = i;

	for (j = 0; j < MAX_DROP_SPRITES; j++) {
		render->dropSprites[j] = &render->mapSprites[i++];
	}

	render->spriteMem = DoomRPG_freeMemory() - render->spriteMem;
	//printf("render->spriteMem %d\n", render->spriteMem);


	for (j = 180; j <= 191; j++) {
		Render_addMapSprites(render, j);
	}

	for (j = 200; j <= 206; j++) {
		Render_addMapSprites(render, j);
	}

	for (j = 81; j <= 92; j++) {
		Render_addMapSprites(render, j);
	}

	Render_addMapSprites(render, 95);
	Render_addMapSprites(render, 96);
	Render_addMapSprites(render, 99);
	Render_addMapSprites(render, 100);
	Render_addMapSprites(render, 136);
	Render_addMapSprites(render, 144);
	Render_addMapSprites(render, 20);
	Render_addMapSprites(render, 21);
	Render_addMapSprites(render, 22);
	Render_addMapSprites(render, 101);
	Render_addMapSprites(render, 102);
	Render_addMapSprites(render, 110);

	for (j = 0; j <= 11; j++) {
		Render_addMapSprites(render, 240 + j);
	}
testsub("Render_beginLoadMapData4");
	// Read Events
	DoomCanvas_updateLoadingBar(render->doomRpg->doomCanvas);

	render->eventMem = DoomRPG_freeMemory();
	render->numTileEvents = DoomRPG_shortAtNext(ioBuffer, &render->ioBufferPos);
	//printf("numTileEvents %d\n", render->numTileEvents);

	SDL_free(render->tileEvents);
	render->tileEvents = NULL;
	if (render->numTileEvents > 0) {
		render->tileEvents = SDL_malloc(render->numTileEvents * sizeof(int));

		for (j = 0; j < render->numTileEvents; j++) {
			evnt = DoomRPG_intAtNext(ioBuffer, &render->ioBufferPos);
			if (evnt & 0x1f80000) {
				render->mapFlags[evnt & 1023] |= BIT_AM_EVENTS;
			}
			render->tileEvents[j] = evnt;
		}
	}

	// Read ByteCodes
testsub("Render_beginLoadMapData5");
	numByteCodes = DoomRPG_shortAtNext(ioBuffer, &render->ioBufferPos);
	//printf("numByteCodes %d\n", numByteCodes);

	SDL_free(render->mapByteCode);
	render->mapByteCode = NULL;
	if (numByteCodes > 0) {
		render->mapByteCode = SDL_malloc(numByteCodes * sizeof(int) * 3);

		for (j = 0; j < numByteCodes; j++) {
			render->mapByteCode[(j * BYTE_CODE_MAX) + BYTE_CODE_ID] = DoomRPG_byteAtNext(ioBuffer, &render->ioBufferPos);
			render->mapByteCode[(j * BYTE_CODE_MAX) + BYTE_CODE_ARG1] = DoomRPG_intAtNext(ioBuffer, &render->ioBufferPos);
			render->mapByteCode[(j * BYTE_CODE_MAX) + BYTE_CODE_ARG2] = DoomRPG_intAtNext(ioBuffer, &render->ioBufferPos);
			if (render->mapByteCode[(j * BYTE_CODE_MAX) + 0] == 34 /*EV_CHANGESPRITE*/) {
				Render_addMapSprites(render, render->mapByteCode[(j * BYTE_CODE_MAX) + BYTE_CODE_ARG1] >> 13);
			}
		}
	}
	render->eventMem = DoomRPG_freeMemory() - render->eventMem;
	//printf("eventMem %d\n", render->eventMem);


	// Read Strings
	DoomCanvas_updateLoadingBar(render->doomRpg->doomCanvas);
	numStrings = DoomRPG_shortAtNext(ioBuffer, &render->ioBufferPos);
	//printf("numStrings %d\n", numStrings);

	render->stringMem = DoomRPG_freeMemory();

testsub("Render_beginLoadMapData6");
	SDL_free(render->mapStringsIDs);
	render->mapStringsIDs = NULL;
	if (numStrings > 0) {
		render->mapStringsIDs = SDL_malloc(numStrings * sizeof(char*));

		render->mapStringCount = 0;
		while (render->mapStringCount < numStrings) {
			strSize = DoomRPG_shortAtNext(ioBuffer, &render->ioBufferPos);
			render->mapStringsIDs[render->mapStringCount] = SDL_calloc(strSize + 1, sizeof(char));
			strncpy(render->mapStringsIDs[render->mapStringCount], &ioBuffer[render->ioBufferPos], strSize);
			render->ioBufferPos += strSize;
			render->mapStringCount++;
		}
	}
testsub("Render_beginLoadMapData7");
	render->stringMem = DoomRPG_freeMemory() - render->stringMem;
	//printf("stringMem %d\n", render->stringMem);

	// Read BlockMap
	DoomCanvas_updateLoadingBar(render->doomRpg->doomCanvas);

	i = 0;
	for (j = 0; j < 256; j++) {
		// Read a byte from ioBuffer
		flags = DoomRPG_byteAtNext(ioBuffer, &render->ioBufferPos);

		// Extract specific bits from flags and update render->mapFlags array
		render->mapFlags[i + 0] |= flags & 3;			// Update the least significant 2 bits
		render->mapFlags[i + 1] |= (flags >> 2) & 3;	// Update the next 2 bits
		render->mapFlags[i + 2] |= (flags >> 4) & 3;	// Update the following 2 bits
		render->mapFlags[i + 3] |= (flags >> 6) & 3;	// Update the last 2 bits
		i += 4;
	}

#if 0 // Test Only
	i = 0;
	for (j = 0; j < 256; j++) {
		byte bitPacked = ((render->mapFlags[i + 3] & 3) << 6) | ((render->mapFlags[i + 2] & 3) << 4) | ((render->mapFlags[i + 1] & 3) << 2) | (render->mapFlags[i + 0] & 3);
		printf("bitPacked[%d] %02x\n", j, bitPacked);
		i += 4;
	}
#endif
testsub("Render_beginLoadMapData8");
	// Read Plane Textures
	render->planeTexturesCnt = 0;

	for (i = 0; i < 2; i++)
	{
		for (j = 0; j < 1024; j++)
		{
			textId = DoomRPG_byteAtNext(ioBuffer, &render->ioBufferPos);
			indx = 0;

			while (render->planeTexturesCnt > indx && render->planeTextureIds[indx] != textId) {
				++indx;
			}

			if (render->planeTexturesCnt == indx) {
				if (render->planeTexturesCnt >= 24) {
					DoomRPG_Error("Exceeds the maximum number of textures allowed in planes");
					return false;
				}
				render->planeTextureIds[indx] = textId;
				Render_addMapTextures(render, textId);
				render->planeTexturesCnt++;
			}

			render->planeTextures[(i * 1024) + j] = render->mediaPlanes[indx];
		}
	}
testsub("Render_beginLoadMapData9");
	SDL_free(ioBuffer);
	//printf("render->ioBufferPos %d\n", render->ioBufferPos);

	DoomCanvas_updateLoadingBar(render->doomRpg->doomCanvas);
	if (Render_loadBitShapes(render) == false) {
		return false;
	}

	DoomCanvas_updateLoadingBar(render->doomRpg->doomCanvas);
	if (Render_loadTexels(render) == false) {
		return false;
	}
testsub("Render_beginLoadMapData10");
	for (i = 0; i < render->planeTexturesCnt; i++)
	{
		palOffset = render->mediaTexelOffsets[(render->mediaTexturesIds[render->planeTextureIds[i]] * 2) + 1];
		texOffset = render->mediaTexelOffsets[(render->mediaTexturesIds[render->planeTextureIds[i]] * 2) + 0];

		for (j = 0; j < (64 * 64); j += 2) {
			mediaPlanes = &render->mediaPlanes[i][j];
			mediaPlanes[0] = render->mediaPalettes[palOffset + (render->mediaTexels[(texOffset >> 1) + (j >> 1)] & 0xF)];
			mediaPlanes[1] = render->mediaPalettes[palOffset + ((render->mediaTexels[(texOffset >> 1) + (j >> 1)] >> 4) & 0xF)];
		}
	}

	for (i = 1; i < render->screenWidth; i++) {
		render->floorColor[i] = render->floorColor[0];
		render->ceilingColor[i] = render->ceilingColor[0];
	}

	SDL_free(render->mapTextureTexels);
	render->mapTextureTexels = NULL;

	SDL_free(render->mapSpriteTexels);
	render->mapSpriteTexels = NULL;

	render->mapMemory = (DoomRPG_freeMemory() + render->mapMemory) - mem;
	//printf("mapMemory %d\n", render->mapMemory);
testsub("Render_beginLoadMapData_end");
	return true;
}

boolean Render_loadBitShapes(Render_t* render)
{
	byte* ioBuffer, * ioBufferBase;
	int i, j, k, updtCnt, shapeDataSize, prevShapeOffset, shapeOffset;
	int prevIoOffset, ioOffset, indx, width, height, pitch, w, h, offset;

	printf("---Render_loadBitShapes---\n");

	render->bitShapeMemory = DoomRPG_freeMemory();

	// Read BitShape Data
	ioBufferBase = DoomRPG_fileOpenRead(render->doomRpg, "bitshapes.bin");
	ioBuffer = ioBufferBase + sizeof(int);

	for (i = 0; i < (render->mapSpriteTexelsCount - 1); i++)
	{
		for (j = 0; j < (render->mapSpriteTexelsCount - 1) - i; j++)
		{
			//swap
			if (render->mediaBitShapeOffsets[render->mapSpriteTexels[j + 1] * 2] <
				render->mediaBitShapeOffsets[render->mapSpriteTexels[j + 0] * 2])
			{
				k = render->mapSpriteTexels[j + 0];
				render->mapSpriteTexels[j + 0] = render->mapSpriteTexels[j + 1];
				render->mapSpriteTexels[j + 1] = k;
			}
		}
	}

	shapeDataSize = 0;
	prevIoOffset = -1;
	for (i = 0; i < render->mapSpriteTexelsCount; i++) {
		ioOffset = render->mediaBitShapeOffsets[render->mapSpriteTexels[i] * 2];
		if (ioOffset != prevIoOffset) {
			shapeDataSize += (ioBuffer[ioOffset + 6] & 255) | ((ioBuffer[ioOffset + 7] << 8) & 65280);
			prevIoOffset = ioOffset;
		}
	}

	DoomCanvas_updateLoadingBar(render->doomRpg->doomCanvas);

	//printf("shapeDataSize %d\n", shapeDataSize);
	SDL_free(render->shapeData);
	render->shapeData = SDL_malloc(shapeDataSize * sizeof(short));

	updtCnt = 0;
	shapeOffset = 0;
	prevShapeOffset = 0;
	prevIoOffset = -1;
	for (i = 0; i < render->mapSpriteTexelsCount; ++i) {
		indx = render->mapSpriteTexels[i];
		ioOffset = render->mediaBitShapeOffsets[indx * 2];

		if (ioOffset == prevIoOffset) {
			render->mediaBitShapeOffsets[indx * 2] = prevShapeOffset;
		}
		else {
			if (++updtCnt == 3) {
				DoomCanvas_updateLoadingBar(render->doomRpg->doomCanvas);
				updtCnt = 0;
			}

			prevIoOffset = ioOffset;
			width = (ioBuffer[(ioOffset + 8) + 1] - ioBuffer[(ioOffset + 8) + 0]) + 1;
			height = (ioBuffer[(ioOffset + 8) + 3] - ioBuffer[(ioOffset + 8) + 2]) + 1;
			pitch = (height & 7) ? (height / 8 + 1) : (height / 8);

			prevShapeOffset = shapeOffset;
			render->shapeData[shapeOffset++] = (ioBuffer[ioOffset + 0] & 0xFF) | (ioBuffer[ioOffset + 1] << 8 & 0xFF00);
			render->shapeData[shapeOffset++] = (ioBuffer[ioOffset + 2] & 0xFF) | (ioBuffer[ioOffset + 3] << 8 & 0xFF00);
			render->shapeData[shapeOffset++] = ioBuffer[ioOffset + 8];
			render->shapeData[shapeOffset++] = ioBuffer[ioOffset + 8 + 1];

			int mediaBitShapeOffset = 0;
			shapeOffset += width;
			for (w = 0; w < width; ++w) {
				render->shapeData[prevShapeOffset + w + 4] = (short)(shapeOffset - prevShapeOffset - 2);
				offset = ioOffset + 8 + 4 + (w * pitch);
				h = 0;
				while (h < height) {
					//blank space
					while (h < height && (ioBuffer[offset + h / 8] & 1 << (h & 7)) == 0x0) {
						++h;
					}
					//exit
					if (h == height) {
						break;
					}
					short n23 = h;
					int n24 = mediaBitShapeOffset;
					//add geometry
					while (h < height && (ioBuffer[offset + h / 8] & 1 << (h & 7)) != 0x0) {
						++mediaBitShapeOffset;
						++h;
					}
					render->shapeData[shapeOffset++] = (n23 + ioBuffer[ioOffset + 8 + 2] & 0xFF) | ((h - n23) << 8);
					render->shapeData[shapeOffset++] = n24;
				}
				render->shapeData[shapeOffset++] = 127;
			}
			//NEW
			// for(h = 0; h < height;  h++){
			// 	for(w = 0; w < width; w++){
			// 		//if()
					
			// 		short n23 = w;
			// 		int n24 = mediaBitShapeOffset;
			// 		if((ioBuffer[offset + h / 8] & 1 << (h & 7) % (w & 7)) != 0)
			// 		++mediaBitShapeOffset;
			// 		render->shapeData[shapeOffset++] = (n23 + ioBuffer[ioOffset + 8 + 2] & 0xFF) | ((h - n23) << 8);
			// 		render->shapeData[shapeOffset++] = n24;
			// 	}
			//	render->shapeData[shapeOffset++] = 127;
			//}
			
			render->mediaBitShapeOffsets[indx * 2] = prevShapeOffset;
		}
	}

	SDL_free(ioBufferBase);

	render->bitShapeMemory = DoomRPG_freeMemory() - render->bitShapeMemory;
	//printf("bitShapeMemory %d\n", render->bitShapeMemory);

	return true;
}

boolean Render_loadTexels(Render_t* render)
{
	int* buff;
	int block = 2048; //new instead of 256 old value 256
	byte* ioBuffer, * ioBufferBase, ioBufferTmp[2048]; //iobuffertmp[256]
	int i, mediaTexelOffset, mapSpriteTexelIndex, i5, i8;
	//Uint32 i, mediaTexelOffset, mapSpriteTexelIndex, i5, i8;
	int mapTextureTexelIndex, n12, n13, n15, mapSpriteTexelsIndex, n22, n24, n26;
	printf("---Render_loadTexels---\n");

	render->texelMemory = DoomRPG_freeMemory();

	for (i = 0; i < render->mapTextureTexelsCount - 1; i++) {
		for (mediaTexelOffset = 0; mediaTexelOffset < (render->mapTextureTexelsCount - 1) - i; mediaTexelOffset++) {
			if (render->mediaTexelOffsets[render->mapTextureTexels[mediaTexelOffset + 1] * 2] <
				render->mediaTexelOffsets[render->mapTextureTexels[mediaTexelOffset + 0] * 2]) {
				Uint32 mapTextureTexels = render->mapTextureTexels[mediaTexelOffset + 0]; //int
				render->mapTextureTexels[mediaTexelOffset + 0] = render->mapTextureTexels[mediaTexelOffset + 1];
				render->mapTextureTexels[mediaTexelOffset + 1] = mapTextureTexels;
			}
		}
	}

	//sort offfsets
	for (mapSpriteTexelIndex = 0; mapSpriteTexelIndex < render->mapSpriteTexelsCount - 1; mapSpriteTexelIndex++) {
		for (i5 = 0; i5 < (render->mapSpriteTexelsCount - 1) - mapSpriteTexelIndex; i5++) {
			//swap
			if (Render_getSTexelOffsets(render, render->mapSpriteTexels[i5 + 1]) <
				Render_getSTexelOffsets(render, render->mapSpriteTexels[i5 + 0])) {
				Uint32 i6 = render->mapSpriteTexels[i5 + 0];
				render->mapSpriteTexels[i5 + 0] = render->mapSpriteTexels[i5 + 1];
				render->mapSpriteTexels[i5 + 1] = i6;
			}
		}
	}
	int i7 = (render->mapTextureTexelsCount * 64 * 64) / 2;
	//int i7 = (render->mapTextureTexelsCount * 64 * 64); // / 2;

	buff = SDL_malloc(render->mapSpriteTexelsCount * sizeof(int));
	for (i8 = 0; i8 < render->mapSpriteTexelsCount; i8++) {
		buff[i8] = Render_getSTexelBufferSize(render, render->mapSpriteTexels[i8]);
		i7 += (buff[i8] / 2);
	}
	//printf("i7 %d\n", i7);

	SDL_free(render->mediaTexels);
	render->mediaTexels = SDL_malloc(i7 * sizeof(*render->mediaTexels));


	int dataSize;
	// Read Wall Texels Data
	ioBuffer = ioBufferBase = DoomRPG_fileOpenRead(render->doomRpg, "wtexels.bin");

	dataSize = DoomRPG_intAt(ioBuffer, 0);
	ioBuffer += sizeof(int);
	//printf("dataSize %d\n", dataSize);


	int LoadingBarCounter = 0;
	int n6 = 0;
	int n7 = -1;
	int m = 0; //int
	int n8 = 0;

	

	for (mapTextureTexelIndex = 0; mapTextureTexelIndex < render->mapTextureTexelsCount; ++mapTextureTexelIndex) {
		Uint32 mediaTexelOffsetIndex = render->mapTextureTexels[mapTextureTexelIndex] * 2; //int
		Uint32 mediaTexelOffset = render->mediaTexelOffsets[mediaTexelOffsetIndex]; //int
		//testsub("Render_loadTexels1");
		if (mediaTexelOffset == n7) {
			//testsub("Render_loadTexels1a");
			render->mediaTexelOffsets[mediaTexelOffsetIndex] = (n8 << 1);
		}
		else {
			//testsub("Render_loadTexels1b");
			n7 = mediaTexelOffset;
			if (++LoadingBarCounter == 3) {
				DoomCanvas_updateLoadingBar(render->doomRpg->doomCanvas);
				LoadingBarCounter = 0;
			}
			//testsub("Render_loadTexels1b1");
			if (mediaTexelOffset > m) {
				Uint32 n11 = (mediaTexelOffset - m) / 2; //int
				for (n12 = 0; n12 < n11; n12 += block) {
					//a(resourceAsStream, ioBuffer, (n12 + 256 > n11) ? (n11 - n12) : 256);
					//testsub("Render_loadTexels1b2");
					//printf("n12=%i n11=%i\n mtoi=%i count%i\n", n12, n11, mediaTexelOffsetIndex, render->mapTextureTexelsCount);
					memcpy(ioBufferTmp, ioBuffer, (n12 + block > n11) ? (n11 - n12) : block);
					ioBuffer += (n12 + block > n11) ? (n11 - n12) : block;
				}
				m += mediaTexelOffset - m;
			}
			else if (m != mediaTexelOffset) {
				//testsub("Render_loadTexels1c");
				DoomRPG_Error("ERROR: passed texture offset texOfs: %d streamPos: %d", mediaTexelOffset, m);
			}
			//testsub("Render_loadTexels2");
			n8 = n6;
			for (n13 = 0; n13 < 2048; n13 += block) {
				int n14 = (n13 + block > 2048) ? (2048 - n13) : block; //mediatexelcount
				//int n14 = 2048;
				//a(resourceAsStream, ioBuffer, n14);

				memcpy(ioBufferTmp, ioBuffer, n14);
				ioBuffer += n14;

				//old
					// for (n15 = 0; n15 < n14; ++n15) {
					// 	//CI4 32 x 64 = 2048
					// 	int test = ((int)(n15 / 2048) * 2048) //picture
					// 				+ (((n15 & 2047) & 31) * 64) //row
					// 				+ (int)((n15 & 2047) / 64); //remaing row
									
					// 	//render->mediaTexels[n6++] = SDL_SwapLE8(ioBufferTmp[n15]); //SDL_SwapLE8(ioBufferTmp[n15]); //original			
					// 	render->mediaTexels[n6++] = (ioBufferTmp[test]); //rdp
					// 	//render->mediaTexels[n6++] = (ioBufferTmp[n15]>>4) & 0xf; //working

					// 	//CI8
					// 	//int n16 = ioBufferTmp[n15] & 0xFF;
					// 	// render->mediaTexels[n6++] = (byte)(n16 >> 4 & 0xF);
					// 	// render->mediaTexels[n6++] = (byte)(n16 & 0xF);

					// }	

				//new
				// if(n14 != block){
				// 	for (n15 = 0; n15 < n14; ++n15) {
				// 		//CI4 32 x 64 = 2048
				// 		int test = ((int)(n15 / 2048) * 2048) //picture
				// 					+ (((n15 & 2047) & 31) * 64) //row
				// 					+ (int)((n15 & 2047) / 64); //remaing row
									
				// 		//render->mediaTexels[n6++] = SDL_SwapLE8(ioBufferTmp[n15]); //SDL_SwapLE8(ioBufferTmp[n15]); //original			
				// 		render->mediaTexels[n6++] = SDL_SwapLE8(ioBufferTmp[test]); //rdp
				// 		//render->mediaTexels[n6++] = (ioBufferTmp[n15]>>4) & 0xf; //working

				// 		//CI8
				// 		//int n16 = ioBufferTmp[n15] & 0xFF;
				// 		// render->mediaTexels[n6++] = (byte)(n16 >> 4 & 0xF);
				// 		// render->mediaTexels[n6++] = (byte)(n16 & 0xF);

				// 	}	
				// }
				// else{
					//convert ci4->ci8 then rotate stream to scale for rdp
					byte temp[4096];
					for(n15 = 0; n15 < n14; ++n15){
						temp[2* n15 + 1] = ioBufferTmp[n15] >> 4;
						temp[2* n15 + 0] = ioBufferTmp[n15] & 0xf;
					}

					int row, col;
					for(row = 0; row < 64; row++){ //y
						for(col = 0; col < 32; col++){ //x

							int test = col * 128 + (row);

							render->mediaTexels[n6++] = (temp[test] << 4) | temp[test + 1];
						}

					}

				//}

			}
			//testsub("Render_loadTexels3");
			m += 4096;
			render->mediaTexelOffsets[2 * render->mapTextureTexels[mapTextureTexelIndex]] = (n8 << 1);
		}
	}

	SDL_free(ioBufferBase);

	int mapTextureTexels = dataSize * 2;

	// Read Sprite Texels Data
	ioBuffer = ioBufferBase = DoomRPG_fileOpenRead(render->doomRpg, "stexels.bin");
	dataSize = DoomRPG_intAt(ioBuffer, 0);
	ioBuffer += sizeof(int);
	//printf("dataSize %d\n", dataSize);
	int n17 = -1;
	//Uint32 n17 = -1;
//testsub("Render_loadTexels_1");
	for (mapSpriteTexelsIndex = 0; mapSpriteTexelsIndex < render->mapSpriteTexelsCount; ++mapSpriteTexelsIndex) {
		//int mediaBitShapeOffset = render->mediaBitShapeOffsets[render->mapSpriteTexels[mapSpriteTexelsIndex] * 2];
		Uint32 mediaBitShapeOffset = render->mediaBitShapeOffsets[render->mapSpriteTexels[mapSpriteTexelsIndex] * 2];
		//printf("mediaBitShapeOffset=%x\n mapSpriteTexels[i]=%x\n mapSpriteTexelsIndex=%x\n mapspriteTexelcount=%x\n",
		//(mediaBitShapeOffset), (render->mapSpriteTexels[mapSpriteTexelsIndex]), mapSpriteTexelsIndex, render->mapSpriteTexelsCount);
		//printf("mbso=%x n17=%i\n", mediaBitShapeOffset, n17);
		if (mediaBitShapeOffset == n17) {
			//testsub("Render_loadTexels_1a");
			render->shapeData[mediaBitShapeOffset + 0] = (short)(n8 & 0xFFFF);
			render->shapeData[mediaBitShapeOffset + 1] = (short)((n8 >> 16) & 0xFFFF);
		}
		else {
			//testsub("Render_loadTexels_1b");
			n17 = mediaBitShapeOffset;
			if (++LoadingBarCounter == 3) {
				DoomCanvas_updateLoadingBar(render->doomRpg->doomCanvas);
				LoadingBarCounter = 0;
			}
			//int mapSpriteTexelIndex = (render->shapeData[mediaBitShapeOffset] & 0xFFFF) | render->shapeData[mediaBitShapeOffset + 1] << 16;
			Uint32 mapSpriteTexelIndex = (render->shapeData[mediaBitShapeOffset] & 0xFFFF) | (render->shapeData[mediaBitShapeOffset + 1] << 16);
			printf("shapedata1=%x shapedata2=%x\n", render->shapeData[mediaBitShapeOffset], render->shapeData[mediaBitShapeOffset + 1]);
			int n20 = buff[mapSpriteTexelsIndex];
			n8 = (n6 << 1);
			printf("mapSpriteTexelIndex=%i mapSpriteTexel=%i\n", mapSpriteTexelIndex, mapTextureTexels);
			if (mapSpriteTexelIndex > mapTextureTexels) {
				int n21 = (mapSpriteTexelIndex - mapTextureTexels) / 2;
				printf("n21=%i\n", n21);
				for (n22 = 0; n22 < n21; n22 += block) {
					//a(resourceAsStream2, r.c, (n22 + 256 > n21) ? (n21 - n22) : 256);
					memcpy(ioBufferTmp, ioBuffer, (n22 + block > n21) ? (n21 - n22) : block);
					ioBuffer += (n22 + block > n21) ? (n21 - n22) : block;
				}
				mapTextureTexels += n21 * 2;
				printf("mapTextureTexels=%i\n", mapTextureTexels);
			}
			else if (mapSpriteTexelIndex != mapTextureTexels) {
				DoomRPG_Error("ERROR: passed texel position: texelBase: %d streamPos: %d", mapSpriteTexelIndex, mapTextureTexels);
			}
			//testsub("Render_loadTexels_2");
			int n23 = n20 / 2;
			for (n24 = 0; n24 < n23; n24 += block) {
				int n25 = (n24 + block > n23) ? (n23 - n24) : block;
				//a(resourceAsStream2, r.c, n25);
				//testsub("Render_loadTexels_2a");
				memcpy(ioBufferTmp, ioBuffer, n25);
				ioBuffer += n25;
				for (n26 = 0; n26 < n25; ++n26) {
					//testsub("Render_loadTexels_2b");
					int test = ((int)(n24 / 2048) * 2048) //picture
					+ (((n26 & 2047) & 31) * 64) //row
					+ (int)((n26 & 2047) / 64); //remaing row
					//CI4
					render->mediaTexels[n6++] = (ioBufferTmp[n26]); //original
					//render->mediaTexels[n6++] = (ioBufferTmp[n26]>>4) & 0xf;

					//CI8
					//int n27 = ioBufferTmp[n26] & 0xFF;
					//render->mediaTexels[n6++] = (byte)(n27 & 0xF);
					//render->mediaTexels[n6++] = (byte)(n27 >> 4 & 0xF);

					//rdp
					//int n27 = ioBufferTmp[n26] & 0xFF;
					//render->mediaTexels[n6 + ((n26 )*256) + (n24/256)] = (ioBufferTmp[n26]);
					//render->mediaTexels[((2* n26 +0)*256) + (n24/256)] = (byte)(n27 & 0xF);
					//render->mediaTexels[((2 * n26 +1)*256) + (n24/256)] = (byte)(n27 >> 4 & 0xF);
				}
			}
			//n6+=n23;
			//testsub("Render_loadTexels_3");
			mapTextureTexels += n23 * 2;
			render->shapeData[mediaBitShapeOffset + 0] = (short)(n8 & 0xFFFF);
			render->shapeData[mediaBitShapeOffset + 1] = (short)(n8 >> 16 & 0xFFFF);
		}
	}

	testsub("Render_loadTexels_end");
	SDL_free(ioBufferBase);

	render->texelMemory = DoomRPG_freeMemory() - render->texelMemory;
	//printf("texelMemory %d\n", render->texelMemory);
	return true;
}

//int Render_getSTexelOffsets(Render_t* render, int i)
Uint32 Render_getSTexelOffsets(Render_t* render, int i)
{
	//int mediaTexelOffset = render->mediaBitShapeOffsets[i * 2];
	Uint32 mediaTexelOffset = render->mediaBitShapeOffsets[i * 2];
	return (render->shapeData[mediaTexelOffset + 0] & 65535) | (render->shapeData[mediaTexelOffset + 1] << 16);
}

int Render_getSTexelBufferSize(Render_t* render, int i)
{
	int offset, offsetSkip, buffSize, beg, end, j, k;

	offset = render->mediaBitShapeOffsets[i * 2];
	beg = render->shapeData[offset + 2];
	end = render->shapeData[offset + 3];
	buffSize = 0;
	offsetSkip = offset + 4;
	for (j = beg; j <= end; ++j) {
		for (k = offset + render->shapeData[offsetSkip + (j - beg)] + 2; (render->shapeData[k] & 0xFF) != 0x7F; k += 2) {
			buffSize += render->shapeData[k] >> 8;
		}
	}
	return (buffSize + 1) & -2;
}

void Render_relinkSprite(Render_t* render, Sprite_t* sprite)
{
	Node_t* node;
	Sprite_t* sprt;
	int i;
	int check1, check2;

	#ifdef BREW
		check1 = 0x30000;
		check2 = 0x10000;
	#else
		check1 = 0x300;
		check2 = 0x100;
	#endif
	//testsub("Render_relinkSprite");
	if (sprite->node) {
		if (sprite->node->sprites == sprite) {
			sprite->node->sprites = sprite->nodeNext;
		}
		else {
			sprt = sprite->node->sprites;
			while (sprt && sprt->nodeNext != sprite) {
				sprt = sprt->nodeNext;
			}
			if (sprt) {
				sprt->nodeNext = sprite->nodeNext;
			}
		}
	}
	//testsub("Render_relinkSprite1");
	node = render->nodes;
	while (node->args1 & check1) {
		i = node->args1 & 65535;
		//testsub("Render_relinkSprite2a");
		if (node->args1 & check2) {
			if (sprite->x > i) {
				node = &render->nodes[node->args2 & 65535];
			}
			else {
				node = &render->nodes[(node->args2 >> 16) & 65535];
			}
		}
		else {

			if (sprite->y > i) {
				node = &render->nodes[node->args2 & 65535];
			}
			else {
				node = &render->nodes[(node->args2 >> 16) & 65535];
			}
		}
		//testsub("Render_relinkSprite2b");
	}
	sprite->node = node;
	sprite->nodeNext = node->sprites;
	node->sprites = sprite;
	//testsub("Render_relinkSprite end");
}

void Render_addMapTextures(Render_t* render, int textureId)
{
	// Nuevo: esto evita desbordamientos de b�fer
	// Esto sucede en el archivo level05.bsp, ya que al leer los datos (floorTex) tiene un valor de 153(0x99)
	// lo que supera el l�mite de memoria inicializado de mediaTexturesIds que es 152,
	// creando as� el desbordamiento, esto tambi�n sucede en dispositivos m�viles BREW
	//
	// New: this prevents buffer overflows
	// This happens in the level05.bsp file, since when reading the data (floorTex) it has a value of 153(0x99)
	// which exceeds the initialized memory limit of mediaTexturesIds which is 152, thus creating the overflow,
	// this also happens on devices mobile BREW
	//

	//testsub("Render_addMapTextures");
	if (textureId >= render->textureCnt) {
		textureId = (render->textureCnt - 1);
	}

	Render_addMapTexture(render, render->mediaTexturesIds[textureId] + 0);
	Render_addMapTexture(render, render->mediaTexturesIds[textureId] + 1);
	Render_addMapTexture(render, render->mediaTexturesIds[textureId] + 2);
	Render_addMapTexture(render, render->mediaTexturesIds[textureId] + 3);
}

void Render_addMapTexture(Render_t* render, int textureIndex)
{
	int i;
	//testsub("Render_addMapTexture");
	for (i = 0; i < render->mapTextureTexelsCount; i++)
	{
		if (render->mapTextureTexels[i] == textureIndex) {
			return;
		}
	}
	render->mapTextureTexels[render->mapTextureTexelsCount++] = textureIndex;
}

void Render_addMapSprites(Render_t* render, int spriteId)
{
	// Nuevo: esto evita desbordamientos de b�fer
	// New avoid buffer overflows

	testsub("Render_addMapSprites");
	if (spriteId >= render->spriteCnt) {
		spriteId = (render->spriteCnt - 1);
	}

	Render_addMapSprite(render, render->mediaSpriteIds[spriteId] + 0);
	Render_addMapSprite(render, render->mediaSpriteIds[spriteId] + 1);
	Render_addMapSprite(render, render->mediaSpriteIds[spriteId] + 2);
	Render_addMapSprite(render, render->mediaSpriteIds[spriteId] + 3);
	if ((spriteId - 17) < 38) {
		Render_addMapSprite(render, render->mediaSpriteIds[spriteId] + 4);
		Render_addMapSprite(render, render->mediaSpriteIds[spriteId] + 5);
		Render_addMapSprite(render, render->mediaSpriteIds[spriteId] + 6);
	}
}

void Render_addMapSprite(Render_t* render, int spriteIndex)
{
	int i;
	//testsub("Render_addMapSprite");
	for (i = 0; i < render->mapSpriteTexelsCount; i++)
	{
		if (render->mapSpriteTexels[i] == spriteIndex) {
			return;
		}
	}
	//printf("texelcount=%i spriteindex=%x\n", render->mapSpriteTexelsCount, spriteIndex);
	render->mapSpriteTexels[render->mapSpriteTexelsCount++] = spriteIndex;

}


void Render_render(Render_t* render, int viewx, int viewy, int viewz, unsigned int viewangle)
{
	int sin_, cos_, vx, vy;
	testsub("Render_render");
	sin_ = render->sinTable[viewangle & 255];
	cos_ = render->sinTable[(viewangle + 64) & 255];

	render->animFrameTime = DoomRPG_GetTimeMS();

	if (!render->skipViewNudge) {
		vx = viewx - ((16 * cos_) >> 16);
		vy = viewy + ((16 * sin_) >> 16);
	}
	else {
		vx = viewx;
		vy = viewy;
	}

	if(consoledebug != SDL_FALSE) return; 

	render->viewX = vx;
	render->viewY = vy;
	render->viewZ = viewz;
	render->viewCos_ = cos_;
	render->viewSin_ = -sin_;
	render->viewTransX = -((vx * render->viewCos_) + (vy * render->viewSin_));
	render->viewSin = sin_;
	render->viewCos = cos_;
	render->viewTransY = -((vx * render->viewSin) + (vy * render->viewCos));
	render->viewAngle = viewangle;

	//printf("pos %d\n", render->pitch * (render->screenY + render->doomRpg->doomCanvas->shakeY) + (render->screenX + render->doomRpg->doomCanvas->shakeX) * sizeof(short));
	render->pixels = &fbuffer[render->pitch * (render->screenY/2 + render->doomRpg->doomCanvas->shakeY) + (render->screenX + render->doomRpg->doomCanvas->shakeX)]; //(short*)&render->framebuffer[render->pitch * (render->screenY + render->doomRpg->doomCanvas->shakeY) + (render->screenX + render->doomRpg->doomCanvas->shakeX) * sizeof(short)];

	render->screenLeft = -render->doomRpg->doomCanvas->shakeX;
	render->screenTop = -render->doomRpg->doomCanvas->shakeY;
	render->screenRight = render->screenWidth - render->doomRpg->doomCanvas->shakeX;
	render->screenBottom = render->screenHeight - render->doomRpg->doomCanvas->shakeY;

	Render_initColumnScale(render);

	render->currentFrameTime = DoomRPG_GetTimeMS();
	testsub("render_render1");
	if (render->doomRpg->doomCanvas->renderFloorCeilingTextures) {
		Render_renderFloorAndCeilingBG(render);
	}
	else {
		Render_renderFloorAndCeilingSolidBG(render);
	}
	
	render->horizonFrameTime = DoomRPG_GetTimeMS();
	testsub("render_render2");
	if (render->skipBSP == 0) { //==0
		Render_renderBSP(render);
	}
	else {
		Render_renderBSPNoclip(render);
	}
	testsub("Render_render1");
	render->frameTime = DoomRPG_GetTimeMS();

	//------------------------------------//

	/*SDL_Rect renderQuad, clip;

	clip.x = render->screenX;
	clip.y = render->screenY;
	clip.w = render->screenWidth;
	clip.h = render->screenHeight;

	renderQuad.x = render->screenX;
	renderQuad.y = render->screenY;
	renderQuad.w = sdlVideo.rendererW;
	renderQuad.h = sdlVideo.rendererH;
	if (clip.w <= renderQuad.w) {
		renderQuad.w = clip.w;
	}
	if (clip.h <= renderQuad.h) {
		renderQuad.h = clip.h;
	}

	SDL_UpdateTexture(render->piDIB, NULL, render->framebuffer, sdlVideo.rendererW * 2);
	SDL_RenderCopy(render->doomRpg->sdlVideo->renderer, render->piDIB, &clip, &renderQuad);*/
	testsub("Render_render_end");
}

void Render_initColumnScale(Render_t* render)
{
	int i;
	for (i = render->screenWidth; --i >= 0; render->columnScale[i] = COLUMN_SCALE_INIT);
}


void Render_renderFloorAndCeilingSolidBG(Render_t* render)
{
	int i, h, pitch;

	pitch = ((render->pitch * render->screenY) + (render->screenX * sizeof(short)));

	 h = render->screenHeight >> 1;
	 rdpsquare(render->screenY,h + render->screenY, *render->ceilingColor);

	// for (i = 0; i < h; i++) {
	// 	SDL_memmove(&render->framebuffer[pitch + (render->pitch * i)], render->ceilingColor, (render->screenWidth * sizeof(short)));
	// }
	//rdpsquare(h+ render->screenY,render->screenHeight, *render->floorColor);
	rdpsquare(h+ render->screenY, 240, *render->floorColor);

	// for (i = h; i < render->screenHeight; i++) {
	// 	SDL_memmove(&render->framebuffer[pitch + (render->pitch * i)], render->floorColor, (render->screenWidth * sizeof(short)));
	// }
}

void Render_renderFloorAndCeilingBG(Render_t* render)
{
	int top;
	int right;
	int left;
	int bottom;

	top = 0;
	if (render->screenTop > top) {
		top = render->screenTop;
	}

	left = 0;
	if (render->screenLeft > left) {
		left = render->screenLeft;
	}

	right = render->screenWidth;
	if (render->screenRight < right) {
		right = render->screenRight;
	}

	bottom = render->screenHeight;
	if (render->screenBottom < bottom) {
		bottom = render->screenBottom;
	}

	while (top < render->halfScreenHeight) {
		Render_drawplane(render, left, top, render->planeTextures + 1024, right);
		top++;
	}

	while (top < bottom) {
		Render_drawplane(render, left, top, render->planeTextures, right);
		top++;
	}
}

void Render_drawplane(Render_t* render, int x, int y, short** planeTextures, int cnt)
{
	int v14;
	int v15;
	int v16;
	int v17;
	int v18;

	int vX, vY, vZ;
	int width, height;
	signed int v6, v9;
	signed int vsin, vcos;

	width = render->screenWidth;
	height = render->screenHeight;

	vX = render->viewX << 16;
	vY = render->viewY << 16;
	vZ = render->viewZ;

	vcos = render->viewCos;
	vsin = render->viewSin;

	v9 = (x << 1);
	v6 = (y << 1);

	if (height > v6) {
		vZ = 64 - vZ;
	}

	v14 = (vZ << 3);
	if (height > v6) {
		v15 = height - v6;
	}
	else {
		v15 = v6 - height;
	}

	v16 = ((width * v14) / (v15 + 1));
	v17 = (unsigned int)v16;
	v18 = ((v16 << 19) / (32 * width)) >> 8;

	Render_spanPlane(render,
		v9 >> 1, // x offset
		v6 >> 1, // y line
		planeTextures,
		vX + ((vcos * v17) >> 3) + (((v18 * (vsin >> 8)) * (v9 - width + 1)) >> 1), //img x start
		vY + ((-vsin * v17) >> 3) + (((v18 * (vcos >> 8)) * (v9 - width + 1)) >> 1), //img y start
		v18 * (vsin >> 8), //img x inc
		v18 * (vcos >> 8), //img y inc
		cnt); //line length
}

void Render_spanPlane(Render_t* render, int x, int y, short** planeTextures, int param_5, int param_6,
	int param_7, int param_8, int cnt)
{
	short* pixels;
	int offset1, offset2;

	pixels = render->pixels + (render->pitch >> 1) * y + x;
	while (--cnt >= 0)
	{
		offset1 = (((unsigned int)(param_6 >> 17) & 0x3E0) + ((unsigned int)(param_5 << 5) >> 27)); //flat
		offset2 = (((unsigned int)(param_6 >> 10) & 0xFC0) + ((unsigned int)(param_5 << 10) >> 26)); //pixel

		*pixels++ = planeTextures[offset1][offset2];

		param_5 += param_7;
		param_6 += param_8;
	}
}

void Render_renderBSP(Render_t* render)
{
	Node_t* viewNode;
	Node_t* viewNodes;
	Sprite_t* viewSprite;

	render->nodeCount = 0;
	render->nodeRasterCount = 0;
	render->lineCount = 0;
	render->lineRasterCount = 0;
	render->spriteCount = 0;
	render->spriteRasterCount = 0;
	render->viewSprites = NULL;

	viewNodes = &render->viewNodes;
	render->viewNodes.next = viewNodes;
	render->viewNodes.prev = viewNodes;
	Render_walkNode(render, 0);

	// New BREW Version
	if (!render->skipLines) {
		for (viewNode = viewNodes->next; viewNodes != viewNode; viewNode = viewNode->next) {
			Render_drawNodeLines(render, viewNode);
		}
	}

	if (!render->skipSprites) {
		for (viewSprite = render->viewSprites; viewSprite != NULL; viewSprite = viewSprite->viewNext) {
			render->spriteCount++;
			Render_renderSpriteObject(render, viewSprite);

		}
	}
}

void Render_renderBSPNoclip(Render_t* render)
{
	Line_t* line;
	int i;

	render->nodeCount = 0;
	render->lineCount = 0;
	render->lineRasterCount = 0;
	render->spriteCount = 0;
	render->spriteRasterCount = 0;

	if (!render->skipLines) {
		for (i = 0; i < render->linesLength; i++) {
			render->lineCount++;
			line = &render->lines[i];
			render->tmpLine.vert1.x = line->vert1.x;
			render->tmpLine.vert1.y = line->vert1.y;
			render->tmpLine.vert1.z = line->vert1.z;
			render->tmpLine.vert2.x = line->vert2.x;
			render->tmpLine.vert2.y = line->vert2.y;
			render->tmpLine.vert2.z = line->vert2.z;
			render->tmpLine.texture = render->mediaTexturesIds[line->texture];
			render->spanMode = 0;
			render->tmpLine.flags = line->flags;
			render->numLines = i;
			Render_drawLines(render, &render->tmpLine);
		}
	}

	if (!render->skipSprites) {
		for (i = 0; i < render->numMapSprites; i++) {
			render->spriteCount++;
			render->numLines = i;
			Render_renderSpriteObject(render, &render->mapSprites[i]);
		}
	}
}

void Render_walkNode(Render_t* render, int i)
{
	int sortZ, mapTextureTexels, mapSpriteTexelIndex, i5, indx;
	Node_t* node;
	Line_t* line;
	Sprite_t* sprite;
	Sprite_t* sprite2;

	node = &render->nodes[i];

	//printf("node->x1 %d \n", node->x1);
	//printf("node->y1 %d \n", node->y1);
	//printf("node->x2 %d \n", node->x2);
	//printf("node->y2 %d \n", node->y2);

	if (Render_cullBoundingBox(render, node)) {
		return;
	}

	//printf("true nodeCount %d \n", nodeCount);

	render->nodeCount++;
	if ((node->args1 & 0x30000) == 0)
	{
		mapTextureTexels = (node->args2 >> 16) & 65535;
		render->lineCount += mapTextureTexels;

		render->nodeRasterCount++;
		node->prev = &render->viewNodes;
		node->next = render->viewNodes.next;
		render->viewNodes.next = node;
		node->next->prev = node;

		for (mapSpriteTexelIndex = 0; mapSpriteTexelIndex < mapTextureTexels; mapSpriteTexelIndex++)
		{
			render->numLines = (node->args2 & 0xffffU) + mapSpriteTexelIndex;
			line = &render->lines[render->numLines];
			render->tmpLine.vert1.x = line->vert1.x;
			render->tmpLine.vert1.y = line->vert1.y;
			render->tmpLine.vert1.z = line->vert1.z;
			render->tmpLine.vert2.x = line->vert2.x;
			render->tmpLine.vert2.y = line->vert2.y;
			render->tmpLine.vert2.z = line->vert2.z;
			render->tmpLine.flags = line->flags | 0x20000000;
			render->spanMode = 0;


			/*printf("-o-o-o-o-o-o-o-o-o-o-o-o-o\n");
			printf("line v1X %d\n", line->vert1.x);
			printf("line v1Y %d\n", line->vert1.y);
			printf("line v1Z %d\n", line->vert1.z);
			printf("line v2X %d\n", line->vert2.x);
			printf("line v2Y %d\n", line->vert2.y);
			printf("line v2Z %d\n", line->vert2.z);*/


			Render_drawLines(render, &render->tmpLine);
		}

		// addSprite
		for (sprite = node->sprites; sprite != NULL; sprite = sprite->nodeNext)
		{
			if ((sprite->info & 0x800000) != 0) {
				sortZ = MAXINT;
			}
			else {
				sortZ = (sprite->x * render->viewCos_) + (sprite->y * render->viewSin_) + render->viewTransX;

				if ((sprite->info & 0x1000000) != 0) {
					sortZ++;
				}
				else
				{
					if (sprite->ent != NULL && sprite->ent->def->eType == 1) {
						sortZ--;
					}
					else {
						indx = sprite->info & 0x1FF;

						/*if (indx >= 193 && indx <= 198) { // J2ME Vesion
							mediaTexelOffset -= 2;
						}*/

						// BREW Version
						if (indx < 184 || indx > 190) {
							if (indx >= 180 && indx <= 191) {
								sortZ -= 2;
							}
						}
						else {
							sortZ = 0;
						}
					}
				}
			}

			if (!(sprite->info & 0x10000000)) {
				sprite->info |= 0x10000000;
			}
			sprite->sortZ = sortZ;

			if (render->viewSprites == NULL) {
				sprite->viewNext = NULL;
				render->viewSprites = sprite;
			}
			else if (sortZ >= render->viewSprites->sortZ) {
				sprite->viewNext = render->viewSprites;
				render->viewSprites = sprite;
			}
			else {
				sprite2 = render->viewSprites;
				while (sprite2->viewNext != NULL && sortZ < sprite2->viewNext->sortZ) {
					sprite2 = sprite2->viewNext;
				}
				sprite->viewNext = sprite2->viewNext;
				sprite2->viewNext = sprite;
			}
		}

		return;
	}

	i5 = node->args1 & 0xFFFF;
	if (((node->args1 & 0x20000) == 0 || render->viewY <= i5) &&
		((node->args1 & 0x10000) == 0 || render->viewX <= i5)) {
		Render_walkNode(render, (node->args2 >> 16) & 0xFFFF);
		Render_walkNode(render, node->args2 & 0xFFFF);
	}
	else {
		Render_walkNode(render, node->args2 & 0xFFFF);
		Render_walkNode(render, (node->args2 >> 16) & 0xFFFF);
	}
}

boolean Render_cullBoundingBox(Render_t* render, Node_t* node)
{
	Line_t* line;
	int i;

	if (render->skipCull) {
		return false;
	}

	if ((render->viewX >= (node->x1 - 5)) &&
		(render->viewX <= (node->x2 + 5)) &&
		(render->viewY >= (node->y1 - 5)) &&
		(render->viewY <= (node->y2 + 5))) {
		return false;
	}

	line = &render->tmpLine;
	if (render->viewX < node->x1)
	{
		if (render->viewY < node->y1)
		{
			line->vert1.x = node->x2;
			line->vert1.y = node->y1;
			line->vert2.x = node->x1;
			line->vert2.y = node->y2;
		}
		else if (render->viewY < node->y2)
		{
			line->vert1.x = node->x1;
			line->vert1.y = node->y1;
			line->vert2.x = node->x1;
			line->vert2.y = node->y2;
		}
		else
		{
			line->vert1.x = node->x1;
			line->vert1.y = node->y1;
			line->vert2.x = node->x2;
			line->vert2.y = node->y2;
		}
	}
	else if (render->viewX < node->x2)
	{
		if (render->viewY < node->y1)
		{
			line->vert1.x = node->x2;
			line->vert1.y = node->y1;
			line->vert2.x = node->x1;
			line->vert2.y = node->y1;
		}
		else if (render->viewY < node->y2)
		{
			return false;
		}
		else
		{
			line->vert1.x = node->x1;
			line->vert1.y = node->y2;
			line->vert2.x = node->x2;
			line->vert2.y = node->y2;
		}
	}
	else if (render->viewY < node->y1)
	{
		line->vert1.x = node->x2;
		line->vert1.y = node->y2;
		line->vert2.x = node->x1;
		line->vert2.y = node->y1;
	}
	else if (render->viewY < node->y2)
	{
		line->vert1.x = node->x2;
		line->vert1.y = node->y2;
		line->vert2.x = node->x2;
		line->vert2.y = node->y1;
	}
	else
	{
		line->vert1.x = node->x1;
		line->vert1.y = node->y2;
		line->vert2.x = node->x2;
		line->vert2.y = node->y1;
	}

	//printf("1-----------\n");
	//printf("line v1X %d\n", line->vert1.x);
	//printf("line v1Y %d\n", line->vert1.y);
	//printf("line v2X %d\n", line->vert2.x);
	//printf("line v2Y %d\n", line->vert2.y);

	Render_transform2DVerts(render, &line->vert1);
	Render_transform2DVerts(render, &line->vert2);
	//printf("2-----------\n");
	//printf("line v1X %d\n", line->vert1.x);
	//printf("line v1Y %d\n", line->vert1.y);
	//printf("line v1Z %d\n", line->vert1.z);
	//printf("line v2X %d\n", line->vert2.x);
	//printf("line v2Y %d\n", line->vert2.y);
	//printf("line v2Z %d\n", line->vert2.z);

	if (Render_clipLine(render, line)) {
		//printf("3-----------\n");
		//printf("line v1X %d\n", line->vert1.x);
		//printf("line v1Y %d\n", line->vert1.y);
		//printf("line v1Z %d\n", line->vert1.z);
		//printf("line v2X %d\n", line->vert2.x);
		//printf("line v2Y %d\n", line->vert2.y);
		//printf("line v2Z %d\n", line->vert2.z);

		Render_projectVertex(render, &line->vert1);
		Render_projectVertex(render, &line->vert2);
		//printf("4-----------\n");
		//printf("line v1X %d\n", line->vert1.x);
		//printf("line v1Y %d\n", line->vert1.y);
		//printf("line v2X %d\n", line->vert2.x);
		//printf("line v2Y %d\n", line->vert2.y);

		// clippedLineVisCheck
		{
			int x1 = (line->vert1.x + 0xFFFF) >> FRACBITS;
			int x2 = (line->vert2.x + 0xFFFF) >> FRACBITS;
			//printf("x1 %d\n", x1);
			//printf("x2 %d\n", x2);

			for (i = x1; i < x2; ++i) {
				if (render->columnScale[i] == COLUMN_SCALE_INIT) {
					return false;
				}
			}
		}
	}

	return true;
}

void Render_transform2DVerts(Render_t* render, Vertex_t* vert)
{
	int x, y;
	x = (vert->x * render->viewCos_) + (vert->y * render->viewSin_) + render->viewTransX;
	y = (vert->x * render->viewSin) + (vert->y * render->viewCos) + render->viewTransY;

	vert->x = x;
	vert->y = y;
}

boolean Render_clipLine(Render_t* render, Line_t* line)
{
	int i, mediaTexelOffset, mapTextureTexels, mapSpriteTexelIndex, i5, i6;

	i = line->vert1.x + line->vert1.y;
	mediaTexelOffset = line->vert2.x + line->vert2.y;
	if (i < 0)
	{
		if (mediaTexelOffset < 0) {
			return false;
		}
		//printf("1\n");
		Render_clipVertex(render, &line->vert1, line, i, mediaTexelOffset);
	}
	else if (mediaTexelOffset < 0)
	{
		//printf("2\n");
		Render_clipVertex(render, &line->vert2, line, -i, -mediaTexelOffset);
	}

	mapTextureTexels = line->vert1.x - line->vert1.y;
	mapSpriteTexelIndex = line->vert2.x - line->vert2.y;
	if (mapSpriteTexelIndex < 0)
	{
		if (mapTextureTexels < 0) {
			return false;
		}
		//printf("3\n");
		Render_clipVertex(render, &line->vert2, line, mapTextureTexels, mapSpriteTexelIndex);
	}
	else if (mapTextureTexels < 0)
	{
		//printf("4\n");
		Render_clipVertex(render, &line->vert1, line, -mapTextureTexels, -mapSpriteTexelIndex);
	}

	i5 = line->vert1.x - 0x40000;
	i6 = line->vert2.x - 0x40000;
	if (i6 < 0)
	{
		if (i5 < 0) {
			return false;
		}
		//printf("5\n");
		Render_clipVertex(render, &line->vert2, line, i5, i6);
	}
	else if (i5 < 0)
	{
		//printf("6\n");
		Render_clipVertex(render, &line->vert1, line, -i5, -i6);
	}

	return true;
}

void Render_clipVertex(Render_t* render, Vertex_t* vert, Line_t* line, int i, int mediaTexelOffset)
{
#if FIXED_VERSION == 1
	fixed_t j, j2;
	int x, y, z;

	j = DoomRPG_FixedDiv((-i), (mediaTexelOffset - i));
	j2 = 0x10000 - j;

	x = (int)(DoomRPG_FixedMul(line->vert1.x, j2) + DoomRPG_FixedMul(line->vert2.x, j));
	y = (int)(DoomRPG_FixedMul(line->vert1.y, j2) + DoomRPG_FixedMul(line->vert2.y, j));
	z = (int)(DoomRPG_FixedMul(line->vert1.z, j2) + DoomRPG_FixedMul(line->vert2.z, j));

	vert->x = x;
	vert->y = y;
	vert->z = z;
#else
	long long j, j2;

	j = (((long long)(-i)) << 16) / ((long long)(mediaTexelOffset - i));
	j2 = 0x10000 - j;

	vert->x = (int)((((((int64_t)line->vert1.x) * j2) + (((int64_t)line->vert2.x) * j)) + (0x8000)) >> 16);
	vert->y = (int)((((((int64_t)line->vert1.y) * j2) + (((int64_t)line->vert2.y) * j)) + (0x8000)) >> 16);
	vert->z = (int)((((((int64_t)line->vert1.z) * j2) + (((int64_t)line->vert2.z) * j)) + (0x8000)) >> 16);
#endif
}

void Render_projectVertex(Render_t* render, Vertex_t* vert)
{
#if FIXED_VERSION == 1
	int y;
	y = vert->y;

	vert->y = (DoomRPG_FixedDiv(render->screenWidth << 16, vert->x));
	vert->x = (DoomRPG_FixedMul(y, vert->y) >> 1);
	vert->x += render->fracHalfScreenWidth;
	vert->z *= vert->y;
#else
	long long y;
	y = vert->y;

	vert->y = (int)((((int64_t)render->screenWidth) * 4294967296L) / ((int64_t)vert->x));
	vert->x = (int)((((int64_t)y) * ((int64_t)vert->y)) >> 17);
	vert->x += render->fracHalfScreenWidth;
	vert->z *= vert->y;
#endif

}

void Render_drawLines(Render_t* render, Line_t* line)
{
	Vertex_t vert;

	//testsub("Render_drawLines");
	// printf("x1=%i y1=%i z1=%i\n x2=%i y2=%i z2=%i\n",
	// 	line->vert1.x, line->vert1.y, line->vert1.z,
	// 	line->vert2.x, line->vert2.y, line->vert2.z);
	//testsub("initline");


	if (((line->vert1.x - render->viewX) * (line->vert2.y - line->vert1.y)) +
		((line->vert1.y - render->viewY) * (-(line->vert2.x - line->vert1.x))) <= 0)
	{
		if (!(line->flags & 1)) {
			return;
		}
		vert.x = line->vert1.x;
		vert.y = line->vert1.y;
		vert.z = line->vert1.z;

		line->vert1.x = line->vert2.x;
		line->vert1.y = line->vert2.y;
		line->vert1.z = line->vert2.z;

		line->vert2.x = vert.x;
		line->vert2.y = vert.y;
		line->vert2.z = vert.z;
	}
//testsub("Render_drawLines2");
	Render_transform2DVerts(render, &line->vert1);
	Render_transform2DVerts(render, &line->vert2);
	//printf("sin=%i cos=%i\n transX=%i transY=%i\n", render->viewSin, render->viewCos, render->viewTransX, render->viewTransY);
//testsub("Render_drawLines3");
	//debug previously commented
	// printf("-z-z-z-z-z-z-z-z-z-z-z-z-z\n");
	// printf("line v1X %d\n", line->vert1.x);
	// printf("line v1Y %d\n", line->vert1.y);
	// printf("line v1Z %d\n", line->vert1.z);
	// testsub("Render_drawLines4");
	// printf("line v2X %d\n", line->vert2.x);
	// printf("line v2Y %d\n", line->vert2.y);
	// printf("line v2Z %d\n", line->vert2.z);
	// testsub("testline1");
	//debug end

	Render_drawLine(render, line);
}

void Render_drawLine(Render_t* render, Line_t* line)
{
	if (Render_clipLine(render, line))
	{
		Render_projectVertex(render, &line->vert1);
		Render_projectVertex(render, &line->vert2);

			// printf("line v1X %d\n", line->vert1.x);
			// printf("line v1Y %d\n", line->vert1.y);
			// printf("line v1Z %d\n", line->vert1.z);
			// printf("line v2X %d\n", line->vert2.x);
			// printf("line v2Y %d\n", line->vert2.y);
			// printf("line v2Z %d\n", line->vert2.z);
			// testsub("testline2");

		if ((line->flags & 0x20000000) != 0) { // BREW
			Render_occludeClippedLine(render, line);
		}
		else  if ((line->flags & 2) != 0) {
			Render_drawSpriteSpan(render, line);
			//printf("drawSpriteSpan\n");
		}
		else
		{
			if ((render->lines[render->numLines].flags & 160) == 0) {
				render->lines[render->numLines].flags |= 128;
			}
			//printf("drawWallSpans\n");
			Render_drawWallSpans(render, line);
		}
	}
}

void Render_occludeClippedLine(Render_t* render, Line_t* line)
{
	int x1, x2, i;

	x2 = line->vert2.x;
	x1 = line->vert1.x;
	if ((x2 - x1) > 0) {
		x1 = (x1 + 0xFFFF) >> FRACBITS;
		x2 = (x2 + 0xFFFF) >> FRACBITS;
		//printf("x1 %d\n", x1);
		//printf("x2 %d\n", x2);


		for (i = x1; i < x2; ++i) {
			render->columnScale[i] = COLUMN_SCALE_OCCLUDED;
		}
	}
}

void Render_drawNodeLines(Render_t* render, Node_t* node)
{
	Line_t* line;
	int lineBeg, lineEnd;

	lineEnd = (node->args2 >> 16) & 65535;
	render->lineCount += lineEnd;

	for (lineBeg = 0; lineBeg < lineEnd; lineBeg++)
	{
		render->numLines = (node->args2 & 65535) + lineBeg;
		line = &render->lines[render->numLines];

		render->tmpLine.vert1.x = line->vert1.x;
		render->tmpLine.vert1.y = line->vert1.y;
		render->tmpLine.vert1.z = line->vert1.z;
		render->tmpLine.vert2.x = line->vert2.x;
		render->tmpLine.vert2.y = line->vert2.y;
		render->tmpLine.vert2.z = line->vert2.z;
		render->tmpLine.flags = line->flags;
		render->tmpLine.texture = render->mediaTexturesIds[line->texture] + (unsigned short)((unsigned int)((render->animFrameTime + line->texture * 3) * 0x400000) >> 0x1e);
		render->spanMode = 0;
		Render_drawLines(render, &render->tmpLine);
	}
}

void Render_drawWallSpans(Render_t* render, Line_t* line)
{
	testsub("--Render_drawWallSpans--");
	int i, mediaTexelOffset, mapTextureTexels, mapSpriteTexelIndex, i5, i6, i7, i8, i9;
	int i12, i13, i14, i15, i16, i17, zPos;

	i = line->vert2.x - line->vert1.x; 
	//printf("x2=%i x1=%i\n", line->vert2.x, line->vert1.x);
	//testsub("line test");
	if (i <= 0) {
		return;
	}

	render->lineRasterCount++;

#if FIXED_VERSION == 1
	mediaTexelOffset = (MAXINT / i) << 1;
	mapTextureTexels = (int)DoomRPG_FixedMul((line->vert2.y - line->vert1.y), mediaTexelOffset);
	mapSpriteTexelIndex = (int)DoomRPG_FixedMul((line->vert2.z - line->vert1.z), mediaTexelOffset);

	//printf("mto=%i mtt=%i msti=%i\n", mediaTexelOffset, mapTextureTexels, mapSpriteTexelIndex);
	//testsub("FixedMul test");
#else
	mediaTexelOffset = (MAXINT / i) << 1;
	mapTextureTexels = (int)((((int)(line->vert2.y - line->vert1.y)) * ((int64_t)mediaTexelOffset)) >> 16);
	mapSpriteTexelIndex = (int)((((int)(line->vert2.z - line->vert1.z)) * ((int64_t)mediaTexelOffset)) >> 16);
#endif

	i5 = (line->vert1.x + 65535) >> 16;
	i6 = (line->vert2.x + 65535) >> 16;

	if (render->screenLeft > i5) {
		i5 = render->screenLeft;
	}

	if (render->screenRight < i6) {
		i6 = render->screenRight;
	}

	//printf("i5=%i i6=%i\n", i5, i6);
	//testsub("i5 i6 values");

#if FIXED_VERSION == 1
	int j = ((i5 << 16) - line->vert1.x);
	i7 = line->vert1.z + DoomRPG_FixedMul(j, mapSpriteTexelIndex);
	i8 = line->vert1.y + DoomRPG_FixedMul(j, mapTextureTexels);
#else
	int64_t j = (int64_t)((i5 << 16) - line->vert1.x);
	i7 = line->vert1.z + ((int)((j * ((int64_t)mapSpriteTexelIndex)) >> 16));
	i8 = line->vert1.y + ((int)((j * ((int64_t)mapTextureTexels)) >> 16));
#endif

	/*printf("-z-z-z-z-z-z-z-z-z-z-z-z-z\n");
	printf("j %d\n", j);
	printf("mediaTexelOffset %d\n", mediaTexelOffset);
	printf("mapTextureTexels %d\n", mapTextureTexels);
	printf("mapSpriteTexelIndex %d\n", mapSpriteTexelIndex);
	printf("Xstart %d\n", i5);
	printf("Xend %d\n", i6);
	printf("i7 %d\n", i7);
	printf("i8 %d\n", i8);*/
	i9 = render->mediaTexelOffsets[(line->texture) * 2];
	//printf("i9 %d\n", i9);
	Render_getSpanMode(render, render->mediaTexelOffsets[line->texture * 2 + 1], render->spanMode);

	//printf("i7=%i i8=%i i9=%i\n i12=%i i13=%i\n", i7, i8, i9, i12, i13);
	//testsub("i7,i8,i9,i12,i13");
	//printf("-.-.-.-.-.-.-.-.-.-.-.-.-.\n");

	rdp_loadtest(render->spanPalettes, render->mediaTexels, (i9 + (i13 << 6)) <<12); //new
	while (i5 < i6)
	{
		i12 = (0x40000000 / i8) << 2;

#if FIXED_VERSION == 1
		i13 = ((int)(DoomRPG_FixedMul(i7, i12) >> 16)) & 63;
#else
		i13 = ((int)((((int64_t)i7) * ((int64_t)i12)) >> 32)) & 63;
#endif

		//i8 += (mapTextureTexels); //original
		i8 += (mapTextureTexels<<1); //x2
		i7 += mapSpriteTexelIndex;
		if (render->columnScale[i5] >= i12)
		{
			render->columnScale[i5] = i12;
			render->columnScale[i5+1] = i12;
			i14 = i12 >> 3;

			i15 = (64 * i8) >> 17;

			if (line->flags & 0xC0010000) { // BREW
				if (!(line->flags & 0xC0000000)) {
					i15 *= 2;
				}
				zPos = 128;
			}
			else {
				zPos = 64;
			}
			zPos = 64;

			i16 = render->halfScreenHeight - (((zPos - render->viewZ) * i8) >> 17);
			//i17 = (i9 + (i13 << 6)) <<12; //original
			i17 = (i9 + (i13 << 7)) <<12; //x2

			if (render->screenTop > i16)
			{
				i17 -= (i14 * (i16 - render->screenTop));
				i15 += (i16 - render->screenTop);
				i16 = render->screenTop;
			}

			if (i16 + i15 > render->screenBottom) {
				i15 = render->screenBottom - i16;
			}

			if (render->spanFunction) {
				//render->spanFunction(render, i5, i16, (i17), i14, i15);
				render->spanFunction(render, i5, i16, i17, i14, i15);
			}
		}
		//i5++; //original
		i5+=2;
	}
	rdp_sync(SYNC_TILE);
}

void Render_renderSpriteObject(Render_t* render, Sprite_t* sprite)
{
	int i, anim;
	short mediaId;


	if (sprite->info & SPRITE_FLAG_HIDDEN) {
		return;
	}

	if (sprite->ent && sprite->ent->monster &&
		!(sprite->ent->info & 0x80000) && !(sprite->info & 0x1000000) &&
		!render->doomRpg->player->noclip && !render->doomRpg->game->disableAI) {
		Game_activate(render->doomRpg->game, sprite->ent);
	}

	render->damageBlend = false;
	if (sprite->ent && sprite->ent->monster)
	{
		if (render->doomRpg->doomCanvas->time < sprite->ent->monster->damageBlendTime) {
			render->damageBlend = true;
		}
	}

	i = sprite->info & 511;
	//anim = (unsigned int)(sprite->info << 19) >> 28;
	anim = (sprite->info & 0x1E00) >> 9;

	if (!(sprite->info & 0x80000000)) {
		if (sprite->ent == NULL) {
			anim = ((render->animFrameTime + (8 * (byte)sprite->ent)) / 250) % 4;
		}
		else if (sprite->ent->monster == NULL)
		{
			if (sprite->ent->def->eType != 2) {
				anim = ((render->animFrameTime + (8 * (byte)sprite->ent)) / 250) % 4;
			}
			else {
				anim = ((render->doomRpg->doomCanvas->time + (256 * (byte)sprite->ent)) / 6000) % 2;
			} 
		}
		else if (anim == 6)
		{
			if (sprite->ent->monster->animFrameTime < render->doomRpg->doomCanvas->time) {
				sprite->ent->monster->animFrameTime = 0;
				anim = 0;
				sprite->info = sprite->info & 0xffffe1ff;
			}
		}
		else if (anim == 4)
		{
			if (sprite->ent->monster->animFrameTime < render->doomRpg->doomCanvas->time) {
				sprite->ent->monster->animFrameTime = 0;
				anim = 2;
				sprite->info = sprite->info & 0xffffe1ffU | 0x400;
			}
		}
		else {
			if ((anim == 0 || anim == 3) && (sprite->ent->monster->animFrameTime < render->doomRpg->doomCanvas->time)) {
				if (anim == 0) {
					anim = 3;
				}
				else {
					anim = 0;
				}
				sprite->info = sprite->info & 0xffffe1ff | anim << 9;
				if (sprite->ent->monster->ce.mType == 4) { // Phantom / Lost Soul / Nightmare
					sprite->ent->monster->animFrameTime = (render->doomRpg->doomCanvas->time + ((byte)sprite->ent) + 250) % MAXTICKS;
				}
				else {
					sprite->ent->monster->animFrameTime = (render->doomRpg->doomCanvas->time + (8 * (byte)sprite->ent) + 4000) % MAXTICKS;
				}
			}
		}
	}

	// Nuevo: remueve las luces de techo si no hay texturas de piso y techo visibles
	// New: Remove ceiling lights if no floor and ceiling textures are visible
	if ((i == 137) && !render->doomRpg->doomCanvas->renderFloorCeilingTextures) {
		return;

	}

	if (sprite->info & 0x40000) {
		Render_renderSprite(render, sprite->x, sprite->y, render->mediaTexturesIds[i] + anim, sprite->info, sprite->renderMode);
	}
	else if (sprite->info & 0x4000000) {
		mediaId = render->mediaSpriteIds[i - 1] + anim;
		Render_renderSprite(render, sprite->x + 16, sprite->y, mediaId, sprite->info, sprite->renderMode);
		Render_renderSprite(render, sprite->x - 16, sprite->y, mediaId, sprite->info, sprite->renderMode);
		Render_renderSprite(render, sprite->x, sprite->y + 16, mediaId, sprite->info, sprite->renderMode);
		Render_renderSprite(render, sprite->x, sprite->y - 16, mediaId, sprite->info, sprite->renderMode);
	}
	else {
		Render_renderSprite(render, sprite->x, sprite->y, render->mediaSpriteIds[i] + anim, sprite->info, sprite->renderMode);
	}

	// spawn light glows
	if (i == 135 || i == 140) {
		Render_renderSprite(render, sprite->x, sprite->y, render->mediaSpriteIds[136] + anim, sprite->info, 7);
	}
	else if (i == 131) {
		Render_renderSprite(render, sprite->x, sprite->y, render->mediaSpriteIds[144] + anim, sprite->info, 7);
	}
}

void Render_renderSprite(Render_t* render, int x, int y, int mediaId, int info, byte renderMode)
{
	int i5;
	int i6;
	int i7;
	Vertex_t* vert1, * vert2;

	//printf("renderSprite\n");

	vert1 = &render->tmpLine.vert1;

	if ((info & 0x780000) == 0) {
		vert1->x = x;
		vert1->y = y;
		Render_transform2DVerts(render, vert1);
		vert1->x -= 0x100000;
		if (vert1->x < 0x40000) {
			return;
		}
	}

	vert2 = &render->tmpLine.vert2;
	vert1->z = 0;
	if ((info & 0x40000) != 0) {
		i6 = -32;
		i5 = 32;
		render->tmpLine.flags = 0;
		vert2->z = 64;
	}
	else {
		i7 = render->mediaBitShapeOffsets[mediaId * 2];
		i6 = render->shapeData[i7 + 2] - 32;
		i5 = render->shapeData[i7 + 3] - 32;
		render->tmpLine.flags = 2;
		vert2->z = i5 - i6;
	}

	render->tmpLine.texture = mediaId;

	render->spanMode = renderMode;
	if ((info & 0x20000000) != 0) { // New From BREW Version
		render->tmpLine.flags = 0x80000000;
	}
	if ((info & 0x40000000) != 0) { // New From BREW Version
		render->tmpLine.flags = 0x40000000;
	}
	if ((info & 0x8000000) != 0) {
		render->tmpLine.flags |= 1;
	}

	if ((info & 0x780000) == 0) {
		vert2->x = vert1->x;
		vert2->y = vert1->y + (i5 << 16);
		vert1->y += i6 << 16;

		Render_drawLine(render, &render->tmpLine);
		return;
	}

	if ((info & 0x80000) != 0) {
		if ((info & 0x2000000) != 0) {
			vert1->x = x - i6;
			vert2->x = x - i5;
		}
		else {
			vert1->x = x + i6;
			vert2->x = x + i5;
		}
		vert1->y = y;
		vert2->y = y;
	}
	else if ((info & 0x100000) != 0) {
		if ((info & 0x2000000) != 0) {
			vert1->x = x + i6;
			vert2->x = x + i5;
		}
		else {
			vert1->x = x - i6;
			vert2->x = x - i5;
		}
		vert1->y = y;
		vert2->y = y;
	}
	else if ((info & 0x400000) != 0) {
		if ((info & 0x2000000) != 0) {
			vert1->y = y - i6;
			vert2->y = y - i5;
		}
		else {
			vert1->y = y + i6;
			vert2->y = y + i5;
		}
		vert1->x = x;
		vert2->x = x;
	}
	else if ((info & 0x200000) != 0) {
		if ((info & 0x2000000) != 0) {
			vert1->y = y + i6;
			vert2->y = y + i5;
		}
		else {
			vert1->y = y - i6;
			vert2->y = y - i5;
		}
		vert1->x = x;
		vert2->x = x;
	}

			// SDL_Rect cliprect;
			// cliprect.x = render->tmpLine.vert1.x;
			// cliprect.y = render->tmpLine.vert1.y;
			// cliprect.w = render->tmpLine.vert2.x - render->tmpLine.vert1.x;
			// cliprect.h = render->tmpLine.vert2.y - render->tmpLine.vert1.y;

			// SDL_SetRenderDrawColor(NULL, 0xff, 0xff, 0xff, 0xff);
			// SDL_RenderFillRect2(NULL,  &cliprect);

	Render_drawLines(render, &render->tmpLine);
}

void Render_drawSpriteSpan(Render_t* render, Line_t* line)
{
	int i, mediaTexelOffset, mapTextureTexels, mapSpriteTexelIndex, i5, i6, i7, i8, i9; //int
	int i12, i13, i14, i15, i16, i17, i18, i19; //int
	int mediaTexelOffset0, mediaTexelOffset1, mediaTexelOffset2, mediaTexelOffset3, mediaTexelOffset4; //int
	short s, s2;

	i = line->vert2.x - line->vert1.x;

	if (i <= 0) {
		return;
	}

	render->spriteRasterCount++;

#if FIXED_VERSION == 1
	mediaTexelOffset = (MAXINT / i) << 1;
	mapTextureTexels = (int)DoomRPG_FixedMul((line->vert2.y - line->vert1.y), mediaTexelOffset);
	mapSpriteTexelIndex = (int)DoomRPG_FixedMul((line->vert2.z - line->vert1.z), mediaTexelOffset);
#else
	mediaTexelOffset = (MAXINT / i) << 1;
	mapTextureTexels = (int)((((int64_t)(line->vert2.y - line->vert1.y)) * ((int64_t)mediaTexelOffset)) >> 16);
	mapSpriteTexelIndex = (int)((((int64_t)(line->vert2.z - line->vert1.z)) * ((int64_t)mediaTexelOffset)) >> 16);
#endif
	i5 = (line->vert1.x + 65535) >> 16;
	i6 = (line->vert2.x + 65535) >> 16;

	if (render->screenLeft > i5) { //x1
		i5 = render->screenLeft;
	}

	if (render->screenRight < i6) { //x2
		i6 = render->screenRight;
	}


	#if FIXED_VERSION == 1
	int j = ((i5 << 16) - line->vert1.x);
	i7 = line->vert1.z + DoomRPG_FixedMul(j, mapSpriteTexelIndex);//((int) ((j * ((long) mapSpriteTexelIndex)) >> 16));
	i8 = line->vert1.y + DoomRPG_FixedMul(j, mapTextureTexels);//((int) ((j * ((long) mapTextureTexels)) >> 16));
#else
	int64_t j = (int64_t)((i5 << 16) - line->vert1.x);
	i7 = line->vert1.z + ((int)((j * ((int64_t)mapSpriteTexelIndex)) >> 16));
	i8 = line->vert1.y + ((int)((j * ((int64_t)mapTextureTexels)) >> 16));
#endif

	s = line->texture;
	i9 = render->mediaBitShapeOffsets[s * 2];

	Render_getSpanMode2(render, render->mediaBitShapeOffsets[line->texture * 2 + 1], render->spanMode);

	i12 = i9 + 4;
	i13 = (render->shapeData[i9] & 65535) | (render->shapeData[i9 + 1] << 16);

	//rdp_loadtest(render->spanPalettes, render->mediaTexels, (i13 + render->shapeData[i17 + 1]) << 12); //new
	//rdp_loadtest(render->spanPalettes, render->mediaTexels, (i13 + render->shapeData[i17 + 1]) << 12); //new
	//rdp_draw_textured_rectangle(i5, mediaTexelOffset3, i6, mediaTexelOffset3 + mediaTexelOffset2, 0);
	while (i5 < i6) {

		i14 = (0x40000000u / i8) << 2;

#if FIXED_VERSION == 1
		i15 = ((int)(DoomRPG_FixedMul(i7, i14) >> 16));
#else
		i15 = (int)((((int64_t)i7) * ((int64_t)i14)) >> 32);
#endif

		i8 += mapTextureTexels;
		i7 += mapSpriteTexelIndex;
		if (render->columnScale[i5] >= i14) {
			i16 = i14 >> 3;
			i17 = i9 + render->shapeData[i12 + i15] + 2;
			while (true) {
				s2 = render->shapeData[i17];
				i18 = s2 & 255;
				if (i18 == 127) {
					break;
				}

				i19 = (64 - i18) - render->viewZ;
				mediaTexelOffset0 = s2 >> 8;
				mediaTexelOffset1 = i13 + render->shapeData[i17 + 1];
				i17 += 2;
				mediaTexelOffset2 = (mediaTexelOffset0 * i8) >> 17;
				mediaTexelOffset3 = render->halfScreenHeight - ((i19 * i8) >> 17);
				mediaTexelOffset4 = (mediaTexelOffset1) << 12; //original

				if (mediaTexelOffset3 < render->screenTop) {
					mediaTexelOffset4 -= (i16 * (mediaTexelOffset3 - render->screenTop));
					mediaTexelOffset2 += (mediaTexelOffset3 - render->screenTop);
					mediaTexelOffset3 = render->screenTop;
				}

				if (mediaTexelOffset3 + mediaTexelOffset2 > render->screenBottom) {
					mediaTexelOffset2 = render->screenBottom - mediaTexelOffset3;
				}

				if (render->spanFunction) {
					render->spanFunction(render, i5, mediaTexelOffset3, (mediaTexelOffset4), i16, mediaTexelOffset2);
				}
			}
		}
		i5++; //original
		//i5+=2; //new
	}
}

void Render_getSpanMode(Render_t* render, int palOffset, byte spanMode)
{
	int i;
	int color, red, green, blue;

	render->spanPalettes = render->mediaPalettes + palOffset;

	if (render->damageBlend) {
		for (i = 0; i < 16; i++) {
			color = render->spanPalettes[i];
			red = (color & 0x1f) + 8;
			green = ((color >> 5) & 0x3f) + 16;
			blue = ((color >> 11) & 0x1f) + 8;

			if (red >= 0x1f) { red = 0x1f; }
			if (green >= 0x3f) { green = 0x3f; }
			if (blue >= 0x1f) { blue = 0x1f; }

			render->newSpanPalette[i] = (blue << 0xb) | (green << 5) | red;
		}

		render->spanPalettes = render->newSpanPalette;
		render->damageBlend = 0;
	}

	switch (spanMode) {
	case 0: // Normal
		render->spanFunction = Render_SpanMode0a;
		break;
	case 1:
		render->spanFunction = Render_SpanMode1;
		break;
	case 2:
		render->spanFunction = Render_SpanMode2;
		break;
	case 3:
		render->spanFunction = Render_SpanMode3;
		break;
	case 4: // Trans25
		render->spanFunction = Render_SpanMode4;
		for (i = 0; i < 16; i++) {
			render->newSpanPalette[i] = ((render->spanPalettes[i] & 0xffffe79f) >> 2);
		}
		render->spanPalettes = render->newSpanPalette;
		break;
	case 5: // Trans50
		render->spanFunction = Render_SpanMode5;
		for (i = 0; i < 16; i++) {
			render->newSpanPalette[i] = ((render->spanPalettes[i] & 0xfffff7df) >> 1);
		}
		render->spanPalettes = render->newSpanPalette;
		break;
	case 6: // Trans75
		render->spanFunction = Render_SpanMode6;
		for (i = 0; i < 16; i++) {
			render->newSpanPalette[i] = ((render->spanPalettes[i] & 0xfffff7df) >> 1) + ((render->spanPalettes[i] & 0xffffe79f) >> 2);
		}
		render->spanPalettes = render->newSpanPalette;
		break;
	case 7: // Add
		render->spanFunction = Render_SpanMode7;
		for (i = 0; i < 16; i++) {
			render->newSpanPalette[i] = render->spanPalettes[i] & 0xf7de;
		}
		render->spanPalettes = render->newSpanPalette;
		break;
	case 8: // Sub
		render->spanFunction = Render_SpanMode8;
		for (i = 0; i < 16; i++) {
			render->newSpanPalette[i] = render->spanPalettes[i] & 0xf7de;
		}
		render->spanPalettes = render->newSpanPalette;
		break;
	case 9: // spectre Belphegor
		render->spanFunction = Render_SpanMode9;
		break;
	default:
		render->spanFunction = NULL;
		break;
	}

	if (render->skipStretch != 0) {
		render->spanFunction = NULL;
	}
}

void Render_getSpanMode2(Render_t* render, int palOffset, byte spanMode)
{
	int i;
	int color, red, green, blue;

	render->spanPalettes = render->mediaPalettes + palOffset;

	if (render->damageBlend) {
		for (i = 0; i < 16; i++) {
			color = render->spanPalettes[i];
			red = (color & 0x1f) + 8;
			green = ((color >> 5) & 0x3f) + 16;
			blue = ((color >> 11) & 0x1f) + 8;

			if (red >= 0x1f) { red = 0x1f; }
			if (green >= 0x3f) { green = 0x3f; }
			if (blue >= 0x1f) { blue = 0x1f; }

			render->newSpanPalette[i] = (blue << 0xb) | (green << 5) | red;
		}

		render->spanPalettes = render->newSpanPalette;
		render->damageBlend = 0;
	}

	switch (spanMode) {
	case 0: // Normal
		render->spanFunction = Render_SpanMode0b;
		break;
	case 1:
		render->spanFunction = Render_SpanMode1;
		break;
	case 2:
		render->spanFunction = Render_SpanMode2;
		break;
	case 3:
		render->spanFunction = Render_SpanMode3;
		break;
	case 4: // Trans25
		render->spanFunction = Render_SpanMode4;
		for (i = 0; i < 16; i++) {
			render->newSpanPalette[i] = ((render->spanPalettes[i] & 0xffffe79f) >> 2);
		}
		render->spanPalettes = render->newSpanPalette;
		break;
	case 5: // Trans50
		render->spanFunction = Render_SpanMode5;
		for (i = 0; i < 16; i++) {
			render->newSpanPalette[i] = ((render->spanPalettes[i] & 0xfffff7df) >> 1);
		}
		render->spanPalettes = render->newSpanPalette;
		break;
	case 6: // Trans75
		render->spanFunction = Render_SpanMode6;
		for (i = 0; i < 16; i++) {
			render->newSpanPalette[i] = ((render->spanPalettes[i] & 0xfffff7df) >> 1) + ((render->spanPalettes[i] & 0xffffe79f) >> 2);
		}
		render->spanPalettes = render->newSpanPalette;
		break;
	case 7: // Add FIRES AND LIGHTS
		render->spanFunction = Render_SpanMode7;
		//render->spanFunction = Render_SpanMode0b;
		for (i = 0; i < 16; i++) {
			render->newSpanPalette[i] = render->spanPalettes[i] & 0xf7de;
		}
		render->spanPalettes = render->newSpanPalette;
		break;
	case 8: // Sub
		render->spanFunction = Render_SpanMode8;
		//render->spanFunction = Render_SpanMode0b;
		for (i = 0; i < 16; i++) {
			render->newSpanPalette[i] = render->spanPalettes[i] & 0xf7de;
		}
		render->spanPalettes = render->newSpanPalette;
		break;
	case 9: // spectre Belphegor
		render->spanFunction = Render_SpanMode9;
		//render->spanFunction = Render_SpanMode0b;
		break;
	default:
		render->spanFunction = NULL;
		break;
	}

	if (render->skipStretch != 0) {
		render->spanFunction = NULL;
	}
}

void Render_SpanMode0a(Render_t* render, int param_2, int param_3, int param_4, int param_5, int param_6)
{
	// testsub("-Render_SpanMode0-");
	// printf("param2=%i\n param3=%i param4=%i\n param5=%i param6=%i\n",
	// 	param_2, param_3, param_4, param_5, param_6);
	// testsub("next");
	int pitch;
	byte* mediaTexels;
	register unsigned short* spanPalettes, * pixels;

	int iVar4;
	int iVar5;

	pitch = render->pitch >> 1;
	spanPalettes = render->spanPalettes;
	mediaTexels = render->mediaTexels;
	// pixels = render->pixels + pitch * param_3 + param_2;

	// for (; param_6 >= 8; param_6 -= 8) {

	// 	iVar4 = param_4 + param_5;
	// 	*pixels = spanPalettes[mediaTexels[param_4 >> 0xd] >> (param_4 >> 10 & 4) & 0xf];
	// 	pixels += pitch;

	// 	iVar5 = iVar4 + param_5;
	// 	*pixels = spanPalettes[mediaTexels[iVar4 >> 0xd] >> (iVar4 >> 10 & 4) & 0xf];
	// 	pixels += pitch;

	// 	iVar4 = iVar5 + param_5;
	// 	*pixels = spanPalettes[mediaTexels[iVar5 >> 0xd] >> (iVar5 >> 10 & 4) & 0xf];
	// 	pixels += pitch;

	// 	iVar5 = iVar4 + param_5;
	// 	*pixels = spanPalettes[mediaTexels[iVar4 >> 0xd] >> (iVar4 >> 10 & 4) & 0xf];
	// 	pixels += pitch;

	// 	iVar4 = iVar5 + param_5;
	// 	*pixels = spanPalettes[mediaTexels[iVar5 >> 0xd] >> (iVar5 >> 10 & 4) & 0xf];
	// 	pixels += pitch;

	// 	iVar5 = iVar4 + param_5;
	// 	*pixels = spanPalettes[mediaTexels[iVar4 >> 0xd] >> (iVar4 >> 10 & 4) & 0xf];
	// 	pixels += pitch;

	// 	iVar4 = iVar5 + param_5;
	// 	*pixels = spanPalettes[mediaTexels[iVar5 >> 0xd] >> (iVar5 >> 10 & 4) & 0xf];
	// 	pixels += pitch;

	// 	param_4 = iVar4 + param_5;
	// 	*pixels = spanPalettes[mediaTexels[iVar4 >> 0xd] >> (iVar4 >> 10 & 4) & 0xf];
	// 	pixels += pitch;
	// }

	// while (param_6--, param_6 >= 0) {
	// 	*pixels = spanPalettes[mediaTexels[param_4 >> 13] >> ((param_4 >> 10) & 4) & 0xf];
	// 	pixels += pitch;
	// 	param_4 += param_5;
	// }


	rdp_coltest(spanPalettes, mediaTexels, param_4, param_2, param_3 + render->screenY, param_6, param_5);

}

void Render_SpanMode0b(Render_t* render, int param_2, int param_3, int param_4, int param_5, int param_6){
	int pitch;
	byte* mediaTexels;
	register unsigned short* spanPalettes, * pixels;

	int iVar4;
	int iVar5;

	pitch = render->pitch >> 1;
	spanPalettes = render->spanPalettes;
	mediaTexels = render->mediaTexels;
	pixels = render->pixels + pitch * param_3 + param_2;

	for (; param_6 >= 8; param_6 -= 8) {

		iVar4 = param_4 + param_5;
		*pixels = spanPalettes[mediaTexels[param_4 >> 0xd] >> (param_4 >> 10 & 4) & 0xf];
		pixels += pitch;

		iVar5 = iVar4 + param_5;
		*pixels = spanPalettes[mediaTexels[iVar4 >> 0xd] >> (iVar4 >> 10 & 4) & 0xf];
		pixels += pitch;

		iVar4 = iVar5 + param_5;
		*pixels = spanPalettes[mediaTexels[iVar5 >> 0xd] >> (iVar5 >> 10 & 4) & 0xf];
		pixels += pitch;

		iVar5 = iVar4 + param_5;
		*pixels = spanPalettes[mediaTexels[iVar4 >> 0xd] >> (iVar4 >> 10 & 4) & 0xf];
		pixels += pitch;

		iVar4 = iVar5 + param_5;
		*pixels = spanPalettes[mediaTexels[iVar5 >> 0xd] >> (iVar5 >> 10 & 4) & 0xf];
		pixels += pitch;

		iVar5 = iVar4 + param_5;
		*pixels = spanPalettes[mediaTexels[iVar4 >> 0xd] >> (iVar4 >> 10 & 4) & 0xf];
		pixels += pitch;

		iVar4 = iVar5 + param_5;
		*pixels = spanPalettes[mediaTexels[iVar5 >> 0xd] >> (iVar5 >> 10 & 4) & 0xf];
		pixels += pitch;

		param_4 = iVar4 + param_5;
		*pixels = spanPalettes[mediaTexels[iVar4 >> 0xd] >> (iVar4 >> 10 & 4) & 0xf];
		pixels += pitch;
	}

	while (param_6--, param_6 >= 0) {
		*pixels = spanPalettes[mediaTexels[param_4 >> 13] >> ((param_4 >> 10) & 4) & 0xf];
		pixels += pitch;
		param_4 += param_5;
	}
	
}


void Render_SpanMode1(Render_t* render, int param_2, int param_3, int param_4, int param_5, int param_6)
{
	int pitch;
	byte* mediaTexels;
	register unsigned short* spanPalettes, * pixels;

	pitch = render->pitch >> 1;
	mediaTexels = render->mediaTexels;
	spanPalettes = render->spanPalettes;
	pixels = render->pixels + pitch * param_3 + param_2;
	while (param_6--, param_6 >= 0) {

		pixels[0] ^= spanPalettes[mediaTexels[param_4 >> 13] >> ((param_4 >> 10) & 4) & 0xf];
		pixels += pitch;
		param_4 += param_5;
	}
}

void Render_SpanMode2(Render_t* render, int param_2, int param_3, int param_4, int param_5, int param_6)
{
	int pitch;
	byte* mediaTexels;
	register unsigned short* spanPalettes, * pixels;

	pitch = render->pitch >> 1;
	mediaTexels = render->mediaTexels;
	spanPalettes = render->spanPalettes;
	pixels = render->pixels + pitch * param_3 + param_2;
	while (param_6--, param_6 >= 0) {

		pixels[0] |= spanPalettes[mediaTexels[param_4 >> 13] >> ((param_4 >> 10) & 4) & 0xf];
		pixels += pitch;
		param_4 += param_5;
	}
}

void Render_SpanMode3(Render_t* render, int param_2, int param_3, int param_4, int param_5, int param_6)
{
	int pitch;
	byte* mediaTexels;
	register unsigned short* spanPalettes, * pixels;

	pitch = render->pitch >> 1;
	mediaTexels = render->mediaTexels;
	spanPalettes = render->spanPalettes;
	pixels = render->pixels + pitch * param_3 + param_2;
	while (param_6--, param_6 >= 0) {

		pixels[0] &= spanPalettes[mediaTexels[param_4 >> 13] >> ((param_4 >> 10) & 4) & 0xf];
		pixels += pitch;
		param_4 += param_5;
	}
}

void Render_SpanMode4(Render_t* render, int param_2, int param_3, int param_4, int param_5, int param_6)
{
	int pitch, color;
	byte* mediaTexels;
	register unsigned short* spanPalettes, * pixels;

	pitch = render->pitch >> 1;
	mediaTexels = render->mediaTexels;
	spanPalettes = render->spanPalettes;
	pixels = render->pixels + pitch * param_3 + param_2;
	while (param_6--, param_6 >= 0) {

		color = spanPalettes[mediaTexels[param_4 >> 13] >> ((param_4 >> 10) & 4) & 0xf];
		pixels[0] = ((int)(pixels[0] & 0xfffff7df) >> 1) + ((int)(pixels[0] & 0xffffe79f) >> 2) + color;

		pixels += pitch;
		param_4 += param_5;
	}
}

void Render_SpanMode5(Render_t* render, int param_2, int param_3, int param_4, int param_5, int param_6)
{
	int pitch, color;
	byte* mediaTexels;
	register unsigned short* spanPalettes, * pixels;

	pitch = render->pitch >> 1;
	mediaTexels = render->mediaTexels;
	spanPalettes = render->spanPalettes;
	pixels = render->pixels + pitch * param_3 + param_2;
	while (param_6--, param_6 >= 0) {

		color = spanPalettes[mediaTexels[param_4 >> 13] >> ((param_4 >> 10) & 4) & 0xf];
		pixels[0] = color + ((pixels[0] & 0xfffff7df) >> 1);
		pixels += pitch;
		param_4 += param_5;
	}
}

void Render_SpanMode6(Render_t* render, int param_2, int param_3, int param_4, int param_5, int param_6)
{
	int pitch, color;
	byte* mediaTexels;
	register unsigned short* spanPalettes, * pixels;

	pitch = render->pitch >> 1;
	mediaTexels = render->mediaTexels;
	spanPalettes = render->spanPalettes;
	pixels = render->pixels + pitch * param_3 + param_2;
	while (param_6--, param_6 >= 0) {

		color = spanPalettes[mediaTexels[param_4 >> 13] >> ((param_4 >> 10) & 4) & 0xf];
		pixels[0] = color + ((int)(pixels[0] & 0xffffe79f) >> 2);
		pixels += pitch;
		param_4 += param_5;
	}
}

void Render_SpanMode7(Render_t* render, int param_2, int param_3, int param_4, int param_5, int param_6)
{
	int pitch, color;
	byte* mediaTexels;
	register unsigned short* spanPalettes, * pixels;

	pitch = render->pitch >> 1;
	mediaTexels = render->mediaTexels;
	spanPalettes = render->spanPalettes;
	pixels = render->pixels + pitch * param_3 + param_2;
	while (param_6--, param_6 >= 0) {

		color = spanPalettes[mediaTexels[param_4 >> 13] >> ((param_4 >> 10) & 4) & 0xf] + (pixels[0] & 0xF7DE);
		pixels[0] = color & 0xF7DE | ((color & 0x10820) >> 1) | ((color & 0x10820) >> 2) | ((color & 0x10820) >> 3);

		pixels += pitch;
		param_4 += param_5;
	}
}

void Render_SpanMode8(Render_t* render, int param_2, int param_3, int param_4, int param_5, int param_6)
{
	int pitch, color;
	byte* mediaTexels;
	register unsigned short* spanPalettes, * pixels;

	pitch = render->pitch >> 1;
	mediaTexels = render->mediaTexels;
	spanPalettes = render->spanPalettes;
	pixels = render->pixels + pitch * param_3 + param_2;
	while (param_6--, param_6 >= 0) {

		color = (pixels[0] | 0x10820) - spanPalettes[mediaTexels[param_4 >> 13] >> ((param_4 >> 10) & 4) & 0xf];
		pixels[0] = color & ((int)(color | 0xFFFEF7DF) >> 1) & ((int)(color | 0xFFFEF7DF) >> 2) & ((int)(color | 0xFFFEF7DF) >> 3) & 0xF7DE;

		pixels += pitch;
		param_4 += param_5;
	}
}

void Render_SpanMode9(Render_t* render, int param_2, int param_3, int param_4, int param_5, int param_6)
{
	int rnd;
	unsigned int r, g, b;
	int pitch, color;
	byte* mediaTexels;
	register unsigned short* spanPalettes, * pixels;

	pitch = render->pitch >> 1;
	mediaTexels = render->mediaTexels;
	spanPalettes = render->spanPalettes;
	pixels = render->pixels + pitch * param_3 + param_2;
	while (param_6--, param_6 >= 0) {

		color = pixels[0];

		rnd = DoomRPG_randNextByte(&render->doomRpg->random) >> 5;

		b = (color & 0x1F);
		g = (color >> 5) & 0x3f;
		r = (color >> 11) & 0x1f;

		b = ((3 * (b >> 2)) + rnd);
		g = ((3 * (g >> 2)) + (rnd << 1));
		r = ((3 * (r >> 2)) + rnd);

		if (b > 31) { b = 31; }
		if (g > 63) { g = 63; }
		if (r > 31) { r = 31; }

		pixels[0] = (r << 11) | (g << 5) | b;

		pixels += pitch;
		param_4 += param_5;
	}
}

void Render_draw2DSprite(Render_t* render, int weaponFrame, int flashFrame, int x, int y, byte renderMode, boolean damageBlend)
{
	int i5;
	int i6 = 32768 / render->screenWidth;
	int i7 = 65536 / i6;
	int i8 = 0;
	int i9 = render->mediaSpriteIds[weaponFrame] + flashFrame;
	int i10 = render->mediaBitShapeOffsets[(i9 * 2) + 0];

	render->damageBlend = damageBlend;
	render->unk3 = 0;

	Render_getSpanMode2(render, render->mediaBitShapeOffsets[(i9 * 2) + 1], renderMode);

	int i11 = (render->shapeData[i10] & 65535) | (render->shapeData[i10 + 1] << 16);
	short s = render->shapeData[i10 + 2];
	short s2 = render->shapeData[i10 + 3];
	int i12 = i10 + 4;
	int i13 = x + ((s * i7) >> 8);
	int i14 = x + ((s2 * i7) >> 8);
	int i15 = i13;
	int i16 = i14 - i13;

	if (i13 < render->screenLeft) {
		i16 = i13 - render->screenLeft;
		i15 = render->screenLeft;
	}

	if (i13 < render->screenLeft) {
		i8 = -(i6 * i16);
	}

	i16 = i14;
	if (i14 > render->screenRight) {
		i16 = render->screenRight;
	}

	while (i15 < i16 && (i5 = i8 >> 8) < i16) {
		i8 += i6;

		int i17 = i10 + render->shapeData[i12 + i5] + 2;
		while (true) {
			short s3 = render->shapeData[i17];
			int i18 = s3 & 255;
			if (i18 == 0x7f) {
				break;
			}

			int i19 = s3 >> 8;
			int mediaTexelOffset0 = i11 + render->shapeData[i17 + 1];

			i17 += 2;
			int mediaTexelOffset1 = (i19 * i7) >> 8;
			int mediaTexelOffset2 = y + ((i18 * i7) >> 8);
			int mediaTexelOffset3 = mediaTexelOffset0 << 12;


			// Port:
			// corregir p�xeles vac�os en la parte inferior del gr�fico
			// fix empty pixels at bottom of graph
			{
				//mediaTexelOffset1 += 1;
			}

			if (mediaTexelOffset2 < render->screenTop) {
				mediaTexelOffset1 += (mediaTexelOffset2 - render->screenTop);
				mediaTexelOffset3 -= (i6 * (mediaTexelOffset2 - render->screenTop));
				mediaTexelOffset2 = render->screenTop;
			}

			if (mediaTexelOffset2 + mediaTexelOffset1 > render->screenBottom) {
				mediaTexelOffset1 = render->screenBottom - mediaTexelOffset2;
			}

			if (render->spanFunction) {
				render->spanFunction(render, i15, mediaTexelOffset2, mediaTexelOffset3, i6 << 4, mediaTexelOffset1);
			}
		}

		i15++;
	}
}


void Render_fadeScreen(Render_t* render, int fade)
{
	int pitch, i, j;
	short color;
	int r, g, b;
	short* pixels;

	pixels = (short*)&render->framebuffer[render->pitch * render->screenY + render->screenX * sizeof(short)];
	pitch = render->pitch >> 1;

	for (i = 0; i < render->screenHeight; i++) {
		for (j = 0; j < render->screenWidth; j++) {
			color = pixels[i * pitch + j];

			b = (color & 0x1F);
			g = (color >> 5) & 0x3f;
			r = (color >> 11) & 0x1f;

			if (r > (fade >> 3)) {
				r = (fade >> 3);
			}

			if (g > (fade >> 2)) {
				g = (fade >> 2);
			}

			if (b > (fade >> 3)) {
				b = (fade >> 3);
			}

			color = (r << 11) | (g << 5) | b;

			pixels[i * pitch + j] = color;
		}
	}
}

void Render_setBerserkColor(Render_t* render) {
	int pitch, i, j;
	short* pixels;
	byte red;

	pitch = render->pitch;
	for (i = 0; i < render->screenHeight; i++) {
		pixels = render->pixels + ((pitch >> 1) * i);
		for (j = 0; j < render->screenWidth; j++) {

			red = ((pixels[0] >> 11) & 0x1f) + 8;
			if (red > 31) {
				red = 31;
			}

			pixels[0] = ((pixels[0] & 0xfffff7df) >> 1) & 0x7ff | (red << 11);
			pixels++;
		}
	}

	// Necesario para actualizar el framebuffer
	// Needed to update the framebuffer

	SDL_Rect renderQuad, clip;

	clip.x = render->screenX;
	clip.y = render->screenY;
	clip.w = render->screenWidth;
	clip.h = render->screenHeight;

	renderQuad.x = render->screenX;
	renderQuad.y = render->screenY;
	renderQuad.w = sdlVideo.rendererW;
	renderQuad.h = sdlVideo.rendererH;
	if (clip.w <= renderQuad.w) {
		renderQuad.w = clip.w;
	}
	if (clip.h <= renderQuad.h) {
		renderQuad.h = clip.h;
	}

	//SDL_UpdateTexture(render->piDIB, NULL, render->framebuffer, sdlVideo.rendererW * 2);
	//SDL_RenderCopy(sdlVideo.renderer, render->piDIB, &clip, &renderQuad);
}

int Render_findEventIndex(Render_t* render, int i)
{
	int index, cnt, numtEvnt, event;

	cnt = 0;
	numtEvnt = render->numTileEvents;
	do {
		index = cnt + ((numtEvnt - cnt) >> 1);
		event = render->tileEvents[index] & 1023;
		if (event == i) {
			return index;
		}
		if (i < event) {
			numtEvnt = index;
		}
		else {
			cnt = index;
		}
		if (index == 0) {
			return -1;
		}
	} while (index != numtEvnt - 1);

	return -1;
}


void Render_renderFloorAndCeilingBG_Test(Render_t* render)
{
	int top;
	int right;
	int left;
	int bottom;

	top = 0;
	if (render->screenTop > top) {
		top = render->screenTop;
	}

	left = 0;
	if (render->screenLeft > left) {
		left = render->screenLeft;
	}

	right = render->screenWidth;
	if (render->screenRight < right) {
		right = render->screenRight;
	}

	bottom = render->screenHeight;
	if (render->screenBottom < bottom) {
		bottom = render->screenBottom;
	}

	Render_getSpanMode(render, render->mediaTexelOffsets[render->mediaTexturesIds[render->ceilingTex] * 2 + 1], 0);
	while (top < render->halfScreenHeight) {
		Render_drawPlane_Test(render, left, top, render->mediaTexelOffsets[render->mediaTexturesIds[render->ceilingTex] * 2], right);
		top++;
	}

	Render_getSpanMode(render, render->mediaTexelOffsets[render->mediaTexturesIds[render->floorTex] * 2 + 1], 0);
	while (top < bottom) {
		Render_drawPlane_Test(render, left, top, render->mediaTexelOffsets[render->mediaTexturesIds[render->floorTex] * 2], right);
		top++;
	}
}

void Render_drawPlane_Test(Render_t* render, int x, int y, int planeTexture, int cnt)
{
	int v14;
	int v15;
	int v16;
	int v17;
	int v18;

	int vX, vY, vZ;
	int width, height;
	signed int v6, v9;
	signed int vsin, vcos;

	width = render->screenWidth;
	height = render->screenHeight;

	vX = render->viewX << 16;
	vY = render->viewY << 16;
	vZ = render->viewZ;

	vcos = render->viewCos;
	vsin = render->viewSin;

	v9 = (x << 1);
	v6 = (y << 1);

	if (height > v6) {
		vZ = 64 - vZ;
	}

	v14 = (vZ << 3);
	if (height > v6) {
		v15 = height - v6;
	}
	else {
		v15 = v6 - height;
	}

	v16 = ((width * v14) / (v15 + 1));
	v17 = (unsigned int)v16;
	v18 = ((v16 << 19) / (32 * width)) >> 8;

	Render_spanPlane_Test(render,
		v9 >> 1,
		v6 >> 1,
		planeTexture,
		vY + ((-vsin * v17) >> 3) + (((v18 * (vcos >> 8)) * (v9 - width + 1)) >> 1),
		vX + ((vcos * v17) >> 3) + (((v18 * (vsin >> 8)) * (v9 - width + 1)) >> 1),
		v18 * (vcos >> 8),
		v18 * (vsin >> 8),
		cnt);
}

void Render_spanPlane_Test(Render_t* render, int x, int y, int planeTexture, int param_5, int param_6, int param_7, int param_8, int cnt)
{
	unsigned short* spanPalettes;
	byte* mediaTexels;
	short* pixels;
	unsigned int offset1, offset2;
	//unsigned int offset3, offset4;

	spanPalettes = render->spanPalettes;
	mediaTexels = render->mediaTexels;
	pixels = render->pixels + (render->pitch >> 1) * y + x;
	while (--cnt >= 0)
	{
		offset1 = param_6 >> 10;
		offset2 = param_5 << 10;
		param_5 += param_7;
		param_6 += param_8;

		int n16 = mediaTexels[((offset1 & 0xFC0) + (offset2 >> 0x1A) + planeTexture)];
		//offset3 = (byte)(n16 & 0xF);
		//offset4 = (byte)(n16 >> 4 & 0xF);

		*pixels++ = spanPalettes[n16];
		//pixels++ = spanPalettes[offset4];
		//--cnt;
	}
}
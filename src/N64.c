//#include <libdragon.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include "N64.h"
//#include <assert.h>

#define SCREENWIDTH (320)
#define SCREENHEIGHT (240)

//#define debug (0)
int mallocSize = 0;
bool consoledebug =SDL_FALSE;
int texrdpmode = 0;
int palrdptex = 0;
int playchannel = 0;
int playsize = 0;
int rumbleEnd = 0;
bool Deblur = 0;
bool FPS = SDL_FALSE;
display_context_t _dc = 0;
long long starttic;
long long endtic;
int resettics = 0;


Uint16 CurColour;
Uint16 ColourKey;
SDL_Rect clipping;
//Uint16 CurScreen[SCREENWIDTH * SCREENHEIGHT];
openfiles = 0;


void vblCallback() {
}

void rdp_enable_texture_copy( void )
{
    /* Set other modes to copy and other defaults */
    __rdp_ringbuffer_queue( 0xEFA000FF );
    __rdp_ringbuffer_queue( 0x00004001 );
    __rdp_ringbuffer_send();
}


char* CurState;
void getstate(const char* state){
    CurState = state;
}

int SDL_Init(){
    init_interrupts();
	display_init(RESOLUTION_320x240, DEPTH_16_BPP, 2, GAMMA_NONE, ANTIALIAS_RESAMPLE);
	dfs_init(DFS_DEFAULT_LOCATION);
	rdp_init();
	rdp_set_default_clipping();
	//rdp_texture_cycle(0, 0, 0x80000000000000);

	rdp_enable_primitive_fill();
	//rdp_enable_blend_fill();
    
    
	 //uint64_t RDPCONFIG = (uint64_t)(ATOMIC_PRIM | ALPHA_DITHER_SEL_NO_DITHER | RGB_DITHER_SEL_NO_DITHER | SAMPLE_TYPE);
	 //rdp_texture_copy(RDPCONFIG);
	 //rdp_texture_cycle(0, 1, RDPCONFIG);

	register_VI_handler(vblCallback);
	controller_init();
	//timer_init();

    clipping.x = -1;
    clipping.y = -1;
    clipping.w = -1;
    clipping.h = -1;

    rumbleEnd = -1;
    //#ifdef debug
        console_init(0);
        console_set_render_mode(RENDER_MANUAL);
    //#endif
    fbuffer = NULL;
    return true;
}

void getpad() {
	// Read current controller status
	controller_scan();
	gKeys = get_keys_pressed();
	Pressed1 = gKeys.c[0];
	//Pressed2 = gKeys.c[1];
	//Pressed3 = gKeys.c[2];
	//Pressed4 = gKeys.c[3];
	gKeys = get_keys_down();
	Down1 = gKeys.c[0];
	//return (unsigned short)(gKeys.c[pad].data >> 16);

			//if(Down1.C_up) {
			//show_console ^= 1; 
			//N64_map[4] = 0x35;
			//i++;
			//}
    if(rumbleEnd > 0 && SDL_GetTicks() > rumbleEnd){
        rumble_stop(0);
        rumbleEnd = -1;
    }
}

void consolevisible(int bol){
    SDL_RenderPresent(NULL);    
    consoledebug ^= 1;
}
void testsub(const char* str){
        
    //#ifdef debug
    if(consoledebug){
        printf(str);
        printf("\n");
        console_render();         
    }

        
        // while( !(_dc = display_lock()) );  
        // fbuffer = (Uint16*)__get_buffer(_dc); 
        // int i;
        // for(i = 0; i < 76800; i++){
        //     fbuffer[i] = CurScreen[i];
        // }
        //graphics_draw_box(_dc, 0,0,320, 20, 255);
        //graphics_draw_text(_dc, 5,5,str);
        //display_show(_dc);
   //#endif
    //else{
        //strcpy(test, str);
        // if(fbuffer!=NULL){    
        // graphics_draw_box(_dc, 0,0,320, 20, 255);
        // graphics_draw_text(_dc, 5,5,str);
        // display_show(_dc);
        // }
    //}

}

SDL_Window* SDL_CreateWindow(const char* title, int posx, int posy, int w, int h, int flags){
    return malloc(sizeof(SDL_Window));
}

void SDL_SetWindowMinimumSize(SDL_Window* Window, int w, int h){

}

void SDL_GetWindowSize(SDL_Window* Window, int* w, int* h){
    w=320;
    h=240;
}

void SDL_SetWindowFullscreen(SDL_Window* Window, int flag){

}

void SDL_WarpMouseInWindow(SDL_Window* Window, int* posx, int* posy){

}

void SDL_DestroyWindow(SDL_Window* Window){

}

Uint32 SDL_PollEvent(SDL_Event* ev){
    //testsub("SDL_PollEvent");
    
    getpad();

    return Down1.data;
}

void SDL_Quit(){

}

Uint16* SDL_CreateRenderer(SDL_Window* window, int val, int flag){
    return malloc(SCREENWIDTH * SCREENHEIGHT * sizeof(uint16_t));
}

void SDL_RenderSetLogicalSize(SDL_Renderer* renderer, int w, int h){
    
}

void SDL_SetRenderDrawColor(SDL_Renderer renderer, int r, int g, int b, int a){
    r>=3;
    g>=3;
    b>=3;
    a=1;
    CurColour = (r <<11) | (g << 6)| (b << 1)| 1;//a;
    //renderer.frontcolour = (r <<11) | (g << 6)| (b << 1)| a;
}

Uint32 SDL_MapRGB(int* format, int r, int g, int b){
    r>>=3;
    g>>=3;
    b>>=3;
    return ((r <<11) | (g << 6)| (b << 1)| 1);
}

void SDL_SetColorKey(SDL_Surface* Surface, int flag, Uint32 colorkey){
    if(flag){
        ColourKey = (Uint16)colorkey;
    }
    else{
        ColourKey = 0;
    }
}

void SDL_RenderSetIntegerScale(Uint16* renderer, int scale){
        //IntegerScale = Scale
}

void rdp_setdeblur(int option){
    Deblur = option;
}
void SetFPSCounter(int option){
    FPS = option;
}

void rdpsquare(int y1, int y2, Uint16 color){
    if(texrdpmode){
        rdp_enable_primitive_fill();
        texrdpmode = SDL_FALSE;
        palrdptex = SDL_FALSE;
    }
    //rdp_set_fill_color(((color&0xf800)>>8), ((color&0x7c0)>>3), ((color&0x3e)<<2),0xff);
    __rdp_ringbuffer_queue( 0x37000000 );
    __rdp_ringbuffer_queue( (color <<16) | color);
    __rdp_ringbuffer_send();

    rdp_sync(SYNC_LOAD);
    rdp_draw_filled_rectangle(0, y1, 320, y2);
    rdp_sync(SYNC_TILE);
}

void testchannel(int chan, int size){
    playchannel = chan;
    playsize = size;
}

void rdp_test(Uint16* renderer){
    int i;
    Uint16* data = renderer;
    int blockheight = 60;
    int blockwidth = 32;
    int Hloopcount= 4;
    Uint16 dataoffset = (blockwidth * blockwidth); 

    //  sprite_t* test = malloc(sizeof(sprite_t));
    // test->bitdepth = 2;
    // test->height=blockheight;
    // test->width = blockwidth;
    // test->hslices=1;
    // test->vslices=1;
    //test->data = data
    
    rdp_set_prim_color(0xff, 0xff, 0xff, 0xff);
    //left side
    for(i = 0; i < Hloopcount; i++){
        // memcpy(test->data, data, dataoffset);
        // rdp_load_texture(test);

        rdp_load_texbuf(data, blockwidth-1, blockheight-1);
        rdp_sync(SYNC_LOAD);

        rdp_draw_textured_rectangle_scaled(0, (i * blockheight), 0, ((i+1)*blockheight),1.0,1.0, 0);
        rdp_sync(SYNC_TILE);

        data+=dataoffset;
    }
    //right side
    // for(i = 0; i < Hloopcount; i++){
    //     rdp_load_texbuf(data, blockwidth, blockheight);
    //     rdp_draw_textured_rectangle_scaled(160, (i * 12), 160, (i+1)*blockheight, 1,1, 0);
    //     data+=dataoffset;  
    //     rdp_sync(SYNC_TILE);   
    // }
}

void rdp_draw_tex(int x1, int y1, int x2, int y2, double scale, int flags){
    uint16_t s = 0;
    uint16_t t = 0;

    /* Cant display < 0, so must clip size and move S,T coord accordingly */
    if ( x1 < 0 )
    {
        if ( x1 < -0) { return; } // prevent N64 crash
        s += (int)(((double)((-x1) << 5)) * (1.0 / 1));
        x1 = 0;
    }

    if ( y1 < 0 )
    {
        if ( y1 < -63 * scale ) { return; }
        t += (int)(((double)((-y1) << 5)) * (1.0 / scale));
        y1 = 0;
    }

    // mirror horizontally or vertically
    if ( flags )
    {	
        if ( flags != 2 )
            s += ( (1+1) + ((4-(0+1)) << 1) ) << 5;
	
        if ( flags != 1 )
            t += ( (63+1) + ((64-(63+1)) << 1) ) << 5;	
    }	

    // fixes 1/4 pixel cycle draw
    // if ( pixel_mode == 1024 )
    // {
    //     bx ++;
    //     by ++;	
    // }

    /* Set up rectangle position in screen space */
    __rdp_ringbuffer_queue( 0x24000000 | x2 << 14 | y2 << 2 );
      __rdp_ringbuffer_queue( x1 << 14 | y1 << 2 );

    /* Set up texture position and scaling to 1:1 copy */
    __rdp_ringbuffer_queue( s << 16 | t );
    __rdp_ringbuffer_queue( ((int)(1024) & 0xFFFF) << 16 | ((int)(1024/ scale) & 0xFFFF) );	
		
    /* Send command */
    __rdp_ringbuffer_send();
}

void rdp_drawspan2(int texcol, int dc_x, int dc_yl, int centery, int dc_yh, double dc_iscale, int dc_texturemid){
//rdp_attach_display(_dc);
    double actscale = dc_iscale;//((dc_iscale >> 1) >> 16);
    //32x32

    uint16_t s = ((int)((texcol & 63) << 5)); // + ((((dc_texturemid)+((dc_yl - centery) + offset) * dc_iscale) >> FRACBITS) << 5);
    //32x64
    //uint16_t s = ((int)((texcol) & 63)<<5); // + ((((dc_texturemid)+((dc_yl - centery) + offset) * dc_iscale) >> FRACBITS) << 5);

    //32x32
    //uint16_t t = ((((dc_texturemid + ((dc_yl - centery) * dc_iscale) >> (FRACBITS)+1) & 31) << 5) + offset); //0;
    //32x64
    uint16_t t = 0; //((int)((dc_yl) & 63) << 5); //0;
    //uint16_t t = (((int)((dc_yl) * (1.0 / dc_iscale)) & 63) << 5); //0;

    int ty = dc_yl;
    int tx = dc_x;
    if ( dc_x < 0 )
    {
       // if ( tx < -64 * 1.0) { 
            return; 
        //} // prevent N64 crash
        //s += (int)(((double)((-tx) << 5) ) * (1.0 / 1.0));
        //tx = 0;
    }

    if (dc_yl < 0)
    {

        if (dc_yl < (dc_yh - dc_yl) * actscale) { return; }
        //t += (int)(((double)((-dc_yl) << 5)) * (1.0 / (float)actscale));
        //t += (dc_texturemid + (dc_yl - centery) * actscale);

        //t += (int)(((double)((-dc_yl* dc_iscale) << 5)) ); //(1.0 / (float)actscale));
        t += ((dc_texturemid + ((int)((dc_yl - centery) * (actscale)) & 63) << 5));

        ty = 0;
    }

    //draw texture1
    __rdp_ringbuffer_queue(0x24000000 | (dc_x+2) << 14 | (dc_yh) << 2);
    //__rdp_ringbuffer_queue128(0xE4000000 | dc_x << 14 | (dc_yh +offset) << 2);
    //load texslot1
    __rdp_ringbuffer_queue((tx) << 14 | ty << 2);
    /* Set up texture position and scaling to 1:1 copy */
    __rdp_ringbuffer_queue(s << 16 | t);
    //__rdp_ringbuffer_queue128(((int)(4096 / 1) & 0xFFFF) << 16 | ((int)(count / (double)actscale) & 0xFFFF));

    //32x32
    //__rdp_ringbuffer_queue128(((int)(4096>>1) & 0xFFFF) << 16 | (int)(FixedMul((fixed_t)(1024<<16), dc_iscale) >> 18)); //((int)(1024/ (double)actscale)) //&0xFFFF);
    //32x64
    __rdp_ringbuffer_queue(((int)(512) & 0xFFFF) << 16 | (int)(((long long)(1024) / (actscale))));   //((int)(1024/ (double)actscale)) //&0xFFFF); //18

    /* Send command */
    rdp_send();
}

void rdp_loadtest(Uint16* paldata, Uint8* pixdata, int pixdataoffset){

	//rdp_texture_cycle(0, 1, ATOMIC_PRIM);
	//rdp_set_prim_color(0xff, 0xff, 0xff, 0xff);


	if(!palrdptex){
        rdp_set_prim_color(0xff,0xff,0xff,0xff);

        if(!Deblur) rdp_texture_cycle(0, 0, ATOMIC_PRIM | EN_TLUT | SAMPLE_TYPE);
        else rdp_texture_cycle(0, 0, ATOMIC_PRIM | EN_TLUT );
        //rdp_texture_copy(ATOMIC_PRIM | EN_TLUT);
        

        palrdptex = SDL_TRUE;
        texrdpmode = SDL_FALSE;
    }
        //rdp_texture_cycle(0, 0, ATOMIC_PRIM | EN_TLUT);
        rdp_load_palette(15, 15, paldata);
        rdp_select_palette(15);
        rdp_load_texture2(pixdata + (pixdataoffset>>0xd), 64, 64,0,0,0,0);


    //for(i = 0; i < spriteheight; i++){
        //spdata[i] = paldata[pixdata[pixdataoffset >> 13] >> ((pixdataoffset >> 10) & 4) & 0xf];
         //*(spdata + 8 + i) = pixdata[pixdataoffset + i];
    //}
    // rdp_load_texture(spdata);
    //rdp_load_texbuf(&pixdata[pixdataoffset>>0xd], spritewidth, spriteheight);


    
    
}

void rdp_coltest(Uint16* paldata, Uint8* pixdata, int pixdataoffset, int x, int y1, int height, int scale){

    int y = y1;

	// if(!palrdptex){
    //     rdp_set_prim_color(0xff,0xff,0xff,0xff);

    //     //rdp_texture_copy(ATOMIC_PRIM | EN_TLUT);
    //     rdp_texture_cycle(0, 0, ATOMIC_PRIM | EN_TLUT);

    //     palrdptex = SDL_TRUE;
    //     texrdpmode = SDL_FALSE;
    // }
    //     //rdp_texture_cycle(0, 0, ATOMIC_PRIM | EN_TLUT);
    //     rdp_load_palette(15, 15, paldata);
    //     rdp_select_palette(15);
    //     rdp_load_texture2(pixdata + (pixdataoffset>>0xd), 64, 64,0,0,0,0);
    //     //rdp_load_texture2(pixdata + (pixdataoffset>>0xd), 64, 64,0,0,0,0);

	//rdp_sync(SYNC_LOAD);
    //rdp_draw_tex(x, y1, x+2, y1 + (height>>1), (height>>1) / (double)(spriteheight>>2), 0);
	//rdp_draw_textured_rectangle_scaled(x, y1, x+2, y1 + height, 1.0, (height / 64.0), 0); //- (param_5 <<7)
	//rdp_drawspan(0, x, y1, y1+ (height)/2, y1 + (height), (1<<17) + ((height / spriteheight)<<11), 1);

    if(consoledebug != true){
        rdp_drawspan2((pixdataoffset>>18), x, y, y+height/2, y+height, height / 64.0, y1 + height/2);
        //rdp_drawspan2((pixdataoffset>>18), x, y1, y1+height/2, y1+height, (height/(double)64.0), y1 + height/2);  
        rdp_sync(SYNC_TILE);        
    }


	// rdp_sync(SYNC_LOAD);
    // //rdp_draw_tex(x, y1 + (height>>1), x+2, y1 + height, (height>>1) / (double)(spriteheight>>2), 0);
	// rdp_draw_textured_rectangle_scaled(x, y1 + height/2, x+2, y1 + height, 1.0, (height) / (double)(spriteheight/4), 0); //- (param_5 <<7)
	// //rdp_drawspan(0, x, y1, y1+ (height)/2, y1 + (height), (1<<17) + ((height / spriteheight)<<11), 1);
	// rdp_sync(SYNC_TILE);

    //free(spdata);
}

void SDL_RenderPresent(Uint16* renderer){

    testsub("SDL_RenderPresent");
    //printf("rend=%x\n", renderer);
    //printf("buf=%x\n", fbuffer);

    MikMod_Update();

    //if(fbuffer == NULL) return;

        #ifndef debug
        if(!consoledebug){

            rdp_detach_display();
             int i;
             Uint16* src = renderer;
             Uint16* dest = fbuffer;
            // //printf("SDL_RenderPresent 1\n");
            //memcpy(dest, src, 76800);

            i = 76800;
            if(FPS){
                endtic = SDL_GetTicks();
                sprintf(test, "%i", 1000 / (endtic - starttic)); 
                graphics_draw_text(_dc, 5,1,test);                
            }

            // while(i--){
            //     if(*src > 0){
            //         *dest++ = *src++;
            //     } 
            //     else
            //     {
            //         src++;
            //         dest++;
            //     }
            //     //dest[i] = CurScreen[i];
            // } 


            // graphics_draw_box(_dc, 0,215, 80, 20, 0xccff);
            // sprintf(test, "s=%s tics=%i", CurState, playsize); // SDL_GetTicks()); //SDL_GetTicks()); //mallocsize
            // graphics_draw_text(_dc, 5,220, test);
            //printf("SDL_RenderPresent 2\n");

            display_show(_dc); 
            fbuffer = NULL;           
        }
            
        #endif  
    //testsub("SDL_RenderPresent end");
    //console_render();


}

void SDL_RenderClear(Uint16* renderer){
    testsub("SDL_RenderClear");
   #ifndef debug
   if(!consoledebug){
        while( !(_dc = display_lock()) );
        //_dc = display_lock();    
        fbuffer = (Uint16*)__get_buffer(_dc);   

        starttic = SDL_GetTicks();
        if (starttic < endtic) resettics = true;
        else resettics = false;

        rdp_attach_display(_dc);  
        rdp_set_clipping(0,0,SCREENWIDTH, SCREENHEIGHT);
    //     rdp_sync(SYNC_PIPE);
        
         rdp_enable_primitive_fill();
        rdp_set_fill_color(0x00, 0x00, 0x00, 0xFF);
        rdp_draw_filled_rectangle(0,0,320,240);
        rdp_sync(SYNC_TILE);
        //rdp_sync(SYNC_FULL);
    
    // //    rdp_enable_texture_copy();
        uint64_t RDPCONFIG = (uint64_t)(ATOMIC_PRIM | ALPHA_DITHER_SEL_NO_DITHER | RGB_DITHER_SEL_NO_DITHER | SAMPLE_TYPE);
        rdp_texture_copy(RDPCONFIG); //RDPCONFIG); //RDPCONFIG);
        //rdp_texture_cycle(0, 1, RDPCONFIG);  
        texrdpmode = SDL_TRUE;
        rdp_sync(SYNC_PIPE);
       }        
        #endif
        //testsub("SDL_RenderClear 1");
            //graphics_fill_screen(_dc, 1);

            //testsub("SDL_RenderClear end");    


}

void SDL_RenderCopy(Uint16* renderer, SDL_Texture* image, SDL_Rect* cliprect, SDL_Rect* renderrect){
    testsub("SDL_RenderCopy");
    sprintf(test, "clip x=%i, y=%i, w=%i, h=%i", cliprect->x, cliprect->y, cliprect->w, cliprect->h);
    testsub(test);
    sprintf(test,"rend x=%i, y=%i, w=%i, h=%i", renderrect->x, renderrect->y, renderrect->w, renderrect->h);
    testsub(test);
    int cx, cy;
    int rx, ry;
    int i, j;
    Uint16 val;
    cx = cliprect->x;
    rx = renderrect->x;
    for(i = 0; i < (cliprect->h-1); i++){
        cy = (cliprect->y + i) * image->w;
        ry = (renderrect->y + i) * SCREENWIDTH; //renderrect->w;

        if(ry > 76800) return;

        for(j = 0; j < (cliprect->w-1); j++){
            #ifdef J2ME
            renderer[ry + (rx + j)] = image[cy + cliprect->w - (cx + j)]; //rgb
            #endif

            #ifdef BREW
            //renderer[ry + ((rx) + j)] = image[cy + cliprect->w - (cx + j)]; //bmp
            val = image->data[cy + image->w - (cx + j)];

            //if(val != ColourKey) CurScreen[ry + ((rx) + j)] = val; //= image->data[cy + image->w - (cx + j)]; //bmp
            if(val) renderer[ry + ((rx) + j )] = image->data[cy + image->w - (cx + j)]; //bmp
            #endif
        }
    }
    testsub("SDL_RenderCopy end");
}

void SDL_RenderCopy2(Uint16* renderer, SDL_Texture* image, SDL_Rect* cliprect, SDL_Rect* renderrect){
    
    if(fbuffer == NULL) return;

    testsub("SDL_RenderCopy2");
    sprintf(test, "clip x=%i, y=%i, w=%i, h=%i", cliprect->x, cliprect->y, cliprect->w, cliprect->h);
    testsub(test);
    sprintf(test,"rend x=%i, y=%i, w=%i, h=%i", renderrect->x, renderrect->y, renderrect->w, renderrect->h);
    testsub(test);
    int cx, cy;
    int rx, ry;
    int i, j;
    Uint16 val;
    cx = cliprect->x;
    rx = renderrect->x;
    int clip = SDL_FALSE;


    if(clipping.x != -1) clip = SDL_TRUE;
    //ry = renderrect->y;
    for(i = 0; i < (cliprect->h-1); i++){

        if(clip){
            if(clipping.y > (renderrect->y + i)) continue;
            if((clipping.y + clipping.h) < (renderrect->y + i)) return;
        }

        
        cy = (cliprect->y + i) * image->w;
        ry = (renderrect->y + i) * SCREENWIDTH; //renderrect->w; 

        if(ry > 76800) return;

        for(j = 0; j < (cliprect->w-1); j++){

            if(clip){
                if(clipping.x > (rx + j)) continue;
                if((clipping.x + clipping.w) < (rx + j)) break;                
            }


            #ifdef J2ME
            renderer[ry + (rx + j)] = image[cy + cliprect->w - (cx + j)]; //rgb
            #endif

            #ifdef BREW
            //renderer[ry + ((rx) + j)] = image[cy + cliprect->w - (cx + j)]; //bmp
            val = image->data[cy + image->w - (cx + j)];

            //if(val != ColourKey) CurScreen[ry + ((rx) + j)] = val; //= image->data[cy + image->w - (cx + j)]; //bmp
            if(val) fbuffer[ry + ((rx) + j )] = val; //image->data[cy + image->w - (cx + j)]; //bmp
            #endif
        }
    }
    testsub("SDL_RenderCopy end");
}

void SDL_DrawTexturedTile(Uint16* imgBitmap, int xDst, int yDst, int width, int height, int flags){
    if(fbuffer == NULL) return;
    
    if(!texrdpmode){
    uint64_t RDPCONFIG = (uint64_t)(ATOMIC_PRIM | ALPHA_DITHER_SEL_NO_DITHER | RGB_DITHER_SEL_NO_DITHER | SAMPLE_TYPE);
	 rdp_texture_copy(RDPCONFIG); //RDPCONFIG); //RDPCONFIG);
	 rdp_texture_cycle(0, 1, RDPCONFIG);  
    texrdpmode = SDL_TRUE;
    palrdptex = SDL_FALSE;
    }
    rdp_set_prim_color( (0xff), (0xff), (0xff), 0xff );
    rdp_load_texbuf(imgBitmap, width, height);
    
    rdp_sync(SYNC_LOAD);
    rdp_draw_textured_rectangle(xDst, yDst, xDst + width, yDst + height, flags);

    rdp_sync(SYNC_TILE);
}

void SDL_DrawTexturedHTiles(SDL_Texture* imgBitmap, int width, int height, int xDst, int yDst){
    int i;

    if(fbuffer == NULL) return;

    if(!texrdpmode){
    uint64_t RDPCONFIG = (uint64_t)(ATOMIC_PRIM | ALPHA_DITHER_SEL_NO_DITHER | RGB_DITHER_SEL_NO_DITHER | SAMPLE_TYPE);
	 rdp_texture_copy(RDPCONFIG);
	 rdp_texture_cycle(0, 1, RDPCONFIG);  
    texrdpmode = SDL_TRUE;
    palrdptex = SDL_FALSE;
    }

    rdp_load_texbuf(imgBitmap->data, imgBitmap->w, imgBitmap->h);
    rdp_sync(SYNC_LOAD);
    //rdp_draw_textured_rectangle(0, yDst, 320, yDst + height, 0);
    
    for(i = 0; i < width; i += imgBitmap->h){   
        rdp_draw_textured_rectangle(i, yDst, i + imgBitmap->w, yDst + height, 2);
    }
    

    rdp_sync(SYNC_TILE);
}

void SDL_RenderSetClipRect(Uint16* renderer, SDL_Rect* cliprect){
    //assert(cliprect);
    if(cliprect != NULL) {
        clipping.x=cliprect->x;
        clipping.y=cliprect->y;
        clipping.w=cliprect->w;
        clipping.h=cliprect->h;        
    }
    else {
        clipping.x=-1;
        clipping.y=-1;
        clipping.w=-1;
        clipping.h=-1;        
    }

}

void SDL_SetRenderDrawBlendMode(Uint16* renderer, int blend){
    //blendmode = blend;
}

void SDL_RenderFillRect(Uint16* renderer, SDL_Rect* cliprect){
    int x,y;
    int right, bottom;
    right = cliprect->x + cliprect->w;
    bottom = cliprect->y + cliprect->h;
    for(y = cliprect->y; y < bottom; y++){
        for(x = cliprect->x; x < right; x++){
            renderer[(y * SCREENWIDTH) + x] = CurColour;
            //CurScreen[(y * SCREENWIDTH) + x] = CurColour;
        }
    }
}

void SDL_RenderFillRect2(Uint16* renderer, SDL_Rect* cliprect){

    if(fbuffer == NULL) return;

    // int x,y;
     int right, bottom;
     right = cliprect->x + cliprect->w;
     bottom = cliprect->y + cliprect->h;
    //     for(y = cliprect->y; y < bottom; y++){
    //     for(x = cliprect->x; x < right; x++){
    //         fbuffer[(y * SCREENWIDTH) + x] = CurColour;
    //         //CurScreen[(y * SCREENWIDTH) + x] = CurColour;
    //     }
    // }
    
    if(texrdpmode || palrdptex){
        rdp_enable_primitive_fill();
        texrdpmode = SDL_FALSE;
        palrdptex = SDL_FALSE;
    }
        rdp_set_fill_color((CurColour>>11)<<3, (CurColour & 0x7c0)>>3, (CurColour & 0x1f)<<1, 0xFF);
        rdp_draw_filled_rectangle(cliprect->x,cliprect->y, right ,bottom);
         rdp_sync(SYNC_TILE);
    // rdp_draw_filled_rectangle(cliprect->x, cliprect->y, right, bottom);
    // rdp_sync(SYNC_FULL);
}

void SDL_RenderDrawRect(Uint16* renderer, SDL_Rect* cliprect){
    // int x,y;
    // int right, bottom;
    // right = cliprect->x + cliprect->w;
    // bottom = cliprect->y + cliprect->h;
    // for(y = cliprect->y; y < bottom; y++){
    //     for(x = cliprect->x; x < right; x++){

    //     }
    // }

    if(fbuffer == NULL) return;

    int x,y,w,h;
    x = cliprect->x;
    y = cliprect->y;
    w = cliprect->w;
    h = cliprect->h;
    SDL_RenderDrawLine(renderer, x, y, x+w, y);
    SDL_RenderDrawLine(renderer, x+w, y, x+w, y+h);
    SDL_RenderDrawLine(renderer, x+w, y+h, x, y+h);
    SDL_RenderDrawLine(renderer, x, y+h, x, y);
}

void SDL_RenderDrawPoints(Uint16* renderer, SDL_Point* points, int count){
    int i;
    SDL_Point* pnt = points;
    for(i = 0; i < count; i++){
        //buffer[(pnt->v * SCREENWIDTH) + pnt->h] = frontcolor;
        pnt++;
    }
}

void SDL_RenderDrawLine(Uint16* renderer, int x1, int y1, int x2, int y2){
    
    if(clipping.x != -1){
        if(x1 < clipping.x) x1 = clipping.x;
        if(y1 < clipping.y) y1 = clipping.y;
        if(x2 > clipping.x + clipping.w) x2 = clipping.x + clipping.w;
        if(y2 > clipping.y + clipping.h) y2 = clipping.y + clipping.h;
    }
    
    int dy = y2 - y1;
	int dx = x2 - x1;
	int sx, sy;

    if(fbuffer == NULL) return;

	if(dy < 0)
	{
		dy = -dy;
		sy = -1;
	}
	else{
        sy = 1;
    }
		

	if(dx < 0)
	{
		dx = -dx;
		sx = -1;
	}
	else{
        sx = 1;
    }
		

	dy <<= 1;
	dx <<= 1;
    //renderer[(y1*SCREENWIDTH)+x1] = CurColour;
    fbuffer[(y1*SCREENWIDTH)+x1] = CurColour;
	//graphics_draw_pixel(disp, x0, y0, color);
	if(dx > dy)
	{
		int frac = dy - (dx >> 1);
		while(x1 != x2)
		{
			if(frac >= 0)
			{
				y1 += sy;
				frac -= dx;
			}
			x1 += sx;
			frac += dy;
			//graphics_draw_pixel(disp, x0, y0 , color);
            //renderer[(y1*SCREENWIDTH)+x1] = CurColour;
            fbuffer[(y1*SCREENWIDTH)+x1] = CurColour;
		}
	}
	else
	{
		int frac = dx - (dy >> 1);
		while(y1 != y2)
		{
			if(frac >= 0)
			{
				x1 += sx;
				frac -= dy;
			}
			y1 += sy;
			frac += dx;
			//graphics_draw_pixel(disp, x0, y0 , color);
            //renderer[(y1*SCREENWIDTH)+x1] = CurColour;
            fbuffer[(y1*SCREENWIDTH)+x1] = CurColour;
		}
	}
}

void SDL_DestroyRenderer(Uint16* renderer){
    free(renderer);
    renderer = NULL;
}

Uint64 SDL_GetTicks(){
    //testsub("SDL_GetTicks");
    return get_ticks_ms(); //100000;
}

Uint16 GetShort(Uint8* data){
    return (*(data + 0)) | (*(data + 1)<<8);
}

Uint32 GetLong(Uint8* data){
    return (*(data + 0)) | (*(data + 1)<<8) | (*(data + 2)<<16) | (*(data + 3)<<24);
}

Uint8 SDL_SwapLE8(Uint8 val){
    Uint8 out = ((val&0xf) << 4) | ((val&0xf0) >> 4);
    return out;
}

Uint16 SDL_SwapLE16(Uint16 val){
    Uint16 out = ((val&0xff) << 8) | ((val&0xff00) >> 8);
    return out;
}

Uint32 SDL_SwapLE32(Uint32 val){
    Uint32 out = (val >> 24) | ((val&0xff0000)>>8) | ((val&0xff00)<<8) | ((val&0xff)<<24);
    return out;
}

//SDL Memory
Uint8* SDL_malloc(int size){
    Uint8* data;
    mallocSize += size;
    data = malloc(size);
    return data;
}

Uint8* SDL_calloc(int Count, int size){
    //testsub("SDL_calloc");
    Uint8* data;
    data = calloc(Count, size);
    //testsub("SDL_calloc_end");
    return data;
}

Uint8* SDL_realloc(Uint8* Bloc, int size){
    return realloc(Bloc, size);
}

void SDL_free(void* data){
    //assert(data);
    if(data != NULL){
       free(data); 
    }
    
    return;
}

void freedata(int size){
    mallocSize -= size;
}

void SDL_memcpy(Uint8* Dest, Uint8* Src, int size){
    memcpy(Dest, Src, size);
    return;
}
int SDL_memcmp(Uint8* Buf1, Uint8* Buf2, int size){
    return memcmp(Buf1, Buf2, size);
}
Uint8* SDL_memset(void* data, int value, int size){
    //assert(data);
    return memset(data, value, size);
}

void SDL_memmove(Uint8* dest, Uint8* src, int size){
    //assert(dest);
    //assert(src);
    memmove(dest, src, size);
    return;
}

//SDL Strings
int SDL_strlen(const char* str){
    return strlen(str);
}

int SDL_strcmp(char* str1, char* str2){
    return strcmp(str1, str2);
}

char* SDL_strchr(char* str, int val){
    return strchr(str, val);
}

int SDL_strcasecmp(char* str1, char* str2){
    return strcmp(str1, str2);
}

int SDL_atoi(const char* str){
    return atoi(str);
}
// void SDL_snprintf(char* str, int size, char* format, ...){
//     snprintf(str, size, format);  
// }

//SDL images
SDL_Texture* SDL_CreateTexture(Uint16* SDL_Renderer, int PixelFormat, int flag, int w, int h){
    SDL_Texture* texture = SDL_malloc(sizeof(SDL_Texture));
    texture->data = SDL_malloc(w*h*sizeof(Uint16));
    texture->w = w;
    texture->h = h;
    return texture;
}

SDL_Texture* SDL_CreateTextureFromSurface(SDL_Renderer* renderer, SDL_Surface* surface){
    
    testsub("SDL_CreateTextureFromSurface");

    int i,j, size, surfacewidth;
    Uint8 val1, val2, transval;

    size = surface->w * surface->h * sizeof(Uint16);

    SDL_Texture* imgdata = SDL_malloc(sizeof(SDL_Texture));
    imgdata->data = SDL_malloc(size);
    imgdata->w = surface->w;
    imgdata->h = surface->h;

    surfacewidth = surface->size / (Uint32)surface->h;

    int surline, imgline;

    size = (surface->w) * (surface->h);
    Uint16 color;
    RGBColour rgb1, rgb2;

    transval=255;
    for(i = 0; i < surface->format->palette->ncolors; i++){
        rgb1 = surface->format->palette->colors[(i)];
        color = ((rgb1.b >> 3) << 11) | ((rgb1.g >> 3) <<6) | ((rgb1.r >> 3) <<1) | 1;
        if(color==ColourKey){
            transval = i;
            break;
        }
    }

    for(i = 0; i < (surface->h); i++){
        surline = i * (surfacewidth);
        imgline = i * (surface->w);
        for(j = 0; j < (surface->w/2); j++){
            //testsub("SDL_CreateTextureFromSurface1a");
            val1 = surface->data[(surline + j)];
            val1>>=4;
            val2 = surface->data[(surline + j)];
            val2 &=0xf;
            //printf("val=%x\n", (val));
            //testsub("SDL_CreateTextureFromSurface1b");
            rgb1 = surface->format->palette->colors[(val1)];
            rgb2 = surface->format->palette->colors[(val2)];
            //testsub("SDL_CreateTextureFromSurface2");
            if(val1 != transval) color = ((rgb1.b >> 3) << 11) | ((rgb1.g >> 3) <<6) | ((rgb1.r >> 3) <<1) | 1;
            else color = 0;

            imgdata->data[size - ((2*j) + imgline + 0)] = color;
            //testsub("SDL_CreateTextureFromSurface3");
            if(val2 != transval) color = ((rgb2.b >> 3) << 11) | ((rgb2.g >> 3) <<6) | ((rgb2.r >> 3) <<1) | 1;
            else color = 0;
            
            imgdata->data[size - ((2 * j) + imgline + 1)] = color;     
            
            // val = surface->data[size - (surline + j + 1)];
            // //printf("val=%x\n", (val));
            // //testsub("SDL_CreateTextureFromSurface1b");
            // rgb2 = surface->format->palette->colors[(val>>4)];
            // rgb1 = surface->format->palette->colors[(val&0xF)];
            // //testsub("SDL_CreateTextureFromSurface2");
            // color = ((rgb1.r >> 3) << 11) | ((rgb1.g >> 3) <<6) | ((rgb1.b >> 3) <<1) | 1;
            // imgdata->data[((2*(j + 1)) + imgline + 2)] = color;
            // //testsub("SDL_CreateTextureFromSurface3");
            // color = ((rgb2.r >> 3) << 11) | ((rgb2.g >> 3) <<6) | ((rgb2.b >> 3) <<1) | 1;
            // imgdata->data[((2*(j+1)) + imgline + 3)] = color;     
        }


    }

    // for(i = 0; i < surface->size; i++){

    //         if(((2*i)+1) > (surface->w * surface->h)) return imgdata;

    //         val = surface->data[surface->size - (i)];
    //         //printf("val=%x\n", (val));
    //         //testsub("SDL_CreateTextureFromSurface1b");
    //         rgb2 = surface->format->palette->colors[(val>>4)];
    //         rgb1 = surface->format->palette->colors[(val&0xF)];
    //         //testsub("SDL_CreateTextureFromSurface2");
    //         color = ((rgb1.r >> 3) << 11) | ((rgb1.g >> 3) <<6) | ((rgb1.b >> 3) <<1) | 1;
    //         imgdata->data[((2*i) + 0)] = color;
    //         //testsub("SDL_CreateTextureFromSurface3");
    //         color = ((rgb2.r >> 3) << 11) | ((rgb2.g >> 3) <<6) | ((rgb2.b >> 3) <<1) | 1;
    //         imgdata->data[((2*i) + 1)] = color;   
    // }
    testsub("SDL_CreateTextureFromSurface_end");

    return imgdata;
}

SDL_Texture* SDL_CreateTiledTextureFromSurface(SDL_Renderer* renderer, SDL_Surface* surface, int spritewidth, int spriteheight){
 testsub("SDL_CreateTiledTextureFromSurface");

    int i,j, k, outsize, bmpsize, surfacewidth, iwidth, tempsize, blockoffset;
    Uint8 val1, val2, transval;
    
    outsize = surface->w * surface->h * sizeof(Uint16);

    SDL_Texture* imgdata = SDL_malloc(sizeof(SDL_Texture));
    imgdata->data = SDL_malloc(outsize);
    memset(imgdata->data, 0, outsize);

    imgdata->w = spritewidth;//surface->w;
    imgdata->h = spriteheight;//surface->h;
 testsub("SDL_CreateTiledTextureFromSurface1");    
    surfacewidth = surface->size / (Uint32)surface->h;
    
    int surline, imgline;
    //Uint8* tempdata = malloc(outsize);
    bmpsize = (surface->w/2) * (surface->h);

    Uint16 color;
    RGBColour rgb1, rgb2;
 testsub("SDL_CreateTiledTextureFromSurface2");
    transval=255;
    for(i = 0; i < surface->format->palette->ncolors; i++){
        rgb1 = surface->format->palette->colors[(i)];
        color = ((rgb1.b >> 3) << 11) | ((rgb1.g >> 3) <<6) | ((rgb1.r >> 3) <<1) | 1;
        if(color==ColourKey){
            transval = i;
            break;
        }
    }
 testsub("SDL_CreateTiledTextureFromSurface3");
     int pos;
     Uint8* data = surface->data;
     iwidth = ((spritewidth + (spritewidth & 1) )>>1);
     int blocksize, blockrowcount;
     blocksize = spritewidth & spriteheight; //iwidth * spriteheight;
     blockrowcount = surface->w / spritewidth; //((surface->w/2)/ (spritewidth/2));
    //  for(i = 0; i < surface->h; i++){ //(surface->h); i++){

    //      surline =  (i * surface->w/2);
    //      imgline = (((i / spriteheight) * (blockrowcount * blocksize))
    //                  + ((i) * (iwidth)  ));

    //     for(j = 0; j < surface->w/2; j++){
    //         //testsub("SDL_CreateTextureFromSurface1a");
    //         val1 = surface->data[bmpsize - (surline + j - (j / (spritewidth)))];

    //         pos = (imgline 
    //             + (((j) / iwidth) * (blocksize)) 
    //             + (((j) % iwidth)));

    //         tempdata[pos] = val1;   

    //     }

    //  }
 testsub("SDL_CreateTiledTextureFromSurface4");
    int suroffset, surfpos, oddsize;
    bmpsize = (surface->w * surface->h);
    oddsize = (spritewidth & 1);
    blocksize = spritewidth * (spriteheight); //iwidth* spriteheight;
    blockrowcount = ((surface->w/2)/ (spritewidth));
    tempsize = 0;
    //loop each block to draw
     for(i = 0; i < ((bmpsize/2)/blocksize); i++){
     //for(i = 0; i < (2); i++){   
        //testsub("i");
        //line to start in surface

        //normal
        data = surface->data;
        data += ( (i / blockrowcount ) * (surface->w/2 * spriteheight)); // *blockrowcount));
        data -= ( (i / blockrowcount ) * (surface->w/2));
        //offset to read from
        data += (surface->w - (((i%blockrowcount))*spritewidth)) + (surface->w/2 * (i/blockrowcount)); //data += (i * (iwidth)) - ((i / 2) * oddsize);
        //data += ((blockrowcount - i) * iwidth) + ((i / 2) * oddsize);

        //reverse
        // data = surface->data + (surface->w/2 * surface->h);
        // data -= ( ((i / blockrowcount )) * (surface->w/2 * spriteheight)); // *blockrowcount));
        // //offset to read from
        // data -= (((i%blockrowcount)*spritewidth) - (surface->w/2 * (i/blockrowcount))); //data += (i * (iwidth)) - ((i / 2) * oddsize);
        // //data += ((blockrowcount - i) * iwidth) + ((i / 2) * oddsize);

        //tempsize = (surface->w/2 * surface->h);
        //tempsize -= ( ((i / blockrowcount )) * (surface->w/2 * spriteheight));
        //tempsize -= (((i%blockrowcount)*spritewidth) - (surface->w/2 * (i/blockrowcount)));
        //printf("initial offset=%i\n", tempsize);
        
        //read each looped line from offset position
        for(j = 0; j < spriteheight; j++){
            //testsub("j");
            surline = (j * surface->w/2);
            //unused suroffset = (i % blockrowcount) * (spritewidth/2);
            surfpos = surline; // + suroffset;

            imgline = (2 * i * blocksize) + (j * spritewidth);//iwidth);
            //imgline = ((i+1) * blocksize) - (j * iwidth);

            //read line into tempdata
            blockoffset = 0; //normal
            //blockoffset = blocksize; //reverse
            for(k = 0; k < (spritewidth*2); k++){ //k<spritewdith/2;k++)
            //for(k = (spritewidth*2)-1; k >= 0; k--){ 

                    if(k == spritewidth){
                        blockoffset += (blocksize - spritewidth); //normal
                        //blockoffset -= (blocksize + spritewidth); //reverse
                    }

                    val1 = *(data + (surfpos - (k/2))); //normal
                    //val1 = *(data - (surfpos + (k/2))); //reverse
                    pos = ((blockoffset + imgline) + k); //normal
                    //pos = ((blockoffset + imgline + spritewidth) + k); //reverse

                    if(k & 1){
                        //((Uint16)(val1&0xf));  //tempdata =;
                        //val1&=0xf; //normal
                        val1>>=4; //reverse
                    }
                    else{
                        //imgdata->data[pos] = ((Uint16)(val1>>4));
                        //val1>>=4; //normal
                        val1&=0xf;  //reverse
                    }
                    rgb1 = surface->format->palette->colors[(val1)];

                    if(val1 != transval) color = ((rgb1.b >> 3) << 11) | ((rgb1.g >> 3) <<6) | ((rgb1.r >> 3) <<1) | 1;
                    else color = 0;

                    //imgdata->data[ pos] = color;
                    if(pos < bmpsize){
                        imgdata->data[bmpsize - pos] = color;
                    }
                    
                    //if(k < spritewidth){
                    //    printf("i=%x j=%x k=%x sp=%i p=%i val=%x", i, j, k, tempsize - surfpos, pos, imgdata->data[pos]);
                     //   testsub("-");
                    //}
                    //tempsize++;

            }            
                    // printf("%i %x%x%x%x%x%x%x%x%x", imgline,
                    //     ((Uint8)(imgdata->data[(imgline + 0)])&0xf), //tempdata
                    //     ((Uint8)(imgdata->data[(imgline + 1)])&0xf),
                    //     ((Uint8)(imgdata->data[(imgline + 2)])&0xf),
                    //     ((Uint8)(imgdata->data[(imgline + 3)])&0xf),
                    //     ((Uint8)(imgdata->data[(imgline + 4)])&0xf),
                    //     ((Uint8)(imgdata->data[(imgline + 5)])&0xf),
                    //     ((Uint8)(imgdata->data[(imgline + 6)])&0xf),
                    //     ((Uint8)(imgdata->data[(imgline + 7)])&0xf),  
                    //     ((Uint8)(imgdata->data[(imgline + 8)])&0xf)    
                    // );
                    // testsub("-"); 
                }
 

     }

    // outsize = (surface->w * surface->h)/2;
    // for(i = 0; i < (outsize/spritewidth); i++){ //i < (bmpsize / iwidth); i++){
    //     surline = (i * (spritewidth)); //iwidth
    //     imgline = i * (spritewidth);

    //     // printf("%i %i %i %i %i %i %i %i %i %i", i*spritewidth,
    //     //     ((Uint8)(imgdata->data[(surline + 0)])), //tempdata
    //     //     ((Uint8)(imgdata->data[(surline + 1)])),
    //     //     ((Uint8)(imgdata->data[(surline + 2)])),
    //     //     ((Uint8)(imgdata->data[(surline + 3)])),
    //     //     ((Uint8)(imgdata->data[(surline + 4)])),
    //     //     ((Uint8)(imgdata->data[(surline + 5)])),
    //     //     ((Uint8)(imgdata->data[(surline + 6)])),
    //     //     ((Uint8)(imgdata->data[(surline + 7)])),  
    //     //        ((Uint8)(imgdata->data[(surline + 8)]))    
    //     // );
    //     // testsub("-");
        
    //     for(j = 0; j < spritewidth; j++){//j < (iwidth); j++){
    //         //testsub("SDL_CreateTextureFromSurface1a");
    //         val1 = imgdata->data[(surline + j)];
    //         // val1>>=4;
    //         // val2 = tempdata[(surline + j)];
    //         // val2 &=0xf;

    //         //printf("val=%x\n", (val));
    //         //testsub("SDL_CreateTextureFromSurface1b");
    //         rgb1 = surface->format->palette->colors[(val1)];
    //         //rgb2 = surface->format->palette->colors[(val2)];
    //         //testsub("SDL_CreateTextureFromSurface2");

    //         //0-3bits
    //         if(val1 != transval) color = ((rgb1.b >> 3) << 11) | ((rgb1.g >> 3) <<6) | ((rgb1.r >> 3) <<1) | 1;
    //         else color = 0;

    //         imgdata->data[((j) + imgline + 0)] = color;
    //         //testsub("SDL_CreateTextureFromSurface3");

    //         //4-8bits
    //         // if(val2 != transval) color = ((rgb2.b >> 3) << 11) | ((rgb2.g >> 3) <<6) | ((rgb2.r >> 3) <<1) | 1;
    //         // else color = 0;
    //         // imgdata->data[((2 * j) + imgline + 1)] = color;     
            
    //         // val = surface->data[size - (surline + j + 1)];
    //         // //printf("val=%x\n", (val));
    //         // //testsub("SDL_CreateTextureFromSurface1b");
    //         // rgb2 = surface->format->palette->colors[(val>>4)];
    //         // rgb1 = surface->format->palette->colors[(val&0xF)];
    //         // //testsub("SDL_CreateTextureFromSurface2");
    //         // color = ((rgb1.r >> 3) << 11) | ((rgb1.g >> 3) <<6) | ((rgb1.b >> 3) <<1) | 1;
    //         // imgdata->data[((2*(j + 1)) + imgline + 2)] = color;
    //         // //testsub("SDL_CreateTextureFromSurface3");
    //         // color = ((rgb2.r >> 3) << 11) | ((rgb2.g >> 3) <<6) | ((rgb2.b >> 3) <<1) | 1;
    //         // imgdata->data[((2*(j+1)) + imgline + 3)] = color;     
    //     }
    //         // printf("%i %x%x%x%x%x%x%x%x", i*iwidth,
    //         // ((imgdata->data[(imgline + 0)]&0xF)),
    //         // ((imgdata->data[(imgline + 1)]&0xF)),
    //         // ((imgdata->data[(imgline + 2)]&0xF)),
    //         // ((imgdata->data[(imgline + 3)]&0xF)),
    //         // ((imgdata->data[(imgline + 4)]&0xF)),
    //         // ((imgdata->data[(imgline + 5)]&0xF)),
    //         // ((imgdata->data[(imgline + 6)]&0xF)),
    //         // ((imgdata->data[(imgline + 7)]&0xF))        
    //         // );
    //         // testsub("-");

    // }

    testsub("SDL_CreateTiledTextureFromSurface_end");
    //free(tempdata);

    return imgdata;
}

// SDL_Texture* SDL_CreateTiledTextureFromSurface(SDL_Renderer* renderer, SDL_Surface* surface, int spritewidth, int spriteheight){
    
//     testsub("SDL_CreateTiledTextureFromSurface");

//     int i,j, size, surfacewidth, blocksize, blockrowcount;
//     Uint8 val1, val2, transval;

//     size = surface->w * surface->h * sizeof(Uint16);

//     SDL_Texture* imgdata = malloc(sizeof(SDL_Texture));
//     imgdata->data = malloc(size);
//     imgdata->w = spritewidth; //surface->w;
//     imgdata->h = surface->h * (surface->w / spritewidth);

//     surfacewidth = surface->size / (Uint32)surface->h;

//     blocksize = (spritewidth) * spriteheight;
//     blockrowcount = surface->w / spritewidth;

//     int surline, imgline;

//     size = (surface->w) * (surface->h);
//     Uint16 color;
//     RGBColour rgb1, rgb2;

//     transval=255;
//     for(i = 0; i < surface->format->palette->ncolors; i++){
//         rgb1 = surface->format->palette->colors[(i)];
//         color = ((rgb1.b >> 3) << 11) | ((rgb1.g >> 3) <<6) | ((rgb1.r >> 3) <<1) | 1;
//         printf("col=%i val=%x ", i, color);
//         testsub("-");
//         if(color==ColourKey){
//             transval = i;
//             break;
//         }
//     }

//     int pos;
//     Uint16* data = imgdata->data;
//     for(i = 0; i < surface->h-1; i++){ //(surface->h); i++){
//         surline = i * (surfacewidth);
//         imgline = ((i / spriteheight) * (blockrowcount)) + (i * spritewidth);
//         for(j = 0; j < surface->w-1; j++){
//             //testsub("SDL_CreateTextureFromSurface1a");
//             val1 = surface->data[(surline + j)];
//             val1>>=4;
//             val2 = surface->data[(surline + j)];
//             val2 &=0xf;
//             //printf("val=%x\n", (val));
//             //testsub("SDL_CreateTextureFromSurface1b");
//             rgb1 = surface->format->palette->colors[(val1)];
//             rgb2 = surface->format->palette->colors[(val2)];
//             //testsub("SDL_CreateTextureFromSurface2");
//             if(val1 != transval) {
//                 color = ((rgb1.b >> 3) << 11) | ((rgb1.g >> 3) <<6) | ((rgb1.r >> 3) <<1) | 1;
//             }
//             else {
//                 color = 0;
//             }
//             pos = size - (imgline + (((2*j) / spritewidth) * blocksize)+ ((2 * j) % spritewidth) + 0);
//             data[pos] = (Uint16)color;

//             //imgdata->data[size - ((2*j) + imgline + 0)] = color;
//             // printf("i=%i j=%i pos=%i val=%x", i, j, 
//             //     (imgline 
//             //     + (i * spritewidth)
//             //     + (((2*j) / spritewidth) * blocksize) 
//             //     + ((2*j) % spritewidth) 
//             //     + 0)
//             // , color);
//             // testsub("-");
//             printf("i=%i j=%i data=%x", i, j, color);
//             //testsub("SDL_CreateTextureFromSurface3");
//             if(val2 != transval){
//                 color = ((rgb2.b >> 3) << 11) | ((rgb2.g >> 3) <<6) | ((rgb2.r >> 3) <<1) | 1;
//             }
//             else{
//                color = 0; 
//             } 
//             pos = size - (imgline + (((2*j) / spritewidth) * blocksize) + ((2*j) % spritewidth)+ 1);
//             data[pos] = (Uint16)color;     
            
//             //imgdata->data[size - ((2*j) + imgline + 1)] = color;
            
//             // val = surface->data[size - (surline + j + 1)];
//             // //printf("val=%x\n", (val));
//             // //testsub("SDL_CreateTextureFromSurface1b");
//             // rgb2 = surface->format->palette->colors[(val>>4)];
//             // rgb1 = surface->format->palette->colors[(val&0xF)];
//             // //testsub("SDL_CreateTextureFromSurface2");
//             // color = ((rgb1.r >> 3) << 11) | ((rgb1.g >> 3) <<6) | ((rgb1.b >> 3) <<1) | 1;
//             // imgdata->data[((2*(j + 1)) + imgline + 2)] = color;
//             // //testsub("SDL_CreateTextureFromSurface3");
//             // color = ((rgb2.r >> 3) << 11) | ((rgb2.g >> 3) <<6) | ((rgb2.b >> 3) <<1) | 1;
//             // imgdata->data[((2*(j+1)) + imgline + 3)] = color;    

//             // printf("i=%i j=%i pos=%i val=%x", i, j, 
//             //     (imgline 
//             //     + (i * spritewidth)
//             //     + (((2*j) / spritewidth) * blocksize) 
//             //     + ((2*j) % spritewidth) 
//             //     + 1)
//             // , color);
//             // testsub("-");
//         }

//     }

//     // for(i = 0; i < surface->size; i++){

//     //         if(((2*i)+1) > (surface->w * surface->h)) return imgdata;

//     //         val = surface->data[surface->size - (i)];
//     //         //printf("val=%x\n", (val));
//     //         //testsub("SDL_CreateTextureFromSurface1b");
//     //         rgb2 = surface->format->palette->colors[(val>>4)];
//     //         rgb1 = surface->format->palette->colors[(val&0xF)];
//     //         //testsub("SDL_CreateTextureFromSurface2");
//     //         color = ((rgb1.r >> 3) << 11) | ((rgb1.g >> 3) <<6) | ((rgb1.b >> 3) <<1) | 1;
//     //         imgdata->data[((2*i) + 0)] = color;
//     //         //testsub("SDL_CreateTextureFromSurface3");
//     //         color = ((rgb2.r >> 3) << 11) | ((rgb2.g >> 3) <<6) | ((rgb2.b >> 3) <<1) | 1;
//     //         imgdata->data[((2*i) + 1)] = color;   
//     // }

// 	 for(i = 0; i < (spritewidth * spriteheight); i++){
// 		printf("tex pos=%x val=%x", i, imgdata->data[i]);
// 		testsub("-");
// 	 }

//     testsub("SDL_CreateTextureFromSurface_end");

//     return imgdata;
// }

void SDL_SetTextureColorMod(){

}

void SDL_UpdateTexture(SDL_Texture* piDIB , SDL_Rect* rect, Uint16* buffer, int width ){
    testsub("SDL_UpdateTexture");
   
    int x,y;
    Uint16 val;
    int actwidth;
    int actheight;

    if(rect){
        actwidth = rect->w;
        actheight = rect->h;
    }
    else{
        actwidth = width / 2;
        actheight = 240;
    }
    sprintf(test, "width=%x height=%x", actwidth, actheight);
    testsub(test);
    for(y = 0; y < actheight; y++){
        for(x = 0; x < actwidth; x++){
            //testsub("test1");
            val = buffer[(y*actwidth) + x];
            //testsub("test2");
            piDIB->data[(y*actwidth)+ x] = val;
            //testsub("test3");
        }
    }
    testsub("SDL_UpdateTexture end");
}

void SDL_UpdateTexture2(Uint16* piDIB , SDL_Rect* rect, Uint16* buffer, int width ){
    testsub("SDL_UpdateTexture");
   
    int x,y;
    Uint16 val;
    int actwidth;
    int actheight;

    if(rect){
        actwidth = rect->w;
        actheight = rect->h;
    }
    else{
        actwidth = width / 2;
        actheight = 240;
    }
    sprintf(test, "width=%x height=%x", actwidth, actheight);
    testsub(test);
    for(y = 0; y < actheight; y++){
        for(x = 0; x < actwidth; x++){
            //testsub("test1");
            val = buffer[(y*actwidth) + x];
            //testsub("test2");
            piDIB[(y*actwidth)+ x] = val;
            //testsub("test3");
        }
    }
    testsub("SDL_UpdateTexture end");
}

void SDL_DestroyTexture(SDL_Texture* tex){
    //assert(tex);
   //testsub("SDL_DestroyTexture");
    if(tex != NULL){
        free(tex->data);
        free(tex);        
    }

}

void SDL_FreeSurface(SDL_Surface* Surface){

    testsub("SDL_FreeSurface");
    //assert(Surface);
    free(Surface->format->palette->colors);
    free(Surface->format->palette);
    free(Surface->format);
    free(Surface->data);
    free(Surface);

    freedata(sizeof(Surface));
    freedata(sizeof(SDL_PixelFormat));
    freedata(sizeof(SDL_Palette));
    testsub("SDL_FreeSurface _end");
}

//SDL Files
SDL_Surface* SDL_LoadBMP_RW(SDL_RWops handle , int bol){

    sprintf(test, "SDL_LoadBMP_RW=%i", handle);
    testsub(test);

    SDL_Surface* surface = SDL_malloc(sizeof(SDL_Surface));
    Uint8* data;
    int i, size, ncolors;
    Uint8 r, g, b;
    Uint32 paletteoffset, dataoffset, val;


    dfs_seek(handle, 0xA, SEEK_SET);
    dfs_read(&dataoffset, sizeof(Uint32), 1, handle);
    dataoffset = SDL_SwapLE32(dataoffset);

    dfs_seek(handle, 0xE, SEEK_SET);
    dfs_read(&paletteoffset, sizeof(Uint32), 1, handle);
    dfs_seek(handle, 0x12, SEEK_SET);
    dfs_read(&val, sizeof(Uint32), 1, handle);
    surface->w = SDL_SwapLE32(val);

    dfs_seek(handle, 0x16, SEEK_SET);
    dfs_read(&val, sizeof(Uint32), 1, handle);
    surface->h = SDL_SwapLE32(val);

    dfs_seek(handle, 0x1A, SEEK_SET);
    dfs_read(&surface->compression, sizeof(Uint8), 1, handle);
    dfs_seek(handle, 0x1C, SEEK_SET);
    dfs_read(&surface->bitsperpixel, sizeof(Uint8), 1, handle);
    
    dfs_seek(handle, 0x22, SEEK_SET);
    dfs_read(&val, sizeof(Uint32), 1, handle);
    surface->size = SDL_SwapLE32(val);
    printf("size=%x", surface->size);
    testsub("-testval-");

    testsub("SDL_LoadBMP_RW1");
    surface->format = SDL_malloc(sizeof(SDL_PixelFormat));
    testsub("SDL_LoadBMP_RW2");
    surface->format->palette = SDL_malloc(sizeof(SDL_Palette));
    testsub("SDL_LoadBMP_RW3");

    //size = (surface->w/2 + (surface->w&1)) * surface->h;
    
    dfs_seek(handle, 0x32, SEEK_SET);
    dfs_read(&val, sizeof(Uint32), 1, handle);
    if(val!=0) surface->format->palette->ncolors = SDL_SwapLE32(val);
    else surface->format->palette->ncolors = 0XF;

    
    surface->format->palette->colors = malloc(surface->format->palette->ncolors*4);
    dfs_seek(handle, 0x36, SEEK_SET);
    dfs_read(surface->format->palette->colors, (surface->format->palette->ncolors * 4), 1, handle);
    

    //size = surface->w/2 * surface->h;
    surface->data = SDL_malloc(surface->size); //24 bit
    
    dfs_seek(handle, dataoffset, SEEK_SET);
    dfs_read(surface->data, 1, (surface->size), handle);
    //surface->data = data;

    printf("width=%i height=%i\n size=%x ncol=%x\n", surface->w, surface->h, surface->size, surface->format->palette->ncolors);

 testsub("SDL_LoadBMP_RW_end");
    dfs_close(handle);

    return surface;
}

SDL_Surface* SDL_Loadrgb_RW(SDL_RWops handle , int bol){

    sprintf(test, "SDL_LoadBMP_RW=%i\n", handle);
    testsub(test);

    SDL_Surface* surface = SDL_malloc(sizeof(SDL_Surface));
    Uint32* data;
    int i, size;
    Uint8 r, g, b;
    Uint16 val;

    size = surface->w * surface->h;
    
    dfs_seek(handle, 6, SEEK_SET);
    dfs_read(&surface->w, sizeof(short), 1, handle);
    dfs_seek(handle, 8, SEEK_SET);
    dfs_read(&surface->h, sizeof(short), 1, handle);

    data = SDL_malloc(size * 3); //24 bit
    dfs_seek(handle, 0x200, SEEK_SET);
    dfs_read(data, (size * 3), 1, handle);
    
    surface->format = SDL_malloc(sizeof(SDL_PixelFormat));//SDL_PIXELFORMAT_RGB565;

    surface->data = SDL_malloc(surface->w * surface->h * sizeof(Uint16)); //16 bit 5551
    for(i = 0; i > (surface->w * surface->h); i++){
        r = ((data[(0*size) + i])>>3);
        g = ((data[(1*size) + i])>>3);
        b = ((data[(2*size) + i])>>3);
        val = (r <<11) | (g << 6) | (b << 1) | 1;
        surface->data[size - i] = val;
    }
    openfiles--;
    dfs_close(handle);
    free(data);

    freedata(sizeof(SDL_Surface));
    freedata(size * 3);
    freedata(surface->w * surface->h * sizeof(Uint16));
    return surface;
}

SDL_RWops SDL_RWFromFile(const char* filename, char* setting){
    //char path[30];
    //sprintf(path, "rom://%s", filename);
    return dfs_open(filename);
}

Uint8* SDL_RWFromMem(Uint8* data, int size){
    return NULL;
}

void SDL_RWread(Uint8* buf, Uint8* ret, int size, int count){
    //dfs_read(buf, size, count, handle);
    //int i;
    memcpy(ret, buf, size);
    // for(i = 0; i < size; i++){
    //     *(ret + i) = *(buf + i);
    // }

}

void SDL_RWseek(SDL_RWops handle, int Offset, int Origin ){
    dfs_seek(handle, Offset, Origin);
    return;
}

void SDL_RWwrite(SDL_RWops* handle, Uint8* data, int size, int count){
    //testsub("SDL_RWwrite");
    int i;
    for(i = 0; i < size; i++){
        *(handle + i) = *(data + i);
        // sprintf(test, "write i=%x size=%x in=%x out=%x",
        //     i, size, *(data + i), *(handle + i));
        // testsub(test);
    }
    handle += size;
}

int SDL_RWsize(SDL_RWops handle){
    return dfs_size(handle);
}

void SDL_RWclose(SDL_RWops handle){
    openfiles--;
    dfs_close(handle);
}

//SDL Console
void SDL_SetHint(int option, const char* setting){

}
void SDL_ShowMessageBox(void* msgbox, void* ptr){

}

char* SDL_GetError(){
    //return CurError;
}

void SDL_Log(const char* str, int id){
    printf("\n");
    printf(str);
    return;
}

//SDL Controller
int SDL_arraysize(int* buttons){

}

Uint8* SDL_GetKeyboardState(void* keyboard){
    testsub("SDL_GetKeyboardState");
    return NULL;
}

char* SDL_GetScancodeName(int scancode){

}

void SDL_SetRelativeMouseMode(int bol){

}

int SDL_GetMouseState(void* mouse, void* ptr){
    testsub("SDL_GetMouseState");
    return 0;
}

void SDL_GetRelativeMouseState(int* x, int* y){

}

int SDL_JoystickOpen(int i){
    return 0;
}

int SDL_NumJoysticks(){
    return 1;
}

int SDL_JoystickIsHaptic(int Joystick){
    return 0;
}

int SDL_JoystickGetButton(Uint8* JoyStick, int i){
    
}

int SDL_JoystickGetAxis(Uint8* JoyStick, int i){

}

int SDL_JoystickNumAxes(Uint8* JoyStick){

}

void SDL_JoystickClose(int joystick){
    
}

int SDL_IsGameController(int i){
    return 1;
}

SDL_GameController* SDL_GameControllerOpen(int i){
    SDL_GameController* control = SDL_malloc(1);
    return control;
}

int SDL_GameControllerGetButton(Uint8* Controller, int i){
    int retval = SDL_FALSE;
    switch (i){
        case SDL_CONTROLLER_BUTTON_A:
            if(Pressed1.A) retval = SDL_TRUE;
            break;
        case SDL_CONTROLLER_BUTTON_B:
            if(Pressed1.B) retval = SDL_TRUE;
            break;
        case SDL_CONTROLLER_BUTTON_START:
            if(Pressed1.start) retval = SDL_TRUE;
            break;
        case SDL_CONTROLLER_BUTTON_DPAD_UP:
            if(Pressed1.up || Pressed1.C_up) retval = SDL_TRUE;
            break;
        case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
        if(Pressed1.down || Pressed1.C_down) retval = SDL_TRUE;
            break;
        case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
        if(Pressed1.left) retval = SDL_TRUE;
            break;
        case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
            if(Pressed1.right) retval = SDL_TRUE;
          break;
        case SDL_CONTROLLER_BUTTON_BACK:
            if(Pressed1.Z) retval = SDL_TRUE;
        case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:
            if(Down1.L) {
                retval = SDL_TRUE;
                //if(Down1.Z) consolevisible(true);
            }
        case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
            if(Pressed1.R) retval = SDL_TRUE;
            
    }

    return retval;

}

int SDL_GameControllerGetAxis(Uint8* Controller, int i){
    //int retval = SDL_FALSE;
    switch (i){
        case SDL_CONTROLLER_AXIS_LEFTY:
            return -(Pressed1.y<<8); //64 << 9 = 32768
            break;
        case SDL_CONTROLLER_AXIS_LEFTX:
            return (Pressed1.x<<8);
            break;
        // case SDL_CONTROLLER_BUTTON_START:
        //     if(Down1.start) retval = SDL_TRUE;
        //     break;
        // case SDL_CONTROLLER_BUTTON_DPAD_UP:
        //     if(Down1.up || Down1.C_up) retval = SDL_TRUE;
        //     break; 
    }
    return 0;
}

int SDL_GameControllerHasRumble(int i){
    return 0;
}

void SDL_GameControllerRumble(int id, Uint16 val1, Uint16 val2, Uint16 val3){
    get_accessories_present();
    if(identify_accessory(0) == ACCESSORY_RUMBLEPAK){
        rumble_start(0);
        rumbleEnd = (SDL_GetTicks() + val3) % MAXTICKS;
    }
    
}

void SDL_GameControllerClose(int controller){
    
}

int SDL_HapticRumbleInit(int Joystick){
    return 0;
}

int SDL_HapticOpenFromJoystick(int Joystick){

}

void SDL_HapticRumblePlay(int haptic, Uint16 val1, Uint16 val2){

}

void SDL_HapticRumbleStop(int haptic){

}

void SDL_HapticClose(int haptic){

}

void Draw_CircleButton(int x, int y, int colour){
    CurColour = colour;
    SDL_RenderDrawLine(NULL, x + 3, y, x+ 4, y);
    SDL_RenderDrawLine(NULL, x + 2, y, x+ 5, y);
    SDL_RenderDrawLine(NULL, x + 1, y, x+ 6, y);
    SDL_RenderDrawLine(NULL, x + 7, y, x+ 0, y);

    SDL_RenderDrawLine(NULL, x + 7, y, x+ 0, y);
    SDL_RenderDrawLine(NULL, x + 1, y, x+ 6, y);
    SDL_RenderDrawLine(NULL, x + 2, y, x+ 5, y);
    SDL_RenderDrawLine(NULL, x + 3, y, x+ 4, y);
}

void Draw_SquareButton(int x, int y, int colour){
    CurColour = colour;
    rdp_draw_filled_rectangle(x, y, x+6, y+8);

}

void Draw_CButton(int x, int y, int dir){
    Draw_CircleButton(x, y, 0xFFFF00);
    CurColour = 0;
    switch(dir){
        case 1: //up
            SDL_RenderDrawLine(NULL, x+2, y+4, x+3, y+2);
            SDL_RenderDrawLine(NULL, x+6, y+4, x+5, y+2);
        break;
        case 2: //right
            SDL_RenderDrawLine(NULL, x+2, y+4, x+3, y+2);
            SDL_RenderDrawLine(NULL, x+6, y+4, x+5, y+2);
        break;
        case 3: //down
            SDL_RenderDrawLine(NULL, x+3, y+4, x+2, y+2);
            SDL_RenderDrawLine(NULL, x+5, y+4, x+6, y+2);
        break;
        case 4: //left
            SDL_RenderDrawLine(NULL, x+2, y+4, x+3, y+2);
            SDL_RenderDrawLine(NULL, x+6, y+4, x+5, y+2);
        break;
    }
}
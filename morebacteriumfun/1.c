/* More Bacterium Fun - Artificial Life Experiments
 * Copyright (c) 2003 John Leach <john@johnleach.co.uk>
 * This program is released under the terms of the GNU GPL version 2
 * see http://johnleach.co.uk/documents/alife for more details */

#include "SDL.h"
#include <stdio.h>
#include <stdlib.h>

#undef RAND_MAX
#define RAND_MAX	1024*100
#define SW	300
#define SH	300

struct cell {
	Uint32 gen,colour;
	Uint32 x,y;
};


int main() {
	SDL_Surface *screen;
	Uint32 x,y,nx,ny,b;
	Uint32 red,green,blue;
	SDL_Rect rect;
	Uint32 bact[16] = {0,1,4,8,12,16,20,24,28,32,36,40,44,48,52,56};
	struct cell petri[SH*SW];
	SDL_Event event;
	int quit = 0;

	
	srand(time(0));
	
	printf("Initialising SDL...\n");
	/* Initialize defaults, Video and Audio */
	if((SDL_Init(SDL_INIT_VIDEO)==-1)) { 
		printf("Could not initialize SDL: %s.\n", SDL_GetError());
		exit(-1);
	}

	atexit(SDL_Quit);


	screen = SDL_SetVideoMode(SW, SH, 8, SDL_SWSURFACE);
	red = SDL_MapRGB(screen->format, 0xff, 0x00, 0x00);
	//green = SDL_MapRGB(screen->format,0x00,0xff, 0x00);
	//blue = SDL_MapRGB(screen->format,0x00,0x00,0xff);
	SDL_WM_SetCaption("Life v0.1 - John Leach <john@johnleach.co.uk>","");

	if ( screen == NULL ) {
		fprintf(stderr, "Couldn't set 640x480x8 video mode: %s\n",SDL_GetError());
		exit(1);
	}

	for (b=0;b<SH*SW;b++)  {
		petri[b].colour=0;
		petri[b].gen=0;
		if (rand()%1024==5) {
			petri[b].colour=red;
			petri[b].gen=5;
			//petri[b].x = (b%SW);
			//petri[b].y = b/SH;
		}
		//if (rand()%1024==0) petri[sy][sx] = SDL_MapRGB(screen->format,0,rand()%200,0);
		//if (rand()%1024==0) petri[sy][sx] = SDL_MapRGB(screen->format,0,0,rand()%200);
	}
	

	rect.w = 1;
	rect.h = 1;
	while (!quit) {
		// Keypress check
		SDL_PollEvent( &event );
		switch( event.type ) {
			case SDL_KEYDOWN:
				if (event.key.keysym.sym == SDLK_ESCAPE)
					quit = 1;
		}
		// Growth loop
		for (b=0;b<SH*SW;b++) {
				//printf("%i,",rand()%100);
				/*if (petri[sy][sx]!=0) {
					nsy = sy+(-2+(rand()%4)) % SH;
					nsx = sx+(-2+(rand()%4)) % SW;
					//if (petri[nsy][nsx]==0) petri[nsy][nsx] = bact[1];
					petri[sy][sx]++;
				} */
				if (petri[b].gen>0) {
					x = b%SW;
					y = b/SH;
					// Age
					petri[b].gen++;
					//	printf("%i.gen = %i\n",b,petri[b].gen);
					
					// Spawn
					if (petri[b].gen>5) {
						ny = y+(-2+(rand()%4)) % SH;
						nx = x+(-2+(rand()%4)) % SW;					
						petri[((ny*SW)+nx)%(SH*SW)].gen = 1;
						petri[((ny*SW)+nx)%(SH*SW)].colour = red;
					}
					//petri[b].x++;
					// Draw
					if (petri[b].gen>1) {
						rect.x = x;
						rect.y = y;
						SDL_FillRect(screen,&rect,petri[b].colour);
					}
				}
		}
		SDL_UpdateRect(screen, 0, 0, 0, 0);
		//printf("loop\n");
	}
	
}

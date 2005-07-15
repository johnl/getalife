/* Bacterium Fun - Artificial Life Experiments
 * Copyright (c) 2003 John Leach
 * This program is released under the terms of the GNU GPL version 2
 * see http://www.johnleach.co.uk/documents/alife for more details */
#include "SDL/SDL.h"
#include <stdio.h>

#define RAND_MAX	1024*100
#define SW	200
#define SH	200

#define GLOBAL_MAX_AGE	12

struct cell {
	Uint32 gen,age,maxage,colour;
	Uint32 x,y;
};


int main() {
	SDL_Surface *screen;
	Uint32 x,y,nx,ny,b,nb;
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


	screen = SDL_SetVideoMode(SW, SH, 16, SDL_SWSURFACE);
	red = SDL_MapRGB(screen->format, 5+rand()%24, 0x00, 0x00);
	green = SDL_MapRGB(screen->format,0x00,5+rand()%24, 0x00);
	blue = SDL_MapRGB(screen->format,0x00,0x00,5+rand()%24);
	printf("%i,%i,%i",red,green,blue);
	SDL_WM_SetCaption("Life v0.2 - John Leach <john@ecsc.co.uk>","");

	if ( screen == NULL ) {
		fprintf(stderr, "Couldn't set 640x480x8 video mode: %s\n",SDL_GetError());
		exit(1);
	}

	for (b=0;b<SH*SW;b++)  {
		petri[b].colour=0;
		petri[b].gen=0;
		if (rand()%2048==0) {
			x = rand()%3;
			if (x==0) petri[b].colour=red;
			if (x==1) petri[b].colour=blue;
			if (x==2) petri[b].colour=green;
			petri[b].gen=1;
			petri[b].maxage = 1+(rand()%GLOBAL_MAX_AGE);
		}
	}
			

	rect.w = 1;
	rect.h = 1;
	while (!quit) {
		// Keypress check
		SDL_PollEvent( &event );
		switch( event.type ) {
			case SDL_QUIT:
				quit = 1;
				break;
			case SDL_KEYDOWN:
				if (event.key.keysym.sym == SDLK_ESCAPE)
					quit = 1;
		}
		// Growth loop
		for (b=0;b<SH*SW;b++) {
				
				if (petri[b].gen>0) {
					if (petri[b].age>0) petri[b].age++;
					x = b%SW;
					y = b/SH;
					// Age
					if (petri[b].gen<6) petri[b].gen++;
					//	printf("%i.gen = %i\n",b,petri[b].gen);
					
					// Spawn
					if (petri[b].gen>4) {
						ny = y+(-2+(rand()%5)) % SH;
						nx = x+(-2+(rand()%5)) % SW;
						nb = ((ny*SW)+nx)%(SH*SW);
						if (petri[nb].colour!=petri[b].colour || petri[nb].gen<4) {
							petri[nb].gen = 1;
							petri[nb].colour = petri[b].colour;
						}
					}
					//petri[b].x++;
					// Draw
					if (petri[b].gen>1) {
						rect.x = x;
						rect.y = y;
						SDL_FillRect(screen,&rect,petri[b].colour * (petri[b].gen*2));
					}
				}
		}
		SDL_UpdateRect(screen, 0, 0, 0, 0);
		//printf("loop\n");
	}
	
}

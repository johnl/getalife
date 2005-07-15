/* More Bacterium Fun - Artificial Life Experiments
 * Copyright (c) 2003 John Leach <john@johnleach.co.uk>
 * This program is released under the terms of the GNU GPL version 2
 * see http://johnleach.co.uk/documents/alife for more details */

#include "SDL.h"
#include <stdio.h>
#include <stdlib.h>

#undef RAND_MAX
#define RAND_MAX	1024*100
#define SW	220
#define SH	220

#define GLOBAL_MAX_AGE	512

struct cell {
	Uint32 gen,age,maxage,growrange,colour,max_findtarget_time;
	Sint32 attacking;
};


int main() {
	SDL_Surface *screen;
	Uint32 x,y,nx,ny,b,nb,i;
	Uint32 red,green,blue,dead;
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


	screen = SDL_SetVideoMode(SW*2, SH*2, 16, SDL_HWSURFACE);
	red = SDL_MapRGB(screen->format, 10, 0x00, 0x00);
	green = SDL_MapRGB(screen->format,0x00,10, 0x00);
	blue = SDL_MapRGB(screen->format,0x00,0x00,10);
	dead = SDL_MapRGB(screen->format,0x00,0x00,0x00);
	//printf("%i,%i,%i",red,green,blue);
	SDL_WM_SetCaption("Life v0.3 - John Leach <john@johnleach.co.uk>","");

	if ( screen == NULL ) {
		fprintf(stderr, "Couldn't set 640x480x8 video mode: %s\n",SDL_GetError());
		exit(1);
	}

	// Initial splurge
	for (b=0;b<=SH*SW;b++)  {
		petri[b].colour=dead;
		petri[b].gen=0;
		petri[b].age=0;
		petri[b].attacking = -1;
		petri[b].max_findtarget_time = 0;
		if (rand()%((SW*SH) / 20)==0) {
			x = rand()%3;
			if (x==0) petri[b].colour=red;
			if (x==1) petri[b].colour=blue;
			if (x==2) petri[b].colour=green;
			petri[b].gen=1;
			petri[b].age=1;
			petri[b].maxage = (rand()%GLOBAL_MAX_AGE);
			petri[b].growrange = (1+rand()%5);
			petri[b].max_findtarget_time = 18;
		}
	}
			
	rect.w = 2;
	rect.h = 2;
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
				x = b%SW;
				y = b/SW;
				
				if (petri[b].gen>0) {
					// Age
					petri[b].age++;
					if (petri[b].gen<15) petri[b].gen++;
					
					// Spores
					if (petri[b].gen>5) {
						if (petri[b].attacking==-1) {
							//printf("attack finding...\n");
							i=0;
							do {
								ny = (y+( (petri[b].growrange*-1) + (rand()%((petri[b].growrange*2)+1)))) % SH;
								nx = (x+( (petri[b].growrange*-1) + (rand()%((petri[b].growrange*2)+1)))) % SW;
								nb = (nx + (ny*SW));
								petri[b].attacking = nb;
								i++;
							} while (petri[nb].colour==petri[b].colour && i<petri[b].max_findtarget_time);
							if (i>=petri[b].max_findtarget_time) petri[b].attacking = -1;
						} else { 
							nb = petri[b].attacking;
							if (petri[nb].gen>1) petri[nb].gen--; else {
							petri[nb].gen = 1;
							petri[nb].colour = petri[b].colour;
							petri[nb].maxage = petri[b].maxage;
							petri[nb].growrange = petri[b].growrange;
							petri[nb].max_findtarget_time = petri[b].max_findtarget_time;
							petri[nb].attacking = -1;
							petri[b].attacking = -1;
							}
						}
					}
					
					// Old Age Mode
					if (petri[b].age>=petri[b].maxage) {
						//petri[b].colour=dead;
						petri[b].gen--;
						petri[b].gen--;
						//petri[b].age=0;
					}
					
					// Grim Reaper
					if (petri[b].gen==0) {
						petri[b].colour=dead;
						petri[b].age=0;
					}
				}
				
				// Draw
				rect.x = x*2;
				rect.y = y*2;
				SDL_FillRect(screen,&rect,( (petri[b].gen*petri[b].colour) + (petri[b].colour*(petri[b].maxage/64))));

		}
		SDL_UpdateRect(screen, 0, 0, 0, 0);
		//printf("loop\n");
	}
	
}

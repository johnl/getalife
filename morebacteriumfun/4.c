/* More Bacterium Fun - Artificial Life Experiments
 * Copyright (c) 2003 John Leach <john@johnleach.co.uk>
 * This program is released under the terms of the GNU GPL version 2
 * see http://johnleach.co.uk/documents/alife for more details */

#include "SDL.h"
#include <stdio.h>
#include <stdlib.h>

// Screen width and screen height (window actually)
#define SW	220
#define SH	220

#define GLOBAL_MAX_AGE	512

// The cell itself!
struct cell {
	Sint32 	gen;		// generation (each cell goes through 15)
	Uint32	age,		// age in loops
			maxage,		// age before cell starts to die
			growrange,	// max distance cell can spore
			colour,		// base colour
			killpower,	// how many gens this cell can deduct off others per cycle
			initialgen;	// how old these guys are born at
	char	cstate[64];	// unused data, so mutations don't always do stuff
};			

// gen a random number
int rn(int min,int max) {
	return (int) min + rand() % (max+1);
}

// mutate the petri memory randomly
void mutate(unsigned char *bytes, size_t sze) {
	struct cell tcell;
	Uint32 a,b,c;
	a = rn(0,SW*SH*sizeof(tcell));
	b = rn(0,254);
	//printf("Mutating byte %i into value %i\n",a,b);
	bytes[a] = b;
}
	
int main() {
	SDL_Surface *screen;			// main surface
	Uint32 x,y,nx,ny,b,nb,i,m;		// temporary variables
	Uint32 red,green,blue,dead;		// base colour values
	SDL_Rect rect;					// rect used to drawing pixels
	struct cell petri[SH*SW];		// the petri dish
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

	// Make screen surface 2x size of petri dish (zoom!)
	screen = SDL_SetVideoMode(SW*2, SH*2, 16, SDL_HWSURFACE);
	if ( screen == NULL ) {
		fprintf(stderr, "Couldn't set video mode: %s\n",SDL_GetError());
		exit(1);
	}
	
	// Set up colours
	red = SDL_MapRGB(screen->format, 10, 0x00, 0x00);
	green = SDL_MapRGB(screen->format,0x00,10, 0x00);
	blue = SDL_MapRGB(screen->format,0x00,0x00,10);
	dead = SDL_MapRGB(screen->format,0x00,0x00,0x00);
	
	// Kudos
	SDL_WM_SetCaption("Life v0.4 - John Leach <john@johnleach.co.uk>","");	

	// Big bang
	for (b=0;b<=SH*SW;b++)  {
		// default cells, dead
		petri[b].colour=0;//rn(0,999999);
		petri[b].gen=0;
		petri[b].age=0;
		petri[b].killpower=2;
		petri[b].initialgen=1;
		petri[b].maxage = rn(1,GLOBAL_MAX_AGE*3);
		
		// Create some random live cells!
		if (rn(0,(SW*SH)/20)==0) {
			x = rn(0,2);
			if (x==0) petri[b].colour=red;
			if (x==1) petri[b].colour=blue;
			if (x==2) petri[b].colour=green;
			petri[b].gen=1;
			petri[b].age=1;
			petri[b].maxage = rn(1,GLOBAL_MAX_AGE);
			petri[b].growrange = rn(0,5);
		}
	}
	
	// Pixels are 2 pixels wide now (zoom!)
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
				// Work out co-ordinates from cell number
				x = b%SW;
				y = b/SW;
				
				// Process only live cells
				if (petri[b].gen>0) {
					// Age
					petri[b].age++;
					if (petri[b].gen<15) petri[b].gen++;
					
					// Spores
					if (petri[b].gen>5) {
						// Choose a new cell to spore into
						ny = (y+( (petri[b].growrange*-1) + (rand()%((petri[b].growrange*2)+1))));
						nx = (x+( (petri[b].growrange*-1) + (rand()%((petri[b].growrange*2)+1))));
						if (ny>=SH) ny = 0;
						if (nx>=SW) nx = 0;
						nb = (nx + (ny*SW));
						// Make sure we can spore there
						if (petri[nb].colour!=petri[b].colour)
						if (petri[nb].gen>1) petri[nb].gen=petri[nb].gen-petri[b].killpower; else {
							// Divide!  - should really copy the memory here as we get more variables
							petri[nb].gen = petri[b].initialgen;
							petri[nb].colour = petri[b].colour;
							petri[nb].maxage = petri[b].maxage;
							petri[nb].growrange = petri[b].growrange;
							petri[nb].initialgen = petri[b].initialgen;
							petri[nb].killpower = petri[b].killpower;
						}
					}
					
					// Old Age Mode
					if (petri[b].age>=petri[b].maxage) {
						petri[b].gen--;
						petri[b].gen--;
					}
					
					// Grim Reaper
					if (petri[b].gen<=0) {
						petri[b].colour=dead;
						petri[b].age=0;
						petri[b].gen=0;
					}
					
					// Mutation
					if (rn(0,SH*SW*100)==0) {
						m = rn(0,100);
						printf("%i: Mutating %i particles\n",time(),m);
						for (i=0;i<m;i++) mutate((unsigned char*) petri, sizeof(petri));
						}
				}
				
				// Draw
				rect.x = (x*2);
				rect.y = (y*2);
				// Colour depends on root colour and generation.
				// A tiny random variance that affects nothing other than display makes it look all crawly and alive :)
				SDL_FillRect(screen,&rect,( (petri[b].gen*petri[b].colour) + (petri[b].colour*(petri[b].maxage/64)) + (rn(0,1)*petri[b].colour)));

		}
		SDL_UpdateRect(screen, 0, 0, 0, 0);
		//printf("loop\n");
	}
	
}

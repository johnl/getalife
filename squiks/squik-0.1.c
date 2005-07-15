/* It's a squiks life - Artificial Life Experiments
 * Copyright (c) 2003 John Leach
 * This program is released under the terms of the GNU GPL version 2
 * see http://www.johnleach.co.uk/documents/alife for more details */

#include <SDL/SDL.h>
#include <stdio.h>
#include <stdlib.h>

#define SCREEN_HEIGHT	300
#define SCREEN_WIDTH	500
#define PRE_SQUIKS	10


struct tsquik {
	unsigned int	age;  /* How many loops old this squink is */
	int		x,y,tx,ty,ax,ay,amx,amy;
	Uint32		colour;
	struct tsquik	*previous,*next;
};

// gen a random number
int rn(int min,int max) {
        return (int) min + rand() % (max+1);
}	

int main() {
	SDL_Surface	*screen;
	SDL_Rect	r;
	SDL_Event	event;
	struct tsquik	squik,*psquik,*nsquik,*fsquik,*newsquik,*tempsquik;
	int		quit = 0;
	unsigned int	i;

	srand(time(0));

	printf("Initialising SDL...");
	/* Initialize defaults, Video and Audio */
	if((SDL_Init(SDL_INIT_VIDEO)==-1)) { 
		printf("Error: %s.\n", SDL_GetError());
		exit(-1);
	} else printf("Done.\n");

	atexit(SDL_Quit);
	screen = SDL_SetVideoMode(SCREEN_WIDTH * 2, SCREEN_HEIGHT * 2, 16, SDL_HWSURFACE);
	if ( screen == NULL ) {
		printf("Couldn't set video mode: %s\n",SDL_GetError());
		exit(1);
	}
	
	// Kudos
	SDL_WM_SetCaption("It's a Squiks life v0.1 - John Leach <john@johnleach.co.uk>","");

	printf("Initialising %i squiks...",PRE_SQUIKS);
	// Initialise the first squiks
	psquik = NULL;
	for (i=0;i<=PRE_SQUIKS;i++) {
		printf(".");
		nsquik = malloc(sizeof(struct tsquik));
		if (psquik!=NULL) {
			psquik->next = nsquik;
			nsquik->previous = psquik;
		} else fsquik = nsquik;
		nsquik->age=0;
		nsquik->ax=0;
		nsquik->ay=0;
		nsquik->x=rn(0,SCREEN_WIDTH);
		nsquik->y=rn(0,SCREEN_HEIGHT);
		nsquik->tx = nsquik->x + rn(-32,32);
		nsquik->ty = nsquik->y + rn(-32,32);
		nsquik->colour = SDL_MapRGB(screen->format,rn(100,255),rn(100,255),rn(100,255));
		psquik=nsquik;
	}
	nsquik->next=fsquik;
	fsquik->previous = nsquik;
	printf("Done.\n");

	printf("Starting loop.\n");
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
		fsquik=nsquik->previous;
		nsquik=nsquik->next;
		while (nsquik->previous!=fsquik) {
			nsquik->age++;

			if (nsquik->x < nsquik->tx) nsquik->ax++;
			if (nsquik->x > nsquik->tx) nsquik->ax--;
			if (nsquik->y < nsquik->ty) nsquik->ay++;
			if (nsquik->y > nsquik->ty) nsquik->ay--;

			if (nsquik->ax > 4) nsquik->ax=4;
			if (nsquik->ax < -4) nsquik->ax=-4;
			if (nsquik->ay > 4) nsquik->ay=4;
			if (nsquik->ay < -4) nsquik->ay=-4;
			
			nsquik->x = nsquik->x + nsquik->ax;
			nsquik->y = nsquik->y + nsquik->ay;

			// Draw the squik
			r.x = nsquik->x*4;
			r.y = nsquik->y*2;
			r.w = 4;
			r.h = 4;
			SDL_FillRect(screen,&r,nsquik->colour);

			if (rn(0,5)==1) {
				nsquik->tx + rn(-32,32);
				nsquik->ty + rn(-32,32);
			}
			if (rn(0,100)==1) {
				nsquik->tx = rn(0,SCREEN_WIDTH);
				nsquik->ty = rn(0,SCREEN_HEIGHT);
			}
			if (rn(0,1000)==1) {
				newsquik = malloc(sizeof(struct tsquik));
				memcpy(newsquik,nsquik,sizeof(struct tsquik));
				newsquik->x + rn (-16,16);
				newsquik->y + rn (-16,16);
				newsquik->age=0;
				newsquik->next = nsquik->next;
				newsquik->previous = nsquik;
				nsquik->next->previous = newsquik;
				nsquik->next = newsquik;
				
			}
			if ((rn(0,1500)==1) && (nsquik->age>1000)) {
				nsquik->previous->next = nsquik->next;
				nsquik->next->previous = nsquik->previous;
				tempsquik=nsquik;
				nsquik=nsquik->next;
				free(tempsquik);
			} else {
				// Next squik
				nsquik=nsquik->next;
			}
		}
		SDL_UpdateRect(screen, 0, 0, 0, 0);
		r.x=0;
		r.y=0;
		r.w=SCREEN_WIDTH*4;
		r.h=SCREEN_HEIGHT*2;
		SDL_FillRect(screen,&r,0);
	}
	printf("End of loop.\n");

	printf("Freeing squiks.");
	fsquik=nsquik->previous;
	nsquik=nsquik->next;
	while (nsquik->previous != fsquik) {
		printf(".");
		psquik=nsquik->next;
		free(nsquik);
		nsquik=psquik;
	}
	printf("Done.\n");
}


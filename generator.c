#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include "recognizer.h"

#define PT_MAXSIZ      2048
#define WIDTH           256
#define HEIGHT          256

int main(void)
{
	SDL_Event event;
	SDL_Window *window;
	SDL_Renderer *renderer;
	Point points[PT_MAXSIZ];
	int siz, quit = 0, down = 0, i;
	do
	{
		if(SDL_Init(SDL_INIT_VIDEO))
		{
			fprintf(stderr, "SDL_Init failed.\n");
			break;
		}

		if(SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, 0, &window, &renderer))
		{
			fprintf(stderr, "SDL_CreateWindowAndRenderer failed.\n");
			break;
		}

		while(!quit)
		{
			SDL_SetRenderDrawColor(renderer,
					255, 255, 255, SDL_ALPHA_OPAQUE);
			SDL_RenderClear(renderer);
			SDL_SetRenderDrawColor(renderer,
					0, 0, 0, SDL_ALPHA_OPAQUE);

			for(i = 1; i < siz; ++i)
			{
				SDL_RenderDrawLine(renderer,
						points[i - 1].X, points[i - 1].Y,
						points[i].X, points[i].Y);
			}

			SDL_RenderPresent(renderer);
			while(SDL_PollEvent(&event))
			{
				switch(event.type)
				{
					case SDL_QUIT:
						quit = 1;
						break;

					case SDL_MOUSEBUTTONUP:
					{
						Point buf[RECOGNIZER_RESAMPLE_POINTS];
						down = 0;
						if(siz <= 1)
						{
							break;
						}

						recognizer_normalize(points, buf, siz);
						siz = 0;

						printf("Point Template[] = \n{\n");
						for(i = 0; i < RECOGNIZER_RESAMPLE_POINTS; ++i)
						{
							printf("\t{ %d, %d },\n", (int)buf[i].X, (int)buf[i].Y);
						}

						printf("};\n\n");
						break;
					}

					case SDL_MOUSEBUTTONDOWN:
						down = 1;
						break;

					case SDL_MOUSEMOTION:
						if(down)
						{
							points[siz].X = (double)event.motion.x;
							points[siz].Y = (double)event.motion.y;
							++siz;
						}
						break;

					default:
						break;
				}
			}
		}
	}
	while(0);

	if(renderer)
	{
		SDL_DestroyRenderer(renderer);
	}

	if(window)
	{
		SDL_DestroyWindow(window);
	}

	SDL_Quit();
	return 0;
}

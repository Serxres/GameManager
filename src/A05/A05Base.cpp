#include <SDL.h>		// Always needs to be included for an SDL app
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <time.h>
#include <iostream>

//Game general information
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

#define FPS 60

int main(int, char*[]) {

	// --- INIT ---
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) throw "No es pot inicialitzar SDL subsystems";
	const Uint8 imgFlags{ IMG_INIT_PNG | IMG_INIT_JPG };
	if (!(IMG_Init(imgFlags) & imgFlags))throw "Error al carregar!";
	if (TTF_Init() != 0) throw "No es pot inicialitzar SDL_ttf";
	const Uint8 mixFlags{ MIX_INIT_MP3 | MIX_INIT_OGG };
	if (!(Mix_Init(mixFlags) & mixFlags)) throw "Error:SDL_mixer init";
	// ---TIIME---
	clock_t lastTime = clock();
	float timeDown = 10;
	float deltaTime = 0;

	// --- WINDOW ---
	SDL_Window *window{ SDL_CreateWindow("SDL...", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN) };
	if (window == nullptr) throw "No es pot inicialitzar SDL_Window";

	// --- RENDERER ---
	SDL_Renderer *renderer{ SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC) };
	if (renderer == nullptr) throw "No es pot inicialitzar SDL_Renderer";

	// --- SPRITES ---
	SDL_Texture *bgTexture{ IMG_LoadTexture(renderer, "../../res/img/bgCastle.jpg") };
	if (bgTexture == nullptr)throw "No s'han pogut crear les textures";
	SDL_Rect bgRect{ 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };

	//SDL_Texture *playerTexture{ IMG_LoadTexture(renderer, "../../res/img/kintoun.png") };
	//if (playerTexture == nullptr) throw "Nos'han pogut crear les textures";
	//SDL_Rect playerRect{ 0, 0, 350, 189 };
	SDL_Rect playerTarget{ 0, 0, 100, 100 };
	// --- Animated Sprite ---

	SDL_Texture *playerTexture{ IMG_LoadTexture(renderer, "../../res/img/spCastle.png") };
	SDL_Rect playerRect, playerPosition;
	int textWidth, textHeight, frameWidth, frameHeight;
	SDL_QueryTexture(playerTexture, NULL, NULL, &textWidth, &textHeight);
	frameWidth = textWidth / 12;
	frameHeight = textHeight / 8;
	playerPosition.x = SCREEN_WIDTH / 2;
	playerPosition.y = SCREEN_HEIGHT / 2;
	playerRect.x = playerRect.y = 0;
	playerPosition.h = playerRect.h = frameHeight;
	playerPosition.w = playerRect.w = frameWidth;
	int frameTime = 0;

	// --- TEXT ---
	TTF_Font *font{ TTF_OpenFont("../../res/ttf/saiyan.ttf", 80) };
	if (font == nullptr) throw "No es pot iicialitzar the TTF_Font";
	SDL_Surface *tmpSurf{ TTF_RenderText_Blended(font, "My first SDL game", SDL_Color{ 255, 150, 0, 255 }) };
	if (tmpSurf == nullptr) TTF_CloseFont(font), throw "Unable to create the SDL text surface";
	SDL_Texture *textTexture{ SDL_CreateTextureFromSurface(renderer, tmpSurf) };
	SDL_Rect textRect{ 100, 50, tmpSurf->w, tmpSurf->h };
	SDL_FreeSurface(tmpSurf);
	TTF_CloseFont(font);

	// --- AUDIO ---
	if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 1024) == -1)
	{
		throw "Unable to initialize SDL_mixer audio systems";
	}
	Mix_Music *soundtrack{ Mix_LoadMUS("../../res/au/mainTheme.mp3") };
	if (!soundtrack) throw "Unable to load the Mix_Music soundtrack";
	Mix_VolumeMusic(MIX_MAX_VOLUME / 2);
	Mix_PlayMusic(soundtrack, -1);

	// --- GAME LOOP ---
	SDL_Event event;
	bool isRunning = true;
	while (isRunning) {
		// HANDLE EVENTS
		while (SDL_PollEvent(&event)) {
			switch (event.key.keysym.sym) {
			case SDL_QUIT:		isRunning = false; break;
			case SDLK_w:
				playerPosition.y -= 10;
				break;
			case SDLK_s:
				playerPosition.y += 10;
				break;
			case SDLK_a:
				playerPosition.x -= 10;
				break;
			case SDLK_d:
				playerPosition.x += 10;
				break;
			case SDL_KEYDOWN:	if (event.key.keysym.sym == SDLK_ESCAPE) isRunning = false; break;
			default:;
			}
		}

		// UPDATE
		deltaTime = (clock() - lastTime);
		lastTime = clock();
		deltaTime /= CLOCKS_PER_SEC;
		timeDown -= deltaTime;
		std::cout << timeDown << std::endl;
		if (timeDown <= 0) timeDown = 10;
		//playerRect.x += (playerTarget.x - playerRect.x) / 10;
		//playerRect.y += (playerTarget.y - playerRect.y) / 10;
		frameTime++;
		if (FPS / frameTime <= 9)
		{
			frameTime = 0;
			playerRect.x += frameWidth;
			if (playerRect.x > textWidth / 8)playerRect.x = 0;
			//if (playerRect.x >= textWidth) playerRect.x = 0;
		}
		if (playerPosition.y <= 150)
		{
			playerPosition.y = 150;
			if (playerPosition.x < 0)
			{ 
				playerPosition.x = 0;
			}
			else if (playerPosition.x > SCREEN_WIDTH - 30)
			{
				playerPosition.x = SCREEN_WIDTH - 30;
			}
		}
		else if (playerPosition.y > SCREEN_HEIGHT - 35)
		{
			playerPosition.y = SCREEN_HEIGHT - 35;
		}
		else if (playerPosition.x > SCREEN_WIDTH - 30)
		{
			playerPosition.x = SCREEN_WIDTH - 30;
		}
		else if (playerPosition.x < 0)
		{
			playerPosition.x = 0;
		}

		// DRAW
		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, bgTexture, nullptr, &bgRect);
		SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);
		//SDL_RenderCopy(renderer, playerTexture, nullptr, &playerRect);
		//Background

		//Animated Sprite
		SDL_RenderCopy(renderer, playerTexture, &playerRect, &playerPosition);
		SDL_RenderPresent(renderer);


	}

	// --- DESTROY ---
	SDL_DestroyTexture(playerTexture);
	SDL_DestroyTexture(textTexture);
	SDL_DestroyTexture(bgTexture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	// --- QUIT ---
	Mix_CloseAudio();

	IMG_Quit();
	TTF_Quit();
	Mix_Quit;

	SDL_Quit();
	return 0;
}

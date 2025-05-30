/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abelov <abelov@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 16:38:05 by abelov            #+#    #+#             */
/*   Updated: 2025/04/28 19:47:32 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include "cub3d.h"

#include <SDL2/SDL.h>
#include "SDL_mixer.h"
#include <stdio.h>

void init_sound()
{
	SDL_Init(SDL_INIT_AUDIO);
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);



	char const *door_sound_file = "resources/sound/cockchafer-gentleman-1.wav";
	char const *gun_sound_file = "resources/sound/laser.wav";
	char const *hash_sound_file = "resources/sound/percussion-28.wav";
	char const *boss_die_sound_file = "resources/sound/explos.wav";
	char const *rocket_sound_file = "resources/sound/apert2.wav";
	char const *portal_sound_file = "resources/sound/falling.wav";


	Mix_Chunk *door_sound = Mix_LoadWAV(door_sound_file);
	Mix_Chunk *gun_sound = Mix_LoadWAV(gun_sound_file);
	Mix_Chunk *hash_sound = Mix_LoadWAV(hash_sound_file);
	Mix_Chunk *portal_sound = Mix_LoadWAV(portal_sound_file);
	Mix_Chunk *boss_die_sound = Mix_LoadWAV(boss_die_sound_file);
	Mix_Chunk *rocket_sound = Mix_LoadWAV(rocket_sound_file);

	printf("Playing sound...\n");
	printf("Press Enter to continue.\n");
	getchar();  // Wait for user input before exiting

	Mix_PlayChannel(-1, door_sound, 0);  // -1 = first free channel, 0 = play once
	Mix_PlayChannel(-1, gun_sound, 0);  // play gunshot simultaneously

	getchar();  // Wait for user input before exiting
	Mix_PlayChannel(-1, rocket_sound, 0);


	getchar();  // Wait for user input before exiting
	Mix_PlayChannel(-1, boss_die_sound, 0);

	getchar();  // Wait for user input before exiting
	Mix_PlayChannel(-1, portal_sound, 0);

	printf("Press Enter to quit.\n");
	getchar();  // Wait for user input before exiting

	Mix_FreeChunk(door_sound);
	Mix_FreeChunk(gun_sound);
	Mix_FreeChunk(hash_sound);
	Mix_FreeChunk(portal_sound);
	Mix_FreeChunk(boss_die_sound);
	Mix_FreeChunk(rocket_sound);
	Mix_CloseAudio();
	SDL_Quit();

}

int main(int argc, char **argv)
{
	t_info *const app = &(t_info) {
		.title = (char *) "cub3d",
		.fullscreen = GO_TO_FULLSCREEN_ON_LOAD
	};

	init_sound();

	printf("fov: %d halffov: %f\n", app->fov_deg, app->fov_rad_half);
	while (app->state != STATE_END)
		app->state = run_state(app, argc, argv);
	cleanup(app);
	return (EXIT_SUCCESS);
}

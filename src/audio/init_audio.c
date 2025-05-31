/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_audio.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abelov <abelov@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/30 22:03:20 by abelov            #+#    #+#             */
/*   Updated: 2025/05/30 22:03:20 by abelov           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

int	init_audio(t_info *const app)
{
	int 	err;
	t_aud	*const aud = &app->audio;

	err = SDL_Init(SDL_INIT_AUDIO);
	if (err < 0)
	{
		ft_dprintf(STDERR_FILENO, "SDL_Init error: %s\n", SDL_GetError());
		exit((cleanup(app), EXIT_FAILURE));
	}

	err = Mix_OpenAudio(aud->frequency, aud->format, aud->no_channels, aud->chunk_size);
	if (err < 0)
	{
		ft_dprintf(STDERR_FILENO, "Mix_OpenAudio error: %s\n", Mix_GetError());
		exit((cleanup(app), EXIT_FAILURE));
	}

	load_sounds(aud);
	Mix_Volume(ch_music, 64);
	// Mix_VolumeChunk(aud->chunks[snd_rocket], 32);
	return (err);
}

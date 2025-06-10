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

static int load_sounds(t_aud *const aud)
{
	int			i;
	Mix_Chunk	*chunk;

	i = -1;
	while (++i < snd_MAX)
	{
		chunk = Mix_LoadWAV(aud->files[i]);
		if (!chunk)
			return (EXIT_FAILURE);
		aud->chunks[i] = chunk;
	}
	return (EXIT_SUCCESS);
}

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

	err = Mix_OpenAudio(aud->frequency, aud->format, aud->nchannels, aud->chunk_size);
	if (err < 0)
	{
		ft_dprintf(STDERR_FILENO, "Mix_OpenAudio error: %s\n", Mix_GetError());
		exit((cleanup(app), EXIT_FAILURE));
	}

	load_sounds(aud);
	// Mix_Volume(ch_music, 64);
	// Mix_VolumeChunk(aud->chunks[snd_rocket], 32);
	return (err);
}

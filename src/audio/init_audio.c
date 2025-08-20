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

static int	load_sounds(t_aud *const aud)
{
	int			i;
	Mix_Chunk	*chunk;

	i = -1;
	while (++i < SND_MAX)
	{
		chunk = Mix_LoadWAV(aud->files[i]);
		if (!chunk)
			return (EXIT_FAILURE);
		aud->chunks[i] = chunk;
	}
	return (EXIT_SUCCESS);
}

static inline __attribute__((always_inline))
int	mix_open_audio(int frequency, Uint16 format, int nchannels, int chunksize)
{
	int	err;

	err = Mix_OpenAudioDevice(frequency, format, nchannels,
			chunksize, SDL_getenv("SDL_AUDIO_DEVICE_NAME"),
			SDL_AUDIO_ALLOW_FREQUENCY_CHANGE | SDL_AUDIO_ALLOW_CHANNELS_CHANGE);
	return (err);
}

__attribute__((noinline))
int	init_audio(t_info *const app)
{
	int				err;
	t_aud *const	aud = &app->audio;

	err = SDL_Init(SDL_INIT_AUDIO);
	if (err < 0)
	{
		ft_dprintf(STDERR_FILENO, "SDL_Init error: %s\n", SDL_GetError());
		exit((cleanup(app), EXIT_FAILURE));
	}
	err = mix_open_audio(aud->frequency, aud->format,
			aud->nchannels, aud->chunk_size);
	if (err < 0)
	{
		ft_dprintf(STDERR_FILENO, "Mix_OpenAudio error: %s\n", Mix_GetError());
		exit((cleanup(app), EXIT_FAILURE));
	}
	Mix_AllocateChannels(ch_MAX + 6);
	Mix_ReserveChannels(ch_MAX);
	load_sounds(aud);
	return (err);
}

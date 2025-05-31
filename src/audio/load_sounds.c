/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sound.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abelov <abelov@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/30 19:58:58 by abelov            #+#    #+#             */
/*   Updated: 2025/05/30 19:58:59 by abelov           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

int load_sounds(t_aud *const aud)
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
	// printf("vol chunk: %d max: %d\n", Mix_VolumeChunk(aud->chunks[snd_door_open], -1), MIX_MAX_VOLUME);
	printf("vol1: %d max: %d\n", Mix_Volume(1, -1), MIX_MAX_VOLUME);
	Mix_Volume(0, 64);
	Mix_VolumeChunk(aud->chunks[snd_rocket], 48);
	// printf("vol1: %d max: %d\n", Mix_Volume(1, -1), MIX_MAX_VOLUME);
	return (EXIT_SUCCESS);
}

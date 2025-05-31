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
	return (EXIT_SUCCESS);
}

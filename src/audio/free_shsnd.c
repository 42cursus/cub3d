/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_shsnd.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abelov <abelov@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/30 22:00:01 by abelov            #+#    #+#             */
/*   Updated: 2025/05/30 22:00:01 by abelov           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

void	free_shsnd(t_info *const app)
{
	t_aud *const	aud = &app->audio;
	int 			i;

	i = -1;
	while (++i < snd_MAX)
		 Mix_FreeChunk(aud->chunks[i]);
	Mix_CloseAudio();
	SDL_Quit();
}

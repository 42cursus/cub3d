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

/**
 *
 * https://www.libsdl.org/projects/old/SDL_mixer/docs/SDL_mixer_10.html
 * @param app
 */
void	free_shsnd(t_info *const app)
{
	t_aud *const	aud = &app->audio;
	int				i;

	Mix_HaltChannel(-1);
	i = -1;
	while (++i < SND_MAX)
	{
		Mix_FreeChunk(aud->chunks[i]);
		aud->chunks[i] = NULL;
	}
	while (Mix_Init(0))
		Mix_Quit();
	Mix_CloseAudio();
	SDL_QuitSubSystem(SDL_INIT_AUDIO);
	SDL_Quit();
}

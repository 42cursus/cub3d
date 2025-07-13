/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   settings_misc.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abelov <abelov@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/13 23:20:43 by abelov            #+#    #+#             */
/*   Updated: 2025/07/13 23:20:44 by abelov           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

void	set_sound_volume(t_info *app, int volume)
{
	int	i;
	int	new_volume;

	if (volume < 0)
		volume = 0;
	if (volume > 100)
		volume = 100;
	app->audio.snd_volume = volume;
	new_volume = (int)((volume / 100.0) * 128 + 0.5);
	i = 1;
	while (i < MIX_CHANNELS)
		Mix_Volume(i++, new_volume);
}

void	set_music_volume(t_info *app, int volume)
{
	if (volume < 0)
		volume = 0;
	if (volume > 100)
		volume = 100;
	app->audio.mus_volume = volume;
	Mix_Volume(ch_music1, volume);
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   settings.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/19 15:13:37 by fsmyth            #+#    #+#             */
/*   Updated: 2025/05/19 15:13:59 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

void	set_sensitivity(t_info *app, int sensitivity)
{
	if (sensitivity < 1)
		sensitivity = 1;
	if (sensitivity > 10)
		sensitivity = 10;
	app->sensitivity = sensitivity;
}


void set_audio(t_info *const app)
{
	t_aud	*aud = &app->audio;

	app->audio.frequency = 44100;
	app->audio.no_channels = 2;
	app->audio.chunk_size = 2048;
	app->audio.format = MIX_DEFAULT_FORMAT; // AUDIO_S16LSB

	aud->files[snd_door] = "resources/sound/cockchafer-gentleman-1.wav";
	aud->files[snd_gun] = "resources/sound/beam_shot.wav";
	aud->files[snd_hash] = "resources/sound/percussion-28.wav";
	aud->files[snd_boss_die] = "resources/sound/explos.wav";
	aud->files[snd_rocket] = "resources/sound/apert2.wav";
	aud->files[snd_portal] = "resources/sound/falling.wav";
	aud->files[snd_door_open] = "resources/sound/door_open.wav";
	aud->files[snd_door_close] = "resources/sound/door_close.wav";
	aud->files[snd_pickup_ammo] = "resources/sound/pickup_ammo.wav";
	aud->files[snd_pickup_health] = "resources/sound/pickup_heal.wav";
	aud->files[snd_enemy_death] = "resources/sound/enemy_death.wav";
}

void	set_fov(t_info *app, int fov)
{
	if (fov < 45)
		fov = 45;
	else if (fov > 140)
		fov = 140;
	app->fov_deg = fov;
	app->fov_rad_half = ((double)fov / 360.0) * M_PI;
	app->fov_opp_len = tan(app->fov_rad_half);
}

void	set_framerate(t_info *app, size_t framerate)
{
	if (framerate < 30)
		framerate = 30;
	else if (framerate > 500)
		framerate = 500;
	app->fr_rate = framerate;
	app->fr_delay = 1000000 / framerate;
	app->fr_scale = framerate / 50.0;
}

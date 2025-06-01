/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   settings.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/19 15:13:37 by fsmyth            #+#    #+#             */
/*   Updated: 2025/06/01 19:28:20 by abelov           ###   ########.fr       */
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

void set_fonts(t_info *const app)
{
	t_typing *const	typing = &app->typ;

	typing->default_size = 50;
	typing->files[fnt_main] = "resources/fonts/ByteBounce.ttf";
	typing->files[fnt_SansMono] = "/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf";
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
	aud->files[snd_rocket] = "resources/sound/missile.wav";
	aud->files[snd_intro] = "resources/sound/falling.wav";
	aud->files[snd_portal] = "resources/sound/roll.wav";
	aud->files[snd_door_open] = "resources/sound/door_open.wav";
	aud->files[snd_door_close] = "resources/sound/door_close.wav";
	aud->files[snd_pickup_ammo] = "resources/sound/pickup_ammo.wav";
	aud->files[snd_pickup_health] = "resources/sound/pickup_heal.wav";
	aud->files[snd_enemy_death] = "resources/sound/enemy_death.wav";
	aud->files[snd_music_boss] = "resources/sound/boss_music.wav";
	aud->files[snd_enemy_attack1] = "resources/sound/enemy_jump.wav";
	aud->files[snd_enemy_attack2] = "resources/sound/enemy_jump2.wav";
	aud->files[snd_enemy_shot] = "resources/sound/enemy_damage.wav";
	aud->files[snd_player_damage] = "resources/sound/samus_damage.wav";
	aud->files[snd_win_music] = "resources/sound/win_music.wav";
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

void	set_sound_volume(t_info *app, int volume)
{
	int	i;
	int	new_volume;

	if (volume < 0)
		volume = 0;
	if (volume > 100)
		volume = 100;
	app->snd_volume = volume;
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
	app->mus_volume = volume;
	Mix_Volume(ch_music, volume);
}

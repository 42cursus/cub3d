/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fsm9.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abelov <abelov@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/14 15:15:53 by abelov            #+#    #+#             */
/*   Updated: 2025/07/14 15:15:53 by abelov           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"
#include <X11/X.h>
#include "fsm.h"

t_ret_code	do_state_intro(void *param)
{
	t_info *const	app = param;
	t_aud *const	aud = &app->audio;
	int				old_fps;

	Mix_PlayChannel(ch_music2, aud->chunks[snd_intro], 0);
	old_fps = app->fr_rate;
	set_framerate(app, 60);
	app->fr_last = get_time_us();
	mlx_loop(app->mlx);
	set_framerate(app, old_fps);
	return (ok);
}

t_ret_code	do_state_mmenu(void *param)
{
	t_info *const	app = param;

	mlx_loop(app->mlx);
	return (app->rc);
}

t_ret_code	do_state_load(void *param)
{
	t_info *const	app = param;

	if (app->rc != ok)
		return (app->rc);
	if (app->lvl && app->lvl->music)
		Mix_PlayChannel(ch_music1, app->lvl->music, -1);
	mlx_loop(app->mlx);
	replace_sky(app, (char *) TEX_DIR"/skybox.xpm");
	return (ok);
}

t_ret_code	do_state_play(void *param)
{
	t_info *const	app = param;

	mlx_mouse_hide(app->mlx, app->win);
	replace_sky_r(app, (char *)TEX_DIR"/fog_sky.xpm");
	replace_sky(app, (char *)TEX_DIR"/fog_sky.xpm");
	draw_sky_alt(app);
	draw_sky_transposed_avx2(app);
	draw_nav(app);
	calculate_offsets(app, app->player);
	app->fr_last = get_time_us();
	app->msg_to_show = -1;
	app->msg_last_time = app->fr_last;
	mlx_loop(app->mlx);
	mlx_mouse_show(app->mlx, app->win);
	return (app->rc);
}

t_ret_code	do_state_pmenu(void *param)
{
	t_info *const	app = param;

	mlx_loop(app->mlx);
	return (app->rc);
}

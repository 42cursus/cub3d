/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fsm8.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abelov <abelov@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/14 15:13:34 by abelov            #+#    #+#             */
/*   Updated: 2025/07/14 15:13:35 by abelov           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"
#include "fsm.h"

void	do_initial_to_end(void *param)
{
	t_info *const	app = param;

	return ;
	(void)app;
}

void	do_intro_to_end(void *param)
{
	t_info *const	app = param;

	return ;
	(void)app;
}

t_ret_code	do_state_win(void *param)
{
	t_info *const	app = param;

	replace_sky(app, (char *)TEX_DIR"/skybox1.xpm");
	draw_sky_alt(app);
	mlx_loop(app->mlx);
	return (app->rc);
}

t_ret_code	do_state_lose(void *param)
{
	t_info *const	app = param;

	replace_sky(app, (char *)TEX_DIR"/skybox1.xpm");
	draw_sky_alt(app);
	mlx_loop(app->mlx);
	return (app->rc);
}

t_ret_code	do_state_credits(void *param)
{
	t_info *const	app = param;
	int				old_fps;

	app->old_fov = app->fov_deg;
	old_fps = app->fr_rate;
	set_fov(app, 70);
	set_framerate(app, 500);
	calculate_credits_offset(app, app->dummy);
	Mix_PlayChannel(ch_music1, app->audio.chunks[snd_credits_finale], 0);
	mlx_mouse_hide(app->mlx, app->win);
	app->fr_last = get_time_us();
	mlx_loop(app->mlx);
	set_fov(app, app->old_fov);
	set_framerate(app, old_fps);
	mlx_mouse_show(app->mlx, app->win);
	return (app->rc);
}

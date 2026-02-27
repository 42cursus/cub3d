/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mouse_hooks.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/08 16:44:07 by fsmyth            #+#    #+#             */
/*   Updated: 2025/08/08 16:44:40 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

int	mouse_move_play(int x, int y, void *param)
{
	t_info *const	app = param;
	int const		dx = x - WIN_WIDTH / 2;
	int const		dy = y - WIN_HEIGHT / 2;
	double const	sense = app->sensitivity;
	double			xx;

	if (dx != 0)
	{
		xx = 300.0 + (80 * (10 - sense)) * pow(1.1, 10 - sense);
		xx = fabs(xx / (dx * app->fr_scale));
		rotate_player(app, app->player, dx > 0, xx);
		mlx_mouse_move(app->mlx, app->win, WIN_WIDTH / 2, WIN_HEIGHT / 2);
		XFlush(app->mlx->display);
	}
	return (0);
	(void) dy;
}

int	mouse_release_play(unsigned int button, int x, int y, void *param)
{
	t_info *const	app = param;

	app->mouse[button] = false;
	return (0);
	((void) x, (void) y);
}

// else if (button == 2)
// {
// 	if (!app->ads)
// 	{
// 		app->ads = 1;
// 		app->old_fov = app->fov_deg;
// 		set_fov(app, 45);
// 	}
// 	else
// 	{
// 		app->ads = 0;
// 		set_fov(app, app->old_fov);
// 	}
// 	calculate_offsets(app, app->player);
// 	replace_sky(app, (char *) TEX_DIR"/skybox.xpm");
// 	draw_sky_alt(app);
// }
int	mouse_press_play(unsigned int button, int x, int y, void *param)
{
	t_info *const	app = param;

	app->mouse[button] = true;
	if (button == 1)
		spawn_projectile(app, app->player, app->lvl, app->player->equipped);
	else if (button == 3)
		app->player->equipped = 0;
	else if (button == 4)
		next_weapon(app->player);
	else if (button == 5)
		prev_weapon(app->player);
	return (0);
	((void) x, (void) y);
}

int	mouse_press_multi(unsigned int button, int x, int y, void *param)
{
	t_info *const	app = param;

	app->mouse[button] = true;
	if (button == 1)
		spawn_projectile_client(app, app->player);
	else if (button == 3)
		app->player->equipped = 0;
	else if (button == 4)
		next_weapon(app->player);
	else if (button == 5)
		prev_weapon(app->player);
	return (0);
	((void) x, (void) y);
}

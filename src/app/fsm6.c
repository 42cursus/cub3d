/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fsm6.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abelov <abelov@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/14 15:11:34 by abelov            #+#    #+#             */
/*   Updated: 2025/07/14 15:11:35 by abelov           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"
#include <X11/X.h>
#include "fsm.h"

void	do_mmenu_to_credits(void *param)
{
	t_info *const	app = param;
	t_dummy			*dummy;
	int				i;

	fill_with_colour(app->bg, 0x000000, 0x000000);
	dummy = ft_calloc(1, sizeof(*dummy));
	dummy->dir = (t_vect){0.0, 1.0};
	dummy->pos = (t_vect){0.0, -0.6};
	dummy->speed = 0.002;
	app->dummy = dummy;
	i = -1;
	while (++i < 12)
		spawn_random_rock(app, 0);
	app->mlx->end_loop = 0;
	mlx_loop_hook(app->mlx, &render_credits, app);
	mlx_hook(app->win, KeyPress, KeyPressMask,
		(void *)&key_press_credits, app);
	mlx_hook(app->win, KeyRelease, KeyReleaseMask,
		(void *)&key_release_credits, app);
	mlx_hook(app->win, ButtonPress, NoEventMask, NULL, app);
	mlx_hook(app->win, ButtonRelease, NoEventMask, NULL, app);
	mlx_hook(app->win, MotionNotify, NoEventMask, NULL, app);
}

void	do_mmenu_to_intro(void *param)
{
	t_info *const	app = param;

	return ;
	(void)app;
}

void	do_mmenu_to_end(void *param)
{
	t_info *const	app = param;

	return ;
	(void)app;
}

void	do_credits_to_mmenu(void *param)
{
	t_info *const	app = param;

	ft_lstclear(&app->dummy->rocks, free);
	app->dummy = (free(app->dummy), NULL);
	replace_image(app, &app->bg, (char *) TEX_DIR"/wall.xpm");
	ft_memset(app->keys, 0, sizeof(bool) * 16);
	mlx_loop_hook(app->mlx, &render_mmenu, app);
	app->mlx->end_loop = 0;
	mlx_hook(app->win, KeyPress, KeyPressMask, (void *)&key_press_mmenu, app);
	mlx_hook(app->win, ButtonPress, NoEventMask, NULL, app);
	mlx_hook(app->win, ButtonRelease, NoEventMask, NULL, app);
	mlx_hook(app->win, KeyRelease, NoEventMask, NULL, app);
	mlx_hook(app->win, MotionNotify, NoEventMask, NULL, app);
	app->menu_state.state = MAIN;
	app->menu_state.selected = 3;
	app->menu_state.no_items = 5;
}

void	do_load_to_mmenu(void *param)
{
	t_info *const	app = param;

	cleanup_maps(app);
	app->player = (free(app->player), NULL);
	replace_image(app, &app->bg, (char *) TEX_DIR"/wall.xpm");
	mlx_loop_hook(app->mlx, &render_mmenu, app);
	app->mlx->end_loop = 0;
	mlx_hook(app->win, KeyPress, KeyPressMask, (void *) &key_press_mmenu, app);
	mlx_hook(app->win, ButtonPress, NoEventMask, NULL, app);
	mlx_hook(app->win, ButtonRelease, NoEventMask, NULL, app);
	mlx_hook(app->win, KeyRelease, NoEventMask, NULL, app);
	mlx_hook(app->win, MotionNotify, NoEventMask, NULL, app);
	app->menu_state.state = MAIN;
	app->menu_state.selected = 0;
	app->menu_state.no_items = 5;
}

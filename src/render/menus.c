/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   menus.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/15 12:42:56 by fsmyth            #+#    #+#             */
/*   Updated: 2025/04/15 13:42:20 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

void	menu_change_option(t_info *app, int dir)
{
	t_menustate	*menu_state;

	menu_state = &app->menu_state;
	if (menu_state->state == OPTIONS)
	{
		if (menu_state->selected == 0)
		{
			set_fov(app, app->fov_deg + (5 * dir));
			replace_sky(app, (char *) "./textures/skybox.xpm");
			if (menu_state->prev != MAIN)
			{
				calculate_offsets(app, app->player);
				draw_sky_alt(app);
				replace_frame(app);
				fast_memcpy_test((int *)app->stillshot->data, (int *)app->canvas->data, WIN_HEIGHT * WIN_WIDTH * sizeof(int));
			}
		}
		if (menu_state->selected == 1)
		{
			set_framerate(app, app->fr_rate + (5 * dir));
		}
		if (menu_state->selected == 2)
		{
			set_sensitivity(app, app->sensitivity + (1 * dir));
		}
		else if (menu_state->selected == 3)
		{
			if (menu_state->prev == MAIN)
				app->timer.active = !app->timer.active;
		}
	}
}

void	menu_select_current(t_info *app)
{
	t_menustate	*menu_state;

	menu_state = &app->menu_state;
	if (menu_state->state == MAIN)
	{
		if (menu_state->selected == 0)
		{
			app->rc = ok;
			app->mlx->end_loop = 1;
		}
		if (menu_state->selected == 1)
		{
			menu_state->state = LVL_SELECT;
			menu_state->selected = 0;
			menu_state->no_items = app->no_maps + 1;
		}
		if (menu_state->selected == 2)
		{
			menu_state->prev = menu_state->state;
			menu_state->state = OPTIONS;
			menu_state->selected = 0;
			menu_state->no_items = 5;
		}
		if (menu_state->selected == 3)
		{
			app->rc = repeat;
			app->mlx->end_loop = 1;
		}
		if (menu_state->selected == 4)
		{
			app->rc = fail;
			app->mlx->end_loop = 1;
		}
	}
	else if (menu_state->state == PAUSE)
	{
		if (menu_state->selected == 0)
		{
			app->rc = ok;
			app->mlx->end_loop = 1;
		}
		if (menu_state->selected == 1)
		{
			app->rc = repeat;
			app->mlx->end_loop = 1;
		}
		if (menu_state->selected == 2)
		{
			menu_state->prev = menu_state->state;
			menu_state->state = OPTIONS;
			menu_state->selected = 0;
			menu_state->no_items = 5;
		}
		if (menu_state->selected == 3)
		{
			app->rc = fail;
			app->mlx->end_loop = 1;
		}
	}
	else if (menu_state->state == OPTIONS)
	{
		if (menu_state->selected == menu_state->no_items - 1)
		{
			menu_state->state = menu_state->prev;
			menu_state->selected = 2;
			menu_state->no_items = 4;
			if (menu_state->state == MAIN)
				menu_state->no_items = 5;
		}
	}
	else if (menu_state->state == LVL_SELECT)
	{
		if (menu_state->selected == app->no_maps)
		{
			menu_state->selected = 0;
			menu_state->state = MAIN;
			menu_state->no_items = 5;
			return ;
		}
		app->current_level = menu_state->selected;
		app->rc = ok;
		app->mlx->end_loop = 1;
	}
	else if (menu_state->state == WIN || menu_state->state == LOSE)
	{
		if (menu_state->selected == 0)
			app->rc = ok;
		if (menu_state->selected == 1)
			app->rc = repeat;
		if (menu_state->selected == 2)
			app->rc = fail;
		app->mlx->end_loop = 1;
	}
}

void	place_menu(const char **strings, t_ivect pos, int scalar, t_info *app);

void	draw_menu_items(t_info *app)
{
	t_menustate	*menu_state;
	int			i;
	char		buf[40];
	char		buf2[40];
	char		buf3[40];
	char		buf4[40];
	t_ivect		pos;

	menu_state = &app->menu_state;
	if (menu_state->state == MAIN)
	{
		const char *strings[] = (const char *[]) {"START", "LEVEL SELECT", "options", "credits", "EXIT"};
		place_menu(strings, (t_ivect){WIN_WIDTH / 2, WIN_HEIGHT / 2}, 4, app);
	}
	else if (menu_state->state == LVL_SELECT)
	{
		i = -1;
		while (++i < app->no_maps)
		{
			// ft_snprintf(buf, 20, "Level %d", i + 1);
			place_str_centred(app->map_ids[i], app, (t_ivect){WIN_WIDTH / 2, 360 + (i * 48)}, 3);
		}
		place_str_centred((char *)"back", app, (t_ivect){WIN_WIDTH / 2, 360 + (i * 48)}, 3);
		pos.x = WIN_WIDTH / 2 - 320;
		pos.y = 330 + (menu_state->selected * 48);
		put_texture(app, &app->shtex->trophy_tex[0], pos.x, pos.y);
	}
	if (menu_state->state == WIN)
	{
		place_str_centred((char *)	"You win", app, (t_ivect){WIN_WIDTH / 2, 340}, 5);
		if (app->timer.active == 1)
		{
			place_str_centred((char *)	"Your time was:", app, (t_ivect){WIN_WIDTH / 2, 420}, 3);
			place_timer(app, app->timer.total_ms, (t_ivect){WIN_WIDTH / 2 - (24  * 4), 460}, 3);
			ft_snprintf(buf, 40, "collected: %d%%", (app->player->pickups_collected * 100) / app->player->total_pickups);
			place_str_centred(buf, app, (t_ivect){WIN_WIDTH / 2, 520}, 3);
			place_menu((const char *[]){"next level", "MAIN MENU", "EXIT"}, (t_ivect){WIN_WIDTH / 2, WIN_HEIGHT / 2 + 100}, 3, app);
		}
		else
			place_menu((const char *[]){"next level", "MAIN MENU", "EXIT"}, (t_ivect){WIN_WIDTH / 2, WIN_HEIGHT / 2}, 3, app);
	}
	if (menu_state->state == LOSE)
	{
		place_str_centred((char *)	"You died", app, (t_ivect){WIN_WIDTH / 2, 340}, 5);
		place_menu((const char *[]){"retry level", "MAIN MENU", "EXIT"}, (t_ivect){WIN_WIDTH / 2, WIN_HEIGHT / 2}, 3, app);
	}
	if (menu_state->state == PAUSE)
	{
		place_menu((const char *[]){"resume", "MAIN MENU", "options", "EXIT"}, (t_ivect){WIN_WIDTH / 2, WIN_HEIGHT / 2}, 3, app);
	}
	if (menu_state->state == OPTIONS)
	{
		ft_snprintf(buf, 40, "fov  %d", app->fov_deg);
		ft_snprintf(buf2, 40, "fps cap  %d", app->fr_rate);
		ft_snprintf(buf4, 40, "sensitivity  %d", app->sensitivity);
		if (app->timer.active == 1)
			ft_snprintf(buf3, 40, "time trial  on", app->fr_rate);
		else
			ft_snprintf(buf3, 40, "time trial  off", app->fr_rate);
		place_menu((const char *[]){buf, buf2, buf4, buf3, "back"}, (t_ivect){WIN_WIDTH / 2, WIN_HEIGHT / 2}, 3, app);
	}
}

void	change_menu_selection(t_info *app, int dir)
{
	t_menustate	*menu_state;
	int			new_selection;

	menu_state = &app->menu_state;
	new_selection = menu_state->selected + dir;
	if (new_selection == menu_state->no_items)
		new_selection = 0;
	else if (new_selection < 0)
		new_selection = menu_state->no_items - 1;
	menu_state->selected = new_selection;
}

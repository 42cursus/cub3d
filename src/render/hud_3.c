/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hud_3.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abelov <abelov@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 14:43:34 by abelov            #+#    #+#             */
/*   Updated: 2025/08/11 14:43:35 by abelov           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

void	format_time(char *buf, int len, size_t time);

inline __attribute__((always_inline, used, externally_visible))
void	place_energy_backup(t_info *app, t_player *player)
{
	const int		backup = player->health / 100;
	const int		max_backup = player->max_health / 100;
	int				i;
	t_ivect			start;
	t_shtex *const	shtex = app->shtex;

	i = -1;
	start = (t_ivect){32, 16};
	while (++i < backup)
	{
		if (i > 6)
			start = (t_ivect){-96, 16};
		put_texture(app, &shtex->energy_tex[1], start.x + i * 16, start.y);
	}
	while (i < max_backup)
	{
		if (i > 6)
			start = (t_ivect){-96, 16};
		put_texture(app, &shtex->energy_tex[2], start.x + (i++) * 16, start.y);
	}
}

inline __attribute__((always_inline, used, externally_visible))
void	place_energy(t_info *app, t_player *player)
{
	int		health;
	char	buf[3];

	put_texture(app, &app->shtex->energy_tex[0], 16, 48);
	health = player->health % 100;
	buf[0] = (health / 10) + '0';
	buf[1] = (health % 10) + '0';
	buf[2] = 0;
	place_str(buf, app, (t_ivect){96, 48}, 2);
	place_energy_backup(app, player);
}

inline __attribute__((always_inline, used, externally_visible))
void	place_weapon(t_info *app)
{
	t_tex	*tex;

	if (app->player->hud.active == 1)
	{
		if ((app->fr_last - app->player->hud.timestart) / 20000 < 6)
			tex = &app->shtex->cannon_tex[1];
		else
		{
			app->player->hud.active = 0;
			tex = &app->shtex->cannon_tex[0];
		}
	}
	else
		tex = &app->shtex->cannon_tex[0];
	put_texture(app, tex, WIN_WIDTH / 2, WIN_HEIGHT - tex->h);
}

inline __attribute__((always_inline, used, externally_visible))
void	place_boss_health(t_info *app)
{
	t_ivect			it;
	t_ivect			start;
	int				end_x;
	u_int			*row;
	t_tex *const	bbar = app->shtex->boss_bar;

	start.y = WIN_HEIGHT * 0.95;
	start.x = WIN_WIDTH / 4;
	end_x = start.x + ((WIN_WIDTH / 2) * (app->lvl->boss_obj->health / 500.0));
	it.y = start.y - 1;
	while (++it.y < start.y + 14)
	{
		row = (u_int *) app->canvas->data + it.y * app->canvas->width;
		it.x = start.x - 1;
		while (++it.x <= end_x)
			row[it.x] = MLX_RED;
	}
	put_texture(app, &bbar[0], start.x - 16, start.y - 1);
	put_texture(app, &bbar[1], start.x + (WIN_WIDTH / 2), start.y - 1);
	place_str((char *)"Phantoon", app, (t_ivect){start.x, start.y - 24}, 2);
}

inline __attribute__((always_inline, used, externally_visible))
void	place_timer(t_info *app, size_t time, t_ivect pos, int scalar)
{
	char	buf[50];

	format_time(buf, 50, time);
	place_str(buf, app, pos, scalar);
}

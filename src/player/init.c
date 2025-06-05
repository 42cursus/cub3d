/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/12 19:31:02 by fsmyth            #+#    #+#             */
/*   Updated: 2025/06/04 22:30:23 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

t_player	*init_player(t_info *app)
{
	t_player	*player;
	t_lvl		*map;

	map = app->map;
	player = ft_calloc(1, sizeof(*player));
	player->pos = map->starting_pos;
	player->health = 99;
	player->max_health = 99;
	player->equipped = pr_BEAM;
	player->ammo[pr_BEAM] = -1;
	player->max_ammo[pr_BEAM] = -1;
	player->dir = map->starting_dir;
	player->angle = atan2(player->dir.y, player->dir.x);
	calculate_offsets(app, player);
	return (player);
}

void	refresh_player(t_info *app, t_player *player)
{
	t_lvl		*map;

	map = app->map;
	player->pos = map->starting_pos;
	player->dir = map->starting_dir;
}

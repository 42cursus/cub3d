/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   player.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/12 19:31:02 by fsmyth            #+#    #+#             */
/*   Updated: 2025/05/19 15:30:57 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"
#include <math.h>
#include <unistd.h>

/**
 * getting hypotenuse length - Pythagoras' theorem
 * @param len1
 * @param len2
 * @return
 */
double	get_hyp_len(double len1, double len2)
{
	return (sqrt(len1 * len1 + len2 * len2));
}

t_player	*init_player(t_info *app)
{
	t_player	*player;
	t_data		*map;

	map = app->map;
	player = ft_calloc(1, sizeof(*player));
	player->pos = map->starting_pos;
	player->health = 99;
	player->max_health = 99;
	player->equipped = BEAM;
	player->ammo[BEAM] = -1;
	player->max_ammo[BEAM] = -1;
	player->dir = map->starting_dir;
	// rotate_vect_inplace(&player->direction, 0.01);
	calculate_offsets(app, player);
	return (player);
}

void	refresh_player(t_info *app, t_player *player)
{
	t_data		*map;

	map = app->map;
	player->pos = map->starting_pos;
	player->dir = map->starting_dir;
}

int	check_tile_open(char tile, t_data *map)
{
	if (tile == 'O' || tile == '0')
		return (1);
	else if (tile == 'B' && map->boss_active == 0)
		return (1);
	return (0);
}

// void	handle_close_door(t_info *app, t_ray *crosshair)
// {
// 	t_anim	*anim;
//
// 	app->map->map[crosshair->maptile.y][crosshair->maptile.x] = 'D';
// 	anim = &app->map->anims[crosshair->maptile.y][crosshair->maptile.x];
// 	anim->active = 1;
// 	anim->timestart = app->last_frame;
// }

void	subtract_health(t_info *app, t_player *player, int damage)
{
	int	new_health;

	if (player->dead == 1)
		return ;
	if (app->last_frame - player->dmg_time < 250000)
		return ;
	new_health = player->health - damage;
	if (new_health < 0)
	{
		new_health = 0;
		printf("You died! :^(\n");
		player->dead = 1;
		app->rc = fail;
		app->mlx->end_loop = 1;
	}
	player->health = new_health;
	player->dmg_time = app->last_frame;
}

void	add_health(t_player *player, int health)
{
	int	new_health;

	new_health = player->health + health;
	if (new_health > player->max_health)
		new_health = player->max_health;
	player->health = new_health;
}

int	count_collectables(t_data *map)
{
	t_list		*current;
	t_object	*cur_obj;
	int			count;

	current = map->items;
	count = 0;
	while (current != NULL)
	{
		cur_obj = current->content;
		if (cur_obj->subtype >= I_ETANK && cur_obj->subtype <= I_MISSILE)
			count++;
		current = current->next;
	}
	return (count);
}

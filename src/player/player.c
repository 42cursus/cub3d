/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   player.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/12 19:31:02 by fsmyth            #+#    #+#             */
/*   Updated: 2025/04/25 13:52:20 by abelov           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"
#include <math.h>
#include <unistd.h>

void	calculate_offsets(t_player *player)
{
	int			i;
	const int	halfwidth = WIN_WIDTH / 2;
	double		offset;
	double		angle;

	offset = 1.0 / halfwidth;
	i = 0;
	while (i < halfwidth)
	{
		angle = atan(1.0 - (i * offset));
		// angle = M_PI_4 * (halfwidth - i) * offset;
		player->angle_offsets[i] = angle;
		player->angle_offsets[WIN_WIDTH - i - 1] = -angle;
		i++;
	}
}

t_player	*init_player(t_data *map)
{
	t_player	*player;

	player = ft_calloc(1, sizeof(*player));
	player->pos = map->starting_pos;
	player->health = 99;
	player->max_health = 99;
	player->equipped = BEAM;
	player->ammo[BEAM] = -1;
	player->max_ammo[BEAM] = -1;
	if (map->starting_dir == 'N')
	{
		player->dir.x = 0;
		player->dir.y = 1;
	}
	else if (map->starting_dir == 'S')
	{
		player->dir.x = 0;
		player->dir.y = -1;
	}
	else if (map->starting_dir == 'E')
	{
		player->dir.x = 1;
		player->dir.y = 0;
	}
	else if (map->starting_dir == 'W')
	{
		player->dir.x = -1;
		player->dir.y = 0;
	}
	// rotate_vect_inplace(&player->direction, 0.01);
	calculate_offsets(player);
	return (player);
}

// void	print_ascii_mmap(t_data *data, t_player *player)
// {
// 	ssize_t	i;
// 	ssize_t	j;
// 	char	**map;
// 	char	player_char;
// 	t_vect	intercept;
// 	int		face;
//
// 	map = data->map;
// 	intercept = find_ray_collision(data, player, player->angle).intcpt;
// 	if (player->angle > M_PI_4 && player->angle <= 3 * M_PI_4)
// 		player_char = '^';
// 	else if (player->angle > 3 * M_PI_4 || player->angle <= -3 * M_PI_4)
// 		player_char = '<';
// 	else if (player->angle > -3 * M_PI_4 && player->angle <= -M_PI_4)
// 		player_char = 'v';
// 	else
// 		player_char = '>';
// 	ft_putstr("\e[2J\e[H");
// 	printf("height: %d width: %d\n", data->height, data->width);
// 	printf("ahead:\t(%f, %f) angle: %f\n", intercept.x, intercept.y, player->angle);
// 	printf("pos:\t(%f, %f)\n", player->pos.x, player->pos.y);
// 	printf("dir:\t(%f, %f)\n", player->direction.x, player->direction.y);
// 	i = data->height;
// 	while (--i >= 0)
// 	{
// 		j = 0;
// 		while (map[i][j])
// 		{
// 			if (map[i][j] == ' ')
// 				ft_putstr(" ");
// 			else if (map[i][j] == '1')
// 				ft_putstr("\e[34m#");
// 			else if (map[i][j] == '0')
// 				ft_putstr(" ");
// 			j++;
// 			write(1, " ", 1);
// 		}
// 		ft_putstr("\e[m\n");
// 	}
// 	ft_printf("\e[%d;%dH\e[1;32m%c\e[m", data->height - (int)player->pos.y + 4, (int)player->pos.x * 2 + 1, player_char);
// 	i = 0;
// 	while (i < WIN_WIDTH)
// 	{
// 		face = player->rays[i].face;
// 		if (face == WEST)
// 			ft_printf("\e[%d;%dH\e[1;31mW\e[m", data->height - (int)player->rays[i].intcpt.y + 4, (int)player->rays[i].intcpt.x * 2 + 1);
// 		else if (face == EAST)
// 			ft_printf("\e[%d;%dH\e[1;31mE\e[m", data->height - (int)player->rays[i].intcpt.y + 4, (int)player->rays[i].intcpt.x * 2 - 1);
// 		else if (face == NORTH)
// 			ft_printf("\e[%d;%dH\e[1;31mN\e[m", data->height - (int)player->rays[i].intcpt.y + 5, (int)player->rays[i].intcpt.x * 2 + 1);
// 		else if (face == SOUTH)
// 			ft_printf("\e[%d;%dH\e[1;31mS\e[m", data->height - (int)player->rays[i].intcpt.y + 4, (int)player->rays[i].intcpt.x * 2 + 1);
// 		i++;
// 	}
// }

int	check_tile_open(char tile, t_data *map)
{
	if (tile == 'O' || tile == '0')
		return (1);
	else if (tile == 'B' && map->boss_active == 0)
		return (1);
	return (0);
}

void	move_entity(t_vect *pos, t_data *data, t_vect dir)
{
	t_vect	new_pos;
	t_cvect	tile;
	char	**map;
	char	both_tile;

	map = data->map;
	new_pos.x = pos->x + (dir.x * 0.1 / FR_SCALE);
	new_pos.y = pos->y + (dir.y * 0.1 / FR_SCALE);
	tile = (t_cvect){map[(int)pos->y][(int)new_pos.x], map[(int)new_pos.y][(int)pos->x]};
	both_tile = map[(int)new_pos.y][(int)new_pos.x];
	if (check_tile_open(both_tile, data))
	{
		if (check_tile_open(tile.x, data))
			pos->x = new_pos.x;
		if (check_tile_open(tile.y, data))
			pos->y = new_pos.y;
	}
	else
	{
		if (check_tile_open(tile.x, data) && check_tile_open(tile.y, data))
		{
			if (get_max_direction(dir) == 'x')
				pos->x = new_pos.x;
			else
				pos->y = new_pos.y;
		}
		else if (check_tile_open(tile.x, data))
			pos->x = new_pos.x;
		else if (check_tile_open(tile.y, data))
			pos->y = new_pos.y;
	}
}

void	rotate_player(t_player *player, int direction, double sensitivity)
{
	if (direction == 0)
		rotate_vect_inplace(&player->dir, M_PI_4 / (sensitivity * FR_SCALE));
	else
		rotate_vect_inplace(&player->dir, -M_PI_4 / (sensitivity * FR_SCALE));
}

void	handle_close_door(t_info *app, t_ray *crosshair)
{
	t_anim	*anim;
	
	app->map->map[crosshair->maptile.y][crosshair->maptile.x] = 'D';
	anim = &app->map->anims[crosshair->maptile.y][crosshair->maptile.x];
	anim->active = 1;
	anim->framestart = app->framecount;
}

void	handle_open_door(t_info *app, t_ray *crosshair)
{
	// t_ray	*crosshair;
	t_anim	*anim;
	char	*doortile;

	// app->player->hud.active = 1;
	// app->player->hud.framestart = app->framecount;
	if (crosshair->distance < 1.0)
	{
		doortile = &app->map->map[crosshair->maptile.y][crosshair->maptile.x];
		anim = &app->map->anims[crosshair->maptile.y][crosshair->maptile.x];
		if (*doortile == 'D')
			*doortile = 'O';
		else if (*doortile == 'O')
			*doortile = 'D';
		else
			return ;
		anim->active = 1;
		anim->framestart = app->framecount;
	}
	if (crosshair->in_front != NULL)
		handle_open_door(app, crosshair->in_front);
}

void	spawn_projectile(t_info *app, t_player *player, t_data *map, int subtype)
{
	t_object	*projectile;

	player->hud.active = 1;
	player->hud.framestart = app->framecount;
	projectile = ft_calloc(1, sizeof(*projectile));
	projectile->subtype = subtype;
	projectile->pos = add_vect(player->pos, scale_vect(player->dir, 0.2));
	projectile->texture = &app->shtex->proj_tex[0];
	if (subtype == BEAM)
		projectile->dir = scale_vect(player->dir, 0.5 / FR_SCALE);
	else
	{
		player->ammo[subtype] -= 1;
		if (player->ammo[subtype] == 0)
			player->equipped = BEAM;
		projectile->dir = scale_vect(player->dir, 0.2 / FR_SCALE);
	}
	projectile->type = O_PROJ;
	projectile->anim.active = 0;
	ft_lstadd_back(&map->objects, ft_lstnew(projectile));
}

void	spawn_enemy_projectile(t_info *app, t_object *enemy, t_vect dir)
{
	t_object	*projectile;

	projectile = ft_calloc(1, sizeof(*projectile));
	// projectile->subtype = subtype;
	projectile->type = O_EPROJ;
	projectile->pos = enemy->pos;
	projectile->dir = dir;
	projectile->texture = &app->shtex->proj_tex[0];
	ft_lstadd_back(&app->map->objects, ft_lstnew(projectile));
}

t_object	*spawn_enemy(t_info *app, t_vect pos, t_vect dir, int subtype)
{
	t_object	*enemy;
	t_data		*map;

	map = app->map;
	enemy = ft_calloc(1, sizeof(*enemy));
	enemy->pos = pos;
	enemy->dir = scale_vect(dir, 1.0 / FR_SCALE);
	// enemy->texture = tex;
	enemy->type = O_ENTITY;
	enemy->subtype = subtype;
	if (subtype == E_ZOOMER)
		enemy->health = 20;
	else if (subtype == E_PHANTOON)
		enemy->health = 500;
	enemy->anim.active = 1;
	enemy->anim.framestart = app->framecount;
	ft_lstadd_back(&map->objects, ft_lstnew(enemy));
	return (enemy);
}

void	spawn_item(t_info *app, t_vect pos, int subtype)
{
	t_object	*item;
	t_data		*map;

	map = app->map;
	item = ft_calloc(1, sizeof(*item));
	item->pos = pos;
	// item->texture = tex;
	item->type = O_ITEM;
	item->subtype = subtype;
	item->anim.active = 1;
	item->anim.framestart = app->framecount;
	ft_lstadd_back(&map->objects, ft_lstnew(item));
}

void	spawn_trigger(t_info *app, t_vect pos, int subtype)
{
	t_object	*trigger;
	t_data		*map;

	map = app->map;
	trigger = ft_calloc(1, sizeof(*trigger));
	trigger->pos = pos;
	// item->texture = tex;
	trigger->type = O_TRIGGER;
	trigger->subtype = subtype;
	trigger->texture = &app->shtex->empty;
	ft_lstadd_back(&map->objects, ft_lstnew(trigger));
}

void	developer_console(t_info *app, t_player *player)
{
	char	*line;
	char	**split;
	t_vect	pos;
	t_vect	dir;
	int		val;

	ft_printf("input command:\t");
	line = get_next_line(STDIN_FILENO);
	line[ft_strlen(line) - 1] = 0;
	split = ft_split(line, ' ');
	if (ft_strcmp(split[0], "spawn") == 0)
	{
		pos = add_vect(player->pos, player->dir);
		if (ft_strcmp(split[1], "item") == 0)
		{
			if (ft_strcmp(split[2], "etank") == 0)
				spawn_item(app, pos, I_ETANK);
			if (ft_strcmp(split[2], "super") == 0)
				spawn_item(app, pos, I_SUPER);
			if (ft_strcmp(split[2], "health") == 0)
				spawn_item(app, pos, I_HEALTH);
			if (ft_strcmp(split[2], "missile") == 0)
				spawn_item(app, pos, I_MISSILE);
			if (ft_strcmp(split[2], "trophy") == 0)
				spawn_item(app, pos, I_TROPHY);
		}
		if (ft_strcmp(split[1], "enemy") == 0)
		{
			val = ft_atoi(split[3]);
			dir = scale_vect(vect(sin(val * M_PI_4 / 2), cos(val * M_PI_4 / 2)), 0.03);
			if (split[4] != NULL)
				dir = scale_vect(dir, ft_atoi(split[4]));
			if (ft_strcmp(split[2], "zoomer") == 0)
				spawn_enemy(app, pos, dir, E_ZOOMER);
		}
	}
	free(line);
	free_split(split);
}

void	next_weapon(t_player *player)
{
	int	next;

	next = (player->equipped + 1) % 3;
	while (player->ammo[next] == 0)
		next = (next + 1) % 3;
	player->equipped = next;
}

void	subtract_health(t_info *app, t_player *player, int damage)
{
	int	new_health;

	if (player->dead == 1)
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
}

void	add_health(t_player *player, int health)
{
	int	new_health;

	new_health = player->health + health;
	if (new_health > player->max_health)
		new_health = player->max_health;
	player->health = new_health;
}

void	add_ammo(t_player *player, int type)
{
	int	new_ammo;

	if (type == MISSILE)
		new_ammo = player->ammo[MISSILE] + 5;
	else
		new_ammo = player->ammo[SUPER] + 1;
	if (new_ammo > player->max_ammo[type])
		new_ammo = player->max_ammo[type];
	player->ammo[type] = new_ammo;
}

void	toggle_boss_doors(t_info *app)
{
	t_anim	**anims;
	char	**map;
	int		i;
	int		j;

	anims = app->map->anims;
	map = app->map->map;
	i = -1;
	while (++i < app->map->height)
	{
		j = -1;
		while (++j < app->map->width)
		{
			if (map[i][j] == 'B')
			{
				anims[i][j].active = 1;
				anims[i][j].framestart = app->framecount;
				// if (app->map->boss_active == 0)
				// 	app->map->boss_active = 1;
				// else
				// 	app->map->boss_active = 0;
			}
		}
	}
}

void	damage_enemy(t_info *app, t_object *enemy, int damage)
{
	enemy->health -= damage;
	if (enemy->health <= 0)
	{
		enemy->dead = 1;
		enemy->anim2.framestart = app->framecount;
		enemy->anim2.active = 1;
		if (enemy->subtype == E_PHANTOON)
		{
			app->map->boss_active = 0;
			toggle_boss_doors(app);
		}
	}
}


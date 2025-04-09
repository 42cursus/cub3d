/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   player.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/12 19:31:02 by fsmyth            #+#    #+#             */
/*   Updated: 2025/04/08 22:59:46 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/cub3d.h"
#include <unistd.h>

void	calculate_offsets(t_player *player)
{
	int		i;
	double	offset;
	double	angle;

	offset = 1.0 / (WIN_WIDTH / 2.0);
	i = 0;
	while (i < WIN_WIDTH / 2)
	{
		angle = atan(1.0 - (i * offset));
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

void	move_player(t_player *player, char **map, t_vect dir)
{
	double	new_x;
	double	new_y;
	char	x_tile;
	char	y_tile;
	char	both_tile;

	new_x = player->pos.x + (dir.x * 0.1);
	new_y = player->pos.y + (dir.y * 0.1);
	x_tile = map[(int)player->pos.y][(int)new_x];
	y_tile = map[(int)new_y][(int)player->pos.x];
	both_tile = map[(int)new_y][(int)new_x];
	if (both_tile == '0' || both_tile == 'O')
	{
		if (x_tile == '0' || x_tile == 'O')
			player->pos.x = new_x;
		if (y_tile == '0' || y_tile == 'O')
			player->pos.y = new_y;
	}
	else
	{
		if ((x_tile == '0' || x_tile == 'O') && (y_tile == '0' || y_tile == 'O'))
		{
			if (get_max_direction(dir) == 'x')
				player->pos.x = new_x;
			else
				player->pos.y = new_y;
		}
		else if (x_tile == '0' || x_tile == 'O')
			player->pos.x = new_x;
		else if (y_tile == '0' || y_tile == 'O')
			player->pos.y = new_y;
	}
}

void	rotate_player(t_player *player, int direction, double sensitivity)
{
	if (direction == 0)
		rotate_vect_inplace(&player->dir, M_PI_4 / sensitivity);
	else
		rotate_vect_inplace(&player->dir, -M_PI_4 / sensitivity);
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
	if (subtype == BEAM)
		projectile->dir = scale_vect(player->dir, 0.5);
	else if (subtype == SUPER)
	{
		player->ammo[SUPER] -= 1;
		if (player->ammo[SUPER] == 0)
			player->equipped = BEAM;
		projectile->dir = scale_vect(player->dir, 0.3);
	}
	projectile->type = O_PROJ;
	projectile->anim.active = 0;
	ft_lstadd_front(&map->objects, ft_lstnew(projectile));
}

void	spawn_enemy(t_info *app, t_vect pos, t_vect dir, int subtype)
{
	t_object	*enemy;
	t_data		*map;

	map = app->map;
	enemy = ft_calloc(1, sizeof(*enemy));
	enemy->pos = pos;
	enemy->dir = dir;
	// enemy->texture = tex;
	enemy->type = O_ENTITY;
	enemy->subtype = subtype;
	enemy->anim.active = 1;
	enemy->anim.framestart = app->framecount;
	ft_lstadd_front(&map->objects, ft_lstnew(enemy));
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
	ft_lstadd_front(&map->objects, ft_lstnew(item));
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
	ft_printf("%s\n", line);
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
		}
		if (ft_strcmp(split[1], "enemy") == 0)
		{
			val = ft_atoi(split[3]);
			ft_printf("%d\n", val);
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

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   player.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/12 19:31:02 by fsmyth            #+#    #+#             */
/*   Updated: 2025/05/16 14:21:39 by fsmyth           ###   ########.fr       */
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

void	calculate_offsets(t_info *app, t_player *player)
{
	int				i;
	double		offset;
	double		angle;
	double		distance;
	double		scalar;

	offset = app->fov_opp_len / (WIN_WIDTH / 2.0);
	i = 0;
	while (i < WIN_WIDTH / 2)
	{
		angle = atan(app->fov_opp_len - (i * offset));
		// angle = M_PI_4 * (halfwidth - i) * offset;
		player->angle_offsets[i] = angle;
		player->angle_offsets[WIN_WIDTH - i - 1] = -angle;
		i++;
	}
	scalar = get_hyp_len(app->fov_opp_len, 1);
	i = 0;
	int j = WIN_HEIGHT / 2;
	while (i < WIN_HEIGHT / 2)
	{
		distance = WIN_WIDTH / (4.0 * (i + 1) * app->fov_opp_len);
		player->floor_offsets[i++] = distance * scalar;
		player->ceil_offsets[--j] = distance * scalar;
	}
}

void	calculate_credits_offset(t_info *app, t_dummy *dummy)
{
	int				i;
	double		distance;
	double		scalar;

	scalar = get_hyp_len(app->fov_opp_len, 1);
	i = 0;
	while (i < WIN_HEIGHT)
	{
		distance = WIN_WIDTH / (4.0 * (i + 1) * app->fov_opp_len);
		dummy->credits_offsets[i++] = distance * scalar;
	}
}

void	set_sensitivity(t_info *app, int sensitivity)
{
	if (sensitivity < 1)
		sensitivity = 1;
	if (sensitivity > 10)
		sensitivity = 10;
	app->sensitivity = sensitivity;
}

void	set_fov(t_info *app, int fov)
{
	if (fov < 45)
		fov = 45;
	else if (fov > 140)
		fov = 140;
	app->fov_deg = fov;
	app->fov_rad_half = ((double)fov / 360.0) * M_PI;
	app->fov_opp_len = tan(app->fov_rad_half);
}

void	set_framerate(t_info *app, size_t framerate)
{
	if (framerate < 30)
		framerate = 30;
	else if (framerate > 500)
		framerate = 500;
	app->framerate = framerate;
	app->fr_delay = 1000000 / framerate;
	app->fr_scale = framerate / 50.0;
	printf("framerate: %ld frametime: %ld fr_scale: %f\n", app->framerate, app->fr_delay, app->fr_scale);
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

static inline __attribute__((always_inline, unused))
int	point_oob(t_vect pos, t_data *map)
{
	return ((pos.x < 0 || pos.x > map->width) || (pos.y < 0 || pos.y > map->height));
}

void	move_entity(t_info *app, t_vect *pos, t_data *data, t_vect dir)
{
	t_vect	new_pos;
	t_cvect	tile;
	char	**map;
	char	both_tile;

	map = data->map;
	new_pos.x = pos->x + dir.x;
	new_pos.y = pos->y + dir.y;
	if (point_oob(new_pos, data))
		return ;
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
	(void)app;
}

void	move_obj_bounce(t_info *app, t_object *obj, t_data *data)
{
	t_vect	new_pos;
	char	**map;
	char	tile;

	new_pos = add_vect(obj->pos, scale_vect(obj->dir, obj->speed / app->fr_scale));
	map = data->map;
	tile = map[(int)new_pos.y][(int)new_pos.x];
	if (point_oob(new_pos, app->map) || !check_tile_open(tile, app->map))
		rotate_vect_inplace(&obj->dir, rand_range(-M_PI, M_PI));
	else
		obj->pos = new_pos;
}

void	rotate_player(t_info *app, t_player *player, int direction, double sensitivity)
{
	if (direction == 0)
		rotate_vect_inplace(&player->dir, M_PI_4 / (sensitivity * app->fr_scale));
	else
		rotate_vect_inplace(&player->dir, -M_PI_4 / (sensitivity * app->fr_scale));
	app->player->angle = atan2(app->player->dir.y, app->player->dir.x);
	if (app->map->outside)
		draw_sky_alt(app);
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

void	handle_open_door(t_info *app, t_ray *crosshair)
{
	// t_ray	*crosshair;
	t_anim	*anim;
	char	*doortile;

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
		anim->timestart = app->last_frame;
	}
	if (crosshair->in_front != NULL)
		handle_open_door(app, crosshair->in_front);
}

void	next_weapon(t_player *player)
{
	int	next;

	next = (player->equipped + 1) % 3;
	while (player->ammo[next] == 0)
		next = (next + 1) % 3;
	player->equipped = next;
}

void	prev_weapon(t_player *player)
{
	int	prev;

	prev = (player->equipped + 2) % 3;
	while (player->ammo[prev] == 0)
		prev = (prev + 2) % 3;
	player->equipped = prev;
}

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
				anims[i][j].timestart = app->last_frame;
			}
		}
	}
}

void	damage_enemy(t_info *app, t_object *enemy, int damage)
{
	enemy->health -= damage;
	enemy->attacking = 1;
	enemy->last_damaged = app->last_frame;
	if (enemy->health <= 0)
	{
		enemy->dead = 1;
		enemy->anim2.timestart = app->last_frame;
		enemy->anim2.active = 1;
		if (enemy->subtype == E_PHANTOON)
		{
			app->map->boss_active = 0;
			toggle_boss_doors(app);
		}
	}
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

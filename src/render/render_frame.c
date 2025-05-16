/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render_frame.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/04 18:07:08 by fsmyth            #+#    #+#             */
/*   Updated: 2025/04/25 13:45:23 by abelov           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"
#include <sys/time.h>
#include <sysexits.h>
#include <signal.h>

size_t	get_time_ms(void)
{
	struct timeval	current_time;

	gettimeofday(&current_time, NULL);
	return (current_time.tv_sec * 1000 + current_time.tv_usec / 1000);
}

size_t	get_time_us(void)
{
	struct timeval	current_time;

	gettimeofday(&current_time, NULL);
	return (current_time.tv_sec * 1000000 + current_time.tv_usec);
}

static inline __attribute__((always_inline, unused))
int	point_oob(t_vect pos, t_data *map)
{
	return ((pos.x < 0 || pos.x > map->width) || (pos.y < 0 || pos.y > map->height));
}

t_list	*delete_object(t_list **obj_list, t_list *obj_node)
{
	t_list	*current;
	t_list	*next;

	current = *obj_list;
	next = obj_node->next;
	if (current == obj_node)
	{
		free(obj_node->content);
		free(obj_node);
		*obj_list = next;
		return (next);
	}
	while (current->next != obj_node)
		current = current->next;
	free(obj_node->content);
	free(obj_node);
	current->next = next;
	return (next);
}

void	start_obj_death(t_object *obj, t_info *app)
{
	obj->anim2.active = 1;
	obj->anim2.timestart = app->last_frame;
}

t_texarr	*handle_animation(t_info *app, t_anim anim)
{
	const int	frame_len = anim.duration / anim.frames;
	const int	timediff = app->last_frame - anim.timestart;
	int			index;

	if (anim.frames == 1)
		return (anim.tex_arr);
	if (anim.loop)
	{
		index = timediff % anim.duration / frame_len;
		return (&anim.tex_arr[index]);
	}
	else
	{
		index = timediff / frame_len;
		if (index >= anim.frames)
			return (NULL);
		return (&anim.tex_arr[index]);
	}
}

t_object	*check_obj_proximity(t_vect pos, t_data *map)
{
	t_list		*current;
	t_object	*cur_obj;

	current = map->enemies;
	while (current != NULL)
	{
		cur_obj = (t_object *)current->data;
		if (cur_obj->dead != 1)
		{
			if (vector_distance(pos, cur_obj->pos) < 0.3)
				return (cur_obj);
		}
		current = current->next;
	}
	return (NULL);
}

void	select_missile_tex(t_object *obj, t_player *player, t_info *app)
{
	t_texarr	*tex;
	double		angle;
	int			index;

	if (obj->subtype == SUPER)
		tex = app->shtex->super_tex + 4;
	else
		tex = app->shtex->missile_tex + 4;
	angle = vector_angle(obj->dir, add_vect(player->dir,
										 normalise_vect(subtract_vect(obj->pos, player->pos))));
	index = (int)((angle + M_PI_4 / 2) / M_PI_4 + 8) % 8;
	obj->texture = &tex[index];
}

// void	calc_dmg_direction(t_info *app, t_object *obj, t_player *player)
// {
// 	// t_vect	dmg_dir;
// 	// double	angle;
//
// 	player->dmg_dir = (subtract_vect(obj->pos, player->pos));
// 	// angle = vector_angle(player->dir, dmg_dir);
// 	// player->dmg_dir = (int)((angle + M_PI_4 / 2) / M_PI_4 + 8) % 8;
// 	// printf("dmg_dir: %d\n", player->dmg_dir);
// 	(void)app;
// }

int	handle_obj_projectile(t_info *app, t_object *obj, t_list **current)
{
	char		*tile;
	t_anim		*anim;
	t_vect		new_pos;
	t_object	*closest;

	if (obj->anim2.active == 1)
	{
		obj->texture = handle_animation(app, obj->anim2);
		if (obj->texture == NULL)
		{
			*current = delete_object(&app->map->projectiles, *current);
			return (1);
		}
		return (0);
	}
	closest = check_obj_proximity(obj->pos, app->map);
	if (closest != NULL)
	{
		start_obj_death(obj, app);
		if (obj->subtype == BEAM)
			damage_enemy(app, closest, 10);
		else if (obj->subtype == SUPER)
			damage_enemy(app, closest, 50);
		else if (obj->subtype == MISSILE)
			damage_enemy(app, closest, 30);
		return (0);
	}
	if (obj->subtype == BEAM)
		obj->texture = &app->shtex->proj_tex[0];
	else
		select_missile_tex(obj, app->player, app);
	new_pos = add_vect(obj->pos, obj->dir);
	if (!point_oob(new_pos, app->map))
	{
		tile = &app->map->map[(int) new_pos.y][(int) new_pos.x];
		if (!check_tile_open(*tile, app->map))
		{
			anim = &app->map->anims[(int) new_pos.y][(int) new_pos.x];
			if (*tile == 'D')
			{
				*tile = 'O';
				anim->active = 1;
				anim->timestart = app->last_frame;
			}
			else if (*tile == 'L')
			{
				if (obj->subtype == SUPER)
				{
					*tile = 'O';
					anim->active = 1;
					anim->timestart = app->last_frame;
				}
			}
			else if (*tile == 'M')
			{
				if (obj->subtype != BEAM)
				{
					*tile = 'O';
					anim->active = 1;
					anim->timestart = app->last_frame;
				}
			}
			start_obj_death(obj, app);
		}
		else
			obj->pos = new_pos;
	}
	else
		obj->pos = new_pos;
	return (0);
}

int	handle_enemy_projectile(t_info *app, t_object *obj, t_list **current)
{
	char		*tile;
	t_vect		new_pos;

	if (obj->anim2.active == 1)
	{
		obj->texture = handle_animation(app, obj->anim2);
		if (obj->texture == NULL)
		{
			*current = delete_object(&app->map->projectiles, *current);
			return (1);
		}
		return (0);
	}
	obj->texture = handle_animation(app, obj->anim);
	if (vector_distance(obj->pos, app->player->pos) < 0.2)
	{
		subtract_health(app, app->player, 20);
		// calc_dmg_direction(app, obj, app->player);
		app->player->dmg_dir = (subtract_vect(obj->pos, app->player->pos));
		start_obj_death(obj, app);
	}
	new_pos = add_vect(obj->pos, obj->dir);
	tile = &app->map->map[(int)new_pos.y][(int)new_pos.x];
	if (ft_strchr("1LDM", *tile))
		start_obj_death(obj, app);
	else
		obj->pos = new_pos;
	return (0);
}

double	rand_range(double lower, double upper)
{
	double	output;
	double	diff;

	diff = upper - lower;
	output = (rand() / (RAND_MAX / diff)) + lower;
	return (output);
}

void	spawn_drops(t_info *app, t_object *obj, int no)
{
	double	seed;
	t_vect	pos;

	if (app->player->ammo[SUPER] == app->player->max_ammo[SUPER]
		&& app->player->ammo[MISSILE] == app->player->max_ammo[MISSILE]
		&& app->player->health == app->player->max_health)
		return ;
	while (no-- > 0)
	{
		pos = obj->pos;
		move_entity(app, &pos, app->map, (t_vect){rand_range(-0.5, 0.5), rand_range(-0.5, 0.5)});
		seed = rand_range(0.0, 1.0);
		// printf("%f\n", seed);
		if (seed < 0.2 && app->player->ammo[SUPER] != app->player->max_ammo[SUPER])
			spawn_item(app, pos, I_AMMO_S);
		else if (seed > 0.2 && seed < 0.5 && app->player->ammo[MISSILE] != app->player->max_ammo[MISSILE])
			spawn_item(app, pos, I_AMMO_M);
		else if (app->player->health != app->player->max_health)
			spawn_item(app, pos, I_HEALTH);
		else
			no++;
	}
}

void	phantoon_ai(t_info *app, t_object *obj)
{
	int		frames;
	t_vect	norm_diff;

	if (app->map->boss_active == 0 || app->player->dead == 1)
		return ;
	norm_diff = normalise_vect(subtract_vect(app->player->pos, obj->pos));
	frames = ((app->last_frame / 20000) % 100);
	if (obj->health > 350)
	{
		rotate_vect_inplace(&obj->dir, M_PI_4 / 20);
		if (frames % 10 == 0)
			spawn_enemy_projectile(app, obj, scale_vect(rotate_vect(norm_diff, rand_range(-0.2, 0.2)), 0.1 / app->fr_scale), P_PHANTOON);
		move_obj_bounce(app, obj, app->map);
	}
	else if (obj->health > 100)
	{
		if (frames == 0)
		{
			obj->dir = rotate_vect(norm_diff, rand_range(-0.1, 0.1));
			obj->speed = 0.09;
		}
		else if (frames > 60)
		{
			obj->speed = 0;
			if (frames % 3 == 0)
				spawn_enemy_projectile(app, obj, scale_vect(rotate_vect(norm_diff, rand_range(-0.3, 0.3)), 0.1 / app->fr_scale), P_PHANTOON);
		}
		move_obj_bounce(app, obj, app->map);
	}
	else
	{
		obj->dir = norm_diff;
		obj->speed = 0.09;
		move_entity(app, &obj->pos, app->map, scale_vect(obj->dir, obj->speed / app->fr_scale));
	}
}

int	check_line_of_sight(t_info *app, t_object *obj, t_player *player)
{
	const t_vect	diff = subtract_vect(obj->pos, player->pos);
	const double	angle = atan2(diff.y, diff.x) - player->angle;
	t_ray			ray;
	t_ray			*child;
	double			dist_wall;
	double			dist_obj;

	ray = ray_dda(app, app->map, player, angle);
	dist_wall = vector_distance(player->pos, ray.intcpt);
	dist_obj = vector_distance(player->pos, obj->pos);
	if (dist_obj > dist_wall)
		// return (printf("\e[31mNOT VISIBLE: \e[33mDISTANCE wall: %f obj: %f\e[m\n", dist_wall, dist_obj), 0);
		return (0);
	child = ray.in_front;
	while (child != NULL)
	{
		if (child->face < 11)
			// return (printf("\e[31mNOT VISIBLE: \e[33mCHILDREN face: %d\e[m\n", child->face), 0);
			return (0);
		child = child->in_front;
	}
	// return (printf("\e[32mVISIBLE\e[m\n"), 1);
	return (1);
}

void	reo_ai(t_info *app, t_object *enemy)
{
	int		frames;
	t_vect	norm_diff;

	// if (!check_line_of_sight(app, enemy, app->player))
	// 	enemy->attacking = 0;
	// else if (vector_distance(enemy->pos, app->player->pos) < 4)
	// 	enemy->attacking = 1;
	if (check_line_of_sight(app, enemy, app->player) && vector_distance(enemy->pos, app->player->pos) < 6)
		enemy->attacking = 1;
	frames = ((app->last_frame / 20000) % 100);
	if (enemy->attacking == 0 || app->player->dead == 1)
	{
		enemy->anim.tex_arr = app->shtex->reo_tex;
		enemy->speed = 0.04;
		if (frames % 25 == 0)
			rotate_vect_inplace(&enemy->dir, rand_range(-M_PI, M_PI));
		move_obj_bounce(app, enemy, app->map);
	}
	else
	{
		enemy->anim.tex_arr = &app->shtex->reo_tex[2];
		norm_diff = normalise_vect(subtract_vect(app->player->pos, enemy->pos));
		enemy->dir = norm_diff;
		enemy->speed = 0.1;
		move_entity(app, &enemy->pos, app->map, scale_vect(enemy->dir, enemy->speed / app->fr_scale));
	}
}

void	atomic_ai(t_info *app, t_object *enemy)
{
	// int		frames;
	t_vect	norm_diff;

	// if (enemy->attacking == 0 && vector_distance(enemy->pos, app->player->pos) < 3)
	// 	enemy->attacking = 1;
	// frames = (app->framecount % (int)(100 * app->fr_scale));
	if (enemy->attacking == 1 && app->player->dead == 0)
	{
		norm_diff = normalise_vect(subtract_vect(app->player->pos, enemy->pos));
		enemy->dir = norm_diff;
		enemy->speed = 0.08;
		move_entity(app, &enemy->pos, app->map, scale_vect(enemy->dir, enemy->speed / app->fr_scale));
	}
	else
	{
		enemy->speed = 0.04;
		move_obj_bounce(app, enemy, app->map);
	}
}

void	holtz_ai(t_info *app, t_object *enemy, t_player *player)
{
	int		frames;
	t_vect	norm_diff;

	frames = ((app->last_frame / 20000) % 100);
	norm_diff = normalise_vect(subtract_vect(app->player->pos, enemy->pos));
	if (!check_line_of_sight(app, enemy, player))
		enemy->attacking = 0;
	else if (vector_distance(enemy->pos, player->pos) < 5)
		enemy->attacking = 1;
	if (frames % 20 == 0 && enemy->attacking == 1)
		spawn_enemy_projectile(app, enemy, scale_vect(norm_diff, 0.2 / app->fr_scale), P_HOLTZ);
	move_obj_bounce(app, enemy, app->map);
}

void	zoomer_ai(t_info *app, t_object *enemy)
{
	move_obj_bounce(app, enemy, app->map);
}

int	handle_obj_entity(t_info *app, t_object *obj, t_list **current)
{
	if (obj->dead == 1)
	{
		obj->texture = handle_animation(app, obj->anim2);
		if (obj->texture == NULL)
		{
			if (obj->subtype == E_PHANTOON)
			{
				spawn_drops(app, obj, 15);
				app->map->boss_obj = NULL;
			}
			else if (obj->subtype == E_HOLTZ)
				spawn_drops(app, obj, 3);
			else //if (obj->subtype == E_HOLTZ)
				spawn_drops(app, obj, 1);
			*current = delete_object(&app->map->enemies, *current);
			return (1);
		}
		return (0);
	}
	if (obj->subtype == E_ZOOMER)
		zoomer_ai(app, obj);
	else if (obj->subtype == E_REO)
		reo_ai(app, obj);
	else if (obj->subtype == E_PHANTOON)
		phantoon_ai(app, obj);
	else if (obj->subtype == E_ATOMIC)
		atomic_ai(app, obj);
	else if (obj->subtype == E_HOLTZ)
		holtz_ai(app, obj, app->player);
	obj->texture = handle_animation(app, obj->anim);
	if (vector_distance(obj->pos, app->player->pos) < 0.5 && !app->player->dead)
	{
		if (obj->subtype != E_ATOMIC)
			subtract_health(app, app->player, 35);
		else
		{
			subtract_health(app, app->player, 80);
			damage_enemy(app, obj, 100);
		}
		move_entity(app, &app->player->pos, app->map, scale_vect(subtract_vect(app->player->pos, obj->pos), 1));
		app->player->dmg_dir = (subtract_vect(obj->pos, app->player->pos));
	}
	return (0);
	(void)current;
}

int	handle_obj_item(t_info *app, t_object *obj, t_list **current)
{
	t_data		*map;
	t_player	*player;

	map = app->map;
	player = app->player;
	obj->texture = handle_animation(app, obj->anim);
	if (vector_distance(player->pos, obj->pos) < 0.5)
	{
		if (obj->subtype == I_ETANK)
		{
			player->max_health += 100;
			player->health += 100;
			player->pickups_collected++;
		}
		else if (obj->subtype == I_SUPER)
		{
			player->max_ammo[SUPER] += 5;
			player->ammo[SUPER] += 5;
			player->pickups_collected++;
		}
		else if (obj->subtype == I_MISSILE)
		{
			player->max_ammo[MISSILE] += 10;
			player->ammo[MISSILE] += 10;
			player->pickups_collected++;
		}
		else if (obj->subtype == I_HEALTH)
			add_health(player, 20);
		else if (obj->subtype == I_AMMO_M)
		{
			if (app->player->ammo[MISSILE] == app->player->max_ammo[MISSILE])
				return (0);
			add_ammo(player, MISSILE);
		}
		else if (obj->subtype == I_AMMO_S)
		{
			if (app->player->ammo[SUPER] == app->player->max_ammo[SUPER])
				return (0);
			add_ammo(player, SUPER);
		}
		else if (obj->subtype == I_TROPHY)
		{
			app->rc = ok;
			app->mlx->end_loop = 1;
		}
		*current = delete_object(&map->items, *current);
		return (1);
	}
	return (0);
}

int	handle_trigger(t_info *app, t_object *obj, t_list **current)
{
	if (obj->subtype == T_BOSS)
	{
		if (vector_distance(obj->pos, app->player->pos) < 1.5)
		{
			toggle_boss_doors(app);
			app->map->boss_active = 1;
			app->map->boss_obj->dir = (t_vect){0.0, 1.0};
			*current = delete_object(&app->map->triggers, *current);
			return (1);
		}
	}
	return (0);
}

void	handle_tele(t_info *app, t_object *tele)
{
	if (vector_distance(app->player->pos, tele->pos) < 0.4)
	{
		app->current_sublevel = tele->subtype;
		app->rc = extra;
		app->mlx->end_loop = 1;
		app->player->tele_pos = tele->pos;
	}
}

void	update_objects(t_info *app, t_player *player, t_data *map)
{
	t_list		*current;
	t_object	*obj;

	current = map->enemies;
	while (current != NULL)
	{
		obj = (t_object *)current->data;
		if (obj->type == O_ENTITY && handle_obj_entity(app, obj, &current))
			continue ;
		obj->norm = rotate_vect(scale_vect(player->dir, 0.5), M_PI_2);
		obj->p2 = add_vect(obj->pos, obj->norm);
		current = current->next;
	}
	current = map->projectiles;
	while (current != NULL)
	{
		obj = (t_object *)current->data;
		if (obj->type == O_PROJ && handle_obj_projectile(app, obj, &current))
			continue ;
		else if (obj->type == O_EPROJ && handle_enemy_projectile(app, obj, &current))
			continue ;
		obj->norm = rotate_vect(scale_vect(player->dir, 0.5), M_PI_2);
		obj->p2 = add_vect(obj->pos, obj->norm);
		current = current->next;
	}
	current = map->items;
	while (current != NULL)
	{
		obj = (t_object *)current->data;
		if (obj->type == O_ITEM && handle_obj_item(app, obj, &current))
			continue ;
		obj->norm = rotate_vect(scale_vect(player->dir, 0.5), M_PI_2);
		obj->p2 = add_vect(obj->pos, obj->norm);
		current = current->next;
	}
	current = map->triggers;
	while (current != NULL)
	{
		obj = (t_object *)current->data;
		if (obj->type == O_TRIGGER && handle_trigger(app, obj, &current))
			continue ;
		if (obj->type == O_TELE)
			handle_tele(app, obj);
		obj->norm = rotate_vect(scale_vect(player->dir, 0.5), M_PI_2);
		obj->p2 = add_vect(obj->pos, obj->norm);
		current = current->next;
	}
}

void fill_with_colour(t_img *img, int f_col, int c_col)
{
	const int	mid = img->height / 2;
	int			i;
	int			j;

	u_int (*pixels)[img->height][img->width] = (void *)img->data;
	i = -1;
	while (++i <= mid)
	{
		j = -1;
		while (++j < img->width)
			(*pixels)[i][j] = c_col;
	}
	i--;
	while (++i < img->height)
	{
		j = -1;
		while (++j < img->width)
			(*pixels)[i][j] = f_col;
	}
}

int	render_win(void *param)
{
	t_info *const app = param;
	size_t			time;

	fast_memcpy_test((int *)app->canvas->data, (int *)app->bg->data, WIN_HEIGHT * WIN_WIDTH * sizeof(int));
	// free_ray_children(&app->player->rays[WIN_WIDTH / 2]);
	update_objects(app, app->player, app->map);
	replace_frame(app);
	place_texarr(app, &app->shtex->title, (WIN_WIDTH - app->shtex->title.x) / 2, 100);
	draw_menu_items(app);
	while (get_time_us() - app->last_frame < app->fr_delay)
		usleep(100);
	time = get_time_us();
	app->frametime = time - app->last_frame;
	app->last_frame = time;
	app->framecount++;
	on_expose(app);
	return (0);
}

int	render_lose(void *param)
{
	size_t			time;
	t_info *const	app = param;

	fast_memcpy_test((int *)app->canvas->data, (int *)app->bg->data, WIN_HEIGHT * WIN_WIDTH * sizeof(int));
	// free_ray_children(&app->player->rays[WIN_WIDTH / 2]);
	update_objects(app, app->player, app->map);
	replace_frame(app);
	place_texarr(app, &app->shtex->title, (WIN_WIDTH - app->shtex->title.x) / 2, 100);
	draw_menu_items(app);
	while (get_time_us() - app->last_frame < app->fr_delay)
		usleep(100);
	time = get_time_us();
	app->frametime = time - app->last_frame;
	app->last_frame = time;
	app->framecount++;
	on_expose(app);
	return (0);
}

int	render_load(void *param)
{
	size_t				time;
	t_info *const app = param;

	fast_memcpy_test((int *)app->canvas->data, (int *)app->bg->data, WIN_HEIGHT * WIN_WIDTH * sizeof(int));
	place_str_centred((char *)	"LOADING", app, (t_ivect){WIN_WIDTH / 2, 400}, 2);
	// while (get_time_us() - app->last_frame < app->fr_delay)
	// 	usleep(100);
	time = get_time_us();
	app->frametime = time - app->last_frame;
	app->last_frame = time;
	app->framecount++;
	if (app->framecount == app->framerate / 4)
	{
		app->rc = ok;
		app->mlx->end_loop = 1;
	}
	on_expose(app);
	return (0);
}

void draw_sky_alt(t_info *const app)
{
	int				i;
	// int				j;
	const double	angle = atan2(app->player->dir.y, app->player->dir.x);
	t_img	*const	sky = app->skybox;
	t_img	*const	bg = app->bg;
	int 			offset;

	app->player->angle = angle;
	offset = (int)((angle - app->fov_rad_half * 2) * (sky->width / M_PI)) / 2;

	u_int (*const pixels_sky)[sky->height][sky->width] = (void *)sky->data;
	u_int (*const pixels_bg)[bg->height][bg->width] = (void *)bg->data;

	int start_x;
	int end_x;

	start_x = (0 - offset + sky->width) % sky->width;
	end_x = (WIN_WIDTH - 1 - offset + sky->width) % sky->width;
	// end_x = start_x + WIN_WIDTH - 1;
	if (end_x > start_x)
	{
		i = -1;
		while(++i < sky->height)
		{
			// fast_memcpy_test(&(*pixels_bg)[i][0], &(*pixels_sky)[i][start_x], WIN_WIDTH);
			ft_memcpy(&(*pixels_bg)[i][0], &(*pixels_sky)[i][start_x], WIN_WIDTH * sizeof(u_int));
		}
	}
	else
	{
		i = -1;
		while(++i < sky->height)
		{
			int	copy1_width = sky->width - start_x;
			ft_memcpy(&(*pixels_bg)[i][0], &(*pixels_sky)[i][start_x], copy1_width * sizeof(u_int));
			ft_memcpy(&(*pixels_bg)[i][copy1_width], &(*pixels_sky)[i][0], (end_x + 1) * sizeof(u_int));
		}
	}
	// i = -1;
	// while(++i < sky->height)
	// {
	// 	j = -1;
	// 	while (++j < WIN_WIDTH)
	// 	{
	// 		start_x = (j - offset + sky->width) % sky->width;
	// 		(*pixels_bg)[i][j] = (*pixels_sky)[i][start_x];
	// 	}
	// }
}

void draw_sky(t_info *const app)
{
	int				i;
	int				j;
	const double	angle = app->player->angle;
	t_img	*const	sky = app->skybox;
	t_img	*const	bg = app->canvas;
	int 			offset;

	offset = (int)((angle - app->fov_rad_half * 2) * (sky->width / M_PI)) / 2;

	u_int (*const pixels_sky)[sky->height][sky->width] = (void *)sky->data;
	u_int (*const pixels_bg)[bg->height][bg->width] = (void *)bg->data;

	int start_x;

	i = -1;
	while(++i < sky->height)
	{
		j = -1;
		while (++j < WIN_WIDTH)
		{
			start_x = (j - offset + sky->width) % sky->width;
			(*pixels_bg)[i][j] = (*pixels_sky)[i][start_x];
		}
	}
}

int	render_play(void *param)
{
	size_t				time;
	t_info *const app = param;

	// if (app->keys[idx_XK_e])
	// 	handle_open_door(app, &app->player->rays[WIN_WIDTH / 2]);
	// free_ray_children(&app->player->rays[WIN_WIDTH / 2]);
	 // if (app->mouse[1])
	 // 	spawn_projectile(app, app->player, app->map);
	if (app->keys[idx_XK_w])
		move_entity(app, &app->player->pos, app->map, scale_vect(app->player->dir, 0.1 / app->fr_scale));
	if (app->keys[idx_XK_s])
		move_entity(app, &app->player->pos, app->map,
					scale_vect(rotate_vect(app->player->dir, M_PI), 0.1 / app->fr_scale));
	if (app->keys[idx_XK_a])
		move_entity(app, &app->player->pos, app->map,
					scale_vect(rotate_vect(app->player->dir, M_PI_2), 0.1 / app->fr_scale));
	if (app->keys[idx_XK_d])
		move_entity(app, &app->player->pos, app->map,
					scale_vect(rotate_vect(app->player->dir, -M_PI_2), 0.1 / app->fr_scale));
	if (app->keys[idx_XK_Right] && !app->keys[idx_XK_Left])
		rotate_player(app, app->player, 1, 12);
	if (app->keys[idx_XK_Left])
		rotate_player(app, app->player, 0, 12);

	// free_ray_children(&app->player->rays[WIN_WIDTH / 2]);
	update_objects(app, app->player, app->map);
	replace_frame(app);
	while (get_time_us() - app->last_frame < app->fr_delay)
		usleep(100);
	time = get_time_us();
	app->frametime = time - app->last_frame;
	app->last_frame = time;
	app->fr_scale = 20000.0/app->frametime;
	app->framecount++;
	on_expose(app);
	draw_hud(app);
	return (0);
}

int	render_mmenu(void *param)
{
	size_t				time;
	t_info *const app = param;

	fast_memcpy_test((int *)app->canvas->data, (int *)app->bg->data, WIN_WIDTH * WIN_HEIGHT * sizeof(int));

	place_texarr(app, &app->shtex->title, (WIN_WIDTH - app->shtex->title.x) / 2, 100);
	draw_menu_items(app);

	while (get_time_us() - app->last_frame < app->fr_delay)
		usleep(100);
	time = get_time_us();
	app->frametime = time - app->last_frame;
	app->last_frame = time;

	on_expose(app);
	return (0);
}

int render_pmenu(void *param)
{
	size_t				time;
	t_info *const app = param;

	fast_memcpy_test((int *)app->canvas->data, (int *)app->stillshot->data, WIN_HEIGHT * WIN_WIDTH * sizeof(int));
	place_texarr(app, &app->shtex->title, (WIN_WIDTH - app->shtex->title.x) / 2, 100);
	draw_menu_items(app);
	while (get_time_us() - app->last_frame < app->fr_delay)
		usleep(100);
	time = get_time_us();
	app->frametime = time - app->last_frame;
	app->last_frame = time;
	on_expose(app);
	return (0);
}

static inline __attribute__((always_inline))
void	my_put_pixel_32(t_img *img, int x, int y, unsigned int colour)
{
	if (colour == MLX_TRANSPARENT)
		return ;
	(*(unsigned int (*)[img->height][img->width])img->data)[y][x] = colour;
}

static inline __attribute__((always_inline))
int	interpolate_colour(int col1, int col2, double frac)
{
	int	r;
	int	g;
	int	b;

	if (col1 == col2)
		return (col1);
	r = ((col2 & MLX_RED) - (col1 & MLX_RED)) * frac + (col1 & MLX_RED);
	g = ((col2 & MLX_GREEN) - (col1 & MLX_GREEN)) * frac + (col1 & MLX_GREEN);
	b = ((col2 & MLX_BLUE) - (col1 & MLX_BLUE)) * frac + (col1 & MLX_BLUE);
	return ((r & MLX_RED) + (g & MLX_GREEN) + b);
}

static inline __attribute__((always_inline))
int	linear_filter_credits(t_vect idx, const t_texarr *tex)
{
	const double	frac = fmod(idx.x, 1);
	// const int		y_int =  floor(y);
	// const int		y_int =  (int)y;
	int				x_upper;

	x_upper = floor(idx.x) + 1;
	// if (x_lower + 1 == tex->x)
	// 	x_upper = 0;
	int col1 = tex->img[(int) idx.y][(int)floor(idx.x)];
	int col2 = tex->img[(int) idx.y][x_upper];
	return (interpolate_colour(col1, col2, frac));
}

void	draw_credits_row(t_info *app, t_vect l_pos, t_vect r_pos, int row)
{
	int				i;
	double			step_x;
	double			curr_x;
	t_vect			idx;
	// t_ivect			idx;
	const t_texarr		*tex = &app->shtex->credits;

	step_x = (r_pos.x - l_pos.x) / WIN_WIDTH;
	curr_x = l_pos.x;

	idx.y = (-l_pos.y) * tex->x;
	if (l_pos.y > 0 || idx.y > tex->y)
		return ;
	i = 0;
	while (i < WIN_WIDTH)
	{
		if (curr_x > -0.5 && curr_x < 0.5)
		{
			idx.x = (0.5 + curr_x) * tex->x;
			// my_put_pixel_32(app->canvas, i, row, tex->img[idx.y][idx.x]);
			// my_put_pixel_32(app->canvas, i, row, bilinear_filter(idx.x, idx.y, tex));
			my_put_pixel_32(app->canvas, i, row, linear_filter_credits(idx, tex));
		}
		curr_x += step_x;
		// idx.x += step_idx;
		i++;
	}
}

void	draw_credits(t_info *app, t_dummy *dummy)
{
	t_vect	l_dir;
	t_vect	r_dir;
	t_vect	l_pos;
	t_vect	r_pos;
	int		row;

	row = 0;
	l_dir = rotate_vect(dummy->dir, app->fov_rad_half);
	r_dir = rotate_vect(dummy->dir, -app->fov_rad_half);
	while (++row < WIN_HEIGHT)
	{
		double depth = dummy->credits_offsets[row - 1];
		l_pos = add_vect(dummy->pos, scale_vect(l_dir, depth));
		r_pos = add_vect(dummy->pos, scale_vect(r_dir, depth));
		draw_credits_row(app, l_pos, r_pos, row);
	}
}

int	render_credits(void *param)
{
	t_info *const	app = param;
	t_dummy			*dummy;
	size_t			time;

	dummy = app->dummy;
	if (app->keys[idx_XK_Up])
		dummy->pos.y += (dummy->speed * 5) / app->fr_scale;
	if (app->keys[idx_XK_Down])
		dummy->pos.y -= (dummy->speed * 3) / app->fr_scale;
	dummy->pos.y -= dummy->speed / app->fr_scale;
	if ((-dummy->pos.y) > ((double)app->shtex->credits.y / app->shtex->credits.x) + 2)
	{
		app->rc = ok;
		app->mlx->end_loop = 1;
	}
	fast_memcpy_test((int *)app->canvas->data, (int *)app->bg->data, WIN_WIDTH * WIN_HEIGHT * sizeof(int));
	draw_credits(app, dummy);
	while (get_time_us() - app->last_frame < app->fr_delay)
		usleep(100);
	time = get_time_us();
	app->frametime = time - app->last_frame;
	app->last_frame = time;
	app->fr_scale = 20000.0 / app->frametime;
	place_fps(app);
	on_expose(app);
	return (0);
}

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
	obj->anim.active = 1;
	obj->anim.framestart = app->framecount;
}

t_object	*check_obj_proximity(t_vect pos, t_data *map)
{
	t_list		*current;
	t_object	*cur_obj;

	current = map->objects;
	while (current != NULL)
	{
		cur_obj = (t_object *)current->data;
		if (cur_obj->type == O_ENTITY && cur_obj->dead != 1)
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
	// angle = vector_angle(obj->dir, subtract_vect(obj->pos, player->pos));
	// angle = vector_angle(obj->dir, subtract_vect(obj->pos, player->pos));
	angle = vector_angle(obj->dir, add_vect(player->dir,
										 normalise_vect(subtract_vect(obj->pos, player->pos))));
	index = (int)((angle + M_PI_4 / 2) / M_PI_4 + 8) % 8;
	// printf("angle: %f index: %d\n", angle, index);
	obj->texture = &tex[index];
}

int	handle_obj_projectile(t_info *app, t_object *obj, t_list **current)
{
	char		*tile;
	t_anim		*anim;
	int			frames;
	t_vect		new_pos;
	t_object	*closest;

	if (obj->anim.active == 1)
	{
		frames = (app->framecount - obj->anim.framestart) / FR_SCALE;
		if (frames > 19)
		{
			*current = delete_object(&app->map->objects, *current);
			return (1);
		}
		else if (obj->subtype == BEAM)
			obj->texture = &app->shtex->proj_tex[1 + (frames / 5)];
		else
			obj->texture = &app->shtex->proj_tex[5 + (frames / 4)];
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
	// else if (obj->subtype == SUPER)
	else
		select_missile_tex(obj, app->player, app);
	new_pos = add_vect(obj->pos, obj->dir);
	tile = &app->map->map[(int)new_pos.y][(int)new_pos.x];
	if (*tile == '1')
		start_obj_death(obj, app);
	else if (*tile == 'D')
	{
		anim = &app->map->anims[(int)new_pos.y][(int)new_pos.x];
		*tile = 'O';
		anim->active = 1;
		anim->framestart = app->framecount;
		start_obj_death(obj, app);
	}
	else if (*tile == 'L')
	{
		if (obj->subtype == SUPER)
		{
			anim = &app->map->anims[(int)new_pos.y][(int)new_pos.x];
			*tile = 'O';
			anim->active = 1;
			anim->framestart = app->framecount;
		}
		start_obj_death(obj, app);
	}
	else if (*tile == 'M')
	{
		if (obj->subtype != BEAM)
		{
			anim = &app->map->anims[(int)new_pos.y][(int)new_pos.x];
			*tile = 'O';
			anim->active = 1;
			anim->framestart = app->framecount;
		}
		start_obj_death(obj, app);
	}
	else
		// if (obj->subtype != SUPER)
			obj->pos = new_pos;
	return (0);
}

int	handle_enemy_projectile(t_info *app, t_object *obj, t_list **current)
{
	char		*tile;
	int			frames;
	t_vect		new_pos;

	if (obj->anim.active == 1)
	{
		frames = (app->framecount - obj->anim.framestart) / FR_SCALE;
		if (frames > 15)
		{
			*current = delete_object(&app->map->objects, *current);
			return (1);
		}
		else
			obj->texture = &app->shtex->phantoon_proj[2 + (frames / 4)];
		return (0);
	}
	else
	{
		frames = app->framecount % 12;
		obj->texture = &app->shtex->phantoon_proj[(frames / 4)];
	}
	// obj->texture = &app->shtex->proj_tex[0];
	if (vector_distance(obj->pos, app->player->pos) < 0.2)
	{
		subtract_health(app, app->player, 20);
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

void	handle_enemy_anim(t_info *app, t_object *enemy)
{
	int		frame_mod;

	if (enemy->subtype == E_ZOOMER)
	{
		frame_mod = (app->framecount - enemy->anim.framestart) % (30 * FR_SCALE);
		enemy->texture = &app->shtex->crawler_tex[frame_mod / (5 * FR_SCALE)];
	}
	else if (enemy->subtype == E_PHANTOON)
	{
		frame_mod = ((app->framecount - enemy->anim.framestart) % (80 * FR_SCALE)) / 8;
		if (frame_mod == 0)
			enemy->texture = &app->shtex->phantoon[0];
		else if (frame_mod == 1)
			enemy->texture = &app->shtex->phantoon[1];
		else if (frame_mod == 2)
			enemy->texture = &app->shtex->phantoon[2];
		else if (frame_mod == 3)
			enemy->texture = &app->shtex->phantoon[1];
		else if (frame_mod == 4)
			enemy->texture = &app->shtex->phantoon[0];
		else if (frame_mod == 5)
			enemy->texture = &app->shtex->phantoon[0];
		else if (frame_mod == 6)
			enemy->texture = &app->shtex->phantoon[3];
		else if (frame_mod == 7)
			enemy->texture = &app->shtex->phantoon[4];
		else if (frame_mod == 8)
			enemy->texture = &app->shtex->phantoon[3];
		else
			enemy->texture = &app->shtex->phantoon[0];
	}
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
		pos.x = rand_range(-0.5, 0.5);
		pos.y = rand_range(-0.5, 0.5);
		pos = (add_vect(pos, obj->pos));
		seed = rand_range(0.0, 1.0);
		// printf("%f\n", seed);
		if (seed < 0.2 && app->player->ammo[SUPER] != app->player->max_ammo[SUPER])
			spawn_item(app, pos, I_AMMO_S);
		else if (seed < 0.6 && app->player->ammo[MISSILE] != app->player->max_ammo[MISSILE])
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

	if (app->map->boss_active == 0)
		return ;
	norm_diff = normalise_vect(subtract_vect(app->player->pos, obj->pos));
	frames = (app->framecount % (100 * FR_SCALE));
	if (obj->health > 350)
	{
		rotate_vect_inplace(&obj->dir, M_PI_4 / 20);
		if (frames % (10 * FR_SCALE) == 0)
			spawn_enemy_projectile(app, obj, scale_vect(rotate_vect(norm_diff, rand_range(-0.2, 0.2)), 0.1 / FR_SCALE));
	}
	else if (obj->health > 100)
	{
		if (frames == 0)
			// obj->dir = rotate_vect((t_vect){0.08 / FR_SCALE, 0.0}, rand_range(-M_PI, M_PI));
			// obj->dir = rotate_vect(scale_vect(norm_diff, 0.08 / FR_SCALE), rand_range(-M_PI_2, M_PI_2));
			obj->dir = rotate_vect(scale_vect(norm_diff, 0.09 / FR_SCALE), rand_range(-0.1, 0.1));
		else if (frames > 60 * FR_SCALE)
		{
			obj->dir = (t_vect){0.0, 0.0};
			if (frames % (3 * FR_SCALE) == 0)
			spawn_enemy_projectile(app, obj, scale_vect(rotate_vect(norm_diff, rand_range(-0.3, 0.3)), 0.1 / FR_SCALE));
		}
	}
	else
		obj->dir = scale_vect(norm_diff, 0.09 / FR_SCALE);
	// (void)frames;
	// (void)norm_diff;
	// (void)app;
	// (void)obj;
}

int	handle_obj_entity(t_info *app, t_object *obj, t_list **current)
{
	char		*tile;
	// t_anim		*anim;
	t_data		*map;;
	t_vect		new_pos;
	int			frames;

	if (obj->dead == 1)
	{
		frames = (app->framecount - obj->anim2.framestart) / FR_SCALE;
		if (frames > 17)
		{
			if (obj->subtype == E_ZOOMER)
				spawn_drops(app, obj, 1);
			else if (obj->subtype == E_PHANTOON)
				spawn_drops(app, obj, 15);
			*current = delete_object(&app->map->objects, *current);
			return (1);
		}
		else
			obj->texture = &app->shtex->explode_tex[frames / 4];
		return (0);
	}
	new_pos = add_vect(obj->pos, obj->dir);
	if (obj->subtype == E_PHANTOON)
		phantoon_ai(app, obj);
	map = app->map;
	handle_enemy_anim(app, obj);
	tile = &map->map[(int)new_pos.y][(int)new_pos.x];
	if (ft_strchr("DML1", *tile))
	{
		// obj->dir.x *= -1;
		// obj->dir.y *= -1;
		rotate_vect_inplace(&obj->dir, rand_range(-M_PI, M_PI));
	}
	else
		obj->pos = new_pos;
	if (vector_distance(obj->pos, app->player->pos) < 0.5 && !app->player->dead)
	{
		subtract_health(app, app->player, 35);
		move_entity(&app->player->pos, app->map, scale_vect(subtract_vect(app->player->pos, obj->pos), 10));
		// app->player->pos = add_vect(app->player->pos,
		// 					  scale_vect(normalise_vect(subtract_vect(app->player->pos, obj->pos)), 0.7));
	}
	return (0);
	(void)current;
}

void	select_item_texture(t_info *app, t_object *obj)
{
	int			frames;
	t_texarr	*texp;

	frames = (app->framecount - obj->anim.framestart) / FR_SCALE;
	texp = app->shtex->etank_tex;
	if (obj->subtype == I_SUPER)
		texp = app->shtex->super_tex;
	else if (obj->subtype == I_MISSILE)
		texp = app->shtex->missile_tex;
	else if (obj->subtype == I_TROPHY)
		texp = app->shtex->trophy_tex;
	else if (obj->subtype == I_AMMO_M)
		texp = app->shtex->missile_ammo;
	else if (obj->subtype == I_AMMO_S)
		texp = app->shtex->super_ammo;
	else if (obj->subtype == I_HEALTH)
	{
		texp = app->shtex->health_pu;
		obj->texture = &texp[(frames % 20) / 5];
		return ;
	}
	if (frames % 10 < 5)
		obj->texture = &texp[0];
	else
		obj->texture = &texp[1];
}

int	handle_obj_item(t_info *app, t_object *obj, t_list **current)
{
	t_data		*map;
	t_player	*player;

	map = app->map;
	player = app->player;
	select_item_texture(app, obj);
	if (vector_distance(player->pos, obj->pos) < 0.5)
	{
		if (obj->subtype == I_ETANK)
		{
			player->max_health += 100;
			player->health += 100;
		}
		else if (obj->subtype == I_SUPER)
		{
			player->max_ammo[SUPER] += 5;
			player->ammo[SUPER] += 5;
		}
		else if (obj->subtype == I_MISSILE)
		{
			player->max_ammo[MISSILE] += 10;
			player->ammo[MISSILE] += 10;
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
		*current = delete_object(&map->objects, *current);
		return (1);
	}
	return (0);
}

int	handle_trigger(t_info *app, t_object *obj, t_list **current)
{
	if (obj->subtype == T_BOSS)
	{
		if (vector_distance(obj->pos, app->player->pos) < 0.6)
		{
			toggle_boss_doors(app);
			app->map->boss_active = 1;
			app->map->boss_obj->dir = (t_vect){0.0, 0.03};
			*current = delete_object(&app->map->objects, *current);
			return (1);
		}
	}
	return (0);
}

void	update_objects(t_info *app, t_player *player, t_data *map)
{
	t_list		*current;
	t_object	*obj;

	current = map->objects;
	while (current != NULL)
	{
		obj = (t_object *)current->data;
		if (obj->type == O_PROJ && handle_obj_projectile(app, obj, &current))
			continue ;
		if (obj->type == O_ENTITY && handle_obj_entity(app, obj, &current))
			continue ;
		if (obj->type == O_ITEM && handle_obj_item(app, obj, &current))
			continue ;
		if (obj->type == O_EPROJ && handle_enemy_projectile(app, obj, &current))
			continue ;
		if (obj->type == O_TRIGGER && handle_trigger(app, obj, &current))
			continue ;
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
	free_ray_children(&app->player->rays[WIN_WIDTH / 2]);
	update_objects(app, app->player, app->map);
	replace_frame(app);
	place_texarr(app, &app->shtex->title, (WIN_WIDTH - app->shtex->title.x) / 2, 100);
	draw_menu_items(app);
	while (get_time_us() - app->last_frame < FRAMETIME)
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
	free_ray_children(&app->player->rays[WIN_WIDTH / 2]);
	update_objects(app, app->player, app->map);
	replace_frame(app);
	place_texarr(app, &app->shtex->title, (WIN_WIDTH - app->shtex->title.x) / 2, 100);
	draw_menu_items(app);
	while (get_time_us() - app->last_frame < FRAMETIME)
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
	while (get_time_us() - app->last_frame < FRAMETIME)
		usleep(100);
	time = get_time_us();
	app->frametime = time - app->last_frame;
	app->last_frame = time;
	app->framecount++;
	if (app->framecount == FRAMERATE / 2)
	{
		app->rc = ok;
		app->mlx->end_loop = 1;
	}
	on_expose(app);
	return (0);
}

void draw_sky(t_info *const app)
{
	int				i;
	int				j;
	const double	angle = app->player->angle;
	t_img	*const	sky = app->skybox;
	t_img	*const	bg = app->canvas;
	int 			offset;

	offset = (int)((angle - M_PI_2) * (sky->width / M_PI)) / 2;

	u_int (*const pixels_sky)[sky->height][sky->width] = (void *)sky->data;
	u_int (*const pixels_bg)[bg->height][bg->width] = (void *)bg->data;

	int source_x;

	i = -1;
	while(++i < sky->height)
	{
		j = -1;
		while (++j < WIN_WIDTH)
		{
			source_x = (j - offset + sky->width) % sky->width;
			(*pixels_bg)[i][j] = (*pixels_sky)[i][source_x];
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
		move_entity(&app->player->pos, app->map, app->player->dir);
	if (app->keys[idx_XK_s])
		move_entity(&app->player->pos, app->map,
					rotate_vect(app->player->dir, M_PI));
	if (app->keys[idx_XK_a])
		move_entity(&app->player->pos, app->map,
					rotate_vect(app->player->dir, M_PI_2));
	if (app->keys[idx_XK_d])
		move_entity(&app->player->pos, app->map,
					rotate_vect(app->player->dir, -M_PI_2));
	if (app->keys[idx_XK_Right] && !app->keys[idx_XK_Left])
		rotate_player(app->player, 1, 12);
	if (app->keys[idx_XK_Left])
		rotate_player(app->player, 0, 12);

	free_ray_children(&app->player->rays[WIN_WIDTH / 2]);
	update_objects(app, app->player, app->map);
	replace_frame(app);
	while (get_time_us() - app->last_frame < FRAMETIME)
		usleep(100);
	time = get_time_us();
	app->frametime = time - app->last_frame;
	app->last_frame = time;
	app->framecount++;
	on_expose(app);
	draw_mmap(app);
	return (0);
}

int	render_mmenu(void *param)
{
	size_t				time;
	t_info *const app = param;

	fast_memcpy_test((int *)app->canvas->data, (int *)app->bg->data, WIN_WIDTH * WIN_HEIGHT * sizeof(int));

	place_texarr(app, &app->shtex->title, (WIN_WIDTH - app->shtex->title.x) / 2, 100);
	draw_menu_items(app);

	while (get_time_us() - app->last_frame < FRAMETIME)
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
	place_str_centred((char *)	"PAUSE", app, (t_ivect){WIN_WIDTH / 2, 400}, 4);
	place_str_centred((char *)	"PRESS [ESC] TO CONTINUE", app, (t_ivect){WIN_WIDTH / 2, 450}, 2);
	place_str_centred((char *)	"!\"#$%&'()*+,-./0123456789:;<=>?@", app, (t_ivect){WIN_WIDTH / 2, 500}, 3);
	place_str_centred((char *)	"abcdefghijklmnopqrstvwxyz[\\]^_`{|}~", app, (t_ivect){WIN_WIDTH / 2, 532}, 3);

	while (get_time_us() - app->last_frame < FRAMETIME)
		usleep(100);
	time = get_time_us();
	app->frametime = time - app->last_frame;
	app->last_frame = time;
	on_expose(app);
	return (0);
}

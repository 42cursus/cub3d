/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 15:16:24 by fsmyth            #+#    #+#             */
/*   Updated: 2025/06/04 23:48:38 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/param.h>
#include "SDL_mixer.h"
#include "cub3d.h"

size_t	count_split_words(char **split);
int		valid_identifier(char *str);
int		str_cmp_whitespace(void *data, void *ref);
t_list	*read_cub(int cubfd);

void	set_starting_dir(t_lvl *map, char dir)
{
	if (dir == 'N')
	{
		map->starting_dir.x = 0;
		map->starting_dir.y = 1;
	}
	else if (dir == 'S')
	{
		map->starting_dir.x = 0;
		map->starting_dir.y = -1;
	}
	else if (dir == 'E')
	{
		map->starting_dir.x = 1;
		map->starting_dir.y = 0;
	}
	else if (dir == 'W')
	{
		map->starting_dir.x = -1;
		map->starting_dir.y = 0;
	}
}

int	parse_texture(t_lvl *data, char *str, int identifier, t_info *app)
{
	t_texture	*tex_addr;
	t_img		tmp;

	if (identifier == NORTH)
		tex_addr = &data->n_tex;
	else if (identifier == SOUTH)
		tex_addr = &data->s_tex;
	else if (identifier == EAST)
		tex_addr = &data->e_tex;
	else if (identifier == WEST)
		tex_addr = &data->w_tex;
	else if (identifier == FLOOR)
	{
		tex_addr = &data->floor_tex;
		data->texs[T_FLOOR] = mlx_xpm_file_to_image(app->mlx, (char *) str, &tmp.width, &tmp.height);

	}
	else if (identifier == CEILING)
	{
		tex_addr = &data->ceil_tex;
		data->texs[T_CEILING] = mlx_xpm_file_to_image(app->mlx, (char *) str, &tmp.width, &tmp.height);
	}
	else
		return (1);
	if (tex_addr->data != NULL)
		return (printf("Error: texture defined multiple times\n"), 1);
	tex_addr->data = img_to_tex(app, str, &tex_addr->x, &tex_addr->y);
	return (0);
}

int	parse_music(t_lvl *lvl, char *str)
{
	if (lvl->music != NULL)
		return (printf("Error: music defined multiple times\n"), 1);
	lvl->music = Mix_LoadWAV(str);
	if (!lvl->music)
		return (printf("Error: failed to load music file\n"), 1);
	return (0);
}

int	parse_levels(t_lvl *data, char *str, int identifier)
{
	char	buf[100];
	int		len;

	len = ft_strrchr(data->sublvls[0], '/') - data->sublvls[0];
	ft_strlcpy(buf, data->sublvls[0], len + 2);
	ft_strlcat(buf, str, 100);
	data->sublvls[identifier + 1 - LVL_A] = ft_strdup(buf);
	return (0);
}

int	parse_line(t_lvl *data, char *line, t_info *app)
{
	char	**split;
	size_t	words;
	int		identifier;
	int		retval;

	split = ft_split(line, ' ');
	retval = 0;
	words = count_split_words(split);
	if (words != 2)
		return (printf("Error: invalid line format\n"), free_split(split), 1);
	identifier = valid_identifier(split[0]);
	if (!identifier)
		return (printf("Error: invalid line identifier\n"),
			free_split(split), 1);
//	else if (identifier == CEILING)
//		retval = parse_colour(data, split[1], identifier);
	else if (identifier <= WEST)
		retval = parse_texture(data, split[1], identifier, app);
	else if (identifier <= LVL_C)
		parse_levels(data, split[1], identifier);
	else
		retval = parse_music(data, split[1]);
	return (free_split(split), retval);
}

int	all_fields_parsed(t_lvl *lvl)
{
	if (lvl->n_tex.data == NULL)
		return (0);
	if (lvl->s_tex.data == NULL)
		return (0);
	if (lvl->e_tex.data == NULL)
		return (0);
	if (lvl->w_tex.data == NULL)
		return (0);
	if (lvl->floor_tex.data == NULL)
		return (0);
	if (lvl->ceil_tex.data == NULL)
	{
		lvl->outside = 1;
		return (1);
	}
//	 if (data->f_col == -1)
//	 	return (0);
//	if (data->c_col == -1)
//		return (0);
	return (1);
}

t_enemypos	*construct_enemypos(double x, double y, int type)
{
	t_enemypos	*new;

	new = ft_calloc(1, sizeof(*new));
	new->pos.x = x;
	new->pos.y = y;
	new->type = type;
	return (new);
}

void	spawn_map_objects(t_info *app, t_lvl *lvl)
{
	char	**map;
	int		i;
	int		j;

	map = lvl->map;
	i = -1;
	while (++i < lvl->height)
	{
		j = -1;
		while (++j < lvl->width)
		{
			if (ft_strchr("mestZAHRPb234", map[i][j]))
			{
				if (map[i][j] == 'm')
					spawn_item(app, (t_vect){j + 0.5, i + 0.5}, I_MISSILE);
				else if (map[i][j] == 't')
					spawn_item(app, (t_vect){j + 0.5, i + 0.5}, I_TROPHY);
				else if (map[i][j] == 'b')
					spawn_trigger(app, (t_vect){j + 0.5, i + 0.5}, T_BOSS);
				else if (map[i][j] == 's')
					spawn_item(app, (t_vect){j + 0.5, i + 0.5}, I_SUPER);
				else if (map[i][j] == 'e')
					spawn_item(app, (t_vect){j + 0.5, i + 0.5}, I_ETANK);
				else if (map[i][j] == 'Z')
				{
					spawn_enemy(app, (t_vect){j + 0.5, i + 0.5}, rotate_vect((t_vect){0.0, 1}, rand_range(-M_PI, M_PI)), E_ZOOMER);
					ft_lstadd_back(&lvl->enemy_pos, ft_lstnew(construct_enemypos(j + 0.5, i + 0.5, E_ZOOMER)));
				}
				else if (map[i][j] == 'A')
				{
					spawn_enemy(app, (t_vect){j + 0.5, i + 0.5}, rotate_vect((t_vect){0.0, 1}, rand_range(-M_PI, M_PI)), E_ATOMIC);
					ft_lstadd_back(&lvl->enemy_pos, ft_lstnew(construct_enemypos(j + 0.5, i + 0.5, E_ATOMIC)));
				}
				else if (map[i][j] == 'R')
				{
					spawn_enemy(app, (t_vect){j + 0.5, i + 0.5}, rotate_vect((t_vect){0, 1}, rand_range(-M_PI, M_PI)), E_REO);
					ft_lstadd_back(&lvl->enemy_pos, ft_lstnew(construct_enemypos(j + 0.5, i + 0.5, E_REO)));
				}
				else if (map[i][j] == 'P')
				{
					lvl->boss_obj = spawn_enemy(app, (t_vect){j + 0.5, i + 0.5}, (t_vect){0, 0}, E_PHANTOON);
					ft_lstadd_back(&lvl->enemy_pos, ft_lstnew(construct_enemypos(j + 0.5, i + 0.5, E_PHANTOON)));
				}
				else if (map[i][j] == 'H')
				{
					lvl->boss_obj = spawn_enemy(app, (t_vect){j + 0.5, i + 0.5}, (t_vect){0, 1}, E_HOLTZ);
					ft_lstadd_back(&lvl->enemy_pos, ft_lstnew(construct_enemypos(j + 0.5, i + 0.5, E_HOLTZ)));
				}
				else if (map[i][j] >= '2' && map[i][j] <= '4')
				{
					if (lvl->sublvls[map[i][j] - '2'] != NULL)
						spawn_teleporter(app, (t_vect){j + 0.5, i + 0.5}, map[i][j] - '1');
				}
				map[i][j] = '0';
			}
		}
	}
}

void	respawn_enemies(t_info *app, t_lvl *map)
{
	t_list		*cur_node;
	t_enemypos	*cur_pos;

	ft_lstclear(&map->enemies, free);
	cur_node = map->enemy_pos;
	while (cur_node != NULL)
	{
		cur_pos = (t_enemypos *)cur_node->data;
		if (cur_pos->type != E_PHANTOON)
			spawn_enemy(app, cur_pos->pos, rotate_vect((t_vect){0.0, 1.0}, rand_range(-M_PI, M_PI)), cur_pos->type);
		else if (map->boss_obj != NULL)
			map->boss_obj = spawn_enemy(app, cur_pos->pos, (t_vect){0, 1}, E_PHANTOON);
		cur_node = cur_node->next;
	}
}

void	remove_drops(t_lvl *map)
{
	t_list	*current;
	t_list	*temp;

	current = map->items;
	if (current == NULL)
		return ;
	while (((t_object *)current->content)->subtype >= I_AMMO_M && ((t_object *)current->content)->subtype <= I_HEALTH)
	{
		map->items = current->next;
		ft_lstdelone(current, free);
		current = map->items;
		if (current == NULL)
			return ;
	}
	while (current->next != NULL)
	{
		if (((t_object *)current->next->content)->subtype >= I_AMMO_M && ((t_object *)current->next->content)->subtype <= I_HEALTH)
		{
			temp = current->next->next;
			ft_lstdelone(current->next, free);
			current->next = temp;
			continue ;
		}
		current = current->next;
	}
}

void	reset_doors(t_lvl *map)
{
	int	i;
	int	j;

	i = -1;
	while (++i < map->height)
	{
		j = -1;
		while (++j < map->width)
		{
			if (map->map[i][j] == 'O')
				map->map[i][j] = 'D';
		}
	}
}

void	refresh_map(t_info *app, t_lvl *map)
{
	respawn_enemies(app, map);
	ft_lstclear(&map->projectiles, free);
	remove_drops(map);
	reset_doors(map);
	reset_anims(app, map);
}

int	count_collectables(t_lvl *map)
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

__attribute__((optnone))
void draw_help(t_lvl *lvl)
{
	t_info *const	app = lvl->app;
	t_point			p;
	t_img			*help;
	int				i;

	static const char *help_msgs[] = {
		"W, A, S, D => Move forward, left, backward, and right",
		"left arrow, right arrow => Rotate left and right",
		"E => Open/close door",
		"Left Shift => large minimap",
		"Left Mouse button => shoot",
		"1, 2, 3 or z => Switch weapons",
		"Right Mouse button => deselect missile",
	};

	help = mlx_new_image(app->mlx, WIN_WIDTH * 0.7, WIN_HEIGHT * 0.7);
	if (!help)
		return ;
	fill_with_colour(help, (int)0xC0000000, (int)0xC0000000);

	p.x = 50;
	p.y = 50;
	i = -1;
	while (++i < (int)(sizeof(help_msgs)/sizeof(help_msgs[0])))
	{
		draw_text_freetype(app, help, help_msgs[i], p);
		p.y += 60;
	}
	lvl->help = help;
}

void draw_large_minimap(t_lvl *lvl)
{
	t_info	*const app = lvl->app;
	t_img	*large_minimap;
	t_img	*scaled;
	t_point p;

	large_minimap = mlx_new_image(app->mlx, WIN_WIDTH * 0.7, WIN_HEIGHT * 0.7);

	if (!large_minimap)
		return ;
	fill_with_colour(large_minimap, (int)0xC0000000, (int)0xC0000000);

	p.x = 50;
	p.y = large_minimap->height - 50;

	draw_text_freetype(app, large_minimap, "Minimap =>", p);

	scaled = build_minimap(app, LARGE_MMAP_SCALE);
	p.x = (large_minimap->width - scaled->width) / 2;
	p.y = (large_minimap->height - scaled->height) / 2;
	place_tile_on_image32(large_minimap, scaled, p);
	lvl->mmap_origin = p;
	lvl->map_scale_factor.x = (double)scaled->width / lvl->minimap_xs->width;
	lvl->map_scale_factor.y = (double)scaled->height / lvl->minimap_xs->height;
	lvl->minimap_xl = large_minimap;
	mlx_destroy_image(app->mlx, scaled);
}

void draw_startup_overlay(t_lvl *lvl)
{
	t_info *const	app = lvl->app;
	t_point			p;
	t_img			*overlay;

	overlay = mlx_new_image(app->mlx, WIN_WIDTH * 0.7, WIN_HEIGHT * 0.7);
	if (!overlay)
		return ;
	fill_with_colour(overlay, (int)0xFF000000, (int)0xFF000000);


	p.x = 350;
	p.y = 50;

	draw_text_freetype(app, overlay, "[PRESS 'H' FOR HELP]", p);

	lvl->overlay = overlay;
}

int	parse_cub(t_info *app, char *filename)
{
	int		fd;
	t_list	*file;
	t_list	*current;
	t_lvl	*lvl;

	fd = open(filename, O_RDONLY);
	lvl = app->map;
	lvl->app = app;
	lvl->sublvls[0] = ft_strdup(filename);
	file = read_cub(fd);
	if (!collect_map(file, lvl))
		return (ft_list_destroy(&file, free),
			printf("Error: map not provided\n"), 1);
	if (!map_is_valid(lvl))
		return (ft_list_destroy(&file, free), 1);
	current = file;
	while (current != NULL)
	{
		if (str_cmp_whitespace(current->data, NULL))
			if (parse_line(lvl, current->data, app))
				return (ft_printf("%s\n", current->data),
					ft_list_destroy(&file, free), 1);
		current = current->next;
	}
	ft_list_destroy(&file, free);
	if (!all_fields_parsed(lvl))
		return (printf("Error: not all fields provided\n"), 1);
	spawn_map_objects(app, lvl);

//	raise(SIGTSTP);

	int small_mmap_scale = 8;
	lvl->minimap_xs = build_minimap(app, small_mmap_scale);

	draw_large_minimap(lvl);
	draw_help(lvl);
	draw_startup_overlay(lvl);

	lvl->anims = create_anim_arr(lvl->width, lvl->height);
	init_anims(app, lvl);
	close(fd);
	ft_lstadd_back(&app->lvl_cache, ft_lstnew(app->map));
	return (0);
}

void	free_map(t_lvl *data)
{
	free(data->n_tex.data);
	free(data->s_tex.data);
	free(data->e_tex.data);
	free(data->w_tex.data);
	free(data->floor_tex.data);
	free(data->ceil_tex.data);
	free(data->sublvls[0]);
	free(data->sublvls[1]);
	free(data->sublvls[2]);
	free(data->sublvls[3]);
	free_split(data->map);
	free_split((char **)data->anims);
	ft_lstclear(&data->enemies, free);
	ft_lstclear(&data->items, free);
	ft_lstclear(&data->triggers, free);
	ft_lstclear(&data->projectiles, free);
	ft_lstclear(&data->logo, free);
	ft_lstclear(&data->enemy_pos, free);
	Mix_FreeChunk(data->music);
	free(data);
}

t_lvl *get_cached_lvl(t_info *app, char *name)
{
	t_list	*current;

	current = app->lvl_cache;
	while (current != NULL)
	{
		if (ft_strcmp(((t_lvl *)current->data)->sublvls[0], name) == 0)
			return (current->data);
		current = current->next;
	}
	return (NULL);
}

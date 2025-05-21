/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 15:16:24 by fsmyth            #+#    #+#             */
/*   Updated: 2025/05/21 13:01:21 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

void	**ft_lst_to_arr(t_list *list)
{
	void	**array;
	t_list	*current;
	int		i;

	array = ft_calloc(ft_lstsize(list) + 1, sizeof(void *));
	i = 0;
	current = list;
	while (current != NULL)
	{
		array[i++] = current->content;
		current = current->next;
	}
	array[i] = NULL;
	return (array);
}

void	free_split(char **split)
{
	int	i;

	i = 0;
	if (split == NULL)
		return ;
	while (split[i])
		free(split[i++]);
	free(split);
}

size_t	count_split_words(char **split)
{
	size_t	i;

	i = 0;
	while (split[i])
		i++;
	return (i);
}

void	print_list(t_list *list)
{
	t_list	*current;

	current = list;
	while (current != NULL)
	{
		ft_printf("%s\n", current->data);
		current = current->next;
	}
}

t_data	*init_map(void)
{
	t_data	*map;

	map = ft_calloc(1, sizeof(*map));
	map->f_col = -1;
	map->c_col = -1;
	return (map);
}

int	str_cmp_whitespace(void *data, void *ref)
{
	char	*line;
	int		i;

	line = (char *)data;
	i = 0;
	while (line[i])
	{
		if (!ft_isspace(line[i++]))
			return (1);
	}
	return (0);
	(void)ref;
}

t_list	*read_cub(int cubfd)
{
	char	*line;
	char	*stripped;
	t_list	*file;

	line = get_next_line(cubfd);
	file = NULL;
	while (line != NULL)
	{
		stripped = ft_strtrim(line, "\t\n");
		free (line);
		ft_lstadd_back(&file, ft_lstnew(stripped));
		line = get_next_line(cubfd);
	}
	return (file);
}

int	is_map_line(char *line)
{
	int	i;

	if (str_cmp_whitespace(line, NULL) == 0)
		return (0);
	i = 0;
	while (line[i])
	{
		if (!ft_strchr(" \t01NSEWDLMemstZARPBHb234", line[i++]))
			return (0);
	}
	return (1);
}

size_t	find_longest_line(char **map)
{
	size_t	longest;
	char	*last1;
	size_t	len;
	size_t	i;

	longest = 0;
	i = -1;
	while (map[++i])
	{
		last1 = ft_strrchr(map[i], '1');
		if (last1 == NULL)
			continue ;
		len = last1 - map[i] + 1;
		if (len > longest)
			longest = len;
	}
	return (longest);
}

void	remove_trailing_lines(t_data *data)
{
	ssize_t	lines;
	size_t	i;
	char	**trimmed_map;

	lines = count_split_words(data->map);
	while (--lines >= 0)
	{
		if (str_cmp_whitespace((data->map)[lines], NULL))
			break ;
		free((data->map)[lines]);
		(data->map)[lines] = NULL;
	}
	trimmed_map = ft_calloc(lines + 2, sizeof(char *));
	i = -1;
	while ((data->map)[++i] != NULL)
		trimmed_map[i] = (data->map)[i];
	trimmed_map[i] = NULL;
	free(data->map);
	data->map = trimmed_map;
}

void	normalise_map(t_data *data)
{
	size_t	longest;
	size_t	i;
	char	*normalised;
	char	*last1;

	remove_trailing_lines(data);
	longest = find_longest_line(data->map);
	i = 0;
	while ((data->map)[i])
	{
		normalised = ft_calloc(longest + 1, 1);
		ft_memset(normalised, ' ', longest);
		last1 = ft_strrchr((data->map)[i], '1');
		if (last1 != NULL)
			ft_memmove(normalised, (data->map)[i], last1 - (data->map)[i] + 1);
		free((data->map)[i]);
		(data->map[i] = normalised);
		i++;
	}
	data->width = longest;
	data->height = i;
}

int	collect_map(t_list	*file, t_data *data)
{
	t_list	*current;

	current = file;
	while (current->next != NULL)
	{
		if (is_map_line(current->next->data))
			break ;
		current = current->next;
	}
	if (current->next == NULL)
		return (ft_list_destroy(&(current->next), NULL), 0);
	ft_list_reverse(&current->next);
	data->map = (char **)ft_lst_to_arr(current->next);
	ft_list_destroy(&(current->next), NULL);
	current->next = NULL;
	return (1);
}

int	surrounding_tiles_valid(char **map, size_t i, size_t j)
{
	if (i == 0 || j == 0)
		return (printf("Error: map not fully bounded\n"), 0);
	if (map[i + 1] == NULL)
		return (printf("Error: map not fully bounded\n"), 0);
	if (map[i][j + 1] == 0)
		return (printf("Error: map not fully bounded\n"), 0);
	if (!ft_strchr("NESW01DLMmsteZAHRPBb234", map[i - 1][j]))
		return (printf("Error: map not fully bounded\n"), 0);
	if (!ft_strchr("NESW01DLMmsteZAHRPBb234", map[i][j - 1]))
		return (printf("Error: map not fully bounded\n"), 0);
	if (!ft_strchr("NESW01DMLmsteZAHRPBb234", map[i][j + 1]))
		return (printf("Error: map not fully bounded\n"), 0);
	if (!ft_strchr("NESW01DMLmsteZAHRPBb234", map[i + 1][j]))
		return (printf("Error: map not fully bounded\n"), 0);
	if (!ft_strchr("NESW01DMLmsteZAHRPBb234", map[i - 1][j - 1]))
		return (printf("Error: map not fully bounded\n"), 0);
	if (!ft_strchr("NESW01DMLmsteZAHRPBb234", map[i + 1][j - 1]))
		return (printf("Error: map not fully bounded\n"), 0);
	if (!ft_strchr("NESW01DMLmsteZAHRPBb234", map[i - 1][j + 1]))
		return (printf("Error: map not fully bounded\n"), 0);
	if (!ft_strchr("NESW01DMLmsteZAHRPBb234", map[i + 1][j + 1]))
		return (printf("Error: map not fully bounded\n"), 0);
	return (1);
}

void	set_starting_dir(t_data *map, char dir)
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

int	check_start_pos(t_data *data , size_t i, size_t j, int *start_found)
{
	char	tile;

	tile = (data->map)[i][j];
	if (ft_strchr("NESW", tile))
	{
		if (*start_found)
			return (printf("Error: starting pos defined multiple times\n"), 0);
		data->starting_pos.x = (double)j + 0.5;
		data->starting_pos.y = (double)i + 0.5;
		set_starting_dir(data, (data->map)[i][j]);
		(data->map)[i][j] = '0';
		*start_found = 1;
	}
	return (1);
}

int	validate_map_tiles(t_data *data, char **map)
{
	size_t	i;
	size_t	j;
	int		start_found;

	i = -1;
	start_found = 0;
	while (map[++i])
	{
		j = -1;
		while (map[i][++j])
		{
			if (ft_strchr("0NEWSDLMmsteZAHRPBb234", map[i][j]))
			{
				if (!surrounding_tiles_valid(map, i, j)
					|| !check_start_pos(data, i, j, &start_found))
				{
					printf("Invalid tile: (%ld, %ld) = %c\n", j, i, map[i][j]);
					printf("on line: %s\n", map[i]);
					j = -1;
					while (map[++j])
					{
						if (j >= i - 1 && j <= i + 1)
							printf("\e[31m%s\e[m\n", map[j]);
						else
							printf("%s\n", map[j]);
					}
					return (0);
				}
			}
		}
	}
	if (!start_found)
		return (printf("Error: no starting pos defined\n"), 0);
	return (1);
}

int	map_is_terminating(char **map)
{
	size_t	i;

	i = 0;
	while (map[i])
	{
		if (!is_map_line(map[i]) && str_cmp_whitespace(map[i], NULL) != 0)
			return (0);
		i++;
	}
	return (1);
}

int	map_is_valid(t_data *data)
{
	if (!map_is_terminating(data->map))
		return (printf("Error: map not defined last\n"), 0);
	normalise_map(data);
	if (!validate_map_tiles(data, data->map))
		return (0);
	return (1);
}

void	print_map(t_data *data)
{
	int	i;

	i = 0;
	while (data->map[i] != NULL)
		ft_printf("<%s>\n", data->map[i++]);
}

int	valid_identifier(char *str)
{
	if (ft_strncmp(str, "NO", 3) == 0)
		return (NORTH);
	if (ft_strncmp(str, "SO", 3) == 0)
		return (SOUTH);
	if (ft_strncmp(str, "EA", 3) == 0)
		return (EAST);
	if (ft_strncmp(str, "WE", 3) == 0)
		return (WEST);
	if (ft_strncmp(str, "F", 2) == 0)
		return (FLOOR);
	if (ft_strncmp(str, "C", 2) == 0)
		return (CEILING);
	if (ft_strncmp(str, "LVL_A", 6) == 0)
		return (LVL_A);
	if (ft_strncmp(str, "LVL_B", 6) == 0)
		return (LVL_B);
	if (ft_strncmp(str, "LVL_C", 6) == 0)
		return (LVL_C);
	return (NONE);
}

int	get_col(char *str)
{
	char	*endptr;
	long	num;

	num = ft_strtol(str, &endptr, 0);
	if (num > 255 || num < 0)
		return (-1);
	if (*endptr != 0)
		return (-1);
	return (num);
}

int	convert_col(char *str)
{
	char	**split;
	int		col;
	int		i;
	int		shift;
	int		num;

	col = 0;
	split = ft_split(str, ',');
	if (count_split_words(split) != 3)
		return (free_split(split), -1);
	i = 0;
	shift = 16;
	while (i < 3)
	{
		num = get_col(split[i++]);
		if (num == -1)
			return (free_split(split), -1);
		col += num << shift;
		shift -= 8;
	}
	return (free_split(split), col);
}

int	parse_colour(t_data *map, char *str, int identifier)
{
	int	*coladdr;

	if (identifier == FLOOR)
		coladdr = &map->f_col;
	else
		coladdr = &map->c_col;
	if (*coladdr != -1)
		return (printf("Error: colour defined multiple times\n"), 1);
	*coladdr = convert_col(str);
	if (*coladdr == -1)
		return (printf("Error: invalid colour\n"), 1);
	return (0);
}

int	parse_texture(t_data *data, char *str, int identifier, t_info *app)
{
	t_texarr	*tex_addr;
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
	if (tex_addr->img != NULL)
		return (printf("Error: texture defined multiple times\n"), 1);
	tex_addr->img = img_to_arr(str, app, &tex_addr->x, &tex_addr->y);
	return (0);
}

int	parse_levels(t_data *data, char *str, int identifier)
{
	char	buf[100];
	int		len;

	len = ft_strrchr(data->sublvls[0], '/') - data->sublvls[0];
	ft_strlcpy(buf, data->sublvls[0], len + 2);
	ft_strlcat(buf, str, 100);
	data->sublvls[identifier + 1 - LVL_A] = ft_strdup(buf);
	return (0);
}

int	parse_line(t_data *data, char *line, t_info *app)
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
	else
		parse_levels(data, split[1], identifier);
	return (free_split(split), retval);
}

int	all_fields_parsed(t_data *data)
{
	if (data->n_tex.img == NULL)
		return (0);
	if (data->s_tex.img == NULL)
		return (0);
	if (data->e_tex.img == NULL)
		return (0);
	if (data->w_tex.img == NULL)
		return (0);
	if (data->floor_tex.img == NULL)
		return (0);
	if (data->ceil_tex.img == NULL)
	{
		data->outside = 1;
		return (1);
	}
//	 if (data->f_col == -1)
//	 	return (0);
//	if (data->c_col == -1)
//		return (0);
	return (1);
}

t_anim	**create_anim_arr(int x, int y)
{
	t_anim	**arr;
	int		i;

	arr = ft_calloc(y + 1, sizeof(t_anim *));
	i = 0;
	while (i < y)
		arr[i++] = ft_calloc(x, sizeof(t_anim));
	return (arr);
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

void	spawn_map_objects(t_info *app, t_data *data)
{
	char	**map;
	int		i;
	int		j;

	map = data->map;
	i = -1;
	while (++i < data->height)
	{
		j = -1;
		while (++j < data->width)
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
					ft_lstadd_back(&data->enemy_pos, ft_lstnew(construct_enemypos(j + 0.5, i + 0.5, E_ZOOMER)));
				}
				else if (map[i][j] == 'A')
				{
					spawn_enemy(app, (t_vect){j + 0.5, i + 0.5}, rotate_vect((t_vect){0.0, 1}, rand_range(-M_PI, M_PI)), E_ATOMIC);
					ft_lstadd_back(&data->enemy_pos, ft_lstnew(construct_enemypos(j + 0.5, i + 0.5, E_ATOMIC)));
				}
				else if (map[i][j] == 'R')
				{
					spawn_enemy(app, (t_vect){j + 0.5, i + 0.5}, rotate_vect((t_vect){0, 1}, rand_range(-M_PI, M_PI)), E_REO);
					ft_lstadd_back(&data->enemy_pos, ft_lstnew(construct_enemypos(j + 0.5, i + 0.5, E_REO)));
				}
				else if (map[i][j] == 'P')
				{
					data->boss_obj = spawn_enemy(app, (t_vect){j + 0.5, i + 0.5}, (t_vect){0, 0}, E_PHANTOON);
					ft_lstadd_back(&data->enemy_pos, ft_lstnew(construct_enemypos(j + 0.5, i + 0.5, E_PHANTOON)));
				}
				else if (map[i][j] == 'H')
				{
					data->boss_obj = spawn_enemy(app, (t_vect){j + 0.5, i + 0.5}, (t_vect){0, 1}, E_HOLTZ);
					ft_lstadd_back(&data->enemy_pos, ft_lstnew(construct_enemypos(j + 0.5, i + 0.5, E_HOLTZ)));
				}
				else if (map[i][j] >= '2' && map[i][j] <= '4')
				{
					if (data->sublvls[map[i][j] - '2'] != NULL)
						spawn_teleporter(app, (t_vect){j + 0.5, i + 0.5}, map[i][j] - '1');
				}
				map[i][j] = '0';
			}
		}
	}
}

void	respawn_enemies(t_info *app, t_data *map)
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

void	remove_drops(t_data *map)
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

void	reset_doors(t_data *map)
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

void	refresh_map(t_info *app, t_data *map)
{
	respawn_enemies(app, map);
	ft_lstclear(&map->projectiles, free);
	remove_drops(map);
	reset_doors(map);
	reset_anims(app, map);
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

int	parse_cub(t_info *app, char *filename)
{
	int		fd;
	t_list	*file;
	t_list	*current;
	t_data	*data;
	t_img	*tiles[16];

	fd = open(filename, O_RDONLY);
	data = app->map;
	data->app = app;
	data->sublvls[0] = ft_strdup(filename);
	load_map_textures(app, tiles);
	file = read_cub(fd);
	if (!collect_map(file, data))
		return (ft_list_destroy(&file, free),
			printf("Error: map not provided\n"), 1);
	if (!map_is_valid(data))
		return (ft_list_destroy(&file, free), 1);
	current = file;
	while (current != NULL)
	{
		if (str_cmp_whitespace(current->data, NULL))
			if (parse_line(data, current->data, app))
				return (ft_printf("%s\n", current->data),
					ft_list_destroy(&file, free), 1);
		current = current->next;
	}
	ft_list_destroy(&file, free);
	if (!all_fields_parsed(data))
		return (printf("Error: not all fields provided\n"), 1);
	// data->floor_tex.img = img_to_arr((char *)"./textures/outside_floor.xpm", app, &data->floor_tex.x, &data->floor_tex.y);
	spawn_map_objects(app, data);
	data->minimap = build_mmap(app, tiles);
	data->anims = create_anim_arr(data->width, data->height);
	init_anims(app, data);
	free_map_textures(app, tiles);
	close(fd);
	ft_lstadd_back(&app->lvlcache, ft_lstnew(app->map));
	return (0);
}

// void	print_t_map(t_data *data)
// {
// 	printf("n_path:\t%s\n", data->n_path);
// 	printf("s_path:\t%s\n", data->s_path);
// 	printf("e_path:\t%s\n", data->e_path);
// 	printf("w_path:\t%s\n", data->w_path);
// 	printf("f_col:\t%#.6X\n", data->f_col);
// 	printf("c_col:\t%#.6X\n", data->c_col);
// 	printf("map dimensions:\t%d x %d\n", data->width, data->height);
// 	printf("map:\n");
// 	print_map(data);
// 	printf("starting_pos:\t(%f, %f)\n", data->starting_pos.x, data->starting_pos.y);
// }

void	free_tex_arr(t_texarr *texture)
{
	int	i;

	if (texture == NULL)
		return ;
	i = 0;
	while (i < texture->y)
		free(texture->img[i++]);
	free(texture->img);
}

void	free_map(t_data *data)
{
	free_tex_arr(&data->n_tex);
	free_tex_arr(&data->s_tex);
	free_tex_arr(&data->e_tex);
	free_tex_arr(&data->w_tex);
	free_tex_arr(&data->floor_tex);
	free_tex_arr(&data->ceil_tex);
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
	ft_lstclear(&data->enemy_pos, free);
	free(data);
}

void	free_shtex(t_info *app)
{
	int	i;

	free_tex_arr(&app->shtex->title);
	mlx_destroy_image(app->mlx, app->shtex->alphabet);
	free_tex_arr(&app->shtex->credits);
	free_tex_arr(&app->shtex->tele);
	free_tex_arr(&app->shtex->scope);
	free_tex_arr(&app->shtex->empty);
	free_tex_arr(&app->shtex->trophy_tex[0]);
	free_tex_arr(&app->shtex->trophy_tex[1]);
	free_tex_arr(&app->shtex->super_ammo[0]);
	free_tex_arr(&app->shtex->super_ammo[1]);
	free_tex_arr(&app->shtex->missile_ammo[0]);
	free_tex_arr(&app->shtex->missile_ammo[1]);
	free_tex_arr(&app->shtex->boss_bar[0]);
	free_tex_arr(&app->shtex->boss_bar[1]);
	i = 0;
	while (i < 7)
		free_tex_arr(&app->shtex->door_tex[i++]);
	i = 0;
	while (i < 2)
		free_tex_arr(&app->shtex->cannon_tex[i++]);
	i = 0;
	while (i < 4)
		free_tex_arr(&app->shtex->reo_tex[i++]);
	i = 0;
	while (i < 6)
		free_tex_arr(&app->shtex->crawler_tex[i++]);
	i = 0;
	while (i < 6)
		free_tex_arr(&app->shtex->atomic_tex[i++]);
	i = 0;
	while (i < 17)
		free_tex_arr(&app->shtex->explode_tex[i++]);
	i = 0;
	while (i < 10)
		free_tex_arr(&app->shtex->proj_tex[i++]);
	i = 0;
	while (i < 3)
		free_tex_arr(&app->shtex->energy_tex[i++]);
	i = 0;
	while (i < 2)
		free_tex_arr(&app->shtex->etank_tex[i++]);
	i = 0;
	while (i < 12)
		free_tex_arr(&app->shtex->super_tex[i++]);
	i = 0;
	while (i < 12)
		free_tex_arr(&app->shtex->missile_tex[i++]);
	i = 0;
	while (i < 7)
		free_tex_arr(&app->shtex->door_super_tex[i++]);
	i = 0;
	while (i < 7)
		free_tex_arr(&app->shtex->door_missile_tex[i++]);
	i = 0;
	while (i < 7)
		free_tex_arr(&app->shtex->door_boss_tex[i++]);
	i = 0;
	while (i < 4)
		free_tex_arr(&app->shtex->health_pu[i++]);
	i = 0;
	while (i < 10)
		free_tex_arr(&app->shtex->phantoon[i++]);
	i = 0;
	while (i < 6)
		free_tex_arr(&app->shtex->phantoon_proj[i++]);
	i = 0;
	while (i < 6)
		free_tex_arr(&app->shtex->holtz_tex[i++]);
	i = 0;
	while (i < 4)
		free_tex_arr(&app->shtex->proj_green_tex[i++]);
	i = 0;
	while (i < 8)
		free_tex_arr(&app->shtex->dmg_tex[i++]);
	mlx_destroy_image(app->mlx, app->shtex->playertile);
	free(app->shtex);
}

t_data *get_cached_lvl(t_info *app, char *name)
{
	t_list	*current;

	current = app->lvlcache;
	while (current != NULL)
	{
		if (ft_strcmp(((t_data *)current->data)->sublvls[0], name) == 0)
			return (current->data);
		current = current->next;
	}
	return (NULL);
}

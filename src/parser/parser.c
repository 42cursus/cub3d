/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 15:16:24 by fsmyth            #+#    #+#             */
/*   Updated: 2025/03/28 14:58:35 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/cub3d.h"

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

	(void)ref;
	line = (char *)data;
	i = 0;
	while (line[i])
	{
		if (!ft_isspace(line[i++]))
			return (1);
	}
	return (0);
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
		if (!ft_strchr(" \t01NSEW", line[i++]))
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

	remove_trailing_lines(data);
	longest = find_longest_line(data->map);
	i = 0;
	while ((data->map)[i])
	{
		normalised = ft_calloc(longest + 1, 1);
		ft_memset(normalised, ' ', longest);
		ft_memmove(normalised, (data->map)[i], ft_strlen((data->map)[i]));
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
	ft_list_reverse_fun(current->next);
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
	if (!ft_strchr("NESW01", map[i - 1][j]))
		return (printf("Error: map not fully bounded\n"), 0);
	if (!ft_strchr("NESW01", map[i][j - 1]))
		return (printf("Error: map not fully bounded\n"), 0);
	if (!ft_strchr("NESW01", map[i][j + 1]))
		return (printf("Error: map not fully bounded\n"), 0);
	if (!ft_strchr("NESW01", map[i + 1][j]))
		return (printf("Error: map not fully bounded\n"), 0);
	if (!ft_strchr("NESW01", map[i - 1][j - 1]))
		return (printf("Error: map not fully bounded\n"), 0);
	if (!ft_strchr("NESW01", map[i + 1][j - 1]))
		return (printf("Error: map not fully bounded\n"), 0);
	if (!ft_strchr("NESW01", map[i - 1][j + 1]))
		return (printf("Error: map not fully bounded\n"), 0);
	if (!ft_strchr("NESW01", map[i + 1][j + 1]))
		return (printf("Error: map not fully bounded\n"), 0);
	return (1);
}

int	check_start_pos(t_data *data , size_t i, size_t j, int *start_found)
{
	if ((data->map)[i][j] != '0')
	{
		if (*start_found)
			return (printf("Error: starting pos defined multiple times\n"), 0);
		data->starting_pos.x = (double)j + 0.5;
		data->starting_pos.y = (double)i + 0.5;
		data->starting_dir = (data->map)[i][j];
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
			if (ft_strchr("0NEWS", map[i][j]))
			{
				if (!surrounding_tiles_valid(map, i, j)
					|| !check_start_pos(data, i, j, &start_found))
				{
					printf("Invalid tile: (%ld, %ld) = %c\n", j, i, map[i][j]);
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

	if (identifier == NORTH)
		tex_addr = &data->n_tex;
	else if (identifier == SOUTH)
		tex_addr = &data->s_tex;
	else if (identifier == EAST)
		tex_addr = &data->e_tex;
	else if (identifier == WEST)
		tex_addr = &data->w_tex;
	else
		return (1);
	if (tex_addr->img != NULL)
		return (printf("Error: texture defined multiple times\n"), 1);
	tex_addr->img = img_to_arr(str, app, &tex_addr->x, &tex_addr->y);
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
	else if (identifier < 3)
		retval = parse_colour(data, split[1], identifier);
	else
		retval = parse_texture(data, split[1], identifier, app);
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
	if (data->f_col == -1)
		return (0);
	if (data->c_col == -1)
		return (0);
	return (1);
}

int	parse_cub(t_info *app, int fd)
{
	t_list	*file;
	t_list	*current;
	t_data	*data;

	data = app->map;
	load_map_textures(app);
	file = read_cub(fd);
	if (!collect_map(file, data))
		return (ft_list_destroy(&file, free),
			printf("Error: map not provided\n"), 1);
	if (!map_is_valid(data))
		return (ft_list_destroy(&file, free), 1);
	ft_list_remove_if(&file, NULL, str_cmp_whitespace, free);
	current = file;
	while (current != NULL)
	{
		if (parse_line(data, current->data, app))
			return (ft_printf("%s\n", current->data),
				ft_list_destroy(&file, free), 1);
		current = current->next;
	}
	ft_list_destroy(&file, free);
	if (!all_fields_parsed(data))
		return (printf("Error: not all fields provided\n"), 1);
	// {
	// 	printf("NORTH: %d %d\n", data->n_tex.x, data->n_tex.y);
	// 	printf("SOUTH: %d %d\n", data->s_tex.x, data->s_tex.y);
	// 	printf("EAST: %d %d\n", data->e_tex.x, data->e_tex.y);
	// 	printf("WEST: %d %d\n", data->w_tex.x, data->w_tex.y);
	// 	exit(0);
	// }
	data->minimap = build_mmap(app);
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
	free_split(data->map);
	free(data);
}

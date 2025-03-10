/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 15:16:24 by fsmyth            #+#    #+#             */
/*   Updated: 2025/03/10 18:45:54 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"
#include <errno.h>

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
	while (split[i])
		free(split[i++]);
	free(split);
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

t_map	*init_map(void)
{
	t_map	*map;

	map = ft_calloc(1, sizeof(*map));
	return (map);
}

int	lst_is_whitespace(void *data, void *ref)
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
	ft_list_remove_if(&file, NULL, lst_is_whitespace, free);
	return (file);
}

int	is_map_line(char *line)
{
	int	i;

	i = 0;
	while (line[i])
	{
		if (!ft_strchr(" \t01NSEW", line[i++]))
			return (0);
	}
	return (1);
}

void	collect_map(t_list	*file, t_map *map)
{
	t_list	*current;

	current = file;
	while (current->next != NULL)
	{
		if (is_map_line(current->next->data))
			break ;
		current = current->next;
	}
	map->map = (char **)ft_lst_to_arr(current->next);
	ft_list_destroy(&(current->next), NULL);
	current->next = NULL;
	// print_list(file);
}

void	print_map(t_map *map)
{
	int	i;

	i = 0;
	while(map->map[i] != NULL)
		ft_printf("%s\n", map->map[i++]);
}

size_t	count_split_words(char **split)
{
	size_t	i;

	i = 0;
	while (split[i])
		i++;
	return (i);
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

int	parse_colour(t_map *map, char *str, int identifier)
{
	int	*coladdr;

	if (identifier == FLOOR)
		coladdr = &map->f_col;
	else
		coladdr = &map->c_col;
	if (*coladdr != 0)
		return (1);
	*coladdr = convert_col(str);
	if (*coladdr == -1)
		return (1);
	return (0);
}

int	parse_texture(t_map *map, char *str, int identifier)
{
	char	**pathaddr;

	if (identifier == NORTH)
		pathaddr = &map->n_path;
	else if (identifier == SOUTH)
		pathaddr = &map->s_path;
	else if (identifier == EAST)
		pathaddr = &map->e_path;
	else if (identifier == WEST)
		pathaddr = &map->w_path;
	else
		return (1);
	if (*pathaddr != NULL)
		return (1);
	*pathaddr = ft_strdup(str);
	return (0);
}

int	parse_line(t_map *map, char *line)
{
	char	**split;
	size_t	words;
	int		identifier;
	int		retval;

	split = ft_split(line, ' ');
	retval = 0;
	words = count_split_words(split);
	if (words != 2)
		return (free_split(split), 1);
	identifier = valid_identifier(split[0]);
	if (!identifier)
		return (free_split(split), 1);
	else if (identifier < 3)
		retval = parse_colour(map, split[1], identifier);
	else
		retval = parse_texture(map, split[1], identifier);
	return (free_split(split), retval);
}

int	parse_cub(t_map *map, t_list *file)
{
	t_list	*current;

	collect_map(file, map);
	current = file;
	while (current != NULL)
	{
		if (parse_line(map, current->data))
			return (ft_printf("%s\n", current->data), ft_list_destroy(&file, free), 1);
		current = current->next;
	}
	ft_list_destroy(&file, free);
	return (0);
}

void	print_t_map(t_map *map)
{
	printf("n_path:\t%s\n", map->n_path);
	printf("s_path:\t%s\n", map->s_path);
	printf("e_path:\t%s\n", map->e_path);
	printf("w_path:\t%s\n", map->w_path);
	printf("f_col:\t%#.6X\n", map->f_col);
	printf("c_col:\t%#.6X\n", map->c_col);
	printf("map:\n");
	print_map(map);
}

void	free_map(t_map *map)
{
	free(map->n_path);
	free(map->s_path);
	free(map->e_path);
	free(map->w_path);
	free_split(map->map);
	free(map);
}

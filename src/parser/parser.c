/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 15:16:24 by fsmyth            #+#    #+#             */
/*   Updated: 2025/07/13 22:01:29 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/cub3d.h"

t_data	*init_map(void)
{
	t_data	*map;

	map = ft_calloc(1, sizeof(*map));
	map->f_col = -1;
	map->c_col = -1;
	return (map);
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
			return (ft_printf("\e[31m%s\e[m\n", current->data),
				ft_list_destroy(&file, free), 1);
		current = current->next;
	}
	ft_list_destroy(&file, free);
	if (!all_fields_parsed(data))
		return (printf("Error: not all fields provided\n"), 1);
	return (0);
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   map_collection.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/21 16:19:56 by fsmyth            #+#    #+#             */
/*   Updated: 2025/05/21 16:25:15 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

size_t	count_split_words(char **split);
void	**ft_lst_to_arr(t_list *list);
int		str_cmp_whitespace(void *data, void *ref);

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

void	remove_trailing_lines(t_lvl *data)
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

void	normalise_map(t_lvl *data)
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

int	collect_map(t_list	*file, t_lvl *data)
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

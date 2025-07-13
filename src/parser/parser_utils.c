/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/13 21:51:38 by fsmyth            #+#    #+#             */
/*   Updated: 2025/07/13 21:52:42 by fsmyth           ###   ########.fr       */
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

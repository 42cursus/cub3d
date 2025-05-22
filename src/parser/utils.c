/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/21 16:31:28 by fsmyth            #+#    #+#             */
/*   Updated: 2025/05/21 16:32:21 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

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

void	print_map(t_data *data)
{
	int	i;

	i = 0;
	while (data->map[i] != NULL)
		ft_printf("<%s>\n", data->map[i++]);
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

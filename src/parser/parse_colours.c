/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_colours.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/21 16:27:59 by fsmyth            #+#    #+#             */
/*   Updated: 2025/05/21 16:29:24 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

size_t	count_split_words(char **split);

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

int	parse_colour(t_lvl *map, char *str, int identifier)
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

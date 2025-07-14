/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_colours.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/13 21:58:56 by fsmyth            #+#    #+#             */
/*   Updated: 2025/07/13 22:00:18 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"
#include "ft/ft_ctype.h"

int	convert_col(char *str)
{
	int		num;
	int		col;
	int		i;
	char	*endptr;

	col = 0;
	i = -1;
	while (++i < 3)
	{
		num = ft_strtol(str, &endptr, 0);
		col += num << (16 - (8 * i));
		if (num > 255 || num < 0)
			return (-1);
		if (*endptr == 0)
			break ;
		if (*endptr != ',')
			return (-1);
		str = endptr + 1;
		if (!ft_isdigit(*str))
			return (-1);
	}
	if (i != 2)
		return (-1);
	return (col);
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
		return (printf("Error: failed to parse colour\n"), 1);
	return (0);
}

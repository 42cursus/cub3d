/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/21 16:31:28 by fsmyth            #+#    #+#             */
/*   Updated: 2025/08/07 16:43:58 by fsmyth           ###   ########.fr       */
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
	if (ft_strncmp(str, "MU", 3) == 0)
		return (MU);
	return (NONE);
}

// void	print_map(t_lvl *data)
// {
// 	int	i;
//
// 	i = 0;
// 	while (data->map[i] != NULL)
// 		ft_printf("<%s>\n", data->map[i++]);
// }

// void	print_list(t_list *list)
// {
// 	t_list	*current;
//
// 	current = list;
// 	while (current != NULL)
// 	{
// 		ft_printf("%s\n", current->data);
// 		current = current->next;
// 	}
// }

void	print_invalid_tile_err(char **map, ssize_t i, ssize_t j)
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
}

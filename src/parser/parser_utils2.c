/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils2.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/13 22:01:09 by fsmyth            #+#    #+#             */
/*   Updated: 2025/07/13 22:01:29 by fsmyth           ###   ########.fr       */
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
	return (NONE);
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

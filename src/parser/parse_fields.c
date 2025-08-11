/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_fields.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/07 17:40:27 by fsmyth            #+#    #+#             */
/*   Updated: 2025/08/11 15:57:25 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

int	parse_colour(t_lvl *lvl, char *str, int identifier);

int	parse_texture(t_lvl *lvl, char *str, int identifier, t_info *app)
{
	t_tex	*tex_addr;
	t_img	tmp;

	if (identifier >= FLOOR && identifier <= WEST)
	{
		tex_addr = &lvl->texs[identifier - 1];
		if (identifier == CEILING || identifier == FLOOR)
		{
			lvl->planes[identifier - 1] = mlx_xpm_file_to_image(app->mlx,
					(char *) str, &tmp.width, &tmp.height);
			if (lvl->planes[identifier - 1] == NULL)
				return (1);
		}
	}
	else
		return (1);
	if (tex_addr->data != NULL)
		return (printf("Error: texture defined multiple times\n"), 1);
	*tex_addr = img_to_tex(app, str);
	return (0);
}

int	parse_music(t_lvl *lvl, char *str)
{
	if (lvl->music != NULL)
		return (printf("Error: music defined multiple times\n"), 1);
	lvl->music = Mix_LoadWAV(str);
	if (!lvl->music)
		return (printf("Error: failed to load music file\n"), 1);
	return (0);
}

int	parse_levels(t_lvl *lvl, char *str, int identifier)
{
	char		buf[100];
	ptrdiff_t	len;

	len = ft_strrchr(lvl->sublvls[0], '/') - lvl->sublvls[0];
	ft_strlcpy(buf, lvl->sublvls[0], len + 2);
	ft_strlcat(buf, str, 100);
	lvl->sublvls[identifier + 1 - LVL_A] = ft_strdup(buf);
	return (0);
}

int	parse_line(t_lvl *data, char *line, t_info *app)
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
	else if (identifier <= WEST)
	{
		retval = parse_texture(data, split[1], identifier, app);
		if (retval == 1 && identifier <= CEILING)
			retval = parse_colour(data, split[1], identifier);
	}
	else if (identifier <= LVL_C)
		parse_levels(data, split[1], identifier);
	else
		retval = parse_music(data, split[1]);
	return (free_split(split), retval);
}
//	else if (identifier == CEILING)

int	all_fields_parsed(t_lvl *lvl)
{
	if (lvl->n_tex.data == NULL)
		return (0);
	if (lvl->s_tex.data == NULL)
		return (0);
	if (lvl->e_tex.data == NULL)
		return (0);
	if (lvl->w_tex.data == NULL)
		return (0);
	if (lvl->floor_tex.data == NULL && lvl->f_col == -1)
		return (0);
	if (lvl->ceil_tex.data == NULL && lvl->c_col == -1)
	{
		lvl->outside = 1;
		return (1);
	}
	return (1);
}
//		retval = parse_colour(data, split[1], identifier);

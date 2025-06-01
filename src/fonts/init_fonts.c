/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_fonts.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abelov <abelov@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/01 18:09:42 by abelov            #+#    #+#             */
/*   Updated: 2025/06/01 18:09:43 by abelov           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

static int	load_fonts(t_typing *const typing, FT_Library ft)
{
	int		i;
	FT_Face	*face;

	i = -1;
	while (++i < fnt_MAX)
	{
		face = &typing->faces[i];
		if (FT_New_Face(ft, typing->files[i], 0, face))
		{
			while (i--)
				FT_Done_Face(typing->faces[i]);
			return (EXIT_FAILURE);
		}
	}
	return (EXIT_SUCCESS);
}

int	init_fonts(t_info *const app)
{
	t_typing *const		typing = &app->typ;
	FT_Library			ft = typing->ft;
	int 				err;

	err = EXIT_FAILURE;
	if (!FT_Init_FreeType(&ft))
		err = load_fonts(typing, ft);
	return (err);
}

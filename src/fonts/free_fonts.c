/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_fonts.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abelov <abelov@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/01 18:10:47 by abelov            #+#    #+#             */
/*   Updated: 2025/06/01 18:10:48 by abelov           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

void free_fonts(t_info *const app)
{
	t_typing *const		typing = &app->typ;
	FT_Library			*ft = &typing->ft;
	FT_Face				face;
	int 				i;

	i = -1;
	while (++i < FNT_MAX)
	{
		face = typing->faces[i];
		FT_Done_Face(face);
	}
	FT_Done_FreeType(*ft);
	*ft = NULL;
}

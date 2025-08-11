/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   draw_text_utils.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abelov <abelov@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/01 17:36:49 by abelov            #+#    #+#             */
/*   Updated: 2025/08/08 17:37:36 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

int	compute_text_width(FT_Face face, const char *text)
{
	int			total_width;
	FT_Vector	delta;
	FT_UInt		glyph_index;
	FT_UInt		prev_glyph;

	total_width = 0;
	prev_glyph = 0;
	while (*text)
	{
		glyph_index = FT_Get_Char_Index(face, *text);
		if (!FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT))
		{
			if (prev_glyph && FT_HAS_KERNING(face))
				if (FT_Get_Kerning(face, prev_glyph, glyph_index,
						FT_KERNING_DEFAULT, &delta) == 0)
					total_width += delta.x;
			total_width += face->glyph->advance.x;
			prev_glyph = glyph_index;
		}
		++text;
	}
	return (total_width >> 6);
}

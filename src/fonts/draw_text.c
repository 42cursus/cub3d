/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   draw_text.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abelov <abelov@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/01 17:36:49 by abelov            #+#    #+#             */
/*   Updated: 2025/06/01 17:36:50 by abelov           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

void	draw_text_freetype(t_info *app, t_img *img, const char *text, t_point c)
{
	int			col;
	int			row;
	t_point		p;
	FT_Bitmap	*bmp;
	double		alpha_frac;

	t_typing *const	typing = &app->typ;
	FT_Face	face = typing->faces[tp_main];

	FT_Set_Pixel_Sizes(face, 0, typing->default_size);

	int pen_x = c.x;
	const char *str = text;
	while(*str)
	{
		if (FT_Load_Char(face, *str, FT_LOAD_RENDER))
			continue;

		bmp = &face->glyph->bitmap;
		row = -1;
		while (++row < (int)bmp->rows)
		{
			col = -1;
			while (++col < (int)bmp->width)
			{
				alpha_frac = 1.0f - bmp->buffer[row * bmp->pitch + col] / 255.0;

				p.x = pen_x + face->glyph->bitmap_left + col;
				p.y = c.y - face->glyph->bitmap_top + row;

				if (alpha_frac != 1)
					put_pixel_alpha(img, p, 0xCCFFFF, alpha_frac);
			}
		}
		pen_x += face->glyph->advance.x >> 6;
		str++;
	}
}

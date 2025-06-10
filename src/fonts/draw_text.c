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
	t_ivect		i;
	t_point		p;
	FT_Bitmap	*bmp;
	double		alpha_frac;
	FT_Face		face = app->typ.faces[fnt_main];

	FT_Set_Pixel_Sizes(face, 0, app->typ.default_size);
	while(*text)
	{
		if (FT_Load_Char(face, *text, FT_LOAD_RENDER))
			continue;
		bmp = &face->glyph->bitmap;
		i.y = -1;
		while (++i.y < (int)bmp->rows)
		{
			i.x = -1;
			while (++i.x < (int)bmp->width)
			{
				alpha_frac = 1.0f - bmp->buffer[i.y * bmp->pitch + i.x] / 255.0;
				p.x = c.x + face->glyph->bitmap_left + i.x;
				p.y = c.y - face->glyph->bitmap_top + i.y;
				if (alpha_frac != 1)
					put_pixel_alpha(img, p, 0xCCFFFF, alpha_frac);
			}
		}
		c.x += face->glyph->advance.x >> 6;
		text++;
	}
}

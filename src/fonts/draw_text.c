/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   draw_text.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abelov <abelov@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/01 17:36:49 by abelov            #+#    #+#             */
/*   Updated: 2025/08/08 17:37:36 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

/**
 * Convert from 26.6 fixed-point format to integer pixels
 * via bit-shifting to the right on 6 ` >> 6`
 * @param app
 * @param img
 * @param text
 * @param c
 */
void	draw_text_freetype(FT_Face face, t_img *img,
			const char *text, t_point c)
{
	t_ivect		i;
	t_point		p;
	FT_Bitmap	*bmp;
	double		alpha_frac;

	while (*(text++))
	{
		if (FT_Load_Char(face, *(text - 1), FT_LOAD_RENDER))
			continue ;
		bmp = &face->glyph->bitmap;
		i.y = -1;
		while (++i.y < (int) bmp->rows)
		{
			i.x = -1;
			while (++i.x < (int) bmp->width)
			{
				alpha_frac = 1.0f - bmp->buffer[i.y * bmp->pitch + i.x] / 255.0;
				p.x = c.x + face->glyph->bitmap_left + i.x;
				p.y = c.y - face->glyph->bitmap_top + i.y;
				if (alpha_frac != 1)
					put_pixel_alpha(img, p, 0xCCFFFF, alpha_frac);
			}
		}
		c.x += face->glyph->advance.x >> 6;
	}
}

void	draw_char_ft_handle_alpha(t_tex *tex, t_point p, double alpha_frac)
{
	u_int32_t	*dst;
	u_int32_t	alpha;

	if (alpha_frac != 1)
	{
		if (p.x >= 0 && p.y >= 0 && p.x < (*tex).w && p.y < (*tex).h)
		{
			dst = (u_int32_t *)(*tex).data + p.y * (*tex).w + p.x;
			alpha = (u_char)((int)(alpha_frac * 255.0) & 0xFF);
			*dst = (alpha << 24) | (0xffea00 & MLX_WHITE);
		}
	}
}

void	draw_char_ft(FT_GlyphSlot glyph, t_tex *tex, t_point c)
{
	t_point				p;
	const FT_Bitmap		*bmp = &glyph->bitmap;
	t_ivect				it;
	double				alpha_frac;

	it.y = -1;
	while (++it.y < (int) bmp->rows)
	{
		it.x = -1;
		while (++it.x < (int) bmp->width)
		{
			alpha_frac = 1.0f - bmp->buffer[it.y * bmp->pitch + it.x] / 255.0;
			p.x = c.x + glyph->bitmap_left + it.x;
			p.y = c.y - glyph->bitmap_top + it.y;
			draw_char_ft_handle_alpha(tex, p, alpha_frac);
		}
	}
}

/**
 * face->glyph->bitmap_left:
 * 		Horizontal offset from pen position to left edge of glyph bitmap
 * face->glyph->bitmap_top:
 * 		Vertical offset from baseline to top edge of bitmap
 * @param app
 * @param tex
 * @param text
 * @param c
 */
void	draw_text_ft_hcentered(FT_Face face, t_tex tex,
			const char *text, t_point c)
{
	FT_Set_Pixel_Sizes(face, 0, 40);
	c.x -= compute_text_width(face, text) / 2;
	while (*text)
	{
		if (!FT_Load_Char(face, *text, FT_LOAD_RENDER))
		{
			draw_char_ft(face->glyph, &tex, c);
			c.x += (int) face->glyph->advance.x >> 6;
		}
		text++;
	}
}

/**
 * face->size->metrics.ascender:
 * 		How high the tallest glyphs go above the baseline
 * face->size->metrics.descender:
 * 		How low glyphs can go below the baseline (negative)
 * @param face
 * @param tex
 * @param lines
 * @param num_lines
 */
void	draw_multiline_text_centered(FT_Face face, t_tex tex,
										char **lines, int num_lines)
{
	int				i;
	t_ivect			pen;
	uint32_t		offset_y;
	const uint32_t	line_height = face->size->metrics.height >> 6;
	const uint32_t	interval = face->size->metrics.ascender >> 6;

	offset_y = (tex.h - num_lines * (line_height + interval)) / 2;
	pen.x = (tex.w) / 2;
	i = -1;
	while (++i < num_lines)
	{
		pen.y = offset_y + i * (line_height + interval) + line_height / 2;
		draw_text_ft_hcentered(face, tex, lines[i], pen);
	}
}

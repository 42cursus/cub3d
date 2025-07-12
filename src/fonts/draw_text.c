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

/**
 * Convert from 26.6 fixed-point format to integer pixels
 * via bit-shifting to the right on 6 ` >> 6`
 * @param app
 * @param img
 * @param text
 * @param c
 */
void draw_text_freetype(FT_Face face, t_img *img, const char *text, t_point c)
{
	t_ivect i;
	t_point p;
	FT_Bitmap *bmp;
	double alpha_frac;

	while (*text)
	{
		if (FT_Load_Char(face, *text, FT_LOAD_RENDER))
			continue;
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
		text++;
	}
}

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
void	draw_text_ft_hcentered(FT_Face face, t_tex tex, const char *text, t_point c)
{
	t_ivect			i;
	t_point			p;
	FT_Bitmap		*bmp;
	double			alpha_frac;

	FT_Set_Pixel_Sizes(face, 0, 40);
	c.x -= compute_text_width(face, text) / 2;
	while (*text)
	{
		if (FT_Load_Char(face, *text, FT_LOAD_RENDER))
		{
			text++;
			continue;
		}
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
				{
					u_int32_t *dst;
					u_int32_t alpha;

					if (p.x >= 0 && p.y >= 0 && p.x < tex.w && p.y < tex.h)
					{
						dst = (u_int32_t *) tex.data + p.y * tex.w + p.x;
						alpha = (u_char) ((int) (alpha_frac * 255.0) & 0xFF);
						*dst = (alpha << 24) | (0xffea00 & MLX_WHITE);
					}
				}
			}
		}
		c.x += face->glyph->advance.x >> 6;
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
	const FT_Size_Metrics	metrics = face->size->metrics;
	const uint32_t			line_height = metrics.height >> 6;
	const uint32_t			interval = metrics.ascender >> 6;
	const uint32_t			total_height = num_lines * (line_height + interval);
	const uint32_t			offset_y = (tex.h - total_height) / 2;
	int						i;
	t_ivect					pen;
	const char				*line;

	i = -1;
	while (++i < num_lines)
	{
		line = lines[i];
		pen.x = (tex.w) / 2;
		pen.y = offset_y + i * (line_height + interval) + line_height / 2;
		draw_text_ft_hcentered(face, tex, line, pen);
	}
}

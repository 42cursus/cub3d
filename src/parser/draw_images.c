/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   draw_images.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/08 14:43:59 by fsmyth            #+#    #+#             */
/*   Updated: 2025/08/08 14:46:01 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

void	draw_help(t_lvl *lvl)
{
	t_info *const		app = lvl->app;
	t_img *const		h = &lvl->help;
	t_ivect3			p;
	FT_Face				face;
	static const char	*help_msgs[] = {
		"W, A, S, D => Move forward, left, backward, and right",
		"left arrow, right arrow => Rotate left and right",
		"E => Open/close door", "Left Shift => large minimap",
		"Left Mouse button => shoot", "1, 2, 3 or z => Switch weapons",
		"Right Mouse button => deselect missile",
	};

	*h = (t_img){.width = WIN_WIDTH * 0.7, .height = WIN_HEIGHT * 0.7};
	h->size_line = (int)(h->width * sizeof(int));
	if (posix_memalign((void **)&h->data, 64, h->height * h->size_line))
		return ;
	fill_with_colour(h, (int)0xC0000000, (int)0xC0000000);
	face = app->typ.faces[fnt_main];
	FT_Set_Pixel_Sizes(face, 0, app->typ.default_size);
	p = (t_ivect3){.x = 50, .y = 50, .z = -1};
	while (++p.z < (int)(sizeof(help_msgs) / sizeof(help_msgs[0])))
	{
		draw_text_freetype(face, h, help_msgs[p.z], p.xy);
		p.y += 60;
	}
}

void	draw_large_minimap(t_lvl *lvl)
{
	t_info *const	app = lvl->app;
	t_img			*large_minimap;
	t_img			*scaled;
	t_point			p;
	FT_Face			face;

	large_minimap = mlx_new_image(app->mlx, WIN_WIDTH * 0.7, WIN_HEIGHT * 0.7);
	if (!large_minimap)
		return ;
	fill_with_colour(large_minimap, (int)0xC0000000, (int)0xC0000000);
	p.x = 50;
	p.y = large_minimap->height - 50;
	face = app->typ.faces[fnt_main];
	FT_Set_Pixel_Sizes(face, 0, app->typ.default_size);
	draw_text_freetype(face, large_minimap, "Minimap =>", p);
	scaled = build_minimap(app, LARGE_MMAP_SCALE);
	p.x = (large_minimap->width - scaled->width) / 2;
	p.y = (large_minimap->height - scaled->height) / 2;
	place_tile_on_image32(large_minimap, scaled, p);
	lvl->mmap_origin = p;
	lvl->map_scale_factor.x = (double)scaled->width / lvl->minimap_xs->width;
	lvl->map_scale_factor.y = (double)scaled->height / lvl->minimap_xs->height;
	lvl->minimap_xl = large_minimap;
	mlx_destroy_image(app->mlx, scaled);
}

void	draw_startup_overlay(t_lvl *lvl)
{
	t_info *const	app = lvl->app;
	t_point			p;
	t_tex			tex;
	t_img			overlay;
	FT_Face			face;

	tex = (t_tex){.w = WIN_WIDTH * 0.7, .h = WIN_HEIGHT * 0.7};
	tex.sl = tex.w * sizeof(int);
	if (posix_memalign((void **) &tex.data, 64, tex.h * tex.sl))
		return ;
	overlay.data = (void *)tex.data;
	overlay.width = tex.w;
	overlay.height = tex.h;
	overlay.size_line = tex.sl;
	fill_with_colour(&overlay, (int)0xFF000000, (int)0xFF000000);
	p.x = tex.w / 2;
	p.y = 50;
	face = app->typ.faces[fnt_main];
	FT_Set_Pixel_Sizes(face, 0, app->typ.default_size);
	draw_text_ft_hcentered(face, tex, "[PRESS 'H' FOR HELP]", p);
	lvl->overlay = overlay;
}

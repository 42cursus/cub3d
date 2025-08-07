/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 15:16:24 by fsmyth            #+#    #+#             */
/*   Updated: 2025/08/07 17:41:25 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/param.h>
#include "SDL_mixer.h"
#include "cub3d.h"
//	if (lvl->f_col == -1)
//	 	return (0);
//	if (lvl->c_col == -1)
//		return (0);

t_enpos	*new_enpos(t_vect pos, int type)
{
	t_enpos	*new;

	new = ft_calloc(1, sizeof(*new));
	new->pos = pos;
	new->type = type;
	return (new);
}

void	do_spawn_thing(t_info *app, t_lvl *lvl, char el, t_ivect it)
{
	t_list			*enpos;
	t_subtype		subtype;
	const t_vect	pos = addi_vect((t_vect){0.5, 0.5}, it);
	const t_vect	dir = rotv(0.0, -1, rand_range(-M_PI, M_PI));
	const t_subtype	lt[CHAR_MAX] = {
	['2'] = 1,
	['3'] = 2,
	['4'] = 3,
	['7'] = 1,
	['8'] = 2,
	['9'] = 3,
	['m'] = I_MISSILE,
	['t'] = I_TROPHY,
	['b'] = T_BOSS,
	['s'] = I_SUPER,
	['e'] = I_ETANK,
	['Z'] = E_ZOOMER,
	['A'] = E_ATOMIC,
	['R'] = E_REO,
	['P'] = E_PHANTOON,
	['H'] = E_HOLTZ,
	['{'] = D_SEAWEED,
	};

	subtype = lt[(u_char) el];
	if (ft_strchr("mest", el))
		spawn_item(app, pos, subtype);
	else if (ft_strchr("234b", el))
		spawn_trigger(app, pos, subtype);
	else if (ft_strchr("789", el))
		spawn_key(app, pos, subtype);
	else if (ft_strchr("{", el))
		spawn_decorative(app, pos, subtype);
	else
	{
		enpos = ft_lstnew(new_enpos(pos, subtype));
		if (ft_strchr("ZAR", el))
		{
			spawn_enemy(app, pos, dir, subtype);
			ft_lstadd_back(&lvl->enemy_pos, enpos);
		}
		else if (el == 'P' || el == 'H')
		{
			lvl->boss_obj = spawn_enemy(app, pos, (t_vect){0, -1}, subtype);
			ft_lstadd_back(&lvl->enemy_pos, enpos);
		}
	}
}

t_obj	*find_matching_tele(t_lvl *lvl, t_obj *key)
{
	t_list	*curr;
	t_obj	*cur_trig;

	curr = lvl->triggers;
	while (curr != NULL)
	{
		cur_trig = curr->data;
		if (cur_trig->type == O_TELE && cur_trig->subtype == key->subtype)
			return (cur_trig);
		curr = curr->next;
	}
	return (NULL);
}

void	lock_teles(t_lvl *lvl)
{
	t_list	*curr;
	t_obj	*cur_obj;
	t_obj	*tele;

	curr = lvl->triggers;
	while (curr != NULL)
	{
		cur_obj = curr->data;
		if (cur_obj->type == O_KEY)
		{
			tele = find_matching_tele(lvl, cur_obj);
			if (tele != NULL)
				tele->attacking = 1;
		}
		curr = curr->next;
	}
}

void	spawn_map_objects(t_info *app, t_lvl *lvl)
{
	char	**map;
	char	el;
	t_ivect	it;

	map = lvl->map;
	it.y = -1;
	while (++it.y < lvl->height)
	{
		it.x = -1;
		while (++it.x < lvl->width)
		{
			el = map[it.y][it.x];
			if (ft_strchr("ODLM", el))
				spawn_door(app, (t_vect){it.x, it.y}, 0);
			else if (ft_strchr("mestZAHRPb234789{", el))
			{
				do_spawn_thing(app, lvl, el, it);
				map[it.y][it.x] = '0';
			}
		}
	}
	lock_teles(lvl);
}

void	respawn_enemies(t_info *app, t_lvl *lvl)
{
	t_list	*cur_node;
	t_enpos	*cur_pos;
	t_vect	dir;
	t_vect	def;

	def = (t_vect){0, 1};
	ft_lstclear(&lvl->enemies, free);
	cur_node = lvl->enemy_pos;
	while (cur_node != NULL)
	{
		cur_pos = (t_enpos *)cur_node->data;
		if (cur_pos->type != E_PHANTOON)
		{
			dir = rotate_vect(def, rand_range(-M_PI, M_PI));
			spawn_enemy(app, cur_pos->pos, dir, cur_pos->type);
		}
		else if (lvl->boss_obj != NULL)
			lvl->boss_obj = spawn_enemy(app, cur_pos->pos, def, E_PHANTOON);
		cur_node = cur_node->next;
	}
}

void	remove_drops(t_lvl *lvl)
{
	t_list	*current;
	t_list	*temp;

	current = lvl->items;
	if (current == NULL)
		return ;
	while (((t_obj *)current->content)->subtype >= I_AMMO_M
		&& ((t_obj *)current->content)->subtype <= I_HEALTH)
	{
		lvl->items = current->next;
		ft_lstdelone(current, free);
		current = lvl->items;
		if (current == NULL)
			return ;
	}
	while (current->next != NULL)
	{
		if (((t_obj *)current->next->content)->subtype >= I_AMMO_M
			&& ((t_obj *)current->next->content)->subtype <= I_HEALTH)
		{
			temp = current->next->next;
			ft_lstdelone(current->next, free);
			current->next = temp;
			continue ;
		}
		current = current->next;
	}
}

void	reset_doors(t_lvl *lvl)
{
	int	i;
	int	j;

	i = -1;
	while (++i < lvl->height)
	{
		j = -1;
		while (++j < lvl->width)
		{
			if (lvl->map[i][j] == 'O')
				lvl->map[i][j] = 'D';
		}
	}
}

void	refresh_map(t_info *app, t_lvl *lvl)
{
	respawn_enemies(app, lvl);
	ft_lstclear(&lvl->projectiles, free);
	remove_drops(lvl);
	reset_doors(lvl);
	reset_anims(app, lvl);
}

int	count_collectables(t_lvl *lvl)
{
	t_list	*current;
	t_obj	*cur_obj;
	int		count;

	current = lvl->items;
	count = 0;
	while (current != NULL)
	{
		cur_obj = current->content;
		if (cur_obj->subtype >= I_ETANK && cur_obj->subtype <= I_MISSILE)
			count++;
		current = current->next;
	}
	return (count);
}

void	draw_help(t_lvl *lvl)
{
	t_info *const		app = lvl->app;
	t_ivect3			p;
	t_img				help;
	FT_Face				face;
	static const char	*help_msgs[] = {
		"W, A, S, D => Move forward, left, backward, and right",
		"left arrow, right arrow => Rotate left and right",
		"E => Open/close door",
		"Left Shift => large minimap",
		"Left Mouse button => shoot",
		"1, 2, 3 or z => Switch weapons",
		"Right Mouse button => deselect missile",
	};

	help = (t_img){.width = WIN_WIDTH * 0.7, .height = WIN_HEIGHT * 0.7};
	help.size_line = (int)(help.width * sizeof(int));
	if (posix_memalign((void **) &help.data, 64, help.height * help.size_line))
		return ;
	fill_with_colour(&help, (int)0xC0000000, (int)0xC0000000);
	face = app->typ.faces[fnt_main];
	FT_Set_Pixel_Sizes(face, 0, app->typ.default_size);
	p.x = 50;
	p.y = 50;
	p.z = -1;
	while (++p.z < (int)(sizeof(help_msgs) / sizeof(help_msgs[0])))
	{
		draw_text_freetype(face, &help, help_msgs[p.z], p.xy);
		p.y += 60;
	}
	lvl->help = help;
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

int	parse_cub(t_info *app, char *filename)
{
	int		fd;
	t_list	*file;
	t_list	*current;
	t_lvl	*lvl;

	fd = open(filename, O_RDONLY);
	if (fd == -1)
		return ((void)printf("Error: map not provided\n"), 1);
	lvl = app->lvl;
	lvl->app = app;
	lvl->sublvls[0] = ft_strdup(filename);
	file = read_file_stripped(fd);
	if (!collect_map(file, lvl))
		return (ft_list_destroy(&file, free),
			printf("Error: map not provided\n"), 1);
	if (!map_is_valid(lvl))
		return (ft_list_destroy(&file, free), 1);
	current = file;
	while (current != NULL)
	{
		if (str_cmp_whitespace(current->data, NULL))
			if (parse_line(lvl, current->data, app))
				return (ft_printf("%s\n", current->data),
					ft_list_destroy(&file, free), 1);
		current = current->next;
	}
	ft_list_destroy(&file, free);
	if (!all_fields_parsed(lvl))
		return (printf("Error: not all fields provided\n"), 1);
	spawn_map_objects(app, lvl);
	lvl->minimap_xs = build_minimap(app, SMALL_MMAP_SCALE);
	draw_large_minimap(lvl);
	draw_help(lvl);
	draw_startup_overlay(lvl);
	lvl->anims = create_anim_arr(lvl->width, lvl->height);
	init_anims(app, lvl);
	close(fd);
	ft_lstadd_back(&app->lvl_cache, ft_lstnew(app->lvl));
	return (0);
}

void	free_map(t_lvl *lvl)
{
	free(lvl->n_tex.data);
	free(lvl->s_tex.data);
	free(lvl->e_tex.data);
	free(lvl->w_tex.data);
	free(lvl->floor_tex.data);
	free(lvl->ceil_tex.data);
	free(lvl->sublvls[0]);
	free(lvl->sublvls[1]);
	free(lvl->sublvls[2]);
	free(lvl->sublvls[3]);
	free_split(lvl->map);
	free_split((char **)lvl->anims);
	ft_lstclear(&lvl->enemies, free);
	ft_lstclear(&lvl->items, free);
	ft_lstclear(&lvl->triggers, free);
	ft_lstclear(&lvl->doors, free);
	ft_lstclear(&lvl->projectiles, free);
	ft_lstclear(&lvl->logo, free);
	ft_lstclear(&lvl->enemy_pos, free);
	Mix_FreeChunk(lvl->music);
	free(lvl);
}

t_lvl	*get_cached_lvl(t_info *app, char *name)
{
	t_list	*current;

	current = app->lvl_cache;
	while (current != NULL)
	{
		if (ft_strcmp(((t_lvl *)current->data)->sublvls[0], name) == 0)
			return (current->data);
		current = current->next;
	}
	return (NULL);
}

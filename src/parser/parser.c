/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 15:16:24 by fsmyth            #+#    #+#             */
/*   Updated: 2025/06/04 23:48:38 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/param.h>
#include "SDL_mixer.h"
#include "cub3d.h"

size_t	count_split_words(char **split);
int		valid_identifier(char *str);
int		str_cmp_whitespace(void *data, void *ref);
t_list	*read_cub(int cubfd);

int	parse_texture(t_lvl *lvl, char *str, int identifier, t_info *app)
{
	t_tex	*tex_addr;
	t_img	tmp;

	if (identifier == NORTH)
		tex_addr = &lvl->n_tex;
	else if (identifier == SOUTH)
		tex_addr = &lvl->s_tex;
	else if (identifier == EAST)
		tex_addr = &lvl->e_tex;
	else if (identifier == WEST)
		tex_addr = &lvl->w_tex;
	else if (identifier == FLOOR)
	{
		tex_addr = &lvl->floor_tex;
		lvl->texs[T_FLOOR] = mlx_xpm_file_to_image(app->mlx, (char *) str, &tmp.width, &tmp.height);
	}
	else if (identifier == CEILING)
	{
		tex_addr = &lvl->ceil_tex;
		lvl->texs[T_CEILING] = mlx_xpm_file_to_image(app->mlx, (char *) str, &tmp.width, &tmp.height);
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
//	else if (identifier == CEILING)
//		retval = parse_colour(data, split[1], identifier);
	else if (identifier <= WEST)
		retval = parse_texture(data, split[1], identifier, app);
	else if (identifier <= LVL_C)
		parse_levels(data, split[1], identifier);
	else
		retval = parse_music(data, split[1]);
	return (free_split(split), retval);
}

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
	if (lvl->floor_tex.data == NULL)
		return (0);
	if (lvl->ceil_tex.data == NULL)
	{
		lvl->outside = 1;
		return (1);
	}
//	if (lvl->f_col == -1)
//	 	return (0);
//	if (lvl->c_col == -1)
//		return (0);
	return (1);
}

t_enpos	*new_enpos(t_vect pos, int type)
{
	t_enpos	*new;

	new = ft_calloc(1, sizeof(*new)); //FIXME: malloc check?
	new->pos = pos;
	new->type = type;
	return (new);
}

void do_spawn_thing(t_info *app, t_lvl *lvl, char el, t_ivect it)
{
	t_list			*enpos;
	t_subtype		subtype;
	const t_vect	pos = addi_vect((t_vect){0.5, 0.5}, it);
	const t_vect	dir = rotv(0.0, -1, rand_range(-M_PI, M_PI));
	const t_subtype	lt[CHAR_MAX] = {
		['2'] = 1,
		['3'] = 2,
		['4'] = 3,
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
	};

	subtype = lt[(u_char) el];
	if (ft_strchr("mest", el))
		spawn_item(app, pos, subtype);
	else if (ft_strchr("234b", el))
		spawn_trigger(app, pos, subtype);
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
				spawn_door(app, (t_vect) {it.x, it.y}, 0);
			else if (ft_strchr("mestZAHRPb234", el))
			{
				do_spawn_thing(app, lvl, el, it);
				map[it.y][it.x] = '0';
			}
		}
	}
}

void	respawn_enemies(t_info *app, t_lvl *lvl)
{
	t_list	*cur_node;
	t_enpos	*cur_pos;
	t_vect	dir;
	t_vect	def = (t_vect) {0, 1};

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
	while (((t_obj *)current->content)->subtype >= I_AMMO_M && ((t_obj *)current->content)->subtype <= I_HEALTH)
	{
		lvl->items = current->next;
		ft_lstdelone(current, free);
		current = lvl->items;
		if (current == NULL)
			return ;
	}
	while (current->next != NULL)
	{
		if (((t_obj *)current->next->content)->subtype >= I_AMMO_M && ((t_obj *)current->next->content)->subtype <= I_HEALTH)
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
	t_list		*current;
	t_obj	*cur_obj;
	int			count;

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

void draw_help(t_lvl *lvl)
{
	t_info *const	app = lvl->app;
	t_point			p;
	t_img			*help;
	int				i;

	static const char *help_msgs[] = {
		"W, A, S, D => Move forward, left, backward, and right",
		"left arrow, right arrow => Rotate left and right",
		"E => Open/close door",
		"Left Shift => large minimap",
		"Left Mouse button => shoot",
		"1, 2, 3 or z => Switch weapons",
		"Right Mouse button => deselect missile",
	};

	help = mlx_new_image(app->mlx, WIN_WIDTH * 0.7, WIN_HEIGHT * 0.7);
	if (!help)
		return ;
	fill_with_colour(help, (int)0xC0000000, (int)0xC0000000);

	p.x = 50;
	p.y = 50;
	i = -1;
	while (++i < (int)(sizeof(help_msgs)/sizeof(help_msgs[0])))
	{
		draw_text_freetype(app, help, help_msgs[i], p);
		p.y += 60;
	}
	lvl->help = help;
}

void draw_large_minimap(t_lvl *lvl)
{
	t_info	*const app = lvl->app;
	t_img	*large_minimap;
	t_img	*scaled;
	t_point p;

	large_minimap = mlx_new_image(app->mlx, WIN_WIDTH * 0.7, WIN_HEIGHT * 0.7);

	if (!large_minimap)
		return ;
	fill_with_colour(large_minimap, (int)0xC0000000, (int)0xC0000000);

	p.x = 50;
	p.y = large_minimap->height - 50;

	draw_text_freetype(app, large_minimap, "Minimap =>", p);

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

void draw_startup_overlay(t_lvl *lvl)
{
	t_info *const	app = lvl->app;
	t_point			p;
	t_img			*overlay;

	overlay = mlx_new_image(app->mlx, WIN_WIDTH * 0.7, WIN_HEIGHT * 0.7);
	if (!overlay)
		return ;
	fill_with_colour(overlay, (int)0xFF000000, (int)0xFF000000);


	p.x = 350;
	p.y = 50;

	draw_text_freetype(app, overlay, "[PRESS 'H' FOR HELP]", p);

	lvl->overlay = overlay;
}

int	parse_cub(t_info *app, char *filename)
{
	int		fd;
	t_list	*file;
	t_list	*current;
	t_lvl	*lvl;

	fd = open(filename, O_RDONLY);
	lvl = app->lvl;
	lvl->app = app;
	lvl->sublvls[0] = ft_strdup(filename);
	file = read_cub(fd);
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

t_lvl *get_cached_lvl(t_info *app, char *name)
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

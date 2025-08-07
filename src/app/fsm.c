/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fsm.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abelov <abelov@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/13 16:58:10 by abelov            #+#    #+#             */
/*   Updated: 2025/08/07 16:12:00 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"
#include <X11/X.h>
#include "fsm.h"

void	destroy_map(t_lvl *lvl)
{
	t_info *const	app = lvl->app;

	mlx_destroy_image(app->mlx, lvl->minimap_xs);
	mlx_destroy_image(app->mlx, lvl->minimap_xl);
	free(lvl->help.data);
	if (lvl->overlay.data != NULL)
		free(lvl->overlay.data);
	if (lvl->planes[T_CEILING])
		mlx_destroy_image(app->mlx, lvl->planes[T_CEILING]);
	if (lvl->planes[T_FLOOR])
		mlx_destroy_image(app->mlx, lvl->planes[T_FLOOR]);
	free_map(lvl);
}

inline __attribute__((always_inline))
void	cleanup_maps(t_info *app)
{
	ft_lstclear(&app->lvl_cache, (void (*)(void *))destroy_map);
	get_pooled_ray(1);
}

void	do_prep(int argc, t_info *const app)
{
	app->mlx = mlx_init();
	set_audio(app);
	set_fonts(app);
	set_fov(app, 110);
	set_framerate(app, FRAMERATE);
	set_sensitivity(app, 7);
	init_audio(app);
	init_fonts(app);
	set_sound_volume(app, 100);
	set_music_volume(app, 100);
	srand(get_time_ms());
	app->map_ids = ft_calloc(argc, sizeof(char *));
	app->no_maps = argc - 1;
}

void	do_load(t_info *const app)
{
	t_tex	tex;

	load_shtex(app);
	app->win = mlx_new_window(app->mlx, WIN_WIDTH, WIN_HEIGHT, app->title);
	app->fr_last = get_time_us();
	app->fr_count = 0;
	app->fr_time = 5000;
	app->stillshot = mlx_new_image(app->mlx, WIN_WIDTH, WIN_HEIGHT);
	app->canvas = mlx_new_image(app->mlx, WIN_WIDTH, WIN_HEIGHT);
	app->canvas_r = mlx_new_image(app->mlx, WIN_HEIGHT, WIN_WIDTH);
	app->pointer = mlx_new_image(app->mlx, 50, 50);
	tex.w = WIN_WIDTH;
	tex.h = WIN_HEIGHT;
	tex.sl = WIN_WIDTH * sizeof(int);
	if (posix_memalign((void **) &tex.data, 64, tex.h * tex.sl))
	{
		app->rc = (printf("Error: posix_memalign: %m\n"), fail);
		return ;
	}
	fill_with_colour_tex(tex, XPM_TRANSPARENT);
	app->overlay = cvttex_img(tex);
}

/**
 * GNU C statement expression:
 * 	https://gcc.gnu.org/onlinedocs/gcc/Statement-Exprs.html
 * @param param
 * @param argc
 * @param argv
 * @return
 */
t_ret_code	do_state_initial(void *param, int argc, char **argv)
{
	t_info *const	app = param;
	int				i;
	const char		*str;
	char			*end;
	int				has_slash;

	do_prep(argc, app);
	i = 0;
	while (++i < argc)
	{
		end = ft_strchrnul(argv[i], '\0');
		has_slash = (end != argv[i] && end[-1] == '/');
		str = ({if (has_slash == true) str = "start.cub"; else str = ""; str;});
		app->map_ids[i - 1] = ft_strjoin(argv[i], str);
	}
	if (app->mlx == NULL)
		return (printf("Error: failed to open map: %m\n"), fail);
	init_menu_select_funcs(app, &app->menu_state);
	do_load(app);
	replace_image(app, &app->bg, NULL);
	replace_image_r(app, &app->bg_r, NULL);
	if (!app->canvas || !app->stillshot || !app->pointer)
		exit(((void) ft_printf(" !! KO !!\n"), cleanup(app), EXIT_FAILURE));
	toggle_fullscreen(app);
	return ((ok & -(SKIP_INTRO)) | (extra & ~(-(SKIP_INTRO))));
}

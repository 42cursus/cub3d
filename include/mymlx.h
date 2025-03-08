/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mymlx.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Charlie Root <ol@epitech.net>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2000/06/02 16:45:48 by Charlie Root      #+#    #+#             */
/*   Updated: 2024/06/02 18:44:25 by abelov           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MYMLX_H
# define MYMLX_H
# include "mymlx_int.h"

void	*mlx_init(void);
void	*mlx_new_window(t_xvar *xvar, int size_x, int size_y, char *title);
int		mlx_clear_window(t_xvar *xvar, t_win_list *win);
//int		mlx_pixel_put(t_xvar *xvar,
// t_win_list *win, int x, int y, int color);
char	*mlx_get_data_addr(t_img *img, int *bits_per_pixel,
			int *size_line, int *endian);
//int		mlx_put_image_to_window(t_xvar *xvar, t_win_list *win, t_img *img,
//			int x, int y);
int		mlx_mouse_hook(t_win_list *win, int (*funct)(), void *param);
int		mlx_key_hook(t_win_list *win, int (*funct)(), void *param);
int		mlx_expose_hook(t_win_list *win, int (*funct)(), void *param);
int		mlx_loop_hook(t_xvar *xvar, int (*funct)(), void *param);
int		mlx_loop(t_xvar *xvar);
int		mlx_loop_end(t_xvar *xvar);
//int		mlx_string_put(t_xvar *xvar, t_win_list *win, int x, int y,
//			int color, char *string);
void	mlx_set_font(t_xvar *xvar, t_win_list *win, char *name);
void	*mlx_xpm_to_image(t_xvar *xvar,
			char **xpm_data, int *width, int *height);
void	*mlx_xpm_file_to_image(t_xvar *xvar,
			char *file, int *width, int *height);
int		mlx_destroy_window(t_xvar *xvar, t_win_list *win);
int		mlx_destroy_image(t_xvar *xvar, t_img *img);
int		mlx_destroy_display(t_xvar *xvar);
//int		mlx_hook(t_win_list *win, int x_event, int x_mask,
//			int (*funct)(), void *param);
int		mlx_do_key_autorepeatoff(t_xvar *xvar);
int		mlx_do_key_autorepeaton(t_xvar *xvar);
int		mlx_do_sync(t_xvar *xvar);
int		mlx_mouse_get_pos(t_xvar *xvar, t_win_list *win,
			int *win_x_return, int *win_y_return);
int		mlx_mouse_move(t_xvar *xvar, t_win_list *win, int x, int y);
int		mlx_mouse_hide(t_xvar *xvar, t_win_list *win);
int		mlx_mouse_show(t_xvar *xvar, t_win_list *win);
int		mlx_get_screen_size(t_xvar *mlx_ptr, int *sizex, int *sizey);

#endif /* MYMLX_H */

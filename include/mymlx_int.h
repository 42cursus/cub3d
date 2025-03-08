/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mymlx_int.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Charlie Root <ol@epitech.net>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2000/06/02 16:45:48 by Charlie Root      #+#    #+#             */
/*   Updated: 2024/06/02 18:32:05 by abelov           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MYMLX_INT_H
# define MYMLX_INT_H
# include <stdlib.h>
# include <stdio.h>
# include <string.h>
# include <unistd.h>
# include <fcntl.h>
# include <sys/mman.h>
# include <X11/Xlib.h>
# include <X11/Xutil.h>
# include <sys/ipc.h>
# include <sys/shm.h>
# include <X11/extensions/XShm.h>
# include <X11/XKBlib.h>
# include <X11/xpm.h>

# define MLX_TYPE_SHM_PIXMAP 3
# define MLX_TYPE_SHM 2
# define MLX_TYPE_XIMAGE 1
# define MLX_MAX_EVENT LASTEvent
# define ENV_DISPLAY "DISPLAY"
# define LOCALHOST "localhost"
# define ERR_NO_TRUECOLOR "MinilibX Error : No TrueColor Visual available.\n"
# define WARN_SHM_ATTACH "MinilibX Warning : X server can't attach sh memory.\n"

typedef struct s_xpm_col
{
	int		name;
	int		col;
}	t_xpm_col;

struct s_col_name
{
	char	*name;
	int		color;
};

typedef struct s_event_list
{
	int		mask;
	int		(*hook)();
	void	*param;
}	t_event_list;

typedef struct s_win_list	t_win_list;
struct s_win_list
{
	Window			window;
	GC				gc;
	t_win_list		*next;
	int				(*mouse_hook)();
	int				(*key_hook)();
	int				(*expose_hook)();
	void			*mouse_param;
	void			*key_param;
	void			*expose_param;
	t_event_list	hooks[MLX_MAX_EVENT];
};

typedef struct s_img
{
	XImage			*image;
	Pixmap			pix;
	GC				gc;
	int				size_line;
	int				bpp;
	int				width;
	int				height;
	int				type;
	int				format;
	char			*data;
	XShmSegmentInfo	shm;
}	t_img;

typedef struct s_xvar
{
	Display		*display;
	Window		root;
	int			screen;
	int			depth;
	Visual		*visual;
	Colormap	cmap;
	int			private_cmap;
	t_win_list	*win_list;
	int			(*loop_hook)();
	void		*loop_param;
	int			use_xshm;
	int			pshm_format;
	int			do_flush;
	int			decrgb[6];
	Atom		wm_delete_window;
	Atom		wm_protocols;
	int			end_loop;
}	t_xvar;

int		mlx_int_do_nothing(void *param);
int		mlx_get_color_value(t_xvar *xvar, int color);
int		mlx_int_get_good_color(t_xvar *xvar, int color);
int		mlx_int_find_in_pcm(void);
int		mlx_int_anti_resize_win(t_xvar *xvar, Window win, int w, int h);
int		mlx_int_wait_first_expose(t_xvar *xvar, Window win);
int		mlx_int_rgb_conversion(t_xvar *xvar);
int		mlx_int_deal_shm(t_xvar *xvar);
int		shm_att_pb(Display *d, XErrorEvent *ev);
int		mlx_int_get_visual(t_xvar *xvar);
int		mlx_int_set_win_event_mask(t_xvar *xvar);
int		mlx_int_str_str_cote(char *str, char *find, int len);
int		mlx_int_str_str(char *str, char *find, int len);
void	*mlx_int_new_xshm_image(t_xvar *xvar,
			int width, int height, int format);
char	**mlx_int_str_to_wordtab(char *str);
t_img	*mlx_new_image(t_xvar *xvar, int width, int height);
t_img	*mlx_new_image2(t_xvar *xvar, int width, int height);

#endif //MYMLX_INT_H

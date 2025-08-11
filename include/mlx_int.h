/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mlx_int.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 19:48:05 by fsmyth            #+#    #+#             */
/*   Updated: 2025/08/11 19:52:58 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
** mlx_int.h for mlx in 
** 
** Made by Charlie Root
** Login   <ol@epitech.net>
** 
** Started on  Mon Jul 31 16:45:48 2000 Charlie Root
** Last update Wed May 25 16:44:16 2011 Olivier Crouzet
*/

/*
** Internal settings for MiniLibX
*/

#ifndef MLX_INT_H

# define MLX_INT_H

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
/* #include	<X11/xpm.h> */

# define MLX_TYPE_SHM_PIXMAP 3
# define MLX_TYPE_SHM 2
# define MLX_TYPE_XIMAGE 1

# define MLX_MAX_EVENT LASTEvent

# define ENV_DISPLAY "DISPLAY"
# define LOCALHOST "localhost"
# define ERR_NO_TRUECOLOR "MinilibX Error : No TrueColor Visual available.\n"
# define WARN_SHM_ATTACH "MinilibX Warning : \
X server can't attach shared memory.\n"

typedef struct s_xpm_col
{
	int		name;
	int		col;
}	t_xpm_col;

struct	s_col_name
{
	char	*name;
	int		color;
};

typedef struct s_event_list
{
	int		mask;
	void	*hook;
	void	*param;
}	t_event_list;

typedef struct s_win_list
{
	Window				window;
	GC					gc;
	struct s_win_list	*next;
	int					(*mouse_hook)(void *);
	int					(*key_hook)(void *);
	int					(*expose_hook)(void *);
	void				*mouse_param;
	void				*key_param;
	void				*expose_param;
	t_event_list		hooks[MLX_MAX_EVENT];
}	t_win_list;

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
	int			(*loop_hook)(void *);
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

int		mlx_int_param_undef(t_xvar *xvar, XEvent *ev, t_win_list *win);
int		mlx_int_param_KeyPress(t_xvar *xvar, XEvent *ev, t_win_list *win);
int		mlx_int_param_KeyRelease(t_xvar *xvar, XEvent *ev, t_win_list *win);
int		mlx_int_param_ButtonPress(t_xvar *xvar, XEvent *ev, t_win_list *win);
int		mlx_int_param_ButtonRelease(t_xvar *xvar, XEvent *ev, t_win_list *win);
int		mlx_int_param_MotionNotify(t_xvar *xvar, XEvent *ev, t_win_list *win);
int		mlx_int_param_Expose(t_xvar *xvar, XEvent *ev, t_win_list *win);
int		mlx_int_param_generic(t_xvar *xvar, XEvent *ev, t_win_list *win);

t_img	*mlx_int_new_image(t_xvar *xvar, int width, int height, int format);
t_img	*mlx_int_new_xshm_image(t_xvar *xvar, int width,
			int height, int format);
char	**mlx_int_str_to_wordtab(char *str);
char	*mlx_int_get_line(char *ptr, int *pos, int size);
char	*mlx_int_static_line(char **xpm_data, int *pos, int size);
void	*mlx_int_parse_xpm(t_xvar *xvar, void *info,
			int info_size, char *(*f)());

t_img	*mlx_new_image2(t_xvar *xvar, int width, int height);

#endif

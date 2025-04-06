/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cub3d.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abelov <abelov@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 19:54:08 by abelov            #+#    #+#             */
/*   Updated: 2025/04/06 20:49:24 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CUB3D_H
# define CUB3D_H
#include "ft/ft_stdlib.h"
# include "libft.h"
# include "mlx.h"
# include "mlx_int.h"
# include <math.h>
#include <sys/types.h>

# define NUM_5 0x35 /* (53) Number 5 on the main keyboard */
# define ESC 0xFF1B /* (53) Number 5 on the main keyboard */
# define UP 65362
# define DOWN 65364
# define RIGHT 65363
# define LEFT 65361
# define KEY_W 0x0077
# define KEY_A 0x0061
# define KEY_S 0x0073
# define KEY_D 0x0064
# define KEY_E 0x0065
# define KEY_X 0x0078

# define WIN_HEIGHT 900
# define WIN_WIDTH 1200

typedef struct s_animation
{
	int		active;
	size_t	framestart;
}	t_anim;

typedef struct s_imgdata
{
	void	*img;
	char	*addr;
	int		width;
	int		height;
	int		bpp;
	int		line_length;
	int		endian;
}	t_imgdata;

typedef struct s_fvect
{
	double	x;
	double	y;
}	t_fvect;

typedef struct s_vect
{
	int	x;
	int	y;
}	t_vect;

typedef struct s_texarr
{
	unsigned int	**img;
	int				x;
	int				y;
}	t_texarr;

typedef struct s_object
{
	t_fvect		pos;
	t_fvect		norm;
	t_fvect		dir;
	t_fvect		p2;
	t_texarr	*texture;
}	t_object;

typedef struct s_ray
{
	t_fvect			intcpt;
	t_vect			maptile;
	int				face;
	t_texarr		*texture;
	double			pos;
	double			distance;
	struct s_ray	*in_front;
}	t_ray;

typedef	struct s_data
{
	t_texarr	n_tex;
	t_texarr	s_tex;
	t_texarr	e_tex;
	t_texarr	w_tex;
	t_texarr	door_tex[7];
	t_texarr	cannon_tex[3];
	void		*playertile;
	t_imgdata	minimap;
	int			f_col;
	int			c_col;
	char		**map;
	t_anim		**anims;
	t_list		*objects;
	t_object	testobj;
	t_fvect		starting_pos;
	char		starting_dir;
	int			height;
	int			width;
}	t_data;

typedef struct s_player
{
	t_fvect	pos;
	t_fvect	direction;
	double	angle;
	t_ray	rays[WIN_WIDTH];
	double	angle_offsets[WIN_WIDTH];
	t_anim	hud;
}	t_player;

enum
{
	NONE = 0,
	FLOOR = 1,
	CEILING = 2,
	NORTH = 3,
	SOUTH = 4,
	EAST = 5,
	WEST = 6,
	DOOR_N = 7,
	DOOR_S = 8,
	DOOR_E = 9,
	DOOR_W = 10,
	DOOR_N_OPEN = 11,
	DOOR_S_OPEN = 12,
	DOOR_E_OPEN = 13,
	DOOR_W_OPEN = 14,
};

typedef struct s_info
{
	struct s_fdf_win
	{
		int	width;
		int	height;
	}			win;
	t_xvar		*mlx;
	t_win_list	*root;
	double		zoom;
	char 		*title;
	t_img		*canvas;
	int			clip_x_origin;
	int			clip_y_origin;
	int			endianness;
	t_data		*map;
	t_player	*player;
	size_t		last_frame;
	size_t		framecount;
}	t_info;

int		check_endianness(void);
void	on_expose(t_info *app);
int		exit_win(void *param);
int		cleanup(t_info *app);
void	replace_image(t_info *app);
int		expose_win(void *param);
int		mouse_win(unsigned int button, int x, int y, void *p);
int		key_win(KeySym key, void *param);
void	mlx_keypress_hook(t_win_list *win, int (*hook)(KeySym, void *), void *param);

t_data	*init_map(void);
void	free_map(t_data *map);
int		parse_cub(t_info *app, int fd);
void	print_t_map(t_data *map);
void	print_ascii_mmap(t_data *data, t_player *player);

t_player	*init_player(t_data *map);
void		move_player(t_player *player, char **map, t_fvect dir);
void		rotate_player(t_player *player, int direction);
void	handle_open_door(t_info *app, t_ray *ray);
void	spawn_projectile(t_info *app, t_player *player, t_data *map);

char	get_max_direction(t_fvect vect);
t_fvect	scale_vect(t_fvect vect, double scalar);
t_fvect	rotate_vect(t_fvect vect, double angle);
void	rotate_vect_inplace(t_fvect *vect, double angle);
t_fvect	add_vect(t_fvect v1, t_fvect v2);
double	vector_distance(t_fvect v1, t_fvect v2);

t_ray	find_ray_collision(t_data *map, t_player *player, double angle);
void	cast_all_rays(t_data *map, t_player *player);
int		determine_face(t_fvect intersect);
void	free_ray_children(t_ray *ray);

void	fill_bg(t_imgdata *canvas, t_data *map);
void	my_put_pixel(t_imgdata *img, int x, int y, int colour);
void	load_map_textures(t_info *app,  void *tiles[]);
void	free_map_textures(t_info *app, void *tiles[]);
unsigned int	**img_to_arr(char *filename, t_info *app, int *x, int *y);
void	draw_rays(t_info *app, t_imgdata *canvas);
void	draw_mmap(t_info *app);
t_imgdata	build_mmap(t_info *app, void *tiles[]);
size_t	get_time_ms(void);

#endif //CUB3D_H


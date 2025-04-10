/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cub3d.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abelov <abelov@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 19:54:08 by abelov            #+#    #+#             */
/*   Updated: 2025/04/10 21:59:50 by fsmyth           ###   ########.fr       */
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

typedef struct s_texarr
{
	unsigned int	**img;
	int				x;
	int				y;
}	t_texarr;

typedef struct s_animation
{
	int			active;
	size_t		framestart;
	t_texarr	*tex_arr;
}	t_anim;

typedef struct s_imgdata
{
	t_img	*img;
	char	*addr;
	int		width;
	int		height;
	int		bpp;
	int		line_length;
	int		endian;
}	t_imgdata;

typedef struct s_vect
{
	double	x;
	double	y;
}	t_vect;

typedef struct s_ivect
{
	int	x;
	int	y;
}	t_ivect;

typedef struct s_object
{
	int			type;
	int			subtype;
	int			dead;
	int			health;
	t_vect		pos;
	t_vect		norm;
	t_vect		dir;
	t_vect		p2;
	t_texarr	*texture;
	t_anim		anim;
	t_anim		anim2;
}	t_object;

typedef struct s_ray
{
	t_vect			intcpt;
	t_ivect			maptile;
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
	t_texarr	door_super_tex[7];
	t_texarr	cannon_tex[2];
	t_texarr	crawler_tex[6];
	t_texarr	proj_tex[10];
	t_texarr	explode_tex[6];
	t_texarr	energy_tex[13];
	t_texarr	etank_tex[2];
	t_texarr	super_tex[12];
	void		*playertile;
	t_imgdata	minimap;
	int			f_col;
	int			c_col;
	char		**map;
	t_anim		**anims;
	t_list		*objects;
	t_object	testobj;
	t_vect		starting_pos;
	char		starting_dir;
	int			height;
	int			width;
}	t_data;

typedef struct s_player
{
	t_vect	pos;
	int		health;
	int		max_health;
	int		ammo[3];
	int		max_ammo[3];
	int		equipped;
	t_vect	dir;
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

enum
{
	O_PROJ = 0,
	O_ENTITY = 1,
	O_ITEM = 2,
};

enum
{
	E_ZOOMER,
	I_ETANK,
	I_SUPER,
};

enum
{
	BEAM,
	MISSILE,
	SUPER,
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
	t_imgdata	canvas;
	t_imgdata	bg;
	int			clip_x_origin;
	int			clip_y_origin;
	int			endianness;
	t_data		*map;
	t_player	*player;
	size_t		last_frame;
	size_t		framecount;
	bool		keys[16];
	bool		mouse[16];
}	t_info;

int		check_endianness(void);
void	on_expose(t_info *app);
int		cleanup(t_info *app);
void	replace_image(t_info *app);
int		expose_win(void *param);
int		mouse_release(unsigned int button, int x, int y, void *param);
int		mouse_press(unsigned int button, int x, int y, void *param);
int		mouse_move(int x, int y, void *param);
int		key_win(KeySym key, void *param);
void	mlx_keypress_hook(t_win_list *win, int (*hook)(KeySym, void *), void *param);

t_data	*init_map(void);
void	free_map(t_data *map);
int		parse_cub(t_info *app, int fd);
void	print_t_map(t_data *map);
void	print_ascii_mmap(t_data *data, t_player *player);
void	free_split(char **split);

t_player	*init_player(t_data *map);
void		move_entity(t_vect *pos, char **map, t_vect dir);
void		rotate_player(t_player *player, int direction, double sensitivity);
void	handle_open_door(t_info *app, t_ray *ray);
void	next_weapon(t_player *player);
void	spawn_projectile(t_info *app, t_player *player, t_data *map, int subtype);
void	spawn_enemy(t_info *app, t_vect pos, t_vect dir, int subtype);
void	spawn_item(t_info *app, t_vect pos, int subtype);
void	developer_console(t_info *app, t_player *player);
void	subtract_health(t_info *app, t_player *player, int damage);
void	damage_enemy(t_info *app, t_object *enemy, int damage);

t_vect	vect(double x, double y);
char	get_max_direction(t_vect vect);
t_vect	scale_vect(t_vect vect, double scalar);
t_vect	rotate_vect(t_vect vect, double angle);
void	rotate_vect_inplace(t_vect *vect, double angle);
t_vect	add_vect(t_vect v1, t_vect v2);
t_vect	subtract_vect(t_vect v1, t_vect v2);
double	vector_distance(t_vect v1, t_vect v2);
double	vector_magnitude(t_vect vect);
t_vect	normalise_vect(t_vect vect);
double	dot_product(t_vect v1, t_vect v2);
double	vector_angle(t_vect v1, t_vect v2);
void	*fast_memcpy_test(void *dest, const void *src, size_t n);

t_ray	find_ray_collision(t_data *map, t_player *player, double angle);
void	cast_all_rays(t_data *map, t_player *player);
int		determine_face(t_vect intersect);
void	free_ray_children(t_ray *ray);

void	fill_bg(t_imgdata *bg, t_data *map);
void	my_put_pixel(t_imgdata *img, int x, int y, int colour);
void	load_map_textures(t_info *app,  void *tiles[]);
void	free_map_textures(t_info *app, void *tiles[]);
unsigned int	**img_to_arr(char *filename, t_info *app, int *x, int *y);
void	draw_rays(t_info *app, t_imgdata *canvas);
void	draw_mmap(t_info *app);
t_imgdata	build_mmap(t_info *app, void *tiles[]);
size_t	get_time_ms(void);

int key_press(KeySym key, void *param);
int key_release(KeySym key, void *param);

#endif //CUB3D_H


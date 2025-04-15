/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cub3d.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abelov <abelov@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 19:54:08 by abelov            #+#    #+#             */
/*   Updated: 2025/04/15 18:50:51 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CUB3D_H
# define CUB3D_H
# include <math.h>
# include <sys/types.h>
# include "libft.h"
# include "mlx.h"
# include "mlx_int.h"
# include "fsm.h"

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

# define WIN_HEIGHT 768
# define WIN_WIDTH 1024

#ifndef FRAMERATE
# define FRAMERATE 50
#endif
#define FR_SCALE (FRAMERATE / 50)
#define FRAMETIME (1000000 / FRAMERATE)

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

//
//typedef enum e_textures
//{
//	DOOR_TEX = 0,
//
//}	t_etext;
//
//
//t_texarr textures[] = {
//	[DOOR_TEX] = {},
//
//};

typedef enum e_menustate
{
	MAIN,
	LVL_SELECT,
}	t_emenus;

typedef struct s_menustate
{
	t_emenus	state;
	int			selected;
	int			no_items;
}	t_menustate;

typedef struct s_shtex
{
	t_texarr	door_tex[7];
	t_texarr	door_super_tex[7];
	t_texarr	door_missile_tex[7];
	t_texarr	cannon_tex[2];
	t_texarr	crawler_tex[6];
	t_texarr	proj_tex[10];
	t_texarr	explode_tex[6];
	t_texarr	energy_tex[3];
	t_texarr	etank_tex[2];
	t_texarr	missile_tex[12];
	t_texarr	super_tex[12];
	t_texarr	health_pu[4];
	t_texarr	missile_ammo[2];
	t_texarr	super_ammo[2];
	t_texarr	trophy_tex[2];
	t_texarr	phantoon[5];
	t_texarr	title;
	t_texarr	alphabet;
	void		*playertile;
}	t_shtex;

typedef	struct s_data
{
	t_texarr	n_tex;
	t_texarr	s_tex;
	t_texarr	e_tex;
	t_texarr	w_tex;
	t_texarr	floor_tex;
	t_imgdata	minimap;
	int			f_col;
	int			c_col;
	char		**map;
	t_anim		**anims;
	t_list		*objects;
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
	int		dead;
	t_vect	dir;
	int		vert_offset;
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

typedef	enum e_type
{
	O_PROJ = 0,
	O_ENTITY = 1,
	O_ITEM = 2,
	O_EPROJ = 3,
}	t_etype;

typedef enum e_subtype
{
	E_ZOOMER,
	E_PHANTOON,
	I_ETANK,
	I_SUPER,
	I_MISSILE,
	I_AMMO_M,
	I_AMMO_S,
	I_HEALTH,
	I_TROPHY
}	t_subtype;

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
	t_imgdata	stillshot;
	int			clip_x_origin;
	int			clip_y_origin;
	int			endianness;
	t_shtex		*shtex;
	t_data		*map;
	char		**map_ids;
	int			no_maps;
	t_player	*player;
	size_t		last_frame;
	// size_t		last_frame_us;
	size_t		frametime;
	size_t		framecount;
	bool		keys[16];
	bool		mouse[16];
	t_state		state;
	t_ret_code	rc;
	t_menustate	menu_state;
	char		mapname[50];
}	t_info;

int		check_endianness(void);
void	on_expose(t_info *app);
int		cleanup(t_info *app);
void	replace_frame(t_info *app);
int		expose_win(void *param);
int		mouse_release_play(unsigned int button, int x, int y, void *param);
int		mouse_press_play(unsigned int button, int x, int y, void *param);
int		mouse_move_play(int x, int y, void *param);
int		key_win(KeySym key, void *param);
void	mlx_keypress_hook(t_win_list *win, int (*hook)(KeySym, void *), void *param);

t_data	*init_map(void);
void	free_map(t_data *map);
int		parse_cub(t_info *app, char *filename);
void	print_t_map(t_data *map);
void	print_ascii_mmap(t_data *data, t_player *player);
void	free_split(char **split);
void	load_shtex(t_info *app);

t_player	*init_player(t_data *map);
void		move_entity(t_vect *pos, char **map, t_vect dir);
void		rotate_player(t_player *player, int direction, double sensitivity);
void	handle_open_door(t_info *app, t_ray *ray);
void	next_weapon(t_player *player);
void	spawn_projectile(t_info *app, t_player *player, t_data *map, int subtype);
void	spawn_enemy_projectile(t_info *app, t_object *enemy, t_vect dir);
void	spawn_enemy(t_info *app, t_vect pos, t_vect dir, int subtype);
void	spawn_item(t_info *app, t_vect pos, int subtype);
void	developer_console(t_info *app, t_player *player);
void	subtract_health(t_info *app, t_player *player, int damage);
void	add_health(t_player *player, int health);
void	damage_enemy(t_info *app, t_object *enemy, int damage);
void	add_ammo(t_player *player, int type);

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
void	*fast_memcpy_test(int *dst, const int *src, size_t count);
void	memcpy_sse2(void *dst_void, const void *src_void, size_t size);

t_ray	find_ray_collision(t_data *map, t_player *player, double angle);
void	cast_all_rays_alt(t_info *app, t_data *map, t_player *player);
int		determine_face(t_vect intersect);
void	free_ray_children(t_ray *ray);

void replace_bg(t_info *app, char *tex_file);
void	fill_bg(t_imgdata *bg, t_data *map);
void	my_put_pixel_32(t_imgdata *img, int x, int y, unsigned int colour);
void	my_put_pixel(t_imgdata *img, int x, int y, int colour);
void	place_texarr(t_info *app, t_texarr *tex, int x, int y);
void	place_str(char *str, t_info *app, t_ivect pos, int scalar);
void	place_str_centred(char *str, t_info *app, t_ivect pos, int scalar);
void	load_map_textures(t_info *app,  void *tiles[]);
void	free_map_textures(t_info *app, void *tiles[]);
unsigned int	**img_to_arr(char *filename, t_info *app, int *x, int *y);
void	draw_rays(t_info *app, t_imgdata *canvas);
void	draw_mmap(t_info *app);
void	free_shtex(t_info *app);
t_imgdata	build_mmap(t_info *app, void *tiles[]);
size_t	get_time_ms(void);
size_t	get_time_us(void);

int key_press_play(KeySym key, void *param);
int key_release_play(KeySym key, void *param);
int key_press_mmenu(KeySym key, void *param);
int key_release_mmenu(KeySym key, void *param);

int key_press_pmenu(KeySym key, void *param);
int key_release_pmenu(KeySym key, void *param);

int key_press_loose(KeySym key, void *param);
int key_release_loose(KeySym key, void *param);

int key_press_win(KeySym key, void *param);
int key_release_win(KeySym key, void *param);

int	render_mmenu(void *param);
int	render_pmenu(void *param);
int	render_play(void *app);
int	render_lose(void *param);
void fill_everything_with_blood(t_imgdata *bg);

void	fill_floor(t_info *app, t_data *map, t_player *player);

void	menu_select_current(t_info *app);
void	draw_menu_items(t_info *app);
void	change_menu_selection(t_info *app, int dir);

t_state run_state(t_info *app, int argc, char **argv);

#endif //CUB3D_H


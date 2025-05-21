/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cub3d.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abelov <abelov@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 19:54:08 by abelov            #+#    #+#             */
/*   Updated: 2025/05/21 13:03:13 by fsmyth           ###   ########.fr       */
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

 // # define WIN_HEIGHT 720
//# define WIN_HEIGHT 960
//# define WIN_WIDTH 1280
# define WIN_HEIGHT 1080
# define WIN_WIDTH 1920
// # define WIN_HEIGHT 900
// # define WIN_WIDTH 1600
// # define WIN_HEIGHT 1008
// # define WIN_WIDTH 1792

#define RAY_POOL_SIZE 5000

#ifndef FRAMERATE
# define FRAMERATE 120
#endif
// #define FR_SCALE (FRAMERATE / 50.0)
// #define FRAMETIME (1000000 / FRAMERATE)

# define MLX_LIME 0x0000ff55
# define MLX_LIGHT_RED 0x00ff5555
# define MLX_RED 0x0bff0000
# define MLX_GREEN 0x0b00ff00
# define MLX_BLUE 0x0b0000ff
# define MLX_TRANSPARENT 0x000042

enum e_dir
{
	LEFT = 0,
	RIGHT
};

typedef struct s_texarr
{
	unsigned int	**img;
	int				x;
	int				y;
}	t_texarr;

typedef struct s_animation
{
	int			active;
	int			loop;
	size_t		timestart;
	t_texarr	*tex_arr;
	size_t		duration;
	int			frames;
}	t_anim;

//typedef struct s_imgdata
//{
//	t_img	*img;
//	char	*data;
//	int		width;
//	int		height;
//	int		bpp;
//	int		size_line;
//	int		endian;
//}	t_imgdata;

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

typedef struct s_cvect
{
	char	x;
	char	y;
}	t_cvect;

typedef struct s_cvect3
{
	char	x;
	char	y;
	char	z;
}	t_cvect3;

typedef struct s_enemypos
{
	int		type;
	t_vect	pos;
}	t_enemypos;

typedef struct s_object
{
	int			type;
	int			subtype;
	int			dead;
	int			attacking;
	int			health;
	size_t		last_damaged;
	t_vect		pos;
	t_vect		norm;
	t_vect		dir;
	double		speed;
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
	int				damaged;
	t_texarr		*texture;
	double			pos;
	double			distance;
	struct s_ray	*in_front;
}	t_ray;

typedef struct s_dda
{
	t_vect		dir;
	t_vect		side_dist;
	t_vect		delta_dist;
	t_ivect		step;
	t_ivect		norm;
	int			faces[2];
	t_texarr	*textures[2];
	double		gradient;
	double		c;
}	t_dda;

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
	PAUSE,
	OPTIONS,
	WIN,
	LOSE,
}	t_emenus;

typedef struct s_menustate
{
	t_emenus	state;
	t_emenus	prev;
	int			selected;
	int			no_items;
}	t_menustate;

typedef struct s_shtex
{
	t_texarr	door_tex[7];
	t_texarr	door_super_tex[7];
	t_texarr	door_missile_tex[7];
	t_texarr	door_boss_tex[7];
	t_texarr	cannon_tex[2];
	t_texarr	crawler_tex[6];
	t_texarr	atomic_tex[6];
	t_texarr	holtz_tex[6];
	t_texarr	reo_tex[4];
	t_texarr	proj_tex[10];
	t_texarr	proj_green_tex[4];
	t_texarr	explode_tex[17];
	t_texarr	energy_tex[3];
	t_texarr	etank_tex[2];
	t_texarr	missile_tex[12];
	t_texarr	super_tex[12];
	t_texarr	health_pu[4];
	t_texarr	missile_ammo[2];
	t_texarr	super_ammo[2];
	t_texarr	trophy_tex[2];
	t_texarr	phantoon[10];
	t_texarr	phantoon_proj[6];
	t_texarr	dmg_tex[8];
	t_texarr	title;
	t_texarr	scope;
	t_img		*alphabet;
	t_texarr	tele;
	t_texarr	credits;
	t_texarr	boss_bar[2];
	t_texarr	empty;
	void		*playertile;
}	t_shtex;

typedef enum e_textures
{
	T_FLOOR,
	T_CEILING,
	NUM_TEXTURES
}	t_enum_tex;

typedef	struct s_data
{
	struct s_info	*app;
	t_texarr		n_tex;
	t_texarr		s_tex;
	t_texarr		e_tex;
	t_texarr		w_tex;
	t_texarr		floor_tex;
	t_texarr		ceil_tex;
	int				outside;
	t_img			*minimap;
	int				f_col;
	int				c_col;
	char			**map;
	t_anim			**anims;
	t_list			*enemies;
	t_list			*items;
	t_list			*triggers;
	t_list			*projectiles;
	t_list			*enemy_pos;
	t_object		*boss_obj;
	bool			boss_active;
	t_vect			starting_pos;
	t_vect			starting_dir;
	int				height;
	int				width;
	char			*sublvls[4];
	t_img			*texs[NUM_TEXTURES];
}	t_data;

typedef struct s_poolnode
{
	t_ray				pool[RAY_POOL_SIZE];
	size_t				stackp;
	struct s_poolnode	*next;
}	t_poolnode;

typedef struct s_pool
{
	char	*pool;
	size_t	stackp;
	size_t	size;
}	t_pool;

typedef struct s_timer
{
	size_t	total_ms;
	size_t	cur_lvl_start;
	size_t	stop_time;
	int		active;
}	t_timer;

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
	// t_pool	raypool;
	double	angle_offsets[WIN_WIDTH];
	double	floor_offsets[WIN_HEIGHT / 2];
	double	ceil_offsets[WIN_HEIGHT / 2];
	t_anim	hud;
	t_vect	tele_pos;
	t_vect	dmg_dir;
	size_t	dmg_time;
	int		total_pickups;
	int		pickups_collected;
}	t_player;

typedef struct s_dummy
{
	t_vect	pos;
	t_vect	dir;
	double	credits_offsets[WIN_HEIGHT];
	t_img	credits;
	double	speed;
}	t_dummy;

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
	LVL_A = 15,
	LVL_B = 16,
	LVL_C = 17,
};

typedef	enum e_type
{
	O_PROJ = 0,
	O_ENTITY = 1,
	O_ITEM = 2,
	O_EPROJ = 3,
	O_TRIGGER = 4,
	O_TELE = 5,
}	t_etype;

typedef enum e_subtype
{
	E_ZOOMER,
	E_ATOMIC,
	E_REO,
	E_HOLTZ,
	E_PHANTOON,
	I_ETANK,
	I_SUPER,
	I_MISSILE,
	I_AMMO_M,
	I_AMMO_S,
	I_HEALTH,
	I_TROPHY,
	T_BOSS,
	P_PHANTOON,
	P_HOLTZ,
}	t_subtype;

enum
{
	BEAM,
	MISSILE,
	SUPER,
};

enum e_idx
{
	idx_XK_a = 0,
	idx_XK_d,
	idx_XK_e,
	idx_XK_s,
	idx_XK_w,
	idx_XK_x,
	idx_XK_Left,
	idx_XK_Up,
	idx_XK_Right,
	idx_XK_Down,
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
	t_img		*skybox;
	t_img		*bg;
	t_img		*stillshot;
	int			clip_x_origin;
	int			clip_y_origin;
	int			endianness;
	t_shtex		*shtex;
	t_data		*map;
	char		**map_ids;
	int			no_maps;
	t_player	*player;
	size_t		last_frame;
	size_t		frametime;
	size_t		framecount;
	bool		keys[16];
	bool		mouse[16];
	t_state		state;
	t_ret_code	rc;
	t_menustate	menu_state;
	int 		current_level;
	int 		current_sublevel;
	int			fov_deg;
	int			ads;
	int			old_fov;
	double		fov_rad_half;
	double		fov_opp_len;
	size_t		framerate;
	size_t		fr_delay;
	double		fr_scale;
	t_list		*lvlcache;
	int			filter;
	int			fullscreen;
	t_timer		timer;
	t_dummy		*dummy;
	int			sensitivity;
}	t_info;

int		check_endianness(void);
void	on_expose(t_info *app);
int		cleanup(t_info *app);
void	replace_frame(t_info *app);
int		expose_win(void *param);
int		mouse_release_play(unsigned int button, int x, int y, void *param);
int		mouse_press_play(unsigned int button, int x, int y, void *param);
int		mouse_move_play(int x, int y, void *param);

t_data	*init_map(void);
void	free_map(t_data *map);
int		parse_cub(t_info *app, char *filename);
t_data	*get_cached_lvl(t_info *app, char *name);
void	free_split(char **split);
void	load_shtex(t_info *app);

t_player	*init_player(t_info *app);
void		refresh_player(t_info *app, t_player *player);
void		refresh_map(t_info *app, t_data *map);
void		move_entity(t_vect *pos, t_data *map, t_vect dir);
void		move_obj_bounce(t_info *app, t_object *obj, t_data *data);
void		rotate_player(t_info *app, t_player *player, int direction, double sensitivity);
void	handle_open_door(t_info *app, t_ray *ray);
void	next_weapon(t_player *player);
void	prev_weapon(t_player *player);

void	spawn_projectile(t_info *app, t_player *player, t_data *map, int subtype);
void	spawn_enemy_projectile(t_info *app, t_object *enemy, t_vect dir, int subtype);
t_object	*spawn_enemy(t_info *app, t_vect pos, t_vect dir, int subtype);
void	spawn_item(t_info *app, t_vect pos, int subtype);
void	spawn_trigger(t_info *app, t_vect pos, int subtype);
void	spawn_teleporter(t_info *app, t_vect pos, int level);

void	developer_console(t_info *app, t_player *player);
void	subtract_health(t_info *app, t_player *player, int damage);
void	add_health(t_player *player, int health);
void	damage_enemy(t_info *app, t_object *enemy, int damage);
void	add_ammo(t_player *player, int type);
void	toggle_boss_doors(t_info *app);
int		check_tile_open(char tile, t_data *map);

double	get_gradient_angle(double angle);
double	get_y_intercept(t_vect pos, double gradient);
t_vect	get_vertical_int(double x, double gradient, double c);
t_vect	get_horizontal_int(double y, double gradient, double c);
double	get_cam_distance(t_vect pos, double angle, t_vect intcpt);
void	add_in_front(t_ray *ray, int face, t_texarr *texture);
t_vect	get_line_intersect(t_vect l1p1, t_vect l1p2, t_vect l2p1, t_vect l2p2);
t_ray	*check_obj_collision(t_object *object, t_ray *ray, t_player *player);
void	order_obj_ray(t_ray *obj, t_ray *ray);
void	calc_object_collisions(t_data *map, t_player *player, t_ray *ray);

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
double	get_hyp_len(double len1, double len2);
void	*fast_memcpy_test(int *dst, const int *src, size_t count);
void	memcpy_sse2(void *dst_void, const void *src_void, size_t size);

void	cast_all_rays_alt(t_info *app, t_data *map, t_player *player);
t_ray	*get_pooled_ray(int flag);
t_poolnode	*add_poolnode(t_poolnode *head);
void	clear_poolnodes(t_poolnode *head, t_poolnode **current);
void	reset_pool(t_poolnode *head, t_poolnode **current);
int		count_poolnodes(t_poolnode *head);
t_ray	ray_dda(t_info *app, t_data *map, t_player *player, double angle);
t_ray	ray_dda_refactor(t_info *app, t_data *map, t_player *player, double angle);
void	free_ray_children(t_ray *ray);

void	replace_image(t_info *app, t_img **img, char *tex_file);
void replace_sky(t_info *app, char *tex_file);
int		dim_colour(int col, double fact);
int		tint_red(int col);
void	fill_with_colour(t_img *img, int f_col, int c_col);
//void	my_put_pixel_32(t_img *img, int x, int y, unsigned int colour);
void	place_texarr(t_info *app, t_texarr *tex, int x, int y);
void	place_str(char *str, t_info *app, t_ivect pos, int scalar);
void	place_str_centred(char *str, t_info *app, t_ivect pos, int scalar);
void	place_fps(t_info *app);
void	place_timer(t_info *app, size_t time, t_ivect pos, int scalar);
void	load_map_textures(t_info *app,  t_img *tiles[]);
void	free_map_textures(t_info *app, t_img *tiles[]);
unsigned int	**img_to_arr(char *filename, t_info *app, int *x, int *y);
void	draw_rays(t_info *app, t_img *canvas);
void	draw_hud(t_info *app);
void	free_shtex(t_info *app);
t_img	*build_mmap(t_info *app, t_img *tiles[]);
size_t	get_time_ms(void);
size_t	get_time_us(void);
double	rand_range(double lower, double upper);

int key_press_play(KeySym key, void *param);
int key_release_play(KeySym key, void *param);
int key_press_mmenu(KeySym key, void *param);
int key_release_mmenu(KeySym key, void *param);

int key_press_pmenu(KeySym key, void *param);
int key_release_pmenu(KeySym key, void *param);

int key_press_lose(KeySym key, void *param);
int key_release_lose(KeySym key, void *param);

int key_press_win(KeySym key, void *param);
int key_release_win(KeySym key, void *param);

int key_press_credits(KeySym key, void *param);
int key_release_credits(KeySym key, void *param);

int	render_mmenu(void *param);
int	render_pmenu(void *param);
int	render_play(void *app);
int	render_load(void *app);
int	render_lose(void *param);
int	render_win(void *param);
int	render_credits(void *param);

void	draw_sky(t_info *app);
void 	draw_sky_alt(t_info *const app);
void	fill_ceiling(t_info *app, t_data *map, t_player *player);
void	fill_floor(t_info *app, t_player *player);

void	menu_select_current(t_info *app);
void	draw_menu_items(t_info *app);
void	change_menu_selection(t_info *app, int dir);
void	menu_change_option(t_info *app, int dir);

t_state run_state(t_info *app, int argc, char **argv);
void	set_fov(t_info *app, int fov);
void	set_framerate(t_info *app, size_t framerate);
void	set_sensitivity(t_info *app, int sensitivity);
void	calculate_offsets(t_info *app, t_player *player);
void	calculate_credits_offset(t_info *app, t_dummy *dummy);

int	bilinear_filter(double x, double y, const t_texarr *tex);
// int	linear_filter_credits(double x, int y, const t_texarr *tex);

void	start_obj_death(t_object *obj, t_info *app);
t_list	*delete_object(t_list **obj_list, t_list *obj_node);
t_object	*check_obj_proximity(t_vect pos, t_data *map);
int	point_oob_global(t_vect pos, t_data *map);
void	select_projectile_tex(t_object *obj, t_player *player, t_info *app);
t_texarr	*handle_animation(t_info *app, t_anim anim);
void	init_anims(t_info *app, t_data *map);
void	reset_anims(t_info *app, t_data *map);
int	count_collectables(t_data *map);
int	handle_obj_projectile(t_info *app, t_object *obj, t_list **current);
int	handle_enemy_projectile(t_info *app, t_object *obj, t_list **current);
void	spawn_drops(t_info *app, t_object *obj, int no);
void	phantoon_ai(t_info *app, t_object *obj);
void	reo_ai(t_info *app, t_object *enemy);
void	atomic_ai(t_info *app, t_object *enemy);
void	holtz_ai(t_info *app, t_object *enemy, t_player *player);
void	zoomer_ai(t_info *app, t_object *enemy);
int	handle_obj_entity(t_info *app, t_object *obj, t_list **current);
int	handle_trigger(t_info *app, t_object *obj, t_list **current);
void	handle_tele(t_info *app, t_object *tele);
int	handle_obj_item(t_info *app, t_object *obj, t_list **current);
void	update_objects(t_info *app, t_player *player, t_data *map);

int		check_line_of_sight(t_info *app, t_object *obj, t_player *player);
int		interpolate_colour(int col1, int col2, double frac);
void	draw_credits(t_info *app, t_dummy *dummy);
t_texarr	*get_open_door_tex(t_anim *anim, t_info *app);
t_texarr	*get_close_door_tex(t_anim *anim, t_info *app);

#endif //CUB3D_H

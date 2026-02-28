/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   joypad.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abelov <abelov@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/12 21:36:04 by abelov            #+#    #+#             */
/*   Updated: 2025/11/12 21:36:05 by abelov           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#define _GNU_SOURCE

#include <linux/input.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdarg.h>

#include "cub3d.h"
#include <libevdev/libevdev.h>

__attribute__ ((unused))
static void die(const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    dprintf(STDERR_FILENO, "pad_evdev: ");
    vdprintf(STDERR_FILENO, fmt, ap);
    dprintf(STDERR_FILENO, " (%m)\n");
    va_end(ap);
    exit(1);
}

__attribute__ ((unused))
static int key_idx(t_info* app, KeySym ks)
{
    /* if you already have get_key_index(), use that instead */
    return get_key_index(ks);
    (void)app;
}

static void set_key(t_info* app, int idx, bool down)
{
    if (idx >= 0) app->keys[idx] = down;
}

static void set_lrud(t_info* app, struct pad_state* ps, bool L, bool R, bool U, bool D)
{
    if (ps->left != (int)L)
    {
        ps->left = L;
        set_key(app, idx_XK_a, L);
    }
    if (ps->right != (int)R)
    {
        ps->right = R;
        set_key(app, idx_XK_d, R);
    }
    if (ps->up != (int)U)
    {
        ps->up = U;
        set_key(app, idx_XK_w, U);
    }
    if (ps->down != (int)D)
    {
        ps->down = D;
        set_key(app, idx_XK_s, D);
    }
}

int pad_init(pad_state* ps, const char* path)
{
    memset(ps, 0, sizeof(*ps));
    ps->dead = 6000;
    ps->hyst = 1500;

    ps->fd = open(path, O_RDONLY | O_NONBLOCK);
    if (ps->fd < 0)
        return -1;

    if (libevdev_new_from_fd(ps->fd, &ps->dev) < 0)
        return -1;

    const struct input_absinfo *ax = libevdev_get_abs_info(ps->dev, ABS_X);
    const struct input_absinfo *ay = libevdev_get_abs_info(ps->dev, ABS_Y);
    const struct input_absinfo *rx = libevdev_get_abs_info(ps->dev, ABS_RX);

    ps->ax_min = ax ? ax->minimum : -32768;
    ps->ax_max = ax ? ax->maximum : 32767;
    ps->ay_min = ay ? ay->minimum : -32768;
    ps->ay_max = ay ? ay->maximum : 32767;

    if (rx) { ps->rx_min = rx->minimum; ps->rx_max = rx->maximum; }
    else    { ps->rx_min = -32768;      ps->rx_max =  32767; }

    /* dynamic deadzone based on range */
    {
        int xrange = ps->ax_max - ps->ax_min;
        int yrange = ps->ay_max - ps->ay_min;
        int range = xrange > yrange ? xrange : yrange;
        if (range <= 0) range = 1;

        /* about 20% deadzone, 1/3 of that for hysteresis */
        ps->dead = range / 5;
        if (ps->dead < 2) ps->dead = 2;
        ps->hyst = ps->dead / 3;
        if (ps->hyst < 1) ps->hyst = 1;
    }

    ps->have_ax = 0;
    ps->have_ay = 0;
    ps->have_rx = 0;

    ps->ax = ps->ay = ps->rx = 0;


    dprintf(STDERR_FILENO,
        "pad: %s X[%d..%d] Y[%d..%d] RX[%d..%d] dead=%d hyst=%d\n",
        libevdev_get_name(ps->dev),
        ps->ax_min, ps->ax_max,
        ps->ay_min, ps->ay_max,
        ps->rx_min, ps->rx_max,
        ps->dead, ps->hyst);
    dprintf(STDERR_FILENO, "pad: %s ready\n", libevdev_get_name(ps->dev));
    return 0;
}

void pad_close(struct pad_state* ps)
{
    if (ps->dev) libevdev_free(ps->dev);
    if (ps->fd >= 0) close(ps->fd);
    memset(ps, 0, sizeof(*ps));
}

/**
 * Convert analog axis to 4 booleans with deadzone + hysteresis
 * @param ps
 * @param x
 * @param y
 * @param L
 * @param R
 * @param U
 * @param D
 */
static void axis_to_lrud(struct pad_state* ps, int x, int y, bool* L, bool* R, bool* U, bool* D)
{
    int cx = (ps->ax_min + ps->ax_max) / 2;
    int cy = (ps->ay_min + ps->ay_max) / 2;
    int dx = x - cx;
    int dy = y - cy;

    int thr = ps->dead;
    int rel = ps->hyst;

    bool wantL = false;
    bool wantR = false;
    bool wantU = false;
    bool wantD = false;

    if (dx < -thr) wantL = true;
    if (dx >  thr) wantR = true;
    if (dy < -thr) wantU = true;
    if (dy >  thr) wantD = true;

    /* apply hysteresis on releases */
    if (!wantL && ps->left) {
        if (dx < -(thr - rel)) wantL = true;
    }
    if (!wantR && ps->right) {
        if (dx > (thr - rel)) wantR = true;
    }
    if (!wantU && ps->up) {
        if (dy < -(thr - rel)) wantU = true;
    }
    if (!wantD && ps->down) {
        if (dy > (thr - rel)) wantD = true;
    }

    *L = wantL;
    *R = wantR;
    *U = wantU;
    *D = wantD;
}

#include <math.h>  /* top of file */

static void pad_apply_right_stick_rotation(struct pad_state *ps, t_info *app) {
    if (!ps->have_rx) return;

    /* compute normalized axis in [-1.0, 1.0] */
    int min = ps->rx_min;
    int max = ps->rx_max;
    int center = (min + max) / 2;
    int span = max - min;
    if (span <= 0) return;

    int raw = ps->rx;
    int delta = raw - center;

    /* deadzone in raw units (reusing ps->dead) */
    if (delta > -ps->dead && delta < ps->dead)
        return;

    double norm = (double)delta / (double)(span / 2);
    if (norm < -1.0) norm = -1.0;
    if (norm >  1.0) norm =  1.0;

    /* map norm to a virtual dx similar to mouse movement */
    /* tweak this constant to taste */
    double virtual_dx = norm * 40.0;

    double xx = 300.0 + 80.0 * (10.0 - app->sensitivity) * pow(1.1, 10.0 - app->sensitivity);
    xx = fabs(xx / (virtual_dx * app->fr_scale));

    rotate_player(app, app->player, virtual_dx > 0.0, xx);
}

void pad_poll(pad_state* ps, t_info* app)
{
    if (!ps || !ps->dev) return;

    // static int hatx = 0;
    // static int haty = 0;

    while (1)
    {
        struct input_event ev;
        int rc = libevdev_next_event(ps->dev, LIBEVDEV_READ_FLAG_NORMAL, &ev);
        if (rc == -EAGAIN) break;
        if (rc != 0 && rc != LIBEVDEV_READ_STATUS_SYNC) break;

        if (ev.type == EV_KEY)
        {
            bool down = ev.value;
            switch (ev.code) {
            case BTN_SOUTH:
                set_key(app, idx_XK_x, down);
                if (down)
                    spawn_projectile(app, app->player, app->lvl, app->player->equipped);
                break;
            case BTN_EAST: set_key(app, idx_XK_Escape, down);
                break;
            case BTN_WEST: set_key(app, idx_XK_Return, down);
                break;
            case BTN_NORTH: set_key(app, idx_XK_F5, down);
                break;
            case BTN_TL:
                if (down)
                    prev_weapon(app->player);
                break;
            case BTN_TR:
                if (down)
                    spawn_projectile(app, app->player, app->lvl, app->player->equipped);
                break;
            /* start/select */
            case BTN_START: menu_go_repeat(app, NULL);
                break;
            case BTN_SELECT:
                break;
            case BTN_DPAD_LEFT:  set_key(app, idx_XK_Left,  down); break;
            case BTN_DPAD_RIGHT: set_key(app, idx_XK_Right, down); break;
            case BTN_DPAD_UP:    set_key(app, idx_XK_Up,    down); break;
            case BTN_DPAD_DOWN:  set_key(app, idx_XK_Down,  down); break;
            default: break;
            }
        }
        else if (ev.type == EV_ABS)
        {
            if (ev.code == ABS_X) {
                ps->ax = ev.value;
                ps->have_ax = 1;
            } else if (ev.code == ABS_Y) {
                ps->ay = ev.value;
                ps->have_ay = 1;
            } else if (ev.code == ABS_RX) {
                ps->rx = ev.value;
                ps->have_rx = 1;
            }
            // else if (ev.code == ABS_HAT0X) hatx = ev.value;
            // else if (ev.code == ABS_HAT0Y) haty = ev.value;
        }
    }

    /* Map left stick to WASD */
    if (ps->have_ax && ps->have_ay) {
        bool L, R, U, D;
        axis_to_lrud(ps, ps->ax, ps->ay, &L, &R, &U, &D);
        set_lrud(app, ps, L, R, U, D);
    }
    pad_apply_right_stick_rotation(ps, app);

    /* Map D-pad to arrows */
    // set_key(app, idx_XK_Left,  hatx < 0);
    // set_key(app, idx_XK_Right, hatx > 0);
    // set_key(app, idx_XK_w,    haty < 0);
    // set_key(app, idx_XK_s,  haty > 0);
}

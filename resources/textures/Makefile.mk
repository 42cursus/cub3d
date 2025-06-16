# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile.mk                                        :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: abelov <abelov@student.42london.com>       +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/05/28 00:37:05 by abelov            #+#    #+#              #
#    Updated: 2025/05/28 00:37:05 by abelov           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

FILES   = arm_cannon_big.xpm \
			arm_cannon_big_firing.xpm \
			boss_bar_left.xpm \
			boss_bar_right.xpm \
			cloud.xpm \
			credits.xpm \
			credits_lorem.xpm \
			dmg0.xpm \
			dmg1.xpm \
			dmg2.xpm \
			dmg3.xpm \
			dmg4.xpm \
			dmg5.xpm \
			dmg6.xpm \
			dmg7.xpm \
			empty.xpm \
			energy.xpm \
			energy_0.xpm \
			energy_1.xpm \
			energy_2.xpm \
			energy_3.xpm \
			energy_4.xpm \
			energy_5.xpm \
			energy_6.xpm \
			energy_7.xpm \
			energy_8.xpm \
			energy_9.xpm \
			energy_bu_empty.xpm \
			energy_bu_full.xpm \
			etank0.xpm \
			etank1.xpm \
			explode0.xpm \
			explode1.xpm \
			explode2.xpm \
			explode3.xpm \
			explode4.xpm \
			explode5.xpm \
			explode_mid0.xpm \
			explode_mid1.xpm \
			explode_mid2.xpm \
			explode_mid3.xpm \
			explode_mid4.xpm \
			explode_mid5.xpm \
			explode_miss0.xpm \
			explode_miss1.xpm \
			explode_miss2.xpm \
			explode_miss3.xpm \
			explode_miss4.xpm \
			explode_miss_big0.xpm \
			explode_miss_big1.xpm \
			explode_miss_big2.xpm \
			explode_miss_big3.xpm \
			explode_miss_big4.xpm \
			floor.xpm \
			floor2.xpm \
			floor3.xpm \
			floor4.xpm \
			floor5.xpm \
			floor_norfair.xpm \
			grass.xpm \
			grass_stone.xpm \
			health_pu0.xpm \
			health_pu1.xpm \
			health_pu2.xpm \
			health_pu3.xpm \
			map_pointer.xpm \
			metroid1.xpm \
			metroid2.xpm \
			missile0.xpm \
			missile1.xpm \
			missile2.xpm \
			missile3.xpm \
			missile4.xpm \
			missile5.xpm \
			missile6.xpm \
			missile7.xpm \
			missile_ammo0.xpm \
			missile_ammo1.xpm \
			missile_off.xpm \
			missile_on.xpm \
			missile_pu0.xpm \
			missile_pu1.xpm \
			mountain.xpm \
			norfair_wall1.xpm \
			norfair_wall2.xpm \
			outside_floor.xpm \
			phantoon_eye0.xpm \
			phantoon_eye1.xpm \
			phantoon_eye2.xpm \
			phantoon_eye3.xpm \
			phantoon_eye4.xpm \
			phantoon_eye5.xpm \
			proj0.xpm \
			proj1.xpm \
			proj2.xpm \
			proj3.xpm \
			proj_green0.xpm \
			proj_green1.xpm \
			proj_green2.xpm \
			proj_green3.xpm \
			proj_super0.xpm \
			proj_super1.xpm \
			proj_super2.xpm \
			proj_super3.xpm \
			proj_super4.xpm \
			projectile2.xpm \
			ruins.xpm \
			scope.xpm \
			skybox.xpm \
			skybox1.xpm \
			small_font.xpm \
			stone_wall.xpm \
			super0.xpm \
			super1.xpm \
			super2.xpm \
			super3.xpm \
			super4.xpm \
			super5.xpm \
			super6.xpm \
			super7.xpm \
			super_ammo0.xpm \
			super_ammo1.xpm \
			super_missile_off.xpm \
			super_missile_on.xpm \
			super_missile_pu0.xpm \
			super_missile_pu1.xpm \
			teleporter.xpm \
			title_card.xpm \
			trophy0.xpm \
			trophy1.xpm \
			wall.xpm

TEXTURES    += $(FILES:%.xpm=$(dir $(lastword $(MAKEFILE_LIST)))%.xpm)

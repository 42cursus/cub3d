/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/24 14:35:24 by fsmyth            #+#    #+#             */
/*   Updated: 2026/02/24 16:35:04 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"
#include <fcntl.h>
#include <netinet/in.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>

int	setup_server(t_server *srv)
{
	srv->sockfd = socket(AF_INET, SOCK_DGRAM, 0);

	if (srv->sockfd < 0)
	{
		perror("Failed to initiate server socket\n");
		// exit(1);
		return (1);
	}

	int flags = fcntl(srv->sockfd, F_GETFL, 0);
	if (flags == -1) {
		perror("fcntl F_GETFL");
		return 1;
	}

	if (fcntl(srv->sockfd, F_SETFL, flags | O_NONBLOCK) == -1) {
		perror("fcntl F_SETFL");
	}

	memset(&srv->servaddr, 0, sizeof(srv->servaddr));
    srv->servaddr.sin_family = AF_INET;
    srv->servaddr.sin_port = htons(8080);
    srv->servaddr.sin_addr.s_addr = INADDR_ANY;


	if (bind(srv->sockfd, (const struct sockaddr *)&srv->servaddr, sizeof(srv->servaddr)) < 0)
	{
		perror("bind");
		// exit(1);
		return (2);
	}
	srv->n_clients = 0;
	return (0);
}

int	setup_client(t_client *client)
{
	client->sockfd = socket(AF_INET, SOCK_DGRAM, 0);

	if (client->sockfd < 0)
	{
		printf("Failed to initiate client socket\n");
		return (1);
	}

	// int flags = fcntl(client->sockfd, F_GETFL, 0);
	// if (flags == -1) {
	// 	perror("fcntl F_GETFL");
	// 	return 1;
	// }
	//
	// if (fcntl(client->sockfd, F_SETFL, flags | O_NONBLOCK) == -1) {
	// 	perror("fcntl F_SETFL");
	// }

	memset(&client->servaddr, 0, sizeof(client->servaddr));
    client->servaddr.sin_family = AF_INET;
    client->servaddr.sin_port = htons(8080);
    // client->servaddr.sin_addr.s_addr = inet_addr("10.18.152.152");
    // client->servaddr.sin_addr.s_addr = inet_addr("10.11.4.5");
    client->servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	client->dropped = 0;

	return (0);
}

int	client_handle_handshake(t_info *app, t_client *client)
{
	socklen_t	len = sizeof(struct sockaddr_in);
	t_clientmsg	cmsg;

	cmsg.id = -1;
	cmsg.type = CMT_CONNECT;

	sendto(client->sockfd, (char *)&cmsg, sizeof(t_clientmsg), 0, (const struct sockaddr *) &client->servaddr, sizeof(client->servaddr));

	// usleep(100000);
	int	id = -1;
	recvfrom(app->client.sockfd, (char *)&id, sizeof(id), 0, (struct sockaddr *)&app->client.servaddr, &len);
	if (id < 0 || id >= SRV_MAX_PLAYERS)
		return (1);
	printf("client id: %d\n", id);
	client->id = id;

	int flags = fcntl(client->sockfd, F_GETFL, 0);
	if (flags == -1) {
		perror("fcntl F_GETFL");
		return 1;
	}

	if (fcntl(client->sockfd, F_SETFL, flags | O_NONBLOCK) == -1) {
		perror("fcntl F_SETFL");
	}

	return (0);
}

void	init_server_state(t_info *app, t_server *srv)
{
	app->srv = srv;
	app->lvl->serialdata[SMT_OBJS].type = SMT_OBJS;
	app->lvl->serialdata[SMT_DOORS].type = SMT_DOORS;
	app->lvl->serialdata[SMT_PLAYER].type = SMT_PLAYER;
	set_framerate(app, 120);

	for (int i = 0; i < SRV_MAX_PLAYERS; i++)
	{
		srv->clients[i].health = 99;
		srv->clients[i].max_health = 99;
		srv->clients[i].max_ammo[P_BEAM] = -1;
		srv->clients[i].pos = app->lvl->starting_pos;
	}
}

pid_t	launch_server(t_info *app)
{
	pid_t	pid = 0;
	t_server srv = {0};

	int retval = setup_server(&srv);
	// int retval = 1;
	if (!retval)
	{
		pid = fork();
		if (pid == 0)
		{
			init_server_state(app, &srv);
			server_loop(app, &srv);
			exit(0);
		}
		close(srv.sockfd);
	}
	if (setup_client(&app->client))
		return -1;
	if (client_handle_handshake(app, &app->client))
		return (-1);
	return (pid);
}

void	server_handle_handshake(t_info *app, t_server *srv, packet_in *packet)
{
	int	id;
	if (srv->n_clients == SRV_MAX_PLAYERS)
		id = -1;
	else
	{
		id = srv->n_clients;
		memcpy(&srv->clientaddr[srv->n_clients++], &packet->sockbuf, sizeof(packet->sockbuf));
	}

	sendto(
		srv->sockfd, (char *)&id, sizeof(id), 0,
		(const struct sockaddr *) &srv->clientaddr[id], sizeof(srv->clientaddr[id])
	);
	printf("server response sent. n_clients: %d\n", srv->n_clients);
	(void)app;
}

void	server_handle_projectiles(t_info *app, t_clientmsg *cdata)
{
	switch (cdata->proj.type) {
		case (PROJ_BEAM):
			spawn_projectile_server(
				app,
				cdata->proj.pos,
				cdata->proj.dir,
				app->lvl, P_BEAM, cdata->id
			);
			break;
		case (PROJ_MISSILE):
			spawn_projectile_server(
				app,
				cdata->proj.pos,
				cdata->proj.dir,
				app->lvl, P_MISSILE, cdata->id
			);
			app->srv->clients[cdata->id].ammo[P_MISSILE] -= 1;
			if (app->srv->clients[cdata->id].ammo[P_MISSILE] == -1)
				app->srv->clients[cdata->id].ammo[P_MISSILE] = 0;
			break;
		case (PROJ_SUPER):
			spawn_projectile_server(
				app,
				cdata->proj.pos,
				cdata->proj.dir,
				app->lvl, P_SUPER, cdata->id
			);
			app->srv->clients[cdata->id].ammo[P_SUPER] -= 1;
			if (app->srv->clients[cdata->id].ammo[P_SUPER] == -1)
				app->srv->clients[cdata->id].ammo[P_SUPER] = 0;
			break;
		default:
			break;
	}
}

void	server_handle_player(t_server *srv, t_clientmsg *cmsg)
{
	srv->clients[cmsg->id].pos = cmsg->player.pos;
	srv->clients[cmsg->id].dir = cmsg->player.dir;
}

const char *stringify_cmsg_type(enum cmsg_type type)
{
	switch (type) {
		case (CMT_CONNECT):
			return ("CMT_CONNECT");
			break;
		case (CMT_PROJ):
			return ("CMT_PROJ");
			break;
		case (CMT_POS):
			return ("CMT_POS");
			break;
		case (CMT_DOOR):
			return ("CMT_DOOR");
			break;
		default:
			return NULL;
			break;
	}
}

const char *stringify_smsg_type(enum smsg_type type)
{
	switch (type) {
		case (SMT_OBJS):
			return ("SMT_OBJS");
			break;
		case (SMT_DOORS):
			return ("SMT_DOORS");
			break;
		case (SMT_PLAYER):
			return ("SMT_EVENT");
			break;
		default:
			return ("OTHER");
			break;
	}
}

void	server_handle_msg(t_info *app, t_server *srv, packet_in *packet)
{
	t_clientmsg *cmsg = &packet->data;
	switch (packet->data.type) {
		case (CMT_CONNECT):
			server_handle_handshake(app, srv, packet);
			break;
		case (CMT_PROJ):
			server_handle_projectiles(app, cmsg);
			break;
		case (CMT_POS):
			server_handle_player(srv, cmsg);
			add_serialplayer(cmsg, app->lvl);
			break;
		case (CMT_DOOR):
			handle_open_door_server(app, cmsg->door.pos);
			break;
		default:
			break;
	}
	// printf("\e[34;1mserver\e[m received msg: %s\n", stringify_cmsg_type(cmsg->type));
}

int	server_receive_messages(t_info *app, t_server *srv)
{
	int		n_msgs = 0;
	errno = 0;
	packet_in	packet;
	socklen_t	len = sizeof(packet.sockbuf);

	recvfrom(
		srv->sockfd, (char *)&packet.data, sizeof(t_clientmsg),
		0, (struct sockaddr *)&packet.sockbuf, &len
	);

	while (errno == 0)
	{
		server_handle_msg(app, srv, &packet);
		n_msgs++;

		recvfrom(
			srv->sockfd, (char *)&packet.data, sizeof(t_clientmsg),
			0, (struct sockaddr *)&packet.sockbuf, &len
		);
	}

	// printf("messages received: %d\n", n_msgs);
	return (n_msgs);
	(void)app;
}

void	server_send_msg(t_server *srv, struct sockaddr_in *client, t_servermsg *smsg)
{
	sendto(
		srv->sockfd, (char *)smsg, sizeof(t_servermsg), 0,
		(const struct sockaddr *) client, sizeof(*client)
	);
}

void	server_send_messages(t_info *app, t_server *srv)
{
	for (int i = 0; i < srv->n_clients; i++)
	{
		memcpy(&app->lvl->serialdata[SMT_PLAYER].payload, &srv->clients[i], sizeof(t_playermult));
		server_send_msg(srv, &srv->clientaddr[i], &app->lvl->serialdata[SMT_OBJS]);
		server_send_msg(srv, &srv->clientaddr[i], &app->lvl->serialdata[SMT_DOORS]);
		server_send_msg(srv, &srv->clientaddr[i], &app->lvl->serialdata[SMT_PLAYER]);
	}
}

void	server_loop(t_info *app, t_server *srv)
{
	while (true)
	{
		app->fr_last = get_time_us();
		server_receive_messages(app, srv);
		// size_t	ts1 = get_time_us();
		// printf("\n\e[32;1m## LOOP TIME ##\e[m\nreceive: %luus\n", ts1 - app->fr_last);
		
		update_objects_mult(app, app->player, app->lvl);
		// size_t ts2 = get_time_us();
		// printf("update: %luus\n", ts2 - ts1);

		server_send_messages(app, srv);
		// ts1 = get_time_us();
		// printf("send: %luus\n", ts1 - ts2);
		// printf("total: %luus\n", ts1 - app->fr_last);
		render_calc_time(app);
		// printf("wait: %luus\n", get_time_us() - ts1);
	}
	(void)app;
	(void)srv;
}

void	client_send_msg(t_client *client, t_clientmsg *cmsg)
{
	sendto(client->sockfd, (char *)cmsg, sizeof(t_clientmsg), 0, (const struct sockaddr *) &client->servaddr, sizeof(client->servaddr));
}

void	client_send_pos(t_info *app)
{
	t_client 	*client = &app->client;
	t_clientmsg	cmsg = {
		.id = app->client.id,
		.type = CMT_POS,
		.player.pos = app->player->pos,
		.player.dir = app->player->dir,
	};

	client_send_msg(client, &cmsg);
}

void	client_send_proj(t_info *app, t_eproj type)
{
	t_client 	*client = &app->client;
	t_clientmsg	cmsg = {
		.id = app->client.id,
		.type = CMT_PROJ,
		.proj.type = type,
		.proj.pos = app->player->pos,
		.proj.dir = app->player->dir,
	};

	client_send_msg(client, &cmsg);
}

void	client_send_door(t_info *app, t_ivect pos)
{
	t_client 	*client = &app->client;
	t_clientmsg	cmsg = {
		.id = app->client.id,
		.type = CMT_DOOR,
		.door.pos = pos,
	};

	client_send_msg(client, &cmsg);
}

void	client_process_msg(t_info *app, t_servermsg *smsg)
{
	switch (smsg->type) {
		case (SMT_OBJS):
			// printf("n_serialobjs: %d\n", smsg->payload.n_serialobjs);
			deserialise_objs(smsg->payload.serialobjs, smsg->payload.n_serialobjs, app->player);
			memcpy(&app->lvl->serialdata[SMT_OBJS], smsg, sizeof(*smsg));
			break;
		case (SMT_DOORS):
			// printf("n_serialdoors: %d\n", smsg->payload.n_serialdoors);
			deserialise_doors(smsg->payload.sdoors, smsg->payload.n_serialdoors, app->lvl);
			break;
		case (SMT_PLAYER):
			// printf("n_serialdoors: %d\n", smsg->payload.n_serialdoors);
			deserialise_player_state(app, smsg);
			break;
		default:
			break;
	}
}

void	client_receive_msgs(t_info *app)
{
	socklen_t	len = sizeof(app->client.servaddr);
	errno = 0;
	int			count = 0;
	t_servermsg smsg;

	app->client.dropped++;
	recvfrom(
		app->client.sockfd,
		(char *)&smsg,
		sizeof(t_servermsg), 0,
		(struct sockaddr *)&app->client.servaddr, &len
	);
	while (errno == 0)
	{
		app->client.dropped = 0;
		count++;
		// printf("\e[32;1mclient\e[m received msg: %s\n", stringify_smsg_type(smsg.type));
		client_process_msg(app, &smsg);
		recvfrom(
			app->client.sockfd,
			(char *)&smsg,
			sizeof(t_servermsg), 0,
			(struct sockaddr *)&app->client.servaddr, &len
		);
	}
	// printf("Packets received this tick: %d\n", count);
	(void)count;
}

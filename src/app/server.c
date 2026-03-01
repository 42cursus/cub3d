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
#include <wchar.h>

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

int	setup_client_host(t_client *client)
{
	client->sockfd = socket(AF_INET, SOCK_DGRAM, 0);

	if (client->sockfd < 0)
	{
		printf("Failed to initiate client socket\n");
		return (1);
	}

	int flags = fcntl(client->sockfd, F_GETFL, 0);
	if (flags == -1) {
		perror("fcntl F_GETFL");
		return 1;
	}

	if (fcntl(client->sockfd, F_SETFL, flags | O_NONBLOCK) == -1) {
		perror("fcntl F_SETFL");
	}

	memset(&client->servaddr, 0, sizeof(client->servaddr));
    client->servaddr.sin_family = AF_INET;
    client->servaddr.sin_port = htons(8080);
    // client->servaddr.sin_addr.s_addr = inet_addr("10.18.152.152");
    // client->servaddr.sin_addr.s_addr = inet_addr("10.11.4.5");
    client->servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	client->dropped = 0;

	return (0);
}

int	setup_client_client(t_client *client, char *ip)
{
	client->sockfd = socket(AF_INET, SOCK_DGRAM, 0);

	if (client->sockfd < 0)
	{
		printf("Failed to initiate client socket\n");
		return (1);
	}

	int flags = fcntl(client->sockfd, F_GETFL, 0);
	if (flags == -1) {
		perror("fcntl F_GETFL");
		return 1;
	}

	if (fcntl(client->sockfd, F_SETFL, flags | O_NONBLOCK) == -1) {
		perror("fcntl F_SETFL");
	}

	memset(&client->servaddr, 0, sizeof(client->servaddr));
    client->servaddr.sin_family = AF_INET;
    client->servaddr.sin_port = htons(8080);
    // client->servaddr.sin_addr.s_addr = inet_addr("10.18.152.152");
    // client->servaddr.sin_addr.s_addr = inet_addr("10.11.4.5");
    client->servaddr.sin_addr.s_addr = inet_addr(ip);

	client->dropped = 0;

	return (0);
}

int	client_handle_handshake(t_info *app, t_client *client)
{
	socklen_t	len = sizeof(struct sockaddr_in);
	t_clientmsg	cmsg = {
		.id = -1,
		.type = CMT_CONNECT,
	};
	strncpy(cmsg.name, client->name, 12);
	printf("cmsg name: %s\nclient name: %s\n", cmsg.name, client->name);

	sendto(client->sockfd, (char *)&cmsg, sizeof(t_clientmsg), 0, (const struct sockaddr *) &client->servaddr, sizeof(client->servaddr));

	// usleep(100000);
	int	id = -1;
	size_t start_time = get_time_ms();
	ssize_t	n = 0;
	while (n <= 0)
	{
		n = recvfrom(app->client.sockfd, (char *)&id, sizeof(id), 0, (struct sockaddr *)&app->client.servaddr, &len);
		if (get_time_ms() - start_time > 2000)
			return 1;
	}
	if (id < 0 || id >= SRV_MAX_PLAYERS)
		return (1);
	printf("client id: %d\n", id);
	client->id = id;

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
		srv->clients[i].id = i;
	}
}

pid_t	launch_server(t_info *app)
{
	pid_t	pid = 0;

	t_server srv = {0};
	if (app->client.hosting)
	{
		int retval = setup_server(&srv);
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
	}
	if (app->client.hosting && setup_client_host(&app->client))
		return -1;
	else if (!app->client.hosting && setup_client_client(&app->client, app->client.ip_str))
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
		if (strlen(packet->data.name) > 0)
			snprintf(srv->clients[id].name, 13, "%s", packet->data.name);
		else
			snprintf(srv->clients[id].name, 13, "Player %d", id + 1);
		printf("packet name: %s\nplayer name: %s\n", packet->data.name, srv->clients[id].name);
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
	// printf("msg id: %d pos: (%f, %f)\n", cmsg->id, cmsg->player.pos.x, cmsg->player.pos.y);
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

void	add_connection_message(t_server *srv, int player_id)
{
	char	buf[256];
	t_textqueue	*msg;

	snprintf(buf, 256, "%s connected", srv->clients[player_id].name);
	msg = textqueue_new(strdup(buf), 6000000, FC_GREEN, 0);
	textqueue_add_back(&srv->msg_queue, msg);
}

void	add_chat_message(t_server *srv, t_clientmsg *cmsg)
{
	char	buf[256];
	t_textqueue	*msg;

	snprintf(buf, 256, "%s: %s", srv->clients[cmsg->id].name, cmsg->chat);
	msg = textqueue_new(strdup(buf), 30000000, FC_BLACK, 0);
	textqueue_add_back(&srv->msg_queue, msg);
}

void	server_handle_msg(t_info *app, t_server *srv, packet_in *packet)
{
	t_clientmsg *cmsg = &packet->data;
	switch (packet->data.type) {
		case (CMT_CONNECT):
			server_handle_handshake(app, srv, packet);
			add_connection_message(srv, srv->n_clients - 1);
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
		case (CMT_CHAT):
			add_chat_message(srv, cmsg);
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

void	server_send_text_queue(t_server *srv)
{
	t_textqueue	*current = srv->msg_queue;
	t_servermsg	msg = {
		.type = SMT_TEXT,
	};

	while (current != NULL)
	{
		strncpy(msg.payload.text, current->str, 511);
		msg.payload.timeout = current->timeout;
		msg.payload.col = current->col;
		for (int i = 0; i < srv->n_clients; i++)
			server_send_msg(srv, &srv->clientaddr[i], &msg);
		current = current->next;
	}
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
	server_send_text_queue(srv);
	clear_textqueue(&srv->msg_queue);
	srv->msg_queue = NULL;
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

void	client_send_chat(t_info *app)
{
	t_clientmsg cmsg = {
		.id = app->client.id,
		.type = CMT_CHAT,
	};

	strncpy(cmsg.chat, app->client.chat, CMSG_CHAT_BUFSIZE - 1);
	app->input.len = 0;
	app->client.chat[0] = '\0';
	client_send_msg(&app->client, &cmsg);
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
		case (SMT_TEXT):
			// printf("n_serialdoors: %d\n", smsg->payload.n_serialdoors);
			deserialise_text(app, smsg);
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

t_textqueue	*textqueue_new(char *text, size_t timeout, t_fontcolor col, size_t time)
{
	t_textqueue *new = calloc(1, sizeof(*new));

	new->next = NULL;
	new->str = text;
	new->col = col;
	new->arrival_time = time;
	new->timeout = timeout;
	return new;
}

void	textqueue_add_back(t_textqueue **queue, t_textqueue *msg)
{
	if (queue == NULL)
		return ;
	if (*queue == NULL)
	{
		*queue = msg;
		return ;
	}

	t_textqueue *current = *queue;
	while (current->next != NULL)
		current = current->next;
	current->next = msg;
}

void	textqueue_add_front(t_textqueue **queue, t_textqueue *msg)
{
	if (queue == NULL)
		return ;
	if (*queue == NULL)
	{
		*queue = msg;
		return ;
	}

	msg->next = *queue;
	*queue = msg;
}

int	textqueue_len(t_textqueue *queue)
{
	int len = 0;
	while (queue != NULL)
	{
		len++;
		queue = queue->next;
	}
	return (len);
}

void	cull_textqueue(t_textqueue **queue, size_t time)
{
	if (queue == NULL || *queue == NULL)
		return ;

	t_textqueue *current = *queue;
	t_textqueue *tmp;
	while (current != NULL && time - current->arrival_time > current->timeout)
	{
		tmp = current;
		current = tmp->next;
		free(tmp->str);
		free(tmp);
	}
	*queue = current;
	if (current == NULL)
		return ;

	while (current->next != NULL)
	{
		if (time - current->next->arrival_time > current->next->timeout)
		{
			tmp = current->next;
			current->next = tmp->next;
			free(tmp->str);
			free(tmp);
		}
		else
		{
			current = current->next;
		}
	}
}

void	clear_textqueue(t_textqueue **queue)
{
	if (queue == NULL)
		return ;

	t_textqueue *current = *queue;
	while (current != NULL)
	{
		t_textqueue *tmp = current;
		current = current->next;
		free(tmp->str);
		free(tmp);
	}
	*queue = NULL;
}

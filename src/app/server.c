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

void	init_server_state(t_info *app, t_server *srv)
{
	app->srv = srv;
	app->lvl->serialdata[SMT_OBJS].type = SMT_OBJS;
	app->lvl->serialdata[SMT_DOORS].type = SMT_DOORS;
	app->lvl->serialdata[SMT_PLAYER].type = SMT_PLAYER;
	set_framerate(app, 120);

	// for (int i = 0; i < SRV_MAX_PLAYERS; i++)
	// {
	// 	srv->clients[i].health = 99;
	// 	srv->clients[i].max_health = 99;
	// 	srv->clients[i].max_ammo[P_BEAM] = -1;
	// 	srv->clients[i].pos = app->lvl->starting_pos;
	// 	srv->clients[i].id = i;
	// }
}

int	client_handle_handshake(t_info *app, t_client *client)
{
	socklen_t	len = sizeof(struct sockaddr_in);
	t_clientmsg	cmsg = {
		.id = -1,
		.type = CMT_CONNECT,
	};
	strncpy(cmsg.name, client->name, 12);

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
	if (id < 0)
		return (1);
	printf("client id: %d\n", id);
	client->id = id;
	usleep(100000);
	return (0);
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

t_playermult	*server_handle_handshake(t_info *app, t_server *srv, packet_in *packet)
{
	int	id;
	t_playermult *player = NULL;
	if (srv->n_clients == SRV_MAX_PLAYERS)
		id = -1;
	else
	{
		player = server_add_client(app, srv, packet);
		id = player->id;
	}
	sendto(
		srv->sockfd, (char *)&id, sizeof(id), 0,
		(const struct sockaddr *) &packet->sockbuf, sizeof(packet->sockbuf)
	);
	printf("server response sent. n_clients: %d count: %d\n", srv->n_clients, srv->id_count);
	return player;
	(void)app;
}

void	server_handle_projectiles(t_info *app, t_clientmsg *cdata)
{
	t_playermult *player = find_player_by_id(app->srv->playertree, cdata->id);

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
			player = find_player_by_id(app->srv->playertree, cdata->id);
			player->ammo[P_MISSILE] -= 1;
			if (player->ammo[P_MISSILE] == -1)
				player->ammo[P_MISSILE] = 0;
			break;
		case (PROJ_SUPER):
			spawn_projectile_server(
				app,
				cdata->proj.pos,
				cdata->proj.dir,
				app->lvl, P_SUPER, cdata->id
			);
			player = find_player_by_id(app->srv->playertree, cdata->id);
			player->ammo[P_SUPER] -= 1;
			if (player->ammo[P_SUPER] == -1)
				player->ammo[P_SUPER] = 0;
			break;
		default:
			break;
	}
}

void	server_handle_player(t_server *srv, t_clientmsg *cmsg)
{
	t_playermult *player = find_player_by_id(srv->playertree, cmsg->id);

	player->pos = cmsg->player.pos;
	player->dir = cmsg->player.dir;
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

void	add_connection_message(t_server *srv, t_playermult *player)
{
	char	buf[256];
	t_textqueue	*msg;

	snprintf(buf, 256, "%s connected", player->name);
	msg = textqueue_new(strdup(buf), 6000000, FC_GREEN, 0);
	textqueue_add_back(&srv->msg_queue, msg);
}

void	add_disconnection_message(t_server *srv, t_playermult *player)
{
	char	buf[256];
	t_textqueue	*msg;

	snprintf(buf, 256, "%s disconnected", player->name);
	msg = textqueue_new(strdup(buf), 6000000, FC_RED, 0);
	textqueue_add_back(&srv->msg_queue, msg);
}

void	add_chat_message(t_server *srv, t_clientmsg *cmsg)
{
	char	buf[256];
	t_textqueue	*msg;
	t_playermult *player = find_player_by_id(srv->playertree, cmsg->id);

	snprintf(buf, 256, "%s: %s", player->name, cmsg->chat);
	msg = textqueue_new(strdup(buf), 30000000, FC_BLACK, 0);
	textqueue_add_back(&srv->msg_queue, msg);
}

void	server_handle_msg(t_info *app, t_server *srv, packet_in *packet)
{
	t_clientmsg		*cmsg = &packet->data;
	t_playermult	*player;

	switch (packet->data.type) {
		case (CMT_CONNECT):
			player = server_handle_handshake(app, srv, packet);
			if (player)
				add_connection_message(srv, player);
			break;
		case (CMT_PROJ):
			server_handle_projectiles(app, cmsg);
			break;
		case (CMT_POS):
			server_handle_player(srv, cmsg);
			add_serialplayer(app, cmsg);
			break;
		case (CMT_DOOR):
			handle_open_door_server(app, cmsg->door.pos);
			break;
		case (CMT_CHAT):
			add_chat_message(srv, cmsg);
			break;
		case (CMT_DISCONNECT):
			add_disconnection_message(srv, find_player_by_id(srv->playertree, cmsg->id));
			server_remove_client(srv, cmsg->id);
			break;
		default:
			break;
	}
}

void	server_receive_messages(t_info *app, t_server *srv)
{
	errno = 0;
	// app->lvl->serialdata[SMT_OBJS].payload.n_serialobjs = 0;
	packet_in	packet;
	socklen_t	len = sizeof(packet.sockbuf);

	recvfrom(
		srv->sockfd, (char *)&packet.data, sizeof(t_clientmsg),
		0, (struct sockaddr *)&packet.sockbuf, &len
	);

	while (errno == 0)
	{
		server_handle_msg(app, srv, &packet);

		recvfrom(
			srv->sockfd, (char *)&packet.data, sizeof(t_clientmsg),
			0, (struct sockaddr *)&packet.sockbuf, &len
		);
	}
	// printf("messages received: %d\n", n_msgs);
}

void	server_send_msg(t_server *srv, struct sockaddr_in *client, t_servermsg *smsg)
{
	sendto(
		srv->sockfd, (char *)smsg, sizeof(t_servermsg), 0,
		(const struct sockaddr *) client, sizeof(*client)
	);
}

void	playertree_send_msg(t_playermult *player, void *msg)
{
	t_server *srv = player->srv;

	server_send_msg(srv, &player->sock, msg);
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
		traverse_playertree_arg(srv->playertree, PRE_ORD, playertree_send_msg, &msg);
		current = current->next;
	}
}

void	playertree_send_state(t_playermult *player, void *info)
{
	t_server	*srv = player->srv;
	t_info		*app = info;

	memcpy(&app->lvl->serialdata[SMT_PLAYER].payload, player, sizeof(t_playermult));
		server_send_msg(srv, &player->sock, &app->lvl->serialdata[SMT_OBJS]);
		server_send_msg(srv, &player->sock, &app->lvl->serialdata[SMT_DOORS]);
		server_send_msg(srv, &player->sock, &app->lvl->serialdata[SMT_PLAYER]);
}

void	server_send_messages(t_info *app, t_server *srv)
{
	traverse_playertree_arg(srv->playertree, PRE_ORD, playertree_send_state, app);
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

void	client_send_disconnect(t_info *app)
{
	t_client 	*client = &app->client;
	t_clientmsg	cmsg = {
		.id = app->client.id,
		.type = CMT_DISCONNECT,
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

t_playermult *playermult_new(char *name, int id)
{
	t_playermult *out = calloc(1, sizeof(*out));

	strncpy(out->name, name, 15);
	out->id = id;
	return out;
}

void	playertree_add(t_playermult **tree, t_playermult *player)
{
	if (tree == NULL)
		return ;

	t_playermult **addr = tree;
	t_playermult *current = *tree;

	while (current != NULL)
	{
		if (current->id > player->id)
			addr = &current->left;
		else if (current->id < player->id)
			addr = &current->right;
		else
			exit(1);
		current = *addr;
	}
	*addr = player;
}

void	traverse_playertree_arg(t_playermult *tree, t_treeorder order, void (*f)(t_playermult *, void *), void *param)
{
	if (tree == NULL)
		return ;

	switch (order) {
		case (PRE_ORD):
			f(tree, param);
			traverse_playertree_arg(tree->left, order, f, param);
			traverse_playertree_arg(tree->right, order, f, param);
			return ;
		case (IN_ORD):
			traverse_playertree_arg(tree->left, order, f, param);
			f(tree, param);
			traverse_playertree_arg(tree->right, order, f, param);
			return ;
		case (POST_ORD):
			traverse_playertree_arg(tree->left, order, f, param);
			traverse_playertree_arg(tree->right, order, f, param);
			f(tree, param);
			return ;
	}
}

void	traverse_playertree_arg2(t_playermult *tree, t_treeorder order, void (*f)(t_playermult *, void *, void *), void *param1, void *param2)
{
	if (tree == NULL)
		return ;

	switch (order) {
		case (PRE_ORD):
			f(tree, param1, param2);
			traverse_playertree_arg2(tree->left, order, f, param1, param2);
			traverse_playertree_arg2(tree->right, order, f, param1, param2);
			return ;
		case (IN_ORD):
			traverse_playertree_arg2(tree->left, order, f, param1, param2);
			f(tree, param1, param2);
			traverse_playertree_arg2(tree->right, order, f, param1, param2);
			return ;
		case (POST_ORD):
			traverse_playertree_arg2(tree->left, order, f, param1, param2);
			traverse_playertree_arg2(tree->right, order, f, param1, param2);
			f(tree, param1, param2);
			return ;
	}
}

void	traverse_playertree(t_playermult *tree, t_treeorder order, void (*f)(void *))
{
	if (tree == NULL)
		return ;

	switch (order) {
		case (PRE_ORD):
			f(tree);
			traverse_playertree(tree->left, order, f);
			traverse_playertree(tree->right, order, f);
			return ;
		case (IN_ORD):
			traverse_playertree(tree->left, order, f);
			f(tree);
			traverse_playertree(tree->right, order, f);
			return ;
		case (POST_ORD):
			traverse_playertree(tree->left, order, f);
			traverse_playertree(tree->right, order, f);
			f(tree);
			return ;
	}
}

void	clear_playertree(t_playermult **tree)
{
	traverse_playertree(*tree, POST_ORD, free);
	*tree = NULL;
}

void	print_playermult(t_playermult *player)
{
	printf("name: %s\n", player->name);
}

t_playermult	*find_player_by_id(t_playermult *tree, int id)
{
	t_playermult *current = tree;

	while (current != NULL)
	{
		if (current->id > id)
			current = current->left;
		else if (current->id < id)
			current = current->right;
		else
			break ;
	}

	return (current);
}

void	fill_clients_array(t_playermult *tree, t_playermult **arr, int *count)
{
	if (tree == NULL)
		return ;

	int idx = (*count)++;
	arr[idx] = tree;
	tree->arr_idx = idx;
	fill_clients_array(tree->left, arr, count);
	fill_clients_array(tree->right, arr, count);
}

t_playermult *playertree_get_successor(t_playermult *tree)
{
	tree = tree->right;

	while (tree != NULL && tree->left != NULL)
		tree = tree->left;

	return tree;
}

void	playermult_copy(t_playermult *dst, t_playermult *src)
{
	dst->pos = src->pos;
	dst->dir = src->dir;
	dst->dmg_dir = src->dmg_dir;
	dst->health = src->health;
	dst->max_health = src->max_health;
	dst->dmg_time = src->dmg_time;
	dst->event = src->event;
	dst->dead = src->dead;
	dst->id = src->id;
	memcpy(dst->ammo, src->ammo, sizeof(int) * 3);
	memcpy(dst->max_ammo, src->max_ammo, sizeof(int) * 3);
	strncpy(dst->name, src->name, SRV_MAX_NAMELEN);
	dst->sock = src->sock;
}

t_playermult *playertree_delete_node(t_playermult *tree, int id)
{
	if (tree == NULL)
		return tree;

	if (tree->id > id)
		tree->left = playertree_delete_node(tree->left, id);
	else if (tree->id < id)
		tree->right = playertree_delete_node(tree->right, id);
	else
	{
		if (tree->left == NULL)
		{
			t_playermult *tmp = tree->right;
			free(tree);
			return tmp;
		}
		if (tree->right == NULL)
		{
			t_playermult *tmp = tree->left;
			free(tree);
			return tmp;
		}
		t_playermult *successor = playertree_get_successor(tree);
		playermult_copy(tree, successor);
		tree->right = playertree_delete_node(tree, successor->id);
	}
	return (tree);
}

t_playermult	*server_add_client(t_info *app, t_server *srv, packet_in *packet)
{
	t_playermult *player;

	int id = srv->id_count++;
	srv->n_clients++;

	if (strlen(packet->data.name) > 0)
		player = playermult_new(packet->data.name, id);
	else
	{
		player = playermult_new("", id);
		snprintf(player->name, 16, "Player %d", id + 1);
	}
	memcpy(&player->sock, &packet->sockbuf, sizeof(packet->sockbuf));
	player->health = 99;
	player->max_health = 99;
	player->max_ammo[P_BEAM] = -1;
	player->pos = app->lvl->starting_pos;
	player->id = id;
	player->srv = srv;
	playertree_add(&srv->playertree, player);
	srv->n_clients = 0;
	fill_clients_array(srv->playertree, srv->clients, &srv->n_clients);
	if (srv->n_clients == 0)
		srv->playertree = NULL;
	return (player);
}

void	server_remove_client(t_server *srv, int id)
{
	srv->playertree = playertree_delete_node(srv->playertree, id);
	srv->n_clients = 0;
	fill_clients_array(srv->playertree, srv->clients, &srv->n_clients);
	if (srv->n_clients == 0)
		srv->playertree = NULL;
}

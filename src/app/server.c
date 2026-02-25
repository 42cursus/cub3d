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
#include <stddef.h>
#include <stdio.h>
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
    client->servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	return (0);
}

pid_t	launch_server(t_info *app)
{
	pid_t	pid = 0;
	t_server srv;

	int retval = setup_server(&srv);
	// int retval = 1;
	if (!retval)
	{
		pid = fork();
		if (pid == 0)
		{
			set_framerate(app, 120);
			server_loop(app, &srv);
			exit(0);
		}
		close(srv.sockfd);
	}
	if (setup_client(&app->client))
		return -1;
	return (pid);
}

int	server_receive_messages(t_info *app, t_server *srv)
{
	int		n_msgs = 0;
	size_t	n;
	socklen_t	len = sizeof(srv->clientaddr[0]);

	n = recvfrom(
		srv->sockfd, (char *)&srv->clientdata[n_msgs], sizeof(t_clientdata),
		0, (struct sockaddr *)&srv->clientaddr[n_msgs], &len
	);

	while (n > 0)
	{
		n_msgs++;
		n = recvfrom(
			srv->sockfd, (char *)&srv->clientdata[n_msgs], sizeof(t_clientdata),
			0, (struct sockaddr *)&srv->clientaddr[n_msgs], &len
		);
	}

	return (n_msgs);
}

void	server_apply_clientdata(t_info *app, t_server *srv, int n_msgs)
{
	for (int i = 0; i < n_msgs; i++)
	{
		switch (srv->clientdata[i].proj) {
			case (PROJ_BEAM):
				spawn_projectile_server(app, srv->clientdata[i].pos, srv->clientdata[i].dir, app->lvl, P_BEAM);
				break;
			case (PROJ_MISSILE):
				spawn_projectile_server(app, srv->clientdata[i].pos, srv->clientdata[i].dir, app->lvl, P_MISSILE);
				break;
			case (PROJ_SUPER):
				spawn_projectile_server(app, srv->clientdata[i].pos, srv->clientdata[i].dir, app->lvl, P_SUPER);
				break;
			default:
				break;
		}
	}
}

void	server_send_messages(t_info *app, t_server *srv, int n_msgs, socklen_t len)
{
	for (int i = 0; i < n_msgs; i++)
	{
		sendto(
			srv->sockfd, (char *)&app->lvl->serialdata, sizeof(t_serialdata), 0,
			(const struct sockaddr *) &srv->clientaddr[i], len
		);
	}
}

void	server_loop(t_info *app, t_server *srv)
{
	size_t		start = get_time_us();
	socklen_t	len = sizeof(srv->clientaddr[0]);

	while (true)
	{
		app->cdata.proj = PROJ_NONE;
		app->fr_last = get_time_us();
		int	n_msgs = server_receive_messages(app, srv);
		server_apply_clientdata(app, srv, n_msgs);

		update_objects(app, app->player, app->lvl);
		render_calc_time(app);


		printf("%7lu\tpos: (%.1f, %.1f) dir: (%.1f, %.1f)\n",
			(app->fr_last - start) / 1000,
			app->player->pos.x, app->player->pos.y,
			app->player->dir.x, app->player->dir.y
		);

		server_send_messages(app, srv, n_msgs, len);
	}
	(void)app;
	(void)srv;
	(void)len;
}

void	client_send_msg(t_info *app)
{
	t_client *client = &app->client;
	app->cdata.pos = app->player->pos;
	app->cdata.dir = app->player->dir;

	sendto(client->sockfd, (char *)&app->cdata, sizeof(t_clientdata), 0, (const struct sockaddr *) &client->servaddr, sizeof(client->servaddr));
}

void	client_receive_msg(t_info *app)
{
	socklen_t	len = sizeof(app->client.servaddr);

	size_t n = recvfrom(app->client.sockfd, (char *)&app->lvl->serialdata, sizeof(t_serialdata), 0, (struct sockaddr *)&app->client.servaddr, &len);
	(void)n;
}

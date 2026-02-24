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

	memset(&client->servaddr, 0, sizeof(client->servaddr));
    client->servaddr.sin_family = AF_INET;
    client->servaddr.sin_port = htons(8080);
    client->servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	return (0);
}

pid_t	launch_server(t_info *app)
{
	pid_t	pid = 0;
	t_server srv;

	int retval = setup_server(&srv);
	if (!retval)
	{
		pid = fork();
		if (pid == 0)
		{
			server_loop(app, &srv);
			exit(0);
		}
		close(srv.sockfd);
	}
	setup_client(&app->client);
	return (pid);
}

void	server_loop(t_info *app, t_server *srv)
{
	size_t		start = get_time_us();
	socklen_t	len = sizeof(srv->clients[0]);

	while (true)
	{
		app->cdata.proj = PROJ_NONE;
		/*size_t n = */ recvfrom(srv->sockfd, (char *)&app->cdata, sizeof(t_clientdata), 0, (struct sockaddr *)&srv->clients[0], &len);
		app->fr_last = get_time_us();
		app->player->pos = app->cdata.pos;
		app->player->dir = app->cdata.dir;
		switch (app->cdata.proj) {
			case (PROJ_BEAM):
				spawn_projectile_server(app, app->cdata.pos, app->cdata.dir, app->lvl, P_BEAM);
				break;
			case (PROJ_MISSILE):
				spawn_projectile_server(app, app->cdata.pos, app->cdata.dir, app->lvl, P_MISSILE);
				break;
			case (PROJ_SUPER):
				spawn_projectile_server(app, app->cdata.pos, app->cdata.dir, app->lvl, P_SUPER);
				break;
			default:
				break;
		}


		update_objects(app, app->player, app->lvl);
		


		printf("%7lu\tpos: (%.1f, %.1f) dir: (%.1f, %.1f)\n",
			(app->fr_last - start) / 1000,
			app->player->pos.x, app->player->pos.y,
			app->player->dir.x, app->player->dir.y
		 );



		sendto(
			srv->sockfd, (char *)&app->lvl->serialdata, sizeof(t_serialdata), 0,
			(const struct sockaddr *) &srv->clients[0], len
		);
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

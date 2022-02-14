//
// Created by root on 1/22/21.
//

#ifndef PROJEKT3_STRUCTURES_H
#define PROJEKT3_STRUCTURES_H

#include <semaphore.h>

#define kolumny 64
#define wiersze 32


#define UP 1
#define DOWN 2
#define LEFT 3
#define RIGHT 4

#define BLACK_SPACE 1
#define WHITE2115 2
#define BUSH 3
#define CAMP 4
#define COIN 5
#define BEAST 6
#define PLAYER 7
#define BACKG 8

struct conn
{
    char player_name[7];
    int player_count;
    sem_t connection;
    sem_t answer;
};

struct point_t
{
    int x;
    int y;
};

struct player_info_t
{
    char player_name[7];
    int player_id;
    int player_pid;
    int type;
    struct point_t position;
    int deaths;
    int coins_collected;
    int coins_brought;
    char znak;
    int bush;
};

struct server_t
{
    char map[wiersze][kolumny];
    int pid;
    struct point_t campsite;
    struct player_info_t info[4];
    int rounds;
    int dead_coins[wiersze][kolumny];
    int player_count;

    sem_t *beast1;
    sem_t *beast2;
};

struct connection_t
{
    char player_name[7];
    int player_id;
    int player_pid;
    int type;
    struct point_t position;
    struct point_t campsite;
    int deaths;
    int coins_collected;
    int coins_brought;
    char map[wiersze][kolumny];

    int rounds;
    int server_pid;
    char fov[5][5];
    int dir;

    int dc;
    sem_t sem1;
    sem_t sem2;
};

struct beast_t
{
    struct point_t b_position;
    pthread_t thread_beast;
    struct server_t *server;
    char znak;
    char beast_fov[5][5];
};

void l_map(char *filename, char array[wiersze][kolumny]);

void disp_map(struct server_t server);

void display_legend();

void spawn_tre(int key, struct server_t *server);

void display_server_stats(struct server_t server);

void player_map(struct server_t server, struct point_t position, char fov[5][5]);

void add_player(struct server_t *server, struct connection_t **conn, char *player_name, int number);

int player_move(struct connection_t **connection, struct server_t *server, int number);

void player_treasure(struct connection_t **connection, struct server_t *server, int number);

void del(struct server_t *server, struct connection_t **conn, char *player_name, int number);

void players_die(struct server_t *server, int number);

struct beast_t *beast_spawn(struct server_t *server);

void *beast_function(void *arg);

void beast_map(struct server_t server, struct point_t position, struct beast_t *beast, char fov[5][5]);

void chase(int dir, struct beast_t *beast);

#endif //PROJEKT3_STRUCTURES_H

//
// Created by root on 1/22/21.
//

#ifndef PROJEKT3_C_STRUCTURES_H
#define PROJEKT3_C_STRUCTURES_H
#define kolumny 64
#define wiersze 32

#include <semaphore.h>

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

struct player_t
{

    char map[wiersze][kolumny];
    int server_pid;
    struct point_t campsite;

    char player_name[7];
    int player_id;
    int player_pid;
    int type;
    struct point_t position;
    int deaths;
    int coins_collected;
    int coins_brought;
    int rounds;
};


void disp_map(char map[wiersze][kolumny]);

void player_map(struct player_t *player, char fov[5][5]);

void player_respond(struct player_t *player, struct connection_t **conn, char *player_name);

void player_move(int dir, struct connection_t **connect, struct player_t *player);

void display_player_stats(struct player_t *player);

void display_legend();

void copy_map(char (*destination)[kolumny], char (*source)[kolumny]);


#endif //PROJEKT3_C_STRUCTURES_H

//
// Created by root on 1/22/21.
//

#include <stdio.h>
#include <ncurses.h>
#include <pthread.h>
#include <stdlib.h>
#include "c_structures.h"
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <semaphore.h>

void copy_map(char (*destination)[kolumny], char (*source)[kolumny])
{
    for (int i = 0; i < wiersze; i++)
    {
        for (int j = 0; j < kolumny; j++)
        {
            if (*(*(destination + i) + j) != '@')
            {
                *(*(destination + i) + j) = *(*(source + i) + j);
            }
        }
    }
}

void disp_map(char map[wiersze][kolumny])
{
    move(0, 0);
    for (int i = 0; i < wiersze; i++)
    {
        for (int j = 0; j < kolumny; j++)
        {
            if (map[i][j] == 'W')
            {
                attron(COLOR_PAIR(BLACK_SPACE));
                printw(" ");
                attroff(COLOR_PAIR(BLACK_SPACE));
            } else
            {
                switch (map[i][j])
                {
                    case ' ':
                    {
                        attron(COLOR_PAIR(WHITE2115));
                        printw("%c", map[i][j]);
                        attroff(COLOR_PAIR(WHITE2115));
                        break;
                    }
                    case '#':
                    {
                        attron(COLOR_PAIR(BUSH));
                        printw("%c", map[i][j]);
                        attroff(COLOR_PAIR(BUSH));
                        break;
                    }
                    case 'A':
                    {
                        attron(COLOR_PAIR(CAMP));
                        printw("%c", map[i][j]);
                        attroff(COLOR_PAIR(CAMP));
                        break;
                    }
                    case 'c':
                    {
                        attrset(COLOR_PAIR(COIN));
                        printw("%c", map[i][j]);
                        attroff(COLOR_PAIR(COIN));
                        break;
                    }
                    case 't':
                    {
                        attrset(COLOR_PAIR(COIN));
                        printw("%c", map[i][j]);
                        attroff(COLOR_PAIR(COIN));
                        break;
                    }
                    case 'T':
                    {
                        attrset(COLOR_PAIR(COIN));
                        printw("%c", map[i][j]);
                        attroff(COLOR_PAIR(COIN));
                        break;
                    }
                    case '@':
                    {
                        printw("%c", map[i][j]);
                        break;
                    }
                    case 'P':
                    {
                        attron(COLOR_PAIR(BACKG));
                        printw("%c", map[i][j]);
                        attroff(COLOR_PAIR(BACKG));
                        break;
                    }
                    case 'D':
                    {
                        attrset(COLOR_PAIR(COIN));
                        printw("%c", map[i][j]);
                        attroff(COLOR_PAIR(COIN));
                        break;
                    }
                    case '*':
                    {
                        attron(COLOR_PAIR(BEAST));
                        printw("%c", map[i][j]);
                        attroff(COLOR_PAIR(BEAST));
                        break;
                    }

                }

            }

        }
        printw("\n");
    }

}

void display_legend()
{

    move(22, kolumny + 5);
    printw("Legend:");
    move(23, kolumny + 5);
    attrset(COLOR_PAIR(PLAYER));
    printw("1234");
    attroff(COLOR_PAIR(PLAYER));
    printw(" - players");
    move(24, kolumny + 5);
    attrset(COLOR_PAIR(BLACK_SPACE));
    printw(" ");
    attroff(COLOR_PAIR(BLACK_SPACE));
    printw(" - wall");
    move(25, kolumny + 5);
    attrset(COLOR_PAIR(BUSH));
    printw("#");
    attroff(COLOR_PAIR(BUSH));
    printw(" - bushes (slow down)");
    move(26, kolumny + 5);
    attrset(COLOR_PAIR(BEAST));
    printw("*");
    attroff(COLOR_PAIR(BEAST));
    printw(" - beasts");
    move(27, kolumny + 5);
    attrset(COLOR_PAIR(COIN));
    printw("c");
    attroff(COLOR_PAIR(COIN));
    printw(" - one coin");
    move(28, kolumny + 5);
    attrset(COLOR_PAIR(COIN));
    printw("t");
    attroff(COLOR_PAIR(COIN));
    printw(" - treasure (10 coins)");
    move(29, kolumny + 5);
    attrset(COLOR_PAIR(COIN));
    printw("T");
    attroff(COLOR_PAIR(COIN));
    printw(" - large treasure (50 coins)");
    move(30, kolumny + 5);
    attrset(COLOR_PAIR(COIN));
    printw("D");
    attroff(COLOR_PAIR(COIN));
    printw(" - dropped coins");
    move(31, kolumny + 5);
    attrset(COLOR_PAIR(CAMP));
    printw("A");
    attroff(COLOR_PAIR(CAMP));
    printw(" - camp");
    refresh();
}

void display_player_stats(struct player_t *player)
{

    move(0, kolumny + 5);
    printw("Server's PID: %d", player->server_pid);
    move(1, kolumny + 5);
    printw("Campsite X/Y: %d/%d ", player->campsite.x, player->campsite.y);
    move(2, kolumny + 5);
    printw("Round number: %ld", player->rounds);
    move(4, kolumny + 5);
    printw("Player");
    move(5, kolumny + 5);
    printw("Number:    %d", player->player_id);
    move(6, kolumny + 5);
    printw("Type       "
           "HUMAN");
    move(7, kolumny + 5);
    printw("Curr X/Y   %d/%d", player->position.x, player->position.y);
    move(8, kolumny + 5);
    printw("Deaths     %d", player->deaths);
    move(10, kolumny + 5);
    printw("Coins:");
    move(11, kolumny + 5);
    printw("found      %d", player->coins_collected);
    move(12, kolumny + 5);
    printw("brought    %d", player->coins_brought);


    refresh();
}

void player_map(struct player_t *player, char fov[5][5])
{
    int y1 = player->position.y;
    int x1 = player->position.x;

    int y2 = player->position.y;
    int x2 = player->position.x;

    for (int i = 1; i <= 2; i++)
    {
        y1 -= 1;
        if (y1 == 0) break;
    }
    for (int i = 1; i <= 2; i++)
    {
        y2 += 1;
        if (y2 == 31) break;
    }
    for (int i = 1; i <= 2; i++)
    {
        x1 -= 1;
        if (x1 == 0) break;
    }
    for (int i = 1; i <= 2; i++)
    {
        x2 += 1;
        if (x2 == 63)
        {

            break;
        }
    }

    int i, j;
    for (i = y1; i <= y2; i++)
    {
        for (j = x1; j <= x2; j++)
        {
            player->map[i][j] = fov[i - y1][j - x1];
            if (player->map[i][j] == 'A')
            {
                player->campsite.x = 30;
                player->campsite.y = 14;
            }
        }
    }
    fov[i - y1][j - x1] = '\0';


}


void player_respond(struct player_t *player, struct connection_t **conn, char *player_name)
{
    int fd = shm_open(player_name, O_CREAT | O_RDWR, 0777);

    ftruncate(fd, sizeof(struct connection_t));

    *conn = (struct connection_t *) mmap(NULL, sizeof(struct connection_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    (*conn)->player_pid = player->player_pid;
    (*conn)->type = 2;
    (*conn)->dc = 0;

    sem_post(&(*conn)->sem2);
    sem_wait(&(*conn)->sem1);

    player->type = (*conn)->type;
    player->server_pid = (*conn)->server_pid;
    player->rounds = (*conn)->rounds;
    player->position = (*conn)->position;
    player->deaths = (*conn)->deaths;
    player->coins_brought = (*conn)->coins_brought;
    player->coins_collected = (*conn)->coins_collected;
    player->player_id = (*conn)->player_id;
    player_map(player, (*conn)->fov);
    strcpy(player->player_name, (*conn)->player_name);


}

void player_move(int dir, struct connection_t **connect, struct player_t *player)
{
    (*connect)->dir = dir;

    sem_post(&(*connect)->sem2);
    sem_wait(&(*connect)->sem1);

    player->deaths = (*connect)->deaths;
    player->rounds = (*connect)->rounds;
    player->position.x = (*connect)->position.x;
    player->position.y = (*connect)->position.y;
    player->coins_brought = (*connect)->coins_brought;
    player->coins_collected = (*connect)->coins_collected;

}


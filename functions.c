#include <stdio.h>
#include <ncurses.h>
#include <pthread.h>
#include <stdlib.h>
#include "structures.h"
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <semaphore.h>
#include <time.h>

void l_map(char *filename, char array[wiersze][kolumny])
{
    FILE *fp;
    fp = fopen(filename, "r");

    if (!fp)
    {
        printf("File open error\n");
        return;
    }
    fseek(fp, 0, SEEK_SET);
    char zm;
    for (int i = 0; i < wiersze; i++)
        for (int j = 0; j < kolumny; j++)
        {
            fscanf(fp, "%c", &zm);
            if (zm != '\n' && zm != '\r' && zm != '\0')
                *(*(array + i) + j) = zm;
            else j--;
        }

}

void disp_map(struct server_t server)
{
    move(0, 0);
    for (int i = 0; i < wiersze; i++)
    {
        for (int j = 0; j < kolumny; j++)
        {
            if (server.map[i][j] == 'W')
            {
                attron(COLOR_PAIR(BLACK_SPACE));
                printw(" ");
                attroff(COLOR_PAIR(BLACK_SPACE));
            } else
            {
                switch (server.map[i][j])
                {
                    case ' ':
                    {
                        attron(COLOR_PAIR(WHITE2115));
                        printw("%c", server.map[i][j]);
                        attroff(COLOR_PAIR(WHITE2115));
                        break;
                    }
                    case '#':
                    {
                        attron(COLOR_PAIR(BUSH));
                        printw("%c", server.map[i][j]);
                        attroff(COLOR_PAIR(BUSH));
                        break;
                    }
                    case 'A':
                    {
                        attron(COLOR_PAIR(CAMP));
                        printw("%c", server.map[i][j]);
                        attroff(COLOR_PAIR(CAMP));
                        break;
                    }
                    case 'c':
                    {
                        attrset(COLOR_PAIR(COIN));
                        printw("%c", server.map[i][j]);
                        attroff(COLOR_PAIR(COIN));
                        break;
                    }
                    case 't':
                    {
                        attrset(COLOR_PAIR(COIN));
                        printw("%c", server.map[i][j]);
                        attroff(COLOR_PAIR(COIN));
                        break;
                    }
                    case 'T':
                    {
                        attrset(COLOR_PAIR(COIN));
                        printw("%c", server.map[i][j]);
                        attroff(COLOR_PAIR(COIN));
                        break;
                    }
                    case 'P':
                    {
                        attron(COLOR_PAIR(BACKG));
                        printw("%c", server.map[i][j]);
                        attroff(COLOR_PAIR(BACKG));
                        break;
                    }
                    case 'D':
                    {
                        attrset(COLOR_PAIR(COIN));
                        printw("%c", server.map[i][j]);
                        attroff(COLOR_PAIR(COIN));
                        break;
                    }
                    case '*':
                    {
                        attron(COLOR_PAIR(BEAST));
                        printw("%c", server.map[i][j]);
                        attroff(COLOR_PAIR(BEAST));
                        break;
                    }
                }

            }

        }
        printw("\n");
    }

    //refresh();
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

void spawn_tre(int key, struct server_t *server)
{
    srand(time(NULL));
    int x_pos, y_pos;
    while (1)
    {
        y_pos = rand() % 31;
        x_pos = rand() % 63;
        int again = 0;

        if (server->map[y_pos][x_pos] == ' ')
        {
            for (int i = 0; i < 4; i++)
            {
                if ((server->info[i].position.x == x_pos && server->info[i].position.y == y_pos))
                {
                    again = 1;
                    break;
                }
            }

            if (again == 0) break;
        }

    }

    server->map[y_pos][x_pos] = (char) key;
}

void display_server_stats(struct server_t server)
{

    int p = kolumny + 19;

    move(0, kolumny + 5);
    printw("Server's PID: %d", server.pid);
    move(1, kolumny + 5);
    printw("Campsite X/Y: %d/%d ", server.campsite.x, server.campsite.y);
    move(2, kolumny + 5);
    printw("Round number: %ld", server.rounds);
    move(4, kolumny + 5);
    printw("Parameter:    Player1  Player2  Player3  Player4");
    move(5, kolumny + 5);
    printw("PID");
    move(6, kolumny + 5);
    printw("TYPE");
    move(7, kolumny + 5);
    printw("Curr X/Y");
    move(8, kolumny + 5);
    printw("Deaths");
    move(10, kolumny + 5);
    printw("Coins:");
    move(11, kolumny + 5);
    printw("carried");
    move(12, kolumny + 5);
    printw("brought");

    for (int i = 0; i < 4; i++)
    {
        move(5, p);
        if (server.info[i].player_pid) printw("%d", server.info[i].player_pid);
        else printw("-     ");
        move(6, p);
        if (server.info[i].type == 1) printw("CPU");
        else if (server.info[i].type == 2) printw("HUMAN");
        else printw("-    ");
        move(7, p);
        if (server.info[i].player_pid) printw("%d/%d  ", server.info[i].position.x, server.info[i].position.y);
        else printw("-/-  ");
        move(8, p);
        if (server.info[i].player_pid) printw("%d ", server.info[i].deaths);
        else printw("- ");
        move(11, p);
        if (server.info[i].player_pid) printw("%d  ", server.info[i].coins_collected);
        else printw("-   ");
        move(12, p);
        if (server.info[i].player_pid) printw("%d   ", server.info[i].coins_brought);
        else printw("-    ");
        p += 9;
    }

    refresh();
}

void player_map(struct server_t server, struct point_t position, char fov[5][5])
{
    char mapa[wiersze][kolumny];
    memcpy(mapa, server.map, sizeof(server.map));

    int y1 = position.y;
    int x1 = position.x;

    int y2 = position.y;
    int x2 = position.x;

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
            fov[i - y1][j - x1] = mapa[i][j];
            //move(i,j);
            //printw("H");
        }
    }
    fov[i - y1][j - x1] = '\0';

}

void add_player(struct server_t *server, struct connection_t **conn, char *player_name, int number)
{
    int fd = shm_open(player_name, O_CREAT | O_RDWR, 0777);

    ftruncate(fd, sizeof(struct connection_t));

    *conn = (struct connection_t *) mmap(NULL, sizeof(struct connection_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    sem_init(&(*conn)->sem1, 1, 0);
    sem_init(&(*conn)->sem2, 1, 0);


    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += 3;

    if (sem_timedwait(&(*conn)->sem2, &ts))
    {
        del(server,conn,player_name,number);
        return;
    }



    (*conn)->deaths = server->info[number].deaths;
    strcpy((*conn)->player_name, player_name);
    (*conn)->server_pid = server->pid;
    (*conn)->rounds = server->rounds;
    (*conn)->coins_collected = server->info[number].coins_collected;
    (*conn)->coins_brought = server->info[number].coins_brought;
    (*conn)->player_id = server->info[number].player_id;
    (*conn)->campsite.x = -1;
    (*conn)->campsite.y = -1;
    (*conn)->dc = 0;
    memcpy((*conn)->map, server->map, sizeof(server->map));


    //x=4 y=2  x=4 y=29  x=59 y=2   x=59 y=29
    if (number == 0)
    {
        (*conn)->position.y = 2;
        (*conn)->position.x = 4;
    }
    if (number == 1)
    {
        (*conn)->position.y = 29;
        (*conn)->position.x = 4;
    }
    if (number == 2)
    {
        (*conn)->position.y = 2;
        (*conn)->position.x = 59;
    }
    if (number == 3)
    {
        (*conn)->position.y = 29;
        (*conn)->position.x = 59;
    }
    server->info[number].position.y = (*conn)->position.y;
    server->info[number].position.x = (*conn)->position.x;
    server->info[number].znak = ' ';
    server->info[number].bush = 0;
    server->player_count++;
    player_map(*server, server->info[number].position, (*conn)->fov);

    sem_post(&(*conn)->sem1);

    server->info[number].player_pid = (*conn)->player_pid;
    server->info[number].type = (*conn)->type;
}

int player_move(struct connection_t **connection, struct server_t *server, int number)
{
    int new_x = server->info[number].position.x;
    int new_y = server->info[number].position.y;

    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += 3;

    if (sem_timedwait(&(*connection)->sem2, &ts))
        return 1;
    //sem_wait(&(*connection)->sem2);

    int dir = (*connection)->dir;

    if (dir == UP)
        new_y--;
    else if (dir == DOWN)
        new_y++;
    else if (dir == LEFT)
        new_x--;
    else if (dir == RIGHT)
        new_x++;
    if (dir == ERR)
        new_x += 0;

    if (server->map[new_y][new_x] != 'W')
    {
        if (server->info[number].znak == '#')
            server->info[number].bush++;

        //
        if (server->info[number].bush != 1)
        {
            server->map[server->info[number].position.y][server->info[number].position.x] = server->info[number].znak;
            server->info[number].znak = server->map[new_y][new_x];
            if (server->info[number].znak == 'c' || server->info[number].znak == 't' ||
                server->info[number].znak == 'T' ||
                server->info[number].znak == 'D')
                server->info[number].znak = ' ';

            //
            server->info[number].bush = 0;

            int old_x = server->info[number].position.x;
            int old_y = server->info[number].position.y;

            server->info[number].position.y = new_y;
            server->info[number].position.x = new_x;
            for (int i = 0; i < 4; i++)
            {
                if (number != i && (new_x == server->info[i].position.x) &&
                    (new_y == server->info[i].position.y))
                {
                    players_die(server, number);
                    players_die(server, i);
                }

            }

            if (server->map[new_y][new_x] == '*')
            {
                server->info[number].position.y = old_y;
                server->info[number].position.x = old_x;
                players_die(server, number);
            }


            (*connection)->position.y = server->info[number].position.y;
            (*connection)->position.x = server->info[number].position.x;
        }
    }


    (*connection)->position.y = server->info[number].position.y;
    (*connection)->position.x = server->info[number].position.x;
    (*connection)->deaths = server->info[number].deaths;
    (*connection)->coins_collected = server->info[number].coins_collected;


    player_treasure(connection, server, number);

    memcpy((*connection)->map, server->map, sizeof(server->map));

    player_map(*server, (*connection)->position, (*connection)->fov);
    sem_post(&(*connection)->sem1);

    return 0;
}

void players_die(struct server_t *server, int number)
{
    server->dead_coins[server->info[number].position.y][server->info[number].position.x] += server->info[number].coins_collected;
    server->map[server->info[number].position.y][server->info[number].position.x] = 'D';
    server->info[number].znak = ' ';
    if (number == 0)
    {
        server->info[number].position.y = 2;
        server->info[number].position.x = 4;
    }
    if (number == 1)
    {
        server->info[number].position.y = 29;
        server->info[number].position.x = 4;
    }
    if (number == 2)
    {
        server->info[number].position.y = 2;
        server->info[number].position.x = 59;
    }
    if (number == 3)
    {
        server->info[number].position.y = 29;
        server->info[number].position.x = 59;
    }
    server->info[number].deaths++;
    server->info[number].coins_collected = 0;


}

void player_treasure(struct connection_t **connection, struct server_t *server, int number)
{
    int x = server->info[number].position.x;
    int y = server->info[number].position.y;

    if (server->map[y][x] == 'c')
    {
        server->map[y][x] = ' ';
        server->info[number].coins_collected += 1;
    }

    if (server->map[y][x] == 't')
    {
        server->map[y][x] = ' ';
        server->info[number].coins_collected += 10;
    }

    if (server->map[y][x] == 'T')
    {
        server->map[y][x] = ' ';
        server->info[number].coins_collected += 50;
    }

    if (server->map[y][x] == 'A')
    {
        server->info[number].coins_brought += server->info[number].coins_collected;
        server->info[number].coins_collected = 0;
    }


    if (server->map[y][x] == 'D')
    {
        server->map[y][x] = ' ';
        server->info[number].coins_collected += server->dead_coins[y][x];
        server->dead_coins[y][x] = 0;
    }

    (*connection)->coins_brought = server->info[number].coins_brought;
    (*connection)->coins_collected = server->info[number].coins_collected;

}

void del(struct server_t *server, struct connection_t **conn, char *player_name, int number)
{
    server->map[server->info[number].position.y][server->info[number].position.x] = server->info[number].znak;
    server->info[number].znak = ' ';
    server->info[number].coins_brought = 0;
    server->info[number].coins_collected = 0;
    server->info[number].position.x = 0;
    server->info[number].position.y = 0;
    server->info[number].player_pid = 0;
    server->info[number].player_id = number + 1;
    server->info[number].deaths = 0;
    server->info[number].type = 0;
    server->player_count--;
    (*conn)->deaths = server->info[number].deaths;
    strcpy((*conn)->player_name, player_name);
    (*conn)->server_pid = server->pid;
    (*conn)->rounds = server->rounds;
    (*conn)->coins_collected = server->info[number].coins_collected;
    (*conn)->coins_brought = server->info[number].coins_brought;
    (*conn)->player_id = server->info[number].player_id;
    (*conn)->campsite.x = -1;
    (*conn)->campsite.y = -1;
    (*conn)->dc = 0;
    sem_destroy(&(*conn)->sem1);
    sem_destroy(&(*conn)->sem2);

    munmap(*conn, sizeof(struct connection_t));
    shm_unlink(player_name);

}

struct beast_t *beast_spawn(struct server_t *server)
{
    srand(time(NULL));
    int x_pos, y_pos;
    while (1)
    {
        y_pos = rand() % 31;
        x_pos = rand() % 63;
        int again = 0;

        if (server->map[y_pos][x_pos] == ' ')
        {
            for (int i = 0; i < 4; i++)
            {
                if ((server->info[i].position.x == x_pos && server->info[i].position.y == y_pos))
                {
                    again = 1;
                    break;
                }
            }

            if (again == 0) break;
        }

    }
    server->map[y_pos][x_pos] = '*';

    struct beast_t *beast = (struct beast_t *) calloc(1, sizeof(struct beast_t));
    if (!beast) return NULL;
    beast->b_position.y = y_pos;
    beast->b_position.x = x_pos;
    beast->server = server;
    beast->znak = ' ';
    pthread_create(&beast->thread_beast, NULL, beast_function, beast);

    return beast;
}

void *beast_function(void *arg)
{
    struct beast_t *beast = (struct beast_t *) arg;
    struct server_t *server = beast->server;

    while (1)
    {
        sem_wait(server->beast1);
        beast_map(*server, beast->b_position, beast, beast->beast_fov);
        sem_post(server->beast2);
    }


    return NULL;
}

void beast_map(struct server_t server, struct point_t position, struct beast_t *beast, char fov[5][5])
{
    char mapa[wiersze][kolumny];
    memcpy(mapa, server.map, sizeof(server.map));
    memset(fov, 0, sizeof(beast->beast_fov));
    int y1 = position.y;
    int x1 = position.x;

    int y2 = position.y;
    int x2 = position.x;

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
    int player = 0;
    struct point_t player_pos;
    for (i = y1; i <= y2; i++)
    {
        for (j = x1; j <= x2; j++)
        {
            fov[i - y1][j - x1] = mapa[i][j];
            if (fov[i - y1][j - x1] == 'P')
            {
                player_pos.y = i - y1;
                player_pos.x = j - x1;
                player = 1;
            }

        }
    }
    fov[i - y1][j - x1] = '\0';


    if (player)
    {
        //              //
        //  wiersz [1]  //
        //              //
        if (player_pos.y == 0)
        {
            if (player_pos.x == 0)
            {
                if (fov[1][2] != 'W' && fov[1][1] != 'W' && fov[0][1] != 'W')
                    chase(UP, beast);
                else if (fov[2][1] != 'W' && fov[1][1] != 'W' && fov[1][0] != 'W')
                    chase(LEFT, beast);
            }
            if (player_pos.x == 1)
            {
                if (fov[1][2] != 'W' && fov[1][1] != 'W')
                    chase(UP, beast);
            }
            if (player_pos.x == 2)
            {
                if (fov[1][2] != 'W')
                    chase(UP, beast);
            }
            if (player_pos.x == 3)
            {
                if (fov[1][2] != 'W' && fov[1][3] != 'W')
                    chase(UP, beast);
            }
            if (player_pos.x == 4)
            {
                if (fov[1][2] != 'W' && fov[1][3] != 'W' && fov[0][3] != 'W')
                    chase(UP, beast);
                else if (fov[2][3] != 'W' && fov[1][3] != 'W' && fov[1][4] != 'W')
                    chase(RIGHT, beast);
            }
        }
        //              //
        //  wiersz [2]  //
        //              //

        if (player_pos.y == 1)
        {
            if (player_pos.x == 0)
            {
                if (fov[2][1] != 'W' && fov[1][1] != 'W')
                    chase(LEFT, beast);
            }
            if (player_pos.x == 1)
            {
                if (fov[2][1] != 'W')
                    chase(LEFT, beast);
                else if (fov[1][2] != 'W')
                    chase(UP, beast);
            }
            if (player_pos.x == 2)
            {
                chase(UP, beast);
            }
            if (player_pos.x == 3)
            {
                if (fov[2][3] != 'W')
                    chase(RIGHT, beast);
                else if (fov[1][2] != 'W')
                    chase(UP, beast);
            }
            if (player_pos.x == 4)
            {
                if (fov[2][3] != 'W' && fov[1][3] != 'W')
                    chase(RIGHT, beast);
            }
        }
        //              //
        //  wiersz [3]  //
        //              //

        if (player_pos.y == 2)
        {
            if (player_pos.x == 0)
            {
                if (fov[2][1] != 'W')
                    chase(LEFT, beast);
            }
            if (player_pos.x == 1)
            {
                chase(LEFT, beast);
            }
            //if (player_pos.x == 2)
            //{
            //    players_die(server,)
            //}
            if (player_pos.x == 3)
            {
                chase(RIGHT, beast);
            }
            if (player_pos.x == 4)
            {
                if (fov[2][3] != 'W')
                    chase(RIGHT, beast);
            }
        }
        //              //
        //  wiersz [4]  //
        //              //

        if (player_pos.y == 3)
        {
            if (player_pos.x == 0)
            {
                if (fov[2][1] != 'W' && fov[3][1] != 'W')
                    chase(LEFT, beast);
            }
            if (player_pos.x == 1)
            {
                if (fov[2][1] != 'W')
                    chase(LEFT, beast);
                else if (fov[3][2] != 'W')
                    chase(DOWN, beast);
            }
            if (player_pos.x == 2)
            {
                chase(DOWN, beast);
            }
            if (player_pos.x == 3)
            {
                if (fov[2][3] != 'W')
                    chase(RIGHT, beast);
                else if (fov[3][2] != 'W')
                    chase(DOWN, beast);
            }
            if (player_pos.x == 4)
            {
                if (fov[2][3] != 'W' && fov[3][3] != 'W')
                    chase(RIGHT, beast);
            }
        }
        //              //
        //  wiersz [5]  //
        //              //
        if (player_pos.y == 4)
        {
            if (player_pos.x == 0)
            {
                if (fov[3][2] != 'W' && fov[3][1] != 'W' && fov[4][1] != 'W')
                    chase(DOWN, beast);
                else if (fov[2][1] != 'W' && fov[3][1] != 'W' && fov[3][0] != 'W')
                    chase(LEFT, beast);
            }
            if (player_pos.x == 1)
            {
                if (fov[3][2] != 'W' && fov[3][1] != 'W')
                    chase(DOWN, beast);
            }
            if (player_pos.x == 2)
            {
                if (fov[3][2] != 'W')
                    chase(DOWN, beast);
            }
            if (player_pos.x == 3)
            {
                if (fov[3][2] != 'W' && fov[3][3] != 'W')
                    chase(DOWN, beast);
            }
            if (player_pos.x == 4)
            {
                if (fov[3][2] != 'W' && fov[3][3] != 'W' && fov[4][3] != 'W')
                    chase(DOWN, beast);
                else if (fov[2][3] != 'W' && fov[3][3] != 'W' && fov[3][4] != 'W')
                    chase(RIGHT, beast);
            }
        }
    }
    player = 0;
}

void chase(int dir, struct beast_t *beast)
{
    struct server_t *server = beast->server;
    int new_x = beast->b_position.x;
    int new_y = beast->b_position.y;
    int flag = 1;

    if (dir == UP)
        new_y--;
    else if (dir == DOWN)
        new_y++;
    else if (dir == LEFT)
        new_x--;
    else if (dir == RIGHT)
        new_x++;
    else if (dir == ERR)
        new_x += 0;

    server->map[beast->b_position.y][beast->b_position.x] = beast->znak;
    beast->znak = server->map[new_y][new_x];


    if (beast->znak == 'P')
    {
        for (int i = 0; i < 4; i++)
        {
            if ((new_x == server->info[i].position.x) &&
                (new_y == server->info[i].position.y))
            {
                players_die(server, i);
                flag = 0;
            }
        }
        beast->znak = server->map[beast->b_position.y][beast->b_position.x];
        server->map[beast->b_position.y][beast->b_position.x] = '*';
    }

    if (flag)
    {
        beast->b_position.y = new_y;
        beast->b_position.x = new_x;
        server->map[new_y][new_x] = '*';
    }

}




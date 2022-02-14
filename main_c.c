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

int main()
{
    //          //
    // ncurses  //
    //          //
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);

    if (has_colors() == FALSE)
    {
        endwin();
        printf("Your terminal does not support color\n");
        return 1;
    }
    //                        //
    // inicjalizacja kolorow  //
    //                        //
    start_color();
    init_pair(BACKG, COLOR_MAGENTA, COLOR_MAGENTA);
    init_pair(BLACK_SPACE, COLOR_BLACK, COLOR_BLUE);
    init_pair(WHITE2115, COLOR_BLACK, COLOR_WHITE);
    init_pair(BUSH, COLOR_YELLOW, COLOR_GREEN);
    init_pair(COIN, COLOR_BLACK, COLOR_YELLOW);
    init_pair(BEAST, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(PLAYER, COLOR_BLACK, COLOR_RED);
    init_pair(CAMP, COLOR_BLACK, COLOR_CYAN);
    //                          //
    // zainicjalizowanie gracza //
    //                          //
    struct player_t player1;
    player1.player_id = 0;
    player1.player_pid = getpid();
    player1.coins_collected = 0;
    player1.coins_brought = 0;
    player1.type = 2;
    player1.deaths = 0;
    player1.rounds = 0;
    player1.campsite.x = -1;
    player1.campsite.y = -1;
    player1.position.y = -1;
    player1.position.x = -1;
    memset(player1.map, '@', sizeof(player1.map));
    //                    //
    // pamiec pomocnicza: //
    //                    //
    int jd = shm_open("connect_to_serv", O_CREAT | O_RDWR, 0777);

    if (jd == -1)
        return 1;

    ftruncate(jd, sizeof(struct conn));

    struct conn *mem_ptr;

    mem_ptr = (struct conn *) mmap(NULL, sizeof(struct conn), PROT_READ | PROT_WRITE, MAP_SHARED, jd, 0);

    sem_init(&mem_ptr->connection, 1, 0);
    sem_init(&mem_ptr->answer, 1, 0);

    if (mem_ptr->player_count >= 4)
    {
        move(32, 32);
        printw("SERVER IS FULL");
        refresh();
        getch();
        endwin();
        return 1;
    }

    sem_post(&mem_ptr->connection);
    sem_wait(&mem_ptr->answer);


    strcpy(player1.player_name, mem_ptr->player_name);
    //                                                      //
    //  inicjalizacja pamieci wspoldzielonej client<>server //
    //                                                      //
    struct connection_t *ptr;
    player_respond(&player1, &ptr, player1.player_name);
    int key;

    //                //
    //  PETLA GLOWNA  //
    //                //

    while (1)
    {
        copy_map(player1.map, ptr->map);
        disp_map(player1.map);
        display_legend();
        display_player_stats(&player1);

        attron(COLOR_PAIR(BACKG));
        move(player1.position.y, player1.position.x);
        printw(" ");
        attroff(COLOR_PAIR(BACKG));


        timeout(100);
        key = getch();
        if (key == 'q' || key == 'Q')
            break;

        if (key == KEY_UP)
        {
            player_move(UP, &ptr, &player1);
            player_map(&player1, ptr->fov);
            disp_map(player1.map);
        }

        if (key == KEY_DOWN)
        {
            player_move(DOWN, &ptr, &player1);
            player_map(&player1, ptr->fov);
            disp_map(player1.map);
        }

        if (key == KEY_LEFT)
        {
            player_move(LEFT, &ptr, &player1);
            player_map(&player1, ptr->fov);
            disp_map(player1.map);
        }

        if (key == KEY_RIGHT)
        {
            player_move(RIGHT, &ptr, &player1);
            player_map(&player1, ptr->fov);
            disp_map(player1.map);
        }

        if (key == ERR)
        {
            player_move(ERR, &ptr, &player1);
            player_map(&player1, ptr->fov);
            disp_map(player1.map);
        }

    }
    //           //
    //  closing  //
    //           //
    ptr->dc = 1;

    endwin();
    return 0;
}

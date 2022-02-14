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

struct server_t main_server;
struct point_t main_campsite = {30, 14};
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int main()
{
    //          //
    // ncurses  //
    //          //
    initscr();
    cbreak();
    noecho();
    curs_set(0);

    sem_t *beast1, *beast2;
    beast1 = sem_open("beast1", O_CREAT, 0777, 0);
    beast2 = sem_open("beast2", O_CREAT, 0777, 0);
    main_server.beast1 = beast1;
    main_server.beast2 = beast2;
    //                        //
    // inicjalizacja kolorow  //
    //                        //
    if (has_colors() == FALSE)
    {
        endwin();
        printf("Your terminal does not support color\n");
        return 1;
    }
    start_color();
    init_pair(BACKG, COLOR_MAGENTA, COLOR_MAGENTA);
    init_pair(BLACK_SPACE, COLOR_BLACK, COLOR_BLUE);
    init_pair(WHITE2115, COLOR_BLACK, COLOR_WHITE);
    init_pair(BUSH, COLOR_YELLOW, COLOR_GREEN);
    init_pair(COIN, COLOR_BLACK, COLOR_YELLOW);
    init_pair(BEAST, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(PLAYER, COLOR_BLACK, COLOR_RED);
    init_pair(CAMP, COLOR_BLACK, COLOR_CYAN);
    //                      //
    //  inicjalizacja nazw  //
    //                      //
    char *names[4];
    names[0] = "Player1";
    names[1] = "Player2";
    names[2] = "Player3";
    names[3] = "Player4";
    //                          //
    // zainicjalizowanie gracza //
    //                          //
    for (int i = 0; i < 4; i++)
    {
        main_server.info[i].coins_brought = 0;
        main_server.info[i].coins_collected = 0;
        main_server.info[i].position.x = 0;
        main_server.info[i].position.y = 0;
        main_server.info[i].player_pid = 0;
        main_server.info[i].player_id = i + 1;
        main_server.info[i].deaths = 0;
        main_server.info[i].type = 0;
        strcpy(main_server.info[i].player_name, names[i]);
    }
    main_campsite.x = 30;
    main_campsite.y = 14;
    main_server.pid = getpid();
    main_server.campsite = main_campsite;
    main_server.rounds = 0;
    main_server.player_count = 0;
    memset(main_server.dead_coins, 0, sizeof(main_server.dead_coins));
    //                              //
    // zapisanie mapy do servera:   //
    //                              //
    l_map("map.txt", main_server.map);
    disp_map(main_server);
    display_legend();
    display_server_stats(main_server);
    //                               //
    // tworzenie pomocniczej pamieci //
    //                               //
    int jd = shm_open("connect_to_serv", O_CREAT | O_RDWR, 0777);

    if (jd == -1)
        return 1;

    ftruncate(jd, sizeof(struct conn));

    struct conn *mem_ptr;
    mem_ptr = (struct conn *) mmap(NULL, sizeof(struct conn), PROT_READ | PROT_WRITE, MAP_SHARED, jd, 0);

    sem_init(&mem_ptr->connection, 1, 0);
    sem_init(&mem_ptr->answer, 1, 0);

    //                                                      //
    //  inicjalizacja pamieci wspoldzielonej client<>server //
    //                                                      //
    int fd, i;
    struct connection_t *ptr[4];

    //                //
    //  PETLA GLOWNA  //
    //                //
    int key;
    struct beast_t *beast = NULL;

    while (1)
    {
        disp_map(main_server);
        display_legend();
        display_server_stats(main_server);

        //           //
        //  PLAYER_1 //
        //           //
        if (main_server.info[0].player_pid == 0)
        {
            int err = sem_trywait(&mem_ptr->connection);
            if (err == 0)
            {
                strcpy(mem_ptr->player_name, names[0]);
                sem_post(&mem_ptr->answer);
                add_player(&main_server, &ptr[0], names[0], 0);
            }
        }

        if (main_server.info[0].player_pid)
        {
            int err = ptr[0]->dc;
            if (err)
                del(&main_server, &ptr[0], names[0], 0);
            else
            {
                if (player_move(&ptr[0], &main_server, 0) == 1)
                    del(&main_server, &ptr[0], names[0], 0);
            }
        }

        if (main_server.info[0].position.y != 0)
        {
            main_server.map[main_server.info[0].position.y][main_server.info[0].position.x] = 'P';
            ptr[0]->rounds = main_server.rounds;
        }
        //--------------------------------------------------------------------------//
        //           //
        //  PLAYER_2 //
        //           //
        if (main_server.info[1].player_pid == 0)
        {
            int err = sem_trywait(&mem_ptr->connection);
            if (err == 0)
            {
                strcpy(mem_ptr->player_name, names[1]);
                sem_post(&mem_ptr->answer);
                add_player(&main_server, &ptr[1], names[1], 1);
            }
        }

        if (main_server.info[1].player_pid)
        {
            int err = ptr[1]->dc;
            if (err)
                del(&main_server, &ptr[1], names[1], 1);
            else
            {
                if (player_move(&ptr[1], &main_server, 1) == 1)
                    del(&main_server, &ptr[1], names[1], 1);
            }
        }

        if (main_server.info[1].position.y != 0)
        {
            main_server.map[main_server.info[1].position.y][main_server.info[1].position.x] = 'P';
            ptr[1]->rounds = main_server.rounds;
        }
        //--------------------------------------------------------------------------//
        //           //
        //  PLAYER_3 //
        //           //
        if (main_server.info[2].player_pid == 0)
        {
            int err = sem_trywait(&mem_ptr->connection);
            if (err == 0)
            {
                strcpy(mem_ptr->player_name, names[2]);
                sem_post(&mem_ptr->answer);
                add_player(&main_server, &ptr[2], names[2], 2);
            }
        }

        if (main_server.info[2].player_pid)
        {
            int err = ptr[2]->dc;
            if (err)
                del(&main_server, &ptr[2], names[2], 2);
            else
            {
                if (player_move(&ptr[2], &main_server, 2) == 1)
                    del(&main_server, &ptr[2], names[2], 2);
            }
        }

        if (main_server.info[2].position.y != 0)
        {
            main_server.map[main_server.info[2].position.y][main_server.info[2].position.x] = 'P';
            ptr[2]->rounds = main_server.rounds;
        }
        //--------------------------------------------------------------------------//
        //           //
        //  PLAYER_4 //
        //           //
        if (main_server.info[3].player_pid == 0)
        {
            int err = sem_trywait(&mem_ptr->connection);
            if (err == 0)
            {
                strcpy(mem_ptr->player_name, names[3]);
                sem_post(&mem_ptr->answer);
                add_player(&main_server, &ptr[3], names[3], 3);
            }
        }

        if (main_server.info[3].player_pid)
        {
            int err = ptr[3]->dc;
            if (err)
                del(&main_server, &ptr[3], names[3], 3);
            else
            {
                if (player_move(&ptr[3], &main_server, 3) == 1)
                    del(&main_server, &ptr[3], names[3], 3);
            }
        }

        if (main_server.info[3].position.y != 0)
        {
            main_server.map[main_server.info[3].position.y][main_server.info[3].position.x] = 'P';
            ptr[3]->rounds = main_server.rounds;
        }
        //--------------------------------------------------------------------------//

        if (beast)
        {
            sem_post(main_server.beast1);
            sem_wait(main_server.beast2);
        }

        timeout(100);
        key = getch();

        if (key == 'T' || key == 't' || key == 'c')
            spawn_tre(key, &main_server);


        if (key == 'q' || key == 'Q') break;


        if (key == 'b' || key == 'B')
        {
            beast = beast_spawn(&main_server);
        }


        main_server.rounds++;
        mem_ptr->player_count = main_server.player_count;
    }

    //           //
    //  closing  //
    //           //
    if (close(jd) != 0)
    {
        printf("Shm error close\n");
        return 1;
    }


    if (shm_unlink("connect_to_serv") != 0)
    {
        printf("Shm error unlink connect to serv\n");
        return 1;
    }

    sem_unlink("beast1");
    sem_unlink("beast2");

    getch();
    endwin();
    return 0;
}

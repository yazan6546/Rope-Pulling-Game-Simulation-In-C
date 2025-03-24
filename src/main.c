#include "common.h"
#include "config.h"
#include "random.h"
#include "player_utils.h"

Config config;

void fork_players(Team team, int num_players);

int main() {

    int id = fork();

    if (id == 0) {
        printf("Child process\n");
        execl("/bin/pwd", "pwd", NULL);
    }
    else {
        wait(NULL);
        printf("Parent process\n");
    }
    load_config("config.txt", &config);

    print_config(&config);

    fork_players(TEAM_A, config.NUM_PLAYERS/2);
    fork_players(TEAM_B, config.NUM_PLAYERS/2);

    wait(NULL);
    while (1) {}
}

void fork_players(Team team, int num_players) {

    Player players[num_players];
    for (int i = 0; i < num_players; i++) {
        const pid_t pid = fork();

        init_random(getpid());

        if (pid == -1) {
            perror("fork");
        }

        else if (pid == 0) {

            char buffer[100];

            generate_random_player(&players[i], &config, team, i);
            serialize_player(&players[i], buffer);

            if (execl("./bin/player", "player", buffer, NULL)) {
                perror("execl");
                exit(1);
            }
        }
        else {
            fflush(stdout);
        }
    }

}



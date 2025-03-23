#include "common.h"
#include "config.h"
#include "random.h"
#include "player.h"

Config config;

int main() {

    int id = fork();

    if (id == 0) {
        printf("Child process\n");
        execl("/bin/pwd", "pwd", NULL);
    } else {
        wait(NULL);
        printf("Parent process\n");
    }
    load_config("../config.txt", &config);

    print_config(&config);

    fork_players(TEAM_A, config.NUM_PLAYERS);
    fork_players(TEAM_B, config.NUM_PLAYERS);

    while (1) {}
}

void fork_players(Team team, int num_players) {
    for (int i = 0; i < num_players; i++) {
        pid_t pid = fork();

        if (pid == -1) {
            perror("fork");
        }

        if (pid == 0) {
            char buffer[100];
            config_to_string(&config, buffer);
            execl("player", "player", buffer, team, NULL);
        }
        else {
        }
    }

}

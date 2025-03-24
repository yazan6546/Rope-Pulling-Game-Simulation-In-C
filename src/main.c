#include "common.h"
#include "config.h"
#include "random.h"
#include "player_utils.h"
#include "file.h"

#define PATH_MAX 4096
Config config;

void fork_players(Team team, int num_players, char *bin_path);

int main(int argc, char *argv[]) {

    char *config_path = NULL;
    handling_file(argc, argv[0], &config_path);
    char *bin_path = binary_dir(config_path);

    printf("Config path: %s\n", argv[0]);

    printf("Config path: %s\n", config_path);
    load_config(config_path, &config);

    print_config(&config);

    fork_players(TEAM_A, config.NUM_PLAYERS/2, bin_path);
    fork_players(TEAM_B, config.NUM_PLAYERS/2, bin_path);

    wait(NULL);
    // while (1) {}

    free(bin_path);
}

void fork_players(Team team, int num_players, char *binary_path) {

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

            // Create full path to player executable
            char player_path[PATH_MAX];
            snprintf(player_path, PATH_MAX, "%s/player", binary_path);

            // When executing:
            if (execl(player_path, "player", buffer, NULL)) {
                perror("execl");
                exit(1);
            }
        }
        else {
            fflush(stdout);
        }
    }

}



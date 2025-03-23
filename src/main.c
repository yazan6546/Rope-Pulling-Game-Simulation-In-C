#include "common.h"
#include "config.h"
#include "random.h"

int main() {

    int id = fork();

    if (id == 0) {
        printf("Child process\n");
        execl("/bin/pwd", "pwd", NULL);
    } else {
        wait(NULL);
        printf("Parent process\n");
    }
    Config config;
    load_config("../config.txt", &config);

    print_config(&config);

    int x = random_float(10, 20);
    printf("Random float: %f\n", x);
}

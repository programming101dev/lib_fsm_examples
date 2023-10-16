#include <p101_fsm/fsm.h>
#include <stdlib.h>

int main(void)
{
    struct p101_error *error;
    struct p101_env   *env;
    struct p101_error *fsm_error;
    struct p101_env   *fsm_env;

    error     = p101_error_create(false);
    env       = p101_env_create(error, true, NULL);
    fsm_error = p101_error_create(false);
    fsm_env   = p101_env_create(error, true, NULL);
    p101_fsm_info_create(env, error, "test-fsm", fsm_env, fsm_error);

    return EXIT_SUCCESS;
}

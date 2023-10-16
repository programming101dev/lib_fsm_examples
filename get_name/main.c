#include <p101_fsm/fsm.h>
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    struct p101_error    *error;
    struct p101_env      *env;
    struct p101_error    *fsm_error;
    struct p101_env      *fsm_env;
    struct p101_fsm_info *fsm;

    error     = p101_error_create(false);
    env       = p101_env_create(error, true, NULL);
    fsm_error = p101_error_create(false);
    fsm_env   = p101_env_create(error, true, NULL);
    fsm       = p101_fsm_info_create(env, error, "test-fsm", fsm_env, fsm_error, NULL);

    if(p101_error_has_error(error))
    {
        fprintf(stderr, "Error creating FSM: %s\n", p101_error_get_message(error));
    }
    else
    {
        const char *name;

        name = p101_fsm_info_get_name(env, fsm);
        printf("FSM name = %s\n", name);
        p101_fsm_info_destroy(env, &fsm);
    }

    return EXIT_SUCCESS;
}

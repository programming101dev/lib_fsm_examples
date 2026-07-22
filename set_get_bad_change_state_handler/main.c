#include <p101_fsm/fsm.h>
#include <stdint.h>
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
    env       = p101_env_create(error, NULL);
    fsm_error = p101_error_create(false);
    fsm_env   = p101_env_create(fsm_error, NULL);
    fsm       = NULL;

    if(p101_error_has_no_error(fsm_error))
    {
        fsm = p101_fsm_info_create(env, error, "test-fsm", fsm_env, fsm_error, NULL);
    }

    if(p101_error_has_error(error))
    {
        fprintf(stderr, "Error creating FSM: %s\n", p101_error_get_message(error));
    }
    else if(p101_error_has_error(fsm_error))
    {
        fprintf(stderr, "Error creating FSM environment: %s\n", p101_error_get_message(fsm_error));
    }
    else
    {
        p101_fsm_info_bad_change_state_handler_func handler;

        handler = p101_fsm_info_get_bad_change_state_handler(fsm);
        printf("Before set: 0x%lx\n", (uintptr_t)handler);

        p101_fsm_info_set_bad_change_state_handler(fsm, p101_fsm_info_default_bad_change_state_handler);
        handler = p101_fsm_info_get_bad_change_state_handler(fsm);
        printf("After set: 0x%lx\n", (uintptr_t)handler);

        p101_fsm_info_set_bad_change_state_handler(fsm, NULL);

        if(p101_error_has_error(fsm_error))
        {
            fprintf(stderr, "Error: %s\n", p101_error_get_message(fsm_error));
        }

        handler = p101_fsm_info_get_bad_change_state_handler(fsm);
        printf("After set (NULL): 0x%lx\n", (uintptr_t)handler);

        p101_fsm_info_destroy(env, &fsm);
    }

    p101_env_destroy(fsm_env);
    p101_error_destroy(fsm_error);
    p101_env_destroy(env);
    p101_error_destroy(error);

    return EXIT_SUCCESS;
}

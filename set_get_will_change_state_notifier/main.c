#include <p101_fsm/fsm.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static void notifier(const struct p101_env *env, struct p101_error *err, const struct p101_fsm_info *info, p101_fsm_state_t from_state_id, p101_fsm_state_t to_state_id);

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
        p101_fsm_info_will_change_state_notifier_func notifier_func;

        notifier_func = p101_fsm_info_get_will_change_state_notifier(fsm);
        printf("Before set: 0x%lx\n", (uintptr_t)notifier_func);

        p101_fsm_info_set_will_change_state_notifier(fsm, notifier);
        notifier_func = p101_fsm_info_get_will_change_state_notifier(fsm);
        printf("After set: 0x%lx\n", (uintptr_t)notifier_func);

        p101_fsm_info_set_will_change_state_notifier(fsm, NULL);
        notifier_func = p101_fsm_info_get_will_change_state_notifier(fsm);
        printf("After set (NULL): 0x%lx\n", (uintptr_t)notifier_func);

        p101_fsm_info_destroy(env, &fsm);
    }

    free(fsm_env);
    free(env);
    p101_error_reset(error);
    free(error);

    return EXIT_SUCCESS;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

static void notifier(const struct p101_env *env, struct p101_error *err, const struct p101_fsm_info *info, p101_fsm_state_t from_state_id, p101_fsm_state_t to_state_id)
{
    P101_TRACE(env);
}

#pragma GCC diagnostic pop

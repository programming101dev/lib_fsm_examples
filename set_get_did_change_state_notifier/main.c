#include <p101_fsm/fsm.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static void notifier(const struct p101_env *env, struct p101_error *err, const struct p101_fsm_info *info, p101_fsm_state_id from_state_id, p101_fsm_state_id to_state_id, p101_fsm_state_id next_state_id);

int main(void)
{
    struct p101_error                      *error;
    struct p101_env                        *env;
    struct p101_error                      *fsm_error;
    struct p101_env                        *fsm_env;
    struct p101_fsm_info                   *fsm;
    static const struct p101_fsm_transition transitions[] = {
        {P101_FSM_INIT, P101_FSM_USER_START, p101_fsm_exit_immediately},
    };

    error     = p101_error_create(false);
    env       = p101_env_create(error, NULL);
    fsm_error = p101_error_create(false);
    fsm_env   = p101_env_create(fsm_error, NULL);
    fsm       = NULL;

    if(p101_error_has_no_error(fsm_error))
    {
        fsm = p101_fsm_info_create(env, error, "test-fsm", fsm_env, fsm_error, transitions, sizeof(transitions) / sizeof(transitions[0]), NULL);
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
        p101_fsm_info_did_change_state_notifier_func notifier_func;

        notifier_func = p101_fsm_info_get_did_change_state_notifier(env, fsm);
        printf("Before set: 0x%lx\n", (uintptr_t)notifier_func);

        p101_fsm_info_set_did_change_state_notifier(env, fsm, notifier);
        notifier_func = p101_fsm_info_get_did_change_state_notifier(env, fsm);
        printf("After set: 0x%lx\n", (uintptr_t)notifier_func);

        p101_fsm_info_set_did_change_state_notifier(env, fsm, NULL);
        notifier_func = p101_fsm_info_get_did_change_state_notifier(env, fsm);
        printf("After set (NULL): 0x%lx\n", (uintptr_t)notifier_func);

        p101_fsm_info_destroy(env, fsm_error, &fsm);
    }

    p101_env_destroy(fsm_env);
    p101_error_destroy(fsm_error);
    p101_env_destroy(env);
    p101_error_destroy(error);

    return EXIT_SUCCESS;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

static void notifier(const struct p101_env *env, struct p101_error *err, const struct p101_fsm_info *info, p101_fsm_state_id from_state_id, p101_fsm_state_id to_state_id, p101_fsm_state_id next_state_id)    // cppcheck-suppress constParameterCallback
{
    P101_TRACE(env);
}

#pragma GCC diagnostic pop

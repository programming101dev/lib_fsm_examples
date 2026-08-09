#include <p101_c/p101_stdio.h>
#include <p101_fsm/fsm.h>
#include <stdlib.h>

static void custom_handler(const struct p101_env *env, struct p101_error *err, const struct p101_fsm_info *info, p101_fsm_state_id from_state_id, p101_fsm_state_id to_state_id, struct p101_fsm_effect_sink *sink, struct p101_fsm_decision *decision);

int main(void)
{
    struct p101_error                      *error;
    struct p101_env                        *env;
    struct p101_error                      *fsm_error;
    struct p101_env                        *fsm_env;
    struct p101_fsm_info                   *fsm;
    int                                     ret_val;
    static const struct p101_fsm_transition transitions[] = {
        {P101_FSM_INIT, P101_FSM_USER_START, p101_fsm_exit_immediately},
    };

    ret_val   = EXIT_FAILURE;
    error     = p101_error_create(false);
    env       = p101_env_create(error, NULL);
    fsm_error = p101_error_create(false);
    fsm_env   = p101_env_create(fsm_error, NULL);
    fsm       = NULL;

    if(p101_error_has_no_error(fsm_error))
    {
        fsm = p101_fsm_info_create(env, error, "test-fsm", fsm_env, fsm_error, transitions, sizeof(transitions) / sizeof(transitions[0]), NULL);
    }

    if(p101_error_has_error(error) || p101_error_has_error(fsm_error))
    {
        goto done;
    }
    else
    {
        p101_fsm_info_bad_change_state_handler_func handler;

        handler = p101_fsm_info_get_bad_change_state_handler(env, fsm);
        p101_printf(env, error, "Default handler installed: %s\n", handler == p101_fsm_info_default_bad_change_state_handler ? "yes" : "no");

        p101_fsm_info_set_bad_change_state_handler(env, fsm, custom_handler);
        handler = p101_fsm_info_get_bad_change_state_handler(env, fsm);
        p101_printf(env, error, "Custom handler installed: %s\n", handler == custom_handler ? "yes" : "no");

        /* NULL restores the safe default rather than leaving no policy. */
        p101_fsm_info_set_bad_change_state_handler(env, fsm, NULL);
        handler = p101_fsm_info_get_bad_change_state_handler(env, fsm);
        p101_printf(env, error, "NULL restored default: %s\n", handler == p101_fsm_info_default_bad_change_state_handler ? "yes" : "no");
    }

    if(p101_error_has_no_error(error) && p101_error_has_no_error(fsm_error))
    {
        ret_val = EXIT_SUCCESS;
    }

done:
    p101_fsm_info_destroy(env, fsm_error, &fsm);
    p101_env_destroy(fsm_env);
    p101_error_destroy(fsm_error);
    p101_env_destroy(env);
    p101_error_destroy(error);

    return ret_val;
}

static void custom_handler(const struct p101_env *env, struct p101_error *err, const struct p101_fsm_info *info, p101_fsm_state_id from_state_id, p101_fsm_state_id to_state_id, struct p101_fsm_effect_sink *sink, struct p101_fsm_decision *decision)
{
    (void)env;
    (void)err;
    (void)info;
    (void)from_state_id;
    (void)to_state_id;
    (void)sink;
    p101_fsm_decide_exit(decision);
}

#include <p101_fsm/fsm.h>
#include <p101_posix/p101_unistd.h>
#include <stdio.h>
#include <stdlib.h>

static void           parse_arguments(const struct p101_env *env, int argc, char *argv[], bool *bad, bool *will, bool *did);
_Noreturn static void usage(const char *program_name, int exit_code, const char *message);
static void           a(const struct p101_env *env, struct p101_error *err, void *arg, struct p101_fsm_effect_sink *sink, struct p101_fsm_decision *decision);
static void           b(const struct p101_env *env, struct p101_error *err, void *arg, struct p101_fsm_effect_sink *sink, struct p101_fsm_decision *decision);
static void           c(const struct p101_env *env, struct p101_error *err, void *arg, struct p101_fsm_effect_sink *sink, struct p101_fsm_decision *decision);
static void           state_error(const struct p101_env *env, struct p101_error *err, void *arg, struct p101_fsm_effect_sink *sink, struct p101_fsm_decision *decision);
static void           will_change_state_notifier_func(const struct p101_env *env, struct p101_error *err, const struct p101_fsm_info *info, p101_fsm_state_t from_state_id, p101_fsm_state_t to_state_id);
static void           did_change_state_notifier_func(const struct p101_env *env, struct p101_error *err, const struct p101_fsm_info *info, p101_fsm_state_t from_state_id, p101_fsm_state_t to_state_id, p101_fsm_state_t next_state_id);
static void           bad_change_state_notifier_func(const struct p101_env *env, struct p101_error *err, const struct p101_fsm_info *info, p101_fsm_state_t from_state_id, p101_fsm_state_t to_state_id);

enum application_states
{
    A = P101_FSM_USER_START,    // 2
    B,
    C,
    ERROR,
};

static const struct p101_fsm_transition transitions[] = {
    {P101_FSM_INIT, A,     a          },
    {A,             B,     b          },
    {B,             C,     c          },
    {C,             A,     a          },
    {C,             ERROR, state_error}
};

#define UNKNOWN_OPTION_MESSAGE_LEN 24    // NOLINT(cppcoreguidelines-macro-to-enum,modernize-macro-to-enum)

int main(int argc, char *argv[])
{
    struct p101_error    *error;
    struct p101_env      *env;
    bool                  bad;
    bool                  will;
    bool                  did;
    struct p101_error    *fsm_error;
    struct p101_env      *fsm_env;
    struct p101_fsm_info *fsm;

    error = p101_error_create(false);
    env   = p101_env_create(error, NULL);
    bad   = false;
    will  = false;
    did   = false;
    parse_arguments(env, argc, argv, &bad, &will, &did);
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
        struct p101_fsm_step_result last_step;
        p101_fsm_run_result         result;
        int                         count;

        if(bad)
        {
            p101_fsm_info_set_bad_change_state_notifier(fsm, bad_change_state_notifier_func);
        }

        if(will)
        {
            p101_fsm_info_set_will_change_state_notifier(fsm, will_change_state_notifier_func);
        }

        if(did)
        {
            p101_fsm_info_set_did_change_state_notifier(fsm, did_change_state_notifier_func);
        }

        count  = 0;
        result = p101_fsm_run(fsm, &count, NULL, &last_step);
        if(result != P101_FSM_RUN_EXITED && p101_error_has_no_error(error) && p101_error_has_no_error(fsm_error))
        {
            P101_ERROR_RAISE_USER(error, "FSM stopped before exit", 1);
        }

        if(p101_error_has_error(fsm_error))
        {
            fprintf(stderr, "FSM error: %s\n", p101_error_get_message(fsm_error));
        }

        if(p101_error_has_error(error))
        {
            fprintf(stderr, "Application error: %s\n", p101_error_get_message(error));
        }

        p101_fsm_info_destroy(env, fsm_error, &fsm);
    }

    p101_env_destroy(fsm_env);
    p101_error_destroy(fsm_error);
    p101_env_destroy(env);
    p101_error_destroy(error);

    return EXIT_SUCCESS;
}

static void parse_arguments(const struct p101_env *env, int argc, char *argv[], bool *bad, bool *will, bool *did)
{
    int opt;

    opterr = 0;

    while((opt = p101_getopt(env, argc, argv, "hbdw")) != -1)
    {
        switch(opt)
        {
            case 'b':
            {
                *bad = true;
                break;
            }
            case 'd':
            {
                *did = true;
                break;
            }
            case 'w':
            {
                *will = true;
                break;
            }
            case 'h':
            {
                usage(argv[0], EXIT_SUCCESS, NULL);
            }
            case '?':
            {
                if(optopt == 'c')
                {
                    usage(argv[0], EXIT_FAILURE, "Option '-c' requires a value.");
                }
                else
                {
                    char message[UNKNOWN_OPTION_MESSAGE_LEN];

                    snprintf(message, sizeof(message), "Unknown option '-%c'.", optopt);
                    usage(argv[0], EXIT_FAILURE, message);
                }
            }
            default:
            {
                usage(argv[0], EXIT_FAILURE, NULL);
            }
        }
    }

    if(optind < argc)
    {
        usage(argv[0], EXIT_FAILURE, "Too many arguments.");
    }
}

_Noreturn static void usage(const char *program_name, int exit_code, const char *message)
{
    if(message)
    {
        fprintf(stderr, "%s\n", message);
    }

    fprintf(stderr, "Usage: %s [-h] [-b] [-d] [-w]\n", program_name);
    fputs("Options:\n", stderr);
    fputs("  -h   Display this help message\n", stderr);
    fputs("  -b   Display 'bad' transitions\n", stderr);
    fputs("  -w   Display 'will' transitions\n", stderr);
    fputs("  -d   Display 'did' transitions\n", stderr);
    exit(exit_code);
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

static void a(const struct p101_env *env, struct p101_error *err, void *arg, struct p101_fsm_effect_sink *sink, struct p101_fsm_decision *decision)    // cppcheck-suppress constParameterCallback
{
    int *count;

    P101_TRACE(env);
    (void)err;
    (void)sink;
    count = ((int *)arg);
    printf("a called with %d\n", *count);
    *count += 1;

    p101_fsm_decide_transition(decision, B);
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

static void b(const struct p101_env *env, struct p101_error *err, void *arg, struct p101_fsm_effect_sink *sink, struct p101_fsm_decision *decision)    // cppcheck-suppress constParameterCallback
{
    int *count;

    P101_TRACE(env);
    (void)err;
    (void)sink;
    count = ((int *)arg);
    printf("b called with %d\n", *count);
    *count += 1;

    p101_fsm_decide_transition(decision, C);
}

#pragma GCC diagnostic pop

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

static void c(const struct p101_env *env, struct p101_error *err, void *arg, struct p101_fsm_effect_sink *sink, struct p101_fsm_decision *decision)    // cppcheck-suppress constParameterCallback
{
    int             *count;
    p101_fsm_state_t next_state;

    P101_TRACE(env);
    (void)err;
    (void)sink;
    count = ((int *)arg);
    printf("c called with %d\n", *count);
    *count += 1;

    if(*count > 3)
    {
        next_state = ERROR;
    }
    else
    {
        next_state = A;
    }

    p101_fsm_decide_transition(decision, next_state);
}

#pragma GCC diagnostic pop

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

static void state_error(const struct p101_env *env, struct p101_error *err, void *arg, struct p101_fsm_effect_sink *sink, struct p101_fsm_decision *decision)    // cppcheck-suppress constParameterCallback
{
    P101_TRACE(env);
    (void)err;
    (void)arg;
    (void)sink;
    p101_fsm_decide_exit(decision);
}

#pragma GCC diagnostic pop

static void will_change_state_notifier_func(const struct p101_env *env, struct p101_error *err, const struct p101_fsm_info *info, p101_fsm_state_t from_state_id, p101_fsm_state_t to_state_id)    // cppcheck-suppress constParameterCallback
{
    P101_TRACE(env);
    printf("%s will change from %d to %d\n", p101_fsm_info_get_name(env, info), from_state_id, to_state_id);
}

static void did_change_state_notifier_func(const struct p101_env *env, struct p101_error *err, const struct p101_fsm_info *info, p101_fsm_state_t from_state_id, p101_fsm_state_t to_state_id,    // cppcheck-suppress constParameterCallback
                                           p101_fsm_state_t next_state_id)
{
    P101_TRACE(env);
    printf("%s did change from %d to %d\n", p101_fsm_info_get_name(env, info), from_state_id, to_state_id);
}

static void bad_change_state_notifier_func(const struct p101_env *env, struct p101_error *err, const struct p101_fsm_info *info, p101_fsm_state_t from_state_id, p101_fsm_state_t to_state_id)    // cppcheck-suppress constParameterCallback
{
    P101_TRACE(env);
    printf("%s can't change from %d to %d\n", p101_fsm_info_get_name(env, info), from_state_id, to_state_id);
}

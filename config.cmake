set(PROJECT_NAME "fsm_run")
set(PROJECT_VERSION "1.0.0")
set(PROJECT_DESCRIPTION "Finite State Machine - run")
set(PROJECT_LANGUAGE "C")

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Common compiler flags
set(STANDARD_FLAGS
        -D_POSIX_C_SOURCE=200809L
        -D_XOPEN_SOURCE=700
        #-D_GNU_SOURCE
        #-D_DARWIN_C_SOURCE
        #-D__BSD_VISIBLE
        -Werror
)

# Define targets
set(EXECUTABLE_TARGETS
        create_destroy
        fsm_run
        get_name
        set_get_bad_change_state_handler
        set_get_bad_change_state_notifier
        set_get_did_change_state_notifier
        set_get_will_change_state_notifier
)

set(LIBRARY_TARGETS "")

set(create_destroy_SOURCES
        create_destroy/main.c
)

set(create_destroy_HEADERS "")

set(create_destroy_LINK_LIBRARIES
        p101_error
        p101_env
        p101_c
        p101_posix
        p101_fsm
        m
)

set(fsm_run_SOURCES
        fsm_run/main.c
)

set(fsm_run_HEADERS "")

set(fsm_run_LINK_LIBRARIES
        p101_error
        p101_env
        p101_c
        p101_posix
        p101_fsm
        m
)

set(get_name_SOURCES
        get_name/main.c
)

set(get_name_HEADERS "")

set(get_name_LINK_LIBRARIES
        p101_error
        p101_env
        p101_c
        p101_posix
        p101_fsm
        m
)

set(set_get_bad_change_state_handler_SOURCES
        create_destroy/main.c
)

set(set_get_bad_change_state_handler_HEADERS "")

set(set_get_bad_change_state_handler_LINK_LIBRARIES
        p101_error
        p101_env
        p101_c
        p101_posix
        p101_fsm
        m
)

set(set_get_bad_change_state_notifier_SOURCES
        set_get_bad_change_state_notifier/main.c
)

set(set_get_bad_change_state_notifier_HEADERS "")

set(set_get_bad_change_state_notifier_LINK_LIBRARIES
        p101_error
        p101_env
        p101_c
        p101_posix
        p101_fsm
        m
)

set(set_get_did_change_state_notifier_SOURCES
        set_get_did_change_state_notifier/main.c
)

set(set_get_did_change_state_notifier_HEADERS "")

set(set_get_did_change_state_notifier_LINK_LIBRARIES
        p101_error
        p101_env
        p101_c
        p101_posix
        p101_fsm
        m
)

set(set_get_will_change_state_notifier_SOURCES
        set_get_will_change_state_notifier/main.c
)

set(set_get_will_change_state_notifier_HEADERS "")

set(set_get_will_change_state_notifier_LINK_LIBRARIES
        p101_error
        p101_env
        p101_c
        p101_posix
        p101_fsm
        m
)

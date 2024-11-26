begin_task()

# Library
add_task_library(exe)

# Playground
add_task_playground(play)

# Tests

# Thread / WaitGroup
add_task_test(thread_wait_group_unit_tests tests/thread/wait_group/unit.cpp)
add_task_test(thread_wait_group_random_tests tests/thread/wait_group/random.cpp)
add_task_test(thread_wait_group_futex_tests tests/thread/wait_group/futex.cpp)
add_task_test(thread_wait_group_model_tests tests/thread/wait_group/model.cpp)
add_task_test(thread_wait_group_storage_tests tests/thread/wait_group/storage.cpp)

# Sched / ThreadPool
add_task_test(sched_thread_pool_unit_tests tests/sched/thread_pool/unit.cpp)
add_task_test(sched_thread_pool_random_tests tests/sched/thread_pool/random.cpp)
add_task_test(sched_thread_pool_stress_tests tests/sched/thread_pool/stress.cpp)

# Sched / RunLoop
add_task_test(sched_run_loop_unit_tests tests/sched/run_loop/unit.cpp)
add_task_test(sched_run_loop_sane_tests tests/sched/run_loop/sane.cpp)

# Fibers

# Fiber / Sched
add_task_test(fiber_sched_thread_pool_unit_tests tests/fiber/sched/thread_pool/unit.cpp)
add_task_test(fiber_sched_run_loop_unit_tests tests/fiber/sched/run_loop/unit.cpp)
add_task_test(fiber_sched_run_loop_sync_tests tests/fiber/sched/run_loop/sync.cpp)

# Fiber / Sync

# Fiber / Sync / Channel

add_task_test(fiber_sync_buffered_channel_unit_tests tests/fiber/sync/channel/buffered/unit.cpp)
add_task_test(fiber_sync_buffered_channel_random_tests tests/fiber/sync/channel/buffered/random.cpp)
add_task_test(fiber_sync_buffered_channel_stress_tests tests/fiber/sync/channel/buffered/stress.cpp)

end_task()

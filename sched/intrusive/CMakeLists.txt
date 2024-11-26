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
add_task_test(sched_thread_pool_intrusive_tests tests/sched/thread_pool/intrusive.cpp)
add_task_test(sched_thread_pool_random_tests tests/sched/thread_pool/random.cpp)
add_task_test(sched_thread_pool_stress_tests tests/sched/thread_pool/stress.cpp)
add_task_test(sched_thread_pool_alloc_tests tests/sched/thread_pool/alloc.cpp)

# Sched / RunLoop
add_task_test(sched_run_loop_unit_tests tests/sched/run_loop/unit.cpp)
add_task_test(sched_run_loop_intrusive_tests tests/sched/run_loop/intrusive.cpp)
add_task_test(sched_run_loop_sane_tests tests/sched/run_loop/sane.cpp)
add_task_test(sched_run_loop_alloc_tests tests/sched/run_loop/alloc.cpp)

# Sched / Submit
add_task_test(sched_submit_alloc_tests tests/sched/submit/alloc.cpp)

# Fibers
add_task_test(fiber_sched_thread_pool_unit_tests tests/fiber/sched/thread_pool/unit.cpp)
add_task_test(fiber_sched_run_loop_unit_tests tests/fiber/sched/run_loop/unit.cpp)
add_task_test(fiber_sched_run_loop_alloc_tests tests/fiber/sched/run_loop/alloc.cpp)
add_task_test(fiber_sched_run_loop_sync_tests tests/fiber/sched/run_loop/sync.cpp)

end_task()

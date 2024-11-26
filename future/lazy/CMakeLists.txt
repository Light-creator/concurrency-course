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

add_task_test(sched_thread_pool_load_balancing_random_tests tests/sched/thread_pool/load_balancing/random.cpp)
add_task_test(sched_thread_pool_load_balancing_random2_tests tests/sched/thread_pool/load_balancing/random2.cpp)

# Sched / RunLoop
add_task_test(sched_run_loop_unit_tests tests/sched/run_loop/unit.cpp)
add_task_test(sched_run_loop_intrusive_tests tests/sched/run_loop/intrusive.cpp)
add_task_test(sched_run_loop_sane_tests tests/sched/run_loop/sane.cpp)
add_task_test(sched_run_loop_alloc_tests tests/sched/run_loop/alloc.cpp)

# Future
add_task_test(future_unit_tests tests/future/unit.cpp)
add_task_test(future_lazy_unit_tests tests/future/lazy_unit.cpp)

end_task()

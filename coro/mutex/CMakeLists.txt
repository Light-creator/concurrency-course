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

# Coroutines

# Coro / Sched
add_task_test(coro_sched_run_loop_unit_tests tests/coro/sched/run_loop/unit.cpp)
add_task_test(coro_sched_thread_pool_unit_tests tests/coro/sched/thread_pool/unit.cpp)

# Coro / Sync / Event
add_task_test(coro_sync_event_unit_tests tests/coro/sync/event/unit.cpp)
add_task_test(coro_sync_event_random_tests tests/coro/sync/event/random.cpp)
add_task_test(coro_sync_event_stress_tests tests/coro/sync/event/stress.cpp)

add_task_test(coro_sync_event_storage_random_tests tests/coro/sync/event/storage/random.cpp)
add_task_test(coro_sync_event_storage_stress_tests tests/coro/sync/event/storage/stress.cpp)

# Coro / Sync / Mutex / ScopedLock
add_task_test(coro_sync_mutex_scoped_lock_unit_tests tests/coro/sync/mutex/scoped_lock/unit.cpp)
add_task_test(coro_sync_mutex_scoped_lock_random_tests tests/coro/sync/mutex/scoped_lock/random.cpp)
add_task_test(coro_sync_mutex_scoped_lock_stress_tests tests/coro/sync/mutex/scoped_lock/stress.cpp)

# Coro / Sync / Mutex / RunMutex
add_task_test(coro_sync_mutex_run_unit_tests tests/coro/sync/mutex/run/unit.cpp)
add_task_test(coro_sync_mutex_run_random_tests tests/coro/sync/mutex/run/random.cpp)
add_task_test(coro_sync_mutex_run_stress_tests tests/coro/sync/mutex/run/stress.cpp)

# Coro / Sync / WaitGroup
add_task_test(coro_sync_wait_group_unit_tests tests/coro/sync/wait_group/unit.cpp)
add_task_test(coro_sync_wait_group_random_tests tests/coro/sync/wait_group/random.cpp)
add_task_test(coro_sync_wait_group_stress_tests tests/coro/sync/wait_group/stress.cpp)

add_task_test(coro_sync_wait_group_storage_random_tests tests/coro/sync/wait_group/storage/random.cpp)
add_task_test(coro_sync_wait_group_storage_stress_tests tests/coro/sync/wait_group/storage/stress.cpp)

end_task()

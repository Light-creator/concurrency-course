begin_task()

add_task_library(memory)

set_task_sources(
        lock_free_stack.hpp
        lock_free_queue.hpp)

add_task_playground(play)

# Tests

# Stack
add_task_test(stack_unit_tests tests/stack/unit.cpp)
add_task_test(stack_random_tests tests/stack/random.cpp)
add_task_test(stack_stress_tests tests/stack/stress.cpp)

# Queue
add_task_test(queue_unit_tests tests/queue/unit.cpp)
add_task_test(queue_random_tests tests/queue/random.cpp)
add_task_test(queue_stress_lf_tests tests/queue/stress/lf.cpp)
add_task_test(queue_stress_lf_mm_tests tests/queue/stress/lf_mm.cpp)

end_task()

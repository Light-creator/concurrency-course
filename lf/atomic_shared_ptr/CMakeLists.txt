begin_task()

add_task_library(memory)

set_task_sources(lock_free_stack.hpp)

add_task_playground(play)

# Tests

# Shared ptr
add_task_test(shared_ptr_unit_tests tests/shared_ptr/unit.cpp)
add_task_test(shared_ptr_random_tests tests/shared_ptr/random.cpp)
add_task_test(shared_ptr_stress_tests tests/shared_ptr/stress.cpp)

# Stack
add_task_test(stack_unit_tests tests/stack/unit.cpp)
add_task_test(stack_random_tests tests/stack/random.cpp)
add_task_test(stack_model_tests tests/stack/model.cpp)
add_task_test(stack_stress_tests tests/stack/stress.cpp)

end_task()

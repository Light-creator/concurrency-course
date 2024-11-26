begin_task()

set_task_sources(
        stamped_ptr.hpp
        lock_free_stack.hpp)

add_task_playground(play)

# Tests

# Stamped ptr
add_task_test(stamped_ptr_unit_tests tests/stamped_ptr/unit.cpp)

# Stack
add_task_test(stack_unit_tests tests/stack/unit.cpp)
add_task_test(stack_random_tests tests/stack/random.cpp)
add_task_test(stack_model_tests tests/stack/model.cpp)
add_task_test(stack_stress_tests tests/stack/stress.cpp)

end_task()

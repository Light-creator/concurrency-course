begin_task()

add_task_library(exe)

add_task_playground(play)

# Tests
add_task_test(unit_tests tests/unit.cpp)
add_task_test(stress_tests tests/stress.cpp)
add_task_test(model_tests tests/model.cpp)
add_task_test(lf_tests tests/lf.cpp)
add_task_test(stat_tests tests/stat.cpp)

end_task()

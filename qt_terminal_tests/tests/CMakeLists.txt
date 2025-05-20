add_executable(my_tests
    test_terminaltextedit.cpp
)

target_link_libraries(my_tests
    terminal_widget
    GTest::gtest_main
    Qt5::Test
)

include(GoogleTest)
gtest_discover_tests(my_tests)

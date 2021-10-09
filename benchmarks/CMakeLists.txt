SET(FILES
    benchmark.cc
    # test_utils.cc
    # fake_table.cc
)

ADD_LIBRARY(libbenchmark STATIC ${FILES})

ADD_DEPENDENCIES(libbenchmark GenError)
TARGET_LINK_LIBRARIES(libbenchmark server_unittest_library)

MYSQL_ADD_EXECUTABLE(benchmark main.cc)
TARGET_LINK_LIBRARIES(benchmark libbenchmark)
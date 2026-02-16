project(test-mem_iterator)

add_executable(${PROJECT_NAME} ${PROJECT_NAME}.cpp)

# target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/../include)
target_link_libraries(${PROJECT_NAME} PRIVATE doctest)

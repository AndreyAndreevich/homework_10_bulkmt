cmake_minimum_required(VERSION 3.2)

if($ENV{TRAVIS_BUILD_NUMBER})
        project(bulkmt VERSION 0.0.$ENV{TRAVIS_BUILD_NUMBER})
else()
        project(bulkmt VERSION 0.0.1)  
endif()

find_package(Boost COMPONENTS unit_test_framework REQUIRED QUIET)
find_package(Threads REQUIRED QUIET)

file(GLOB SOURCE ${CMAKE_SOURCE_DIR}/src/*.cpp)
add_executable(${PROJECT_NAME} ${SOURCE} main.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_SOURCE_DIR}/src
        )

target_link_libraries(${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT})

set(TEST_NAME bulkmt_test)
file(GLOB TESTS ${CMAKE_SOURCE_DIR}/tests/*.cpp)
add_executable(${TEST_NAME} ${SOURCE} ${TESTS})
set_target_properties(${PROJECT_NAME} ${TEST_NAME} PROPERTIES
        CXX_STANDARD 14
        CXX_STANDARD_REQUIRED ON   
        COMPILE_OPTIONS -Wpedantic -Wall -Wextra
        )
#set(CMAKE_CXX_FLAGS "-fsanitize=thread")
set_target_properties(${TEST_NAME} PROPERTIES
        COMPILE_DEFINITIONS BOOST_TEST_DYN_LINK
        INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIR} 
        )

target_link_libraries(${TEST_NAME} ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

target_include_directories(${TEST_NAME} PRIVATE
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/tests
        )

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)

set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})

set(CPACK_PACKAGE_CONTACT l.a.r.p@yandex.ru)

include(CPack)

enable_testing()

add_test(${TEST_NAME} ${TEST_NAME})

option(ALL_TESTS "Запукает повторяющиеся тесты" OFF)
message(STATUS "ALL_TESTS: ${ALL_TESTS}")
if (${ALL_TESTS}) 
        target_compile_definitions(${TEST_NAME} PRIVATE
                -DALL_TESTS
                -DN_DELETED=1000
                -DN_REPEAT=10000
                )
endif()

option(OPTIONAL_TESTS "Запуск опциональных тестов с указанием количества потоков" OFF)
set(OPTIONAL optional)
message(STATUS "OPTIONAL_TESTS: ${OPTIONAL_TESTS}")
if(${OPTIONAL_TESTS})
        add_executable(${OPTIONAL} ${SOURCE} ${CMAKE_SOURCE_DIR}/optional/optional_test.cpp)
        set_target_properties(${OPTIONAL} PROPERTIES
                CXX_STANDARD 14
                CXX_STANDARD_REQUIRED ON   
                COMPILE_OPTIONS -Wpedantic -Wall -Wextra
                )            
        target_link_libraries(${OPTIONAL} ${CMAKE_THREAD_LIBS_INIT})                
        target_include_directories(${OPTIONAL} PRIVATE
                ${CMAKE_SOURCE_DIR}/src
                )       
endif()
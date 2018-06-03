cmake_minimum_required(VERSION 3.2)

if($ENV{TRAVIS_BUILD_NUMBER})
        project(bulkmt VERSION 0.0.$ENV{TRAVIS_BUILD_NUMBER})
else()
        project(bulkmt VERSION 0.0.1)  
endif()

find_package(Boost COMPONENTS unit_test_framework REQUIRED)
find_package(Threads REQUIRED)

include_directories(
        ${CMAKE_SOURCE_DIR}/src
        )

file(GLOB SOURCE ${CMAKE_SOURCE_DIR}/src/*.cpp)
add_executable(${PROJECT_NAME} ${SOURCE} main.cpp)
target_link_libraries(${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT})

set(TEST_NAME bulkmt_test)
file(GLOB TESTS ${CMAKE_SOURCE_DIR}/tests/*.cpp)
add_executable(${TEST_NAME} ${SOURCE} ${TESTS})

set_target_properties(${PROJECT_NAME} ${TEST_NAME} PROPERTIES
        CXX_STANDARD 14
        CXX_STANDARD_REQUIRED ON
        COMPILE_OPTIONS -Wpedantic -Wall -Wextra
        )

set_target_properties(${TEST_NAME} PROPERTIES
        COMPILE_DEFINITIONS BOOST_TEST_DYN_LINK 
        INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIR} 
        )

target_link_libraries(${TEST_NAME} ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)

set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})

set(CPACK_PACKAGE_CONTACT l.a.r.p@yandex.ru)

include(CPack)

enable_testing()

add_test(${TEST_NAME} ${TEST_NAME})
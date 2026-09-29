cmake_minimum_required(VERSION 3.23)
project(f16_flight_health_testbench VERSION 0.1.0 LANGUAGES CXX)

add_executable(f16-baseline src/main.cpp)
target_compile_features(f16-baseline PRIVATE cxx_std_17)
set_target_properties(f16-baseline PROPERTIES CXX_EXTENSIONS OFF)

if(MSVC)
  target_compile_options(f16-baseline PRIVATE /W4 /permissive-)
else()
  target_compile_options(f16-baseline PRIVATE -Wall -Wextra -Wpedantic)
endif()

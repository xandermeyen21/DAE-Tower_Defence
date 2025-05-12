cmake_minimum_required(VERSION 3.16)

project(DAE-Tower_Defence)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Find SDL2 and its extensions using environment variables (set by GitHub Action)
find_path(SDL2_INCLUDE_DIR SDL.h
  HINTS "$ENV{SDL2_ROOT}/include"
)

find_library(SDL2_LIBRARY
  NAMES SDL2
  HINTS "$ENV{SDL2_ROOT}/lib/x64"
)

find_library(SDL2_IMAGE_LIBRARY
  NAMES SDL2_image
  HINTS "$ENV{SDL2_IMAGE_ROOT}/lib/x64"
)

find_library(SDL2_MIXER_LIBRARY
  NAMES SDL2_mixer
  HINTS "$ENV{SDL2_MIXER_ROOT}/lib/x64"
)

find_library(SDL2_TTF_LIBRARY
  NAMES SDL2_ttf
  HINTS "$ENV{SDL2_TTF_ROOT}/lib/x64"
)

# Add executable
add_executable(${PROJECT_NAME}
  src/main.cpp
  # Add other source files here
)

# Link SDL2 libraries
target_include_directories(${PROJECT_NAME} PRIVATE
  ${SDL2_INCLUDE_DIR}
  include  # Your project's include dir
)

target_link_libraries(${PROJECT_NAME}
  ${SDL2_LIBRARY}
  ${SDL2_IMAGE_LIBRARY}
  ${SDL2_MIXER_LIBRARY}
  ${SDL2_TTF_LIBRARY}
)

# Copy SDL2 DLLs to executable directory after build
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "$ENV{SDL2_ROOT}/lib/x64/SDL2.dll"
    $<TARGET_FILE_DIR:${PROJECT_NAME}>
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "$ENV{SDL2_IMAGE_ROOT}/lib/x64/SDL2_image.dll"
    $<TARGET_FILE_DIR:${PROJECT_NAME}>
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "$ENV{SDL2_MIXER_ROOT}/lib/x64/SDL2_mixer.dll"
    $<TARGET_FILE_DIR:${PROJECT_NAME}>
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "$ENV{SDL2_TTF_ROOT}/lib/x64/SDL2_ttf.dll"
    $<TARGET_FILE_DIR:${PROJECT_NAME}>
)

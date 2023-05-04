add_executable(nbtshow
    nbtshow.cpp
)

target_link_libraries(nbtshow nbtview)
target_include_directories(nbtshow PRIVATE include)


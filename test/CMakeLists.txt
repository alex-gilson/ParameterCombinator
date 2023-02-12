
add_executable(ParameterCombinatorTs
                    ParameterCombinatorTs.cpp
               )

if(MSVC)
  target_compile_options(ParameterCombinatorTs PRIVATE /W4 /WX)
else()
  target_compile_options(ParameterCombinatorTs PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

target_include_directories(ParameterCombinatorTs
                              PRIVATE
                                   .
                          )

target_link_libraries(ParameterCombinatorTs
                         PRIVATE
                              ParameterCombinator
                    )

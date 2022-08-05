
add_executable(ParameterCombinatorTs
                    ParameterCombinatorTs.cpp
               )

target_compile_options(ParameterCombinatorTs PUBLIC -Wall -pedantic -g)

target_include_directories(ParameterCombinatorTs
                              PRIVATE
                                   .
                          )

target_link_libraries(ParameterCombinatorTs
                         PRIVATE
                              ParameterCombinator
                    )

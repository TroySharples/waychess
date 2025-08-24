add_executable(waychess ${CMAKE_CURRENT_SOURCE_DIR}/waychess.cpp)
target_link_libraries(waychess PRIVATE lib-waychess)
install(TARGETS waychess DESTINATION bin)

add_executable(whychess ${CMAKE_CURRENT_SOURCE_DIR}/whychess.cpp)
target_link_libraries(whychess PRIVATE lib-waychess)
install(TARGETS whychess DESTINATION bin)

add_executable(waychess-perft ${CMAKE_CURRENT_SOURCE_DIR}/perft.cpp)
target_link_libraries(waychess-perft PRIVATE lib-waychess)
install(TARGETS waychess-perft DESTINATION bin)

add_executable(waychess-print ${CMAKE_CURRENT_SOURCE_DIR}/print.cpp)
target_link_libraries(waychess-print PRIVATE lib-waychess)
install(TARGETS waychess-print DESTINATION bin)

add_executable(waychess-evaluate ${CMAKE_CURRENT_SOURCE_DIR}/evaluate.cpp)
target_link_libraries(waychess-evaluate PRIVATE lib-waychess)
install(TARGETS waychess-evaluate DESTINATION bin)

add_executable(waychess-puzzler ${CMAKE_CURRENT_SOURCE_DIR}/puzzler.cpp)
target_link_libraries(waychess-puzzler PRIVATE lib-waychess)
install(TARGETS waychess-puzzler DESTINATION bin)


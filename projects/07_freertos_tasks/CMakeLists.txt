enable_training_freertos()
add_training_can_peer()
add_freertos_firmware(project07 src/main.c)
target_link_libraries(project07 PRIVATE training_common)

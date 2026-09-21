enable_training_freertos()
add_training_can_peer()
add_freertos_firmware(project09 src/main.c)
target_link_libraries(project09 PRIVATE training_common)

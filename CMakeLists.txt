cmake_minimum_required (VERSION 2.6)
project (MQTT_embed_server)
add_executable(${PROJECT_NAME} main.c)
include_directories(/usr/include/Eclipse-Paho-MQTT-C-1.3.9-Linux/include)
target_link_libraries(${PROJECT_NAME} paho-mqtt3c)

include_directories(/usr/include/mariadb /usr/include/mariadb/mysql /usr/local/include)
target_link_libraries(MQTT_embed_server PJ_RPI)
target_link_libraries(MQTT_embed_server mariadb)
cmake_minimum_required(VERSION 3.0)

include_directories(
   .
)

add_executable(testclient examples/example.c src/iec60870-104_client.c)


target_include_directories(testclient PRIVATE src include)



target_link_libraries(testclient lib60870)









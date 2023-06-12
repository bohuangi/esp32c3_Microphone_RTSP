set(srcs "protobuf-c-rpc/protobuf-c-rpc/protobuf-c-rpc-data-buffer.c"
    "protobuf-c-rpc/protobuf-c-rpc/protobuf-c-rpc-server.c"
    "protobuf-c-rpc/protobuf-c-rpc/protobuf-c-rpc-dispatch.c"
    "protobuf-c-rpc/protobuf-c-rpc/protobuf-c-rpc-client.c"
    "proto_src/esp_audiotest.pb-c.c")

set(includes "protobuf-c-rpc/protobuf-c-rpc" "proto_src")

set(requires  "protobuf-c" ) 
idf_component_register (SRCS ${srcs} 
    INCLUDE_DIRS ${includes}
    REQUIRES ${requires}
    )


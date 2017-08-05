# ProfaneDB

ProfaneDB is a database for Protocol Buffer objects.  
The key used for retrieval of objects is defined within `.proto` files, this is then used to store a single copy of each object, and allow references between objects without data duplication.


## Configuration and usage

For a more detailed explanation, go to the [project's website](http://profanedb.gitlab.io/docs/quick-start/).

### Schema definition

Schema definition comes with every message definition, using [Protocol Buffer field options](https://developers.google.com/protocol-buffers/docs/proto#options):

*At the moment only one key per message can be set*

```protobuf
import "profanedb/protobuf/options.proto";

message Test {
    int32 field_one_int = 1 [ (profanedb.protobuf.options).key = true ];
    string field_two_str = 2;
    bool field_three_bool = 3;
    bytes field_four_bytes = 4;
    
    Nested field_five_nested = 5;
}

message Nested {
    string nested_field_one_str = 1 [ (profanedb.protobuf.options).key = true ];
    int64 nested_field_two_int = 2;
    double nested_field_three_double = 3;
}
```

ProfaneDB can either be used as a gRPC server (service definition in profanedb/protobuf/db.proto) or as a library (interface in profanedb/db.hpp).

### CLI parameters

```
profanedb_server --rocksdb_path /tmp/profanedb -I /usr/include -S /your/schema/dir
```

The most important parameters are the include path (`-I`) and schema path (`-S`).

- The **include path** is used to retrieve `google/protobuf/*.proto`,
  `profanedb/protobuf/*.proto` and any other dependencies.
- The **schema path** has your definitions with the *key* option set.


## Build

ProfaneDB uses [CMake](https://cmake.org/), and depends on Protobuf, gRPC, RocksDB, Boost.

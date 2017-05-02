# ProfaneDB

ProfaneDB is a database for Protocol Buffer objects.  
The key used for retrieval of objects is defined within `.proto` files, this is then used to store a single copy of each object, and allow references between objects without data duplication.


## Configuration and usage

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

ProfaneDB can either be used as a gRPC server (service definition in prodisdb/protobuf/db.proto) or as a library (interface in prodisdb/storage/db.h).


## Storage

Data is simply serialized using Protobuf own encoding, and stored in a RocksDB database.  
However, since Protobuf allows nesting messages, whenever a nested message has a primary key set, the parent message is modified to use a reference to the key, and the child object is stored on its own.

Assuming at least Test.field_one_int and Nested.nested_field_one_str are set in the previous sample definition, an encoded message could look like this:

*(in protobuf text format)*

```protobuf
Test {
    field_one_int: 123
    field_two_str: "a string"
    
    field_five_nested: Nested {
        nested_field_one_str: "unique key"
    }
}
```

Messages are now split. Nested comes with its own key `nested_field_one_str`,
which identifies it as a unique entity.  
`Test.field_five_nested` is now made into a `string` field,
to hold a reference to that message unique key: `Nested$unique key`.

```protobuf
Nested {
    nested_field_one_str: "unique key"
}

Test {
    field_one_int: 123
    field_two_str: "a string"
    
    field_five_nested: "Nested.nested_field_one_str$unique key"
}
```

The data is then stored in RocksDB
with `Test.field_one_int$123` and `Nested.nested_field_one_str$unique key`
as keys.
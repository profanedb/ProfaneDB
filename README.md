# ProdisDB

ProdisDB is a database for Protocol Buffer objects.  
It is a very simple frontend for Redis (which effectively stores the Protobuf serialised objects) for quick prototyping.  
The key used for retrieval of objects is defined within `.proto` files, this is then used to store a single copy of each object, and allow references between objects without data duplication.

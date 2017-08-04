import grpc
from google.protobuf import any_pb2

from profanedb.protobuf import db_pb2, db_pb2_grpc, storage_pb2

import test_pb2

def run():
    channel = grpc.insecure_channel('localhost:50051')
    stub = db_pb2_grpc.DbStub(channel)

    to_serialize = test_pb2.KeyInt(
            int_key = 12312
    )

    serializable = any_pb2.Any()
    serializable.Pack(to_serialize)

    stub.Put(db_pb2.PutReq(
        serializable = serializable
    ))

    key = storage_pb2.Key(
            message_type = "schema.KeyInt",
            field = "int_key",
            value = b"12312"
    )

    retrieved = stub.Get(db_pb2.GetReq(
        key = key
    ))

    print(retrieved)


if __name__ == '__main__':
    run()

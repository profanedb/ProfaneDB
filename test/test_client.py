import grpc

from prodisdb.protobuf import db_pb2, db_pb2_grpc

import test_pb2

from google.protobuf import any_pb2

def run():
    channel = grpc.insecure_channel('localhost:50051')
    stub = db_pb2_grpc.DbStub(channel)

    to_serialize = test_pb2.Test(
        field_one_int = 123,
        field_two_str = "my_string",
        field_three_bool = True
    )

    serializable = any_pb2.Any()
    serializable.Pack(to_serialize)

    print (serializable.TypeName())

    stub.Put(db_pb2.PutReq(
        serializable = serializable
    ))

if __name__ == '__main__':
    run()

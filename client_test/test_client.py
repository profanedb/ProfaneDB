import grpc

from profanedb.protobuf import db_pb2, db_pb2_grpc

import test_pb2, nested_pb2

from google.protobuf import any_pb2

def run():
    channel = grpc.insecure_channel('localhost:50051')
    stub = db_pb2_grpc.DbStub(channel)

    to_serialize = test_pb2.Test(
        field_one_int = 123,
        field_two_str = "my_string",
        field_three_bool = True,
        field_four_bytes = b'bytes',

        field_five_nested =
            test_pb2.Nested(
                nested_field_one_str = "nested string",
                nested_field_two_int = 1902923490,
                nested_field_three_double = 1728.543344
            ),

        field_six_externalnested =
            nested_pb2.ExternalNested(
                field_one_double = 123.456
            ),

        field_seven_repeated_keyable =
            test_pb2.Nested.KeyableNestedInNested(
                nested_in_nested_field_one_repeated_str = ["repeated", "string", "key"]
            )
    )

    serializable = any_pb2.Any()
    serializable.Pack(to_serialize)

    stub.Put(db_pb2.PutReq(
        serializable = serializable
    ))

if __name__ == '__main__':
    run()

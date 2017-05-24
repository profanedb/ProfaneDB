/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2017  <copyright holder> <email>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "schema.h"

profanedb::boot::protobuf::Schema::Schema()
{
}

profanedb::boot::protobuf::Schema::~Schema()
{
}

bool profanedb::boot::protobuf::Schema::IsKeyable(const google::protobuf::Message & message) const
{
    const Descriptor * descriptor = message.GetDescriptor();
    
    for (int i = 0; i < descriptor->field_count(); i++) {
        if (descriptor->field(i)->options().GetExtension(profanedb::protobuf::options).key())
            return true;
    }
    return false;
}

profanedb::protobuf::Key profanedb::boot::protobuf::Schema::GetKey(const google::protobuf::Message & message) const
{
    const Descriptor * descriptor = message.GetDescriptor();
    
    for (int i = 0; i < descriptor->field_count(); i++) {
        if (descriptor->field(i)->options().GetExtension(profanedb::protobuf::options).key()) {
            return FieldToKey(message, descriptor->field(i));
        }
    }
    
    throw std::runtime_error(message.GetTypeName() + " is not keyable");
}

std::vector<const google::protobuf::Message *> profanedb::boot::protobuf::Schema::GetNestedMessages(
    const google::protobuf::Message & message) const
{
    std::vector<const google::protobuf::Message *> nested;
    
    const Reflection * reflection = message.GetReflection();
    const Descriptor * descriptor = message.GetDescriptor();
    
    for (int i = 0; i < descriptor->field_count(); i++) {
        if (descriptor->field(i)->type() == FieldDescriptor::TYPE_MESSAGE) {
            nested.push_back(&reflection->GetMessage(message,descriptor->field(i)));
        }
    }
    
    return nested;
}

profanedb::protobuf::Key profanedb::boot::protobuf::Schema::FieldToKey(
    const Message & message,
    const google::protobuf::FieldDescriptor * fd)
{
    profanedb::protobuf::Key key;
    *key.mutable_message_type() = message.GetTypeName();
    *key.mutable_field() = fd->name();
    
    const google::protobuf::Reflection * reflection = message.GetReflection();
    
    if (fd->is_repeated()) {
        for (int y = 0; y < reflection->FieldSize(message, fd);  y++) {
            switch (fd->cpp_type()) {
                #define HANDLE_TYPE(CPPTYPE,  METHOD)                                                              \
                case google::protobuf::FieldDescriptor::CPPTYPE_##CPPTYPE:                                         \
                    *key.mutable_value() += "$" + std::to_string(reflection->GetRepeated##METHOD(message, fd, y)); \
                    break;
                    
                    HANDLE_TYPE(INT32 , Int32 );
                    HANDLE_TYPE(INT64 , Int64 );
                    HANDLE_TYPE(UINT32, UInt32);
                    HANDLE_TYPE(UINT64, UInt64);
                    HANDLE_TYPE(DOUBLE, Double);
                    HANDLE_TYPE(FLOAT , Float );
                    HANDLE_TYPE(BOOL  , Bool  );
                    #undef HANDLE_TYPE
                    
                case google::protobuf::FieldDescriptor::CPPTYPE_ENUM:
                    *key.mutable_value() += "$" + std::to_string(reflection->GetRepeatedEnum(message, fd, y)->index());
                    break;
                case google::protobuf::FieldDescriptor::CPPTYPE_STRING:
                    *key.mutable_value() += "$" + reflection->GetRepeatedString(message, fd, y);
                    break;
                case google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE:
                    *key.mutable_value() += "$" + reflection->GetRepeatedMessage(message, fd, y).SerializeAsString();
                    break;
            }
        }
    } else {
        switch (fd->cpp_type()) {
            #define HANDLE_TYPE(CPPTYPE, METHOD)                                             \
            case google::protobuf::FieldDescriptor::CPPTYPE_##CPPTYPE:               	     \
                *key.mutable_value() = std::to_string(reflection->Get##METHOD(message, fd)); \
                break;
                
                HANDLE_TYPE(INT32 , Int32 );
                HANDLE_TYPE(INT64 , Int64 );
                HANDLE_TYPE(UINT32, UInt32);
                HANDLE_TYPE(UINT64, UInt64);
                HANDLE_TYPE(DOUBLE, Double);
                HANDLE_TYPE(FLOAT , Float );
                HANDLE_TYPE(BOOL  , Bool  );
                #undef HANDLE_TYPE
                
            case google::protobuf::FieldDescriptor::CPPTYPE_ENUM:
                *key.mutable_value() = std::to_string(reflection->GetEnum(message, fd)->index());
                break;
            case google::protobuf::FieldDescriptor::CPPTYPE_STRING:
                *key.mutable_value() = reflection->GetString(message, fd);
                break;
            case google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE:
                *key.mutable_value() = reflection->GetMessage(message, fd).SerializeAsString();
                break;
        }
    }
    
    return key;
}

/*
 * ProfaneDB - A Protocol Buffers database.
 * Copyright (C) 2017  "Giorgio Azzinnaro" <giorgio.azzinnaro@gmail.com>
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

#include "util.h"

using google::protobuf::Message;
using google::protobuf::FieldDescriptor;
using google::protobuf::Reflection;

namespace profanedb {
namespace format {
namespace protobuf {
    
void CopyMessage(const Message & from, Message * to)
{
    std::vector< const FieldDescriptor * > setFields;
    from.GetReflection()->ListFields(from, &setFields);

    for (auto fromField : setFields)
        CopyField(fromField, from, to);
}

void CopyField(const FieldDescriptor * fromField, const Message & from, Message * to)
{
    const Reflection * fromReflection = from.GetReflection();
    const Reflection * toReflection = to->GetReflection();
    const FieldDescriptor * toField = to->GetDescriptor()->field(fromField->index());
    
    if (fromField->is_repeated()) {
        for (int y = 0; y < fromReflection->FieldSize(from, fromField); y++) {
            switch (fromField->cpp_type()) {
                #define HANDLE_TYPE(CPPTYPE, METHOD)                          \
                case FieldDescriptor::CPPTYPE_##CPPTYPE:                      \
                    toReflection->Add##METHOD(to, toField,                    \
                    fromReflection->GetRepeated##METHOD(from, fromField, y)); \
                    break;
                    
                    HANDLE_TYPE(INT32 , Int32 );
                    HANDLE_TYPE(INT64 , Int64 );
                    HANDLE_TYPE(UINT32, UInt32);
                    HANDLE_TYPE(UINT64, UInt64);
                    HANDLE_TYPE(DOUBLE, Double);
                    HANDLE_TYPE(FLOAT , Float );
                    HANDLE_TYPE(BOOL  , Bool  );
                    HANDLE_TYPE(STRING, String);
                    HANDLE_TYPE(ENUM  , Enum  );
                    #undef HANDLE_TYPE
                    
                case FieldDescriptor::CPPTYPE_MESSAGE:
                    toReflection->AddMessage(to, toField)->MergeFrom(
                        fromReflection->GetRepeatedMessage(from, fromField, y));
                    break;
            }
        }
    } else {
        switch (fromField->cpp_type()) {
            #define HANDLE_TYPE(CPPTYPE, METHOD)               \
            case FieldDescriptor::CPPTYPE_##CPPTYPE:           \
                toReflection->Set##METHOD(to, toField,         \
                fromReflection->Get##METHOD(from, fromField)); \
                break;
                
                HANDLE_TYPE(INT32 , Int32 );
                HANDLE_TYPE(INT64 , Int64 );
                HANDLE_TYPE(UINT32, UInt32);
                HANDLE_TYPE(UINT64, UInt64);
                HANDLE_TYPE(DOUBLE, Double);
                HANDLE_TYPE(FLOAT , Float );
                HANDLE_TYPE(BOOL  , Bool  );
                HANDLE_TYPE(STRING, String);
                HANDLE_TYPE(ENUM  , Enum  );
                #undef HANDLE_TYPE
                
            case google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE:
                CopyMessage(
                        fromReflection->GetMessage(from, fromField),
                        toReflection->MutableMessage(to, toField));
                break;
        }
    }
}
    
}
}
}

#include "parser.h"

profanedb::storage::Parser::Parser()
{
    sourceTree.MapPath("", "/usr/include"); // google/protobuf/... should be here
    sourceTree.MapPath("", "/home/giorgio/Documents/ProfaneDB/src"); // HACK profanedb/options
    sourceTree.MapPath("", "/home/giorgio/Documents/ProfaneDB/test"); // HACK The DB schema defined by the user
    
    inputStream = sourceTree.Open("");
    if (inputStream == NULL) {
        std::cerr << "Couldn't open .proto source tree: " << sourceTree.GetLastErrorMessage() << std::endl;
    }
    
    descriptorDb = new compiler::SourceTreeDescriptorDatabase(&sourceTree);
    descriptorDb->RecordErrorsTo(errCollector);
    
    pool = new DescriptorPool(descriptorDb);
    
    pool->FindFileByName("profanedb/protobuf/options.proto");
    pool->FindFileByName("test.proto");
}

profanedb::storage::Parser::~Parser()
{
}

map<std::string, const google::protobuf::Message &> profanedb::storage::Parser::ParseMessage(const google::protobuf::Any& serializable)
{
    std::string type = serializable.type_url();
    
    const Descriptor * definition = pool->FindMessageTypeByName(type.substr(type.rfind('/')+1, string::npos));
    
    Message * container = messageFactory.GetPrototype(definition)->New();
    serializable.UnpackTo(container);

    return ParseMessage(*container);
}

map<std::string, const google::protobuf::Message &> profanedb::storage::Parser::ParseMessage(const google::protobuf::Message& message)
{
    auto dependencies = new map<std::string, const google::protobuf::Message &>();
    auto fields = new std::vector< const FieldDescriptor * >();
    
    message.GetReflection()->ListFields(message, fields);
    
    std::string key;
    
    for (auto const & fd: *fields) {
        if (fd->message_type() != NULL) {
            auto nested = ParseMessage(message.GetReflection()->GetMessage(message, fd, &messageFactory));
            dependencies->insert(nested.begin(), nested.end());
        } else {
            auto options = fd->options().GetExtension(profanedb::protobuf::options);
            
            if (options.key()) {
                key = fd->full_name() + "$" + FieldToString(&message, fd);
            }
        }
    }
    
    dependencies->insert( std::pair< std::string, const google::protobuf::Message & >(key, message) );
    
    return *dependencies;
}

std::string profanedb::storage::Parser::FieldToString(const google::protobuf::Message * container, const google::protobuf::FieldDescriptor * fd)
{
    const Reflection * reflection = container->GetReflection();
    std::string field_bytes;
    
    switch (fd->cpp_type()) {
        case FieldDescriptor::CPPTYPE_INT32:
            field_bytes = std::to_string(reflection->GetInt32(*container, fd));
            break;
        case FieldDescriptor::CPPTYPE_INT64:
            field_bytes = std::to_string(reflection->GetInt64(*container, fd));
            break;
        case FieldDescriptor::CPPTYPE_UINT32:
            field_bytes = std::to_string(reflection->GetUInt32(*container, fd));
            break;
        case FieldDescriptor::CPPTYPE_UINT64:
            field_bytes = std::to_string(reflection->GetUInt64(*container, fd));
            break;
        case FieldDescriptor::CPPTYPE_DOUBLE:
            field_bytes = std::to_string(reflection->GetDouble(*container, fd));
            break;
        case FieldDescriptor::CPPTYPE_FLOAT:
            field_bytes = std::to_string(reflection->GetFloat(*container, fd));
            break;
        case FieldDescriptor::CPPTYPE_BOOL:
            field_bytes = std::to_string(reflection->GetBool(*container, fd));
            break;
        case FieldDescriptor::CPPTYPE_ENUM:
            field_bytes = std::to_string(reflection->GetEnum(*container, fd)->index());
            break;
        case FieldDescriptor::CPPTYPE_STRING:
            field_bytes = reflection->GetString(*container, fd);
            break;
        case FieldDescriptor::CPPTYPE_MESSAGE:
            // TODO Normalize data here
            break;
    }
    return field_bytes;
}

profanedb::storage::Parser::ErrorCollector::ErrorCollector()
{
}

void profanedb::storage::Parser::ErrorCollector::AddError(const string & filename, int line, int column, const string & message)
{
    if (line == -1) { // Entire file error
        std::cerr << filename << " error: " << message << "\n";
    } else {
        std::cerr << filename << " " << line+1 << ":" << column+1 << " error: " << message << "\n";
    }
}

void profanedb::storage::Parser::ErrorCollector::AddWarning(const string & filename, int line, int column, const string & message)
{
    std::cerr << filename << " " << line+1 << ":" << column+1 << " warning: " << message << "\n";
}

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

void profanedb::storage::Parser::ParseMessage(const Any& serializable)
{
    std::string type = serializable.type_url();
    
    const Descriptor* definition = pool->FindMessageTypeByName(type.substr(type.rfind('/')+1, string::npos));
    const FieldDescriptor* fd;
    
    // TODO Should check for multiple keys (might be supported later), for now throw error
    for (int idx = 0; idx < definition->field_count(); idx++) {
        fd = definition->field(idx);
        
        if (fd->options().GetExtension(profanedb::protobuf::options).key()) {
            std::cout << fd->full_name() << " is key" << std::endl;
            break;
        }
    }
    
    Message* container = messageFactory.GetPrototype(definition)->New();
    
    serializable.UnpackTo(container);
    std::cout << container->GetReflection()->GetInt32(*container, fd) << std::endl;
}

profanedb::storage::Parser::ErrorCollector::ErrorCollector()
{
}

void profanedb::storage::Parser::ErrorCollector::AddError(const string& filename, int line, int column, const string& message)
{
    if (line == -1) { // Entire file error
        std::cerr << filename << " error: " << message << "\n";
    } else {
        std::cerr << filename << " " << line+1 << ":" << column+1 << " error: " << message << "\n";
    }
}

void profanedb::storage::Parser::ErrorCollector::AddWarning(const string& filename, int line, int column, const string& message)
{
    std::cerr << filename << " " << line+1 << ":" << column+1 << " warning: " << message << "\n";
}

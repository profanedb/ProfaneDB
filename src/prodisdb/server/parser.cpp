#include "parser.h"

prodisdb::server::Parser::Parser()
{
    sourceTree.MapPath("", "/usr/include"); // google/protobuf/... should be here
    sourceTree.MapPath("", "/home/giorgio/Documents/ProdisDB/src"); // HACK prodisdb/options
    sourceTree.MapPath("", "/home/giorgio/Documents/ProdisDB/test"); // HACK The DB schema defined by the user
    
    inputStream = sourceTree.Open("");
    if (inputStream == NULL) {
        std::cerr << "Couldn't open .proto source tree: " << sourceTree.GetLastErrorMessage() << std::endl;
    }
    
    descriptorDb = new compiler::SourceTreeDescriptorDatabase(&sourceTree);
    descriptorDb->RecordErrorsTo(errCollector);
    
    pool = new DescriptorPool(descriptorDb);
    
    pool->FindFileByName("prodisdb/protobuf/options.proto");
    pool->FindFileByName("test.proto");
    

}

prodisdb::server::Parser::~Parser()
{
}

void prodisdb::server::Parser::ParseMessage(const protobuf::Serializable& message)
{
    const Descriptor* definition = pool->FindMessageTypeByName(message.type());
    const FieldDescriptor* fd;
    
    // TODO Should check for multiple keys (might be supported later), for now throw error
    for (int idx = 0; idx < definition->field_count(); idx++) {
        fd = definition->field(idx);
        std::cout
            << fd->full_name()
            << " is key: "
            << fd->options().GetExtension(prodisdb::protobuf::options).key()
            << std::endl;
        
        if (fd->options().GetExtension(prodisdb::protobuf::options).key()) {
            break;
        }
    }
    
    std::cout << message.type() << std::endl;
}

prodisdb::server::Parser::ErrorCollector::ErrorCollector()
{
}

void prodisdb::server::Parser::ErrorCollector::AddError(const string& filename, int line, int column, const string& message)
{
    if (line == -1) { // Entire file error
        std::cerr << filename << " error: " << message << "\n";
    } else {
        std::cerr << filename << " " << line+1 << ":" << column+1 << " error: " << message << "\n";
    }
}

void prodisdb::server::Parser::ErrorCollector::AddWarning(const string& filename, int line, int column, const string& message)
{
    std::cerr << filename << " " << line+1 << ":" << column+1 << " warning: " << message << "\n";
}

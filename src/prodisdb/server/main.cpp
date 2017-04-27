#include <iostream>
#include <string>
#include <google/protobuf/compiler/importer.h>
#include <google/protobuf/io/zero_copy_stream.h>

#include <prodisdb/protobuf/options.pb.h>
#include <prodisdb/server/prodis_error_collector.h>

const std::string VIRTUAL_PROTO_MAP = "schema";
const std::string PROTO_DIR = "/home/giorgio/Documents/ProdisDB/test"; // TODO Should be config

int main(int argc, char *argv[]) {
    google::protobuf::compiler::DiskSourceTree *sourceTree;
    google::protobuf::io::ZeroCopyInputStream *inputStream;
    google::protobuf::compiler::MultiFileErrorCollector *errCollector = new ProdisErrorCollector();
    google::protobuf::compiler::Importer *importer;
    google::protobuf::compiler::SourceTreeDescriptorDatabase *descriptorDb;
    google::protobuf::DescriptorPool *pool;
    
    sourceTree = new google::protobuf::compiler::DiskSourceTree();
    sourceTree->MapPath(VIRTUAL_PROTO_MAP, PROTO_DIR);
    sourceTree->MapPath("", "/home/giorgio/Documents/ProdisDB/src");
    sourceTree->MapPath("", "/usr/include");
    
    inputStream = sourceTree->Open(VIRTUAL_PROTO_MAP);
    if (inputStream == NULL) {
        std::cerr << "Couldn't open .proto source tree: " << sourceTree->GetLastErrorMessage() << "\n";
    }
    
    descriptorDb = new google::protobuf::compiler::SourceTreeDescriptorDatabase(sourceTree);
    descriptorDb->RecordErrorsTo(errCollector);
    
    pool = new google::protobuf::DescriptorPool(descriptorDb);
    
    pool->FindFileByName("prodisdb/protobuf/options.proto");
    pool->FindFileByName(VIRTUAL_PROTO_MAP + "/test.proto");
    
    const google::protobuf::Descriptor *message = pool->FindMessageTypeByName("test.Test");
    const google::protobuf::FieldDescriptor *fd;
    
    for (int idx = 0; idx < message->field_count(); idx++) {
        fd = message->field(idx);
        std::cout
            << fd->full_name()
            << " is primary key: "
            << fd->options().GetExtension(prodisdb::protobuf::options).pk()
            << "\n";
        
        if (fd->options().GetExtension(prodisdb::protobuf::options).pk()) {
            break;
        }
    }
}

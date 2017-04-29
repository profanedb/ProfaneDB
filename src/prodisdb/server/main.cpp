#include <iostream>
#include <string>
#include <google/protobuf/compiler/importer.h>
#include <google/protobuf/io/zero_copy_stream.h>

#include <prodisdb/protobuf/options.pb.h>
#include <prodisdb/server/prodis_error_collector.h>

const std::string PROTO_DIR = "/home/giorgio/Documents/ProdisDB/test"; // TODO Should be config

using namespace google::protobuf;

int main(int argc, char *argv[]) {
    compiler::DiskSourceTree *sourceTree;
    io::ZeroCopyInputStream *inputStream;
    compiler::MultiFileErrorCollector *errCollector = new ProdisErrorCollector();
    compiler::Importer *importer;
    compiler::SourceTreeDescriptorDatabase *descriptorDb;
    DescriptorPool *pool;

    sourceTree = new compiler::DiskSourceTree();
    sourceTree->MapPath("", "/home/giorgio/Documents/ProdisDB/src"); // prodis options
    sourceTree->MapPath("", "/usr/include"); // /usr/include is for google/protobuf/... files
    
    sourceTree->MapPath("", PROTO_DIR); // Here goes the schema defined by the user
    
    inputStream = sourceTree->Open(""); // To load files in PROTO_DIR
    if (inputStream == NULL) {
        std::cerr << "Couldn't open .proto source tree: " << sourceTree->GetLastErrorMessage() << "\n";
    }
    
    descriptorDb = new compiler::SourceTreeDescriptorDatabase(sourceTree);
    descriptorDb->RecordErrorsTo(errCollector);
    
    pool = new DescriptorPool(descriptorDb);
    
    // HACK DescriptorPool::BuildFile should be used (for performance)
    pool->FindFileByName("prodisdb/protobuf/options.proto");
    pool->FindFileByName("test.proto");
    
    const Descriptor *message = pool->FindMessageTypeByName("test.Test");
    const FieldDescriptor *fd;
    
    // TODO Should check for multiple keys (might be supported later), for now throw error
    for (int idx = 0; idx < message->field_count(); idx++) {
        fd = message->field(idx);
        std::cout
            << fd->full_name()
            << " is key: "
            << fd->options().GetExtension(prodisdb::protobuf::options).key()
            << "\n";
        
        if (fd->options().GetExtension(prodisdb::protobuf::options).key()) {
            break;
        }
    }
}

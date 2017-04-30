#ifndef PARSER_H
#define PARSER_H

#include <iostream>

#include <google/protobuf/compiler/importer.h>
#include <google/protobuf/io/zero_copy_stream.h>
#include <google/protobuf/dynamic_message.h>

#include <prodisdb/protobuf/db.pb.h>
#include <prodisdb/protobuf/options.pb.h>

using namespace google::protobuf;

namespace prodisdb {
namespace server {
    
class Parser
{
public:
    Parser();
    ~Parser();
    
    void ParseMessage(const Any& serializable);

private:
    io::ZeroCopyInputStream* inputStream;
    compiler::DiskSourceTree sourceTree;
    compiler::MultiFileErrorCollector* errCollector = new ErrorCollector();
    compiler::SourceTreeDescriptorDatabase* descriptorDb;
    DescriptorPool* pool;
    
    DynamicMessageFactory messageFactory;
    
    class ErrorCollector : public compiler::MultiFileErrorCollector {
    public:
        ErrorCollector();
        void AddError(const string & filename, int line, int column, const string & message) override;
        void AddWarning(const string & filename, int line, int column, const string & message) override;
    };
};

}
}

#endif // PARSER_H

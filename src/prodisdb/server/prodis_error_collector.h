#ifndef PRODISERRORCOLLECTOR_H
#define PRODISERRORCOLLECTOR_H

#include <iostream>
#include <google/protobuf/compiler/importer.h>

class ProdisErrorCollector : public google::protobuf::compiler::MultiFileErrorCollector
{
public:
    ProdisErrorCollector();
    virtual void AddError(const std::__cxx11::string& filename, int line, int column, const std::__cxx11::string& message);
    virtual void AddWarning(const std::__cxx11::string& filename, int line, int column, const std::__cxx11::string& message);
};

#endif // PRODISERRORCOLLECTOR_H

#include "prodis_error_collector.h"

ProdisErrorCollector::ProdisErrorCollector()
{

}

void ProdisErrorCollector::AddError(const std::__cxx11::string& filename, int line, int column, const std::__cxx11::string& message)
{
    if (line == -1) { // Entire file error
        std::cerr << filename << " error: " << message << "\n";
    } else {
        std::cerr << filename << " " << line+1 << ":" << column+1 << " error: " << message << "\n";
    }
}

void ProdisErrorCollector::AddWarning(const std::__cxx11::string& filename, int line, int column, const std::__cxx11::string& message)
{
    std::cerr << filename << " " << line+1 << ":" << column+1 << " warning: " << message << "\n";
}

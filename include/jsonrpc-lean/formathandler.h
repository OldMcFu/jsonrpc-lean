#ifndef JSONRPC_LEAN_FORMATHANDLER_H
#define JSONRPC_LEAN_FORMATHANDLER_H

#include <memory>
#include <string>

namespace jsonrpc {

    class Reader;
    class Writer;

    class FormatHandler {
    public:
        virtual ~FormatHandler() {}

        virtual bool CanHandleRequest(const std::string& contentType) = 0;
        virtual std::string GetContentType() = 0;
        virtual bool UsesId() = 0;
        virtual std::unique_ptr<Reader> CreateReader(const std::string& data) = 0;
        virtual std::unique_ptr<Writer> CreateWriter() = 0;
    };

} // namespace jsonrpc

#endif // JSONRPC_LEAN_FORMATHANDLER_H

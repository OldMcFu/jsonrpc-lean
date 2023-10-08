#ifndef JSONRPC_LEAN_JSONREQUESTDATA_H
#define JSONRPC_LEAN_JSONREQUESTDATA_H

#include "formatteddata.h"

#define RAPIDJSON_NO_SIZETYPEDEFINE
namespace rapidjson { typedef ::std::size_t SizeType; }

#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

namespace jsonrpc {

    class JsonFormattedData final : public FormattedData {
    public:
        JsonFormattedData() : Writer(myStringBuffer) {

        }

        const char* GetData() override {
            return myStringBuffer.GetString();
        }

        size_t GetSize() override {
            return myStringBuffer.GetSize();
        }

        rapidjson::Writer<rapidjson::StringBuffer> Writer;

    private:

        rapidjson::StringBuffer myStringBuffer;
        
    };

} // namespace jsonrpc

#endif // JSONRPC_LEAN_JSONREQUESTDATA_H
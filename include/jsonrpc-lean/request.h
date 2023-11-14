#ifndef JSONRPC_LEAN_REQUEST_H
#define JSONRPC_LEAN_REQUEST_H

#include "value.h"

#include <deque>
#include <string>

namespace jsonrpc {

    class Writer;

    class Request {
    public:
        typedef std::deque<Value> Parameters;

        Request(std::string methodName, Parameters parameters, Value id)
            : myMethodName(std::move(methodName)),
            myParameters(std::move(parameters)),
            myId(std::move(id)) {
            // Empty
        }

        const std::string& GetMethodName() const { return myMethodName; }
        const Parameters& GetParameters() const { return myParameters; }
        const Value& GetId() const { return myId; }

        void Write(Writer& writer) const {
            Write(myMethodName, myParameters, myId, writer);
        }

        static void Write(const std::string& methodName, const Parameters& params, const Value& id, Writer& writer) {
            writer.StartDocument();
            writer.StartRequest(methodName, id);
            if(params.size() > 1){
                writer.StartArray();
            }
            for (auto& param : params) {
                writer.StartParameter();
                param.Write(writer);
                writer.EndParameter();
            }
            if(params.size() > 1){
                writer.EndArray();
            }
            writer.EndRequest();
            writer.EndDocument();
        }

    private:
        std::string myMethodName;
        Parameters myParameters;
        Value myId;
    };

} // namespace jsonrpc

#endif //JSONRPC_LEAN_REQUEST_H

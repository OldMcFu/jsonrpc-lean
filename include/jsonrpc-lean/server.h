#ifndef JSONRPC_LEAN_SERVER_H
#define JSONRPC_LEAN_SERVER_H

#include "request.h"
#include "value.h"
#include "fault.h"
#include "formathandler.h"
#include "jsonformathandler.h"
#include "reader.h"
#include "response.h"
#include "writer.h"
#include "formatteddata.h"
#include "jsonformatteddata.h"
#include "dispatcher.h"


#include <string>

namespace jsonrpc {

    class Server {
    public:
        Server() {}
        ~Server() {}

        Server(const Server&) = delete;
        Server& operator=(const Server&) = delete;
        Server(Server&&) = delete;
        Server& operator=(Server&&) = delete;

        void RegisterFormatHandler(FormatHandler& formatHandler) {
            myFormatHandlers.push_back(&formatHandler);
        }

        Dispatcher& GetDispatcher() { return myDispatcher; }

        // aContentType is here to allow future implementation of other rpc formats with minimal code changes
        // Will return NULL if no FormatHandler is found, otherwise will return a FormatedData
        // If aRequestData is a Notification (the client doesn't expect a response), the returned FormattedData will have an empty ->GetData() buffer and ->GetSize() will be 0
        std::shared_ptr<jsonrpc::FormattedData> HandleRequest(const std::string& aRequestData, const std::string& aContentType = "application/json") {

            // first find the correct handler
            FormatHandler *fmtHandler = nullptr;
            for (auto handler : myFormatHandlers) {
                if (handler->CanHandleRequest(aContentType)) {
                    fmtHandler = handler;
                }
            }

            if (fmtHandler == nullptr) {
                // no FormatHandler able to handle this request type was found
                return nullptr;
            }
            
            auto writer = fmtHandler->CreateWriter();

            try {
                auto reader = fmtHandler->CreateReader(aRequestData);
                Request request = reader->GetRequest();
                reader.reset();

                auto response = myDispatcher.Invoke(request.GetMethodName(), request.GetParameters(), request.GetId());
                if (!response.GetId().IsBoolean() || response.GetId().AsBoolean() != false) {
                    // if Id is false, this is a notification and we don't have to write a response
                    response.Write(*writer);
                }
            } catch (const Fault& ex) {
                Response(ex.GetCode(), ex.GetString(), Value()).Write(*writer);
            }

            return writer->GetData();
        }
    private:
        Dispatcher myDispatcher;
        std::vector<FormatHandler*> myFormatHandlers;
    };

} // namespace jsonrpc

#endif // JSONRPC_LEAN_SERVER_H


#ifndef JSONRPC_LEAN_ASYNC_SERVER_H
#define JSONRPC_LEAN_ASYNC_SERVER_H

#include <vector>
#include <future>

#include "jsonrpc-lean/jsonformathandler.h"
#include "jsonrpc-lean/formathandler.h"
#include "jsonrpc-lean/server.h"
#include "jsonrpc-lean/threadSafeRPCQueue.h"

namespace jsonrpc
{

    class AsyncServer
    {
    public:
        AsyncServer()
        {
        }
        ~AsyncServer() {}

        AsyncServer(const AsyncServer &) = delete;
        AsyncServer &operator=(const AsyncServer &) = delete;
        AsyncServer(AsyncServer &&) = delete;
        AsyncServer &operator=(AsyncServer &&) = delete;

        void RegisterFormatHandler(FormatHandler &formatHandler)
        {
            myFormatHandlers.push_back(&formatHandler);
        }

        Dispatcher &GetDispatcher() { return myDispatcher; }

        // aContentType is here to allow future implementation of other rpc formats with minimal code changes
        // Will return NULL if no FormatHandler is found, otherwise will return a FormatedData
        // If aRequestData is a Notification (the client doesn't expect a response), the returned FormattedData will have an empty ->GetData() buffer and ->GetSize() will be 0
        void HandleRequest(const std::string &aRequestData)
        {

            // auto callBack = std::bind(&jsonrpc::asyncTask, aRequestData, tsQueue, myDispatcher, fmtHandler);
            // auto hand=async(launch::async,&A::do_rand_stf,this,i,j);
            std::cout << aRequestData << std::endl;
            myWorkers.push_back(std::async(std::launch::async, &AsyncServer::serverAsyncTask, this, aRequestData));
        }

        std::shared_ptr<jsonrpc::FormattedData> checkResponseQueue()
        {
            if (!myWorkers.empty())
            {
                for (unsigned int i = 0; i < myWorkers.size(); i++)
                {
                    if (myWorkers[i].wait_for(std::chrono::seconds(0)) == std::future_status::ready)
                    {
                        myWorkers.erase(myWorkers.begin() + i);
                        myWorkers.shrink_to_fit();
                    }
                }
                std::cout << "Worker Queue Size: " << myWorkers.size() << std::endl;
                return tsQueue.pop();
            }
            else
            {
                return nullptr;
            }
        }

        void serverAsyncTask(std::string aRequestData)
        {

            // first find the correct handler
            const std::string aContentType = "application/json";

            FormatHandler *fmtHandler = nullptr;
            for (auto handler : myFormatHandlers)
            {
                if (handler->CanHandleRequest(aContentType))
                {
                    fmtHandler = handler;
                }
            }

            if (fmtHandler != nullptr)
            {
                auto writer = fmtHandler->CreateWriter();

                try
                {
                    auto reader = fmtHandler->CreateReader(aRequestData);
                    Request request = reader->GetRequest();
                    reader.reset();

                    auto response = myDispatcher.Invoke(request.GetMethodName(), request.GetParameters(), request.GetId());
                    if (!response.GetId().IsBoolean() || response.GetId().AsBoolean() != false)
                    {
                        // if Id is false, this is a notification and we don't have to write a response
                        response.Write(*writer);
                    }
                }
                catch (const Fault &ex)
                {
                    Response(ex.GetCode(), ex.GetString(), Value()).Write(*writer);
                }

                tsQueue.push(writer->GetData());
            }
        }

    private:
        TsRpcQueue tsQueue;
        Dispatcher myDispatcher;

        std::vector<FormatHandler *> myFormatHandlers;
        std::vector<std::future<void>> myWorkers;
    };
}

#endif // JSONRPC_LEAN_ASYNC_SERVER_H
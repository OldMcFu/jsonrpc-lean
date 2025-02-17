#pragma once
#ifndef JSONRPC_LEAN_JSONFORMATHANDLER_H
#define JSONRPC_LEAN_JSONFORMATHANDLER_H

#include "formathandler.h"
#include "jsonreader.h"
#include "jsonwriter.h"

#include <memory>

namespace jsonrpc {

    const char APPLICATION_JSON[] = "application/json";

    class JsonFormatHandler : public FormatHandler {
    public:
        explicit JsonFormatHandler() {}

        // FormatHandler
        bool CanHandleRequest(const std::string& contentType) override {
            return contentType == APPLICATION_JSON;
        }

        std::string GetContentType() override {
            return APPLICATION_JSON;
        }

        bool UsesId() override {
            return true;
        }

        std::unique_ptr<Reader> CreateReader(const std::string& data) override {
            return std::unique_ptr<Reader>(std::make_unique<JsonReader>(std::move(data)));
        }

        std::unique_ptr<Writer> CreateWriter() override {
            return std::unique_ptr<Writer>(std::make_unique<JsonWriter>());
        }

    private:

    };

} // namespace jsonrpc

#endif // JSONRPC_LEAN_JSONFORMATHANDLER_H

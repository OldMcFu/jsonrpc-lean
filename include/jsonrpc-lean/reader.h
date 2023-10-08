#ifndef JSONRPC_LEAN_READER_H
#define JSONRPC_LEAN_READER_H

namespace jsonrpc {

    class Request;
    class Response;
    class Value;

    class Reader {
    public:
        virtual ~Reader() {}

        virtual Request GetRequest() = 0;
        virtual Response GetResponse() = 0;
        virtual Value GetValue() = 0;
    };

} // namespace jsonrpc

#endif // JSONRPC_LEAN_READER_H

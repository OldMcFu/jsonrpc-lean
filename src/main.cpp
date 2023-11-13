#include "jsonrpc-lean/jsonformathandler.h"
#include "jsonrpc-lean/formathandler.h"
#include "jsonrpc-lean/asyncServer.h"

#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>


#include <iostream>
#include <numeric>
#include <string>
#include <memory>
#include <stdint.h>
#include <thread>

using namespace std::chrono_literals;

static void print_json_value(const rapidjson::Value &value) {
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    value.Accept(writer);

    std::cout << buffer.GetString() << std::endl;
}

static rapidjson::Document str_to_json(const char* json) {
    rapidjson::Document document;
    document.Parse(json);
    return std::move(document);
}

class Math {
public:
	int Add(int a, int b) {
		std::this_thread::sleep_for(10s);
		return a + b;
	}

	int64_t AddArray(const jsonrpc::Value::Array& a) {
		return std::accumulate(a.begin(), a.end(), int64_t(0),
			[](const int64_t& a, const jsonrpc::Value& b) { return a + b.AsInteger32(); });
	};
};

std::string Concat(const std::string& a, const std::string& b) {
	std::this_thread::sleep_for(11s);
	return a + b;
}

jsonrpc::Value ToBinary(const std::string& s) {
	return jsonrpc::Value(s, true);
}

std::string FromBinary(const jsonrpc::Value& b) {
	return{ b.AsBinary().begin(), b.AsBinary().end() };
}

jsonrpc::Value::Struct ToStruct(const jsonrpc::Value::Array& a) {
	jsonrpc::Value::Struct s;
	for (size_t i = 0; i < a.size(); ++i) {
		s[std::to_string(i)] = jsonrpc::Value(a[i]);
	}
	return s;
}

void PrintNotification(const std::string& a) {
    std::cout << "notification: " << a << std::endl;
}

void RunServer() {
	
	Math math;
	jsonrpc::AsyncServer server;
	jsonrpc::JsonFormatHandler jsonFormatHandler;
	server.RegisterFormatHandler(jsonFormatHandler);

	auto& dispatcher = server.GetDispatcher();
	// if it is a member method, you must use this 3 parameter version, passing an instance of an object that implements it
	dispatcher.AddMethod("add", &Math::Add, math);
	dispatcher.AddMethod("add_array", &Math::AddArray, math); 
	
	// if it is just a regular function (non-member or static), you can you the 2 parameter AddMethod
	dispatcher.AddMethod("concat", &Concat);
	dispatcher.AddMethod("to_binary", &ToBinary);
	dispatcher.AddMethod("from_binary", &FromBinary);
	dispatcher.AddMethod("to_struct", &ToStruct);
	dispatcher.AddMethod("print_notification", &PrintNotification);

	dispatcher.GetMethod("add")
		.SetHelpText("Add two integers")
		.AddSignature(jsonrpc::Value::Type::INTEGER_32, jsonrpc::Value::Type::INTEGER_32, jsonrpc::Value::Type::INTEGER_32);

	//bool run = true;
	//dispatcher.AddMethod("exit", [&]() { run = false; }).SetHidden();
	const char addRequest[] = "{\"jsonrpc\":\"2.0\",\"method\":\"add\",\"id\":0,\"params\":[3,2]}";
	const char concatRequest[] = "{\"jsonrpc\":\"2.0\",\"method\":\"concat\",\"id\":1,\"params\":[\"Hello, \",\"World!\"]}";
	const char addArrayRequest[] = "{\"jsonrpc\":\"2.0\",\"method\":\"add_array\",\"id\":2,\"params\":[[1000,2147483647]]}";
	const char toBinaryRequest[] = "{\"jsonrpc\":\"2.0\",\"method\":\"to_binary\",\"id\":3,\"params\":[\"Hello World!\"]}";
	const char toStructRequest[] = "{\"jsonrpc\":\"2.0\",\"method\":\"to_struct\",\"id\":4,\"params\":[[12,\"foobar\",[12,\"foobar\"]]]}";
	const char printNotificationRequest[] = "{\"jsonrpc\":\"2.0\",\"method\":\"print_notification\",\"params\":[\"This is just a notification, no response expected!\"]}";

    rapidjson::Document document;
    document.Parse(addRequest);
    assert(document.HasMember("jsonrpc"));
    assert(document["jsonrpc"].IsString());
    //std::cout << document["jsonrpc"].GetString() << std::endl;
    //print_json_value(document);  // works

	
    server.HandleRequest(addRequest);
	server.HandleRequest(addRequest);
	server.HandleRequest(addRequest);
	server.HandleRequest(addRequest);
    
    server.HandleRequest(concatRequest);

    server.HandleRequest(addArrayRequest);

    server.HandleRequest(toBinaryRequest);

    server.HandleRequest(toStructRequest);

    server.HandleRequest(printNotificationRequest);

	int cnt = 0;
	while(1)
	{
		std::shared_ptr<jsonrpc::FormattedData> outputFormatedData = server.checkResponseQueue();
		if (outputFormatedData != nullptr)
		{
			std::cout << "response: " << outputFormatedData->GetData() << " \t" << cnt << std::endl;
			cnt += 1;
		} 
		std::this_thread::sleep_for(1s);

		if (cnt >= 8) break;
	}

}

int main() {
	try {
		RunServer();
	} catch (const std::exception& ex) {
		std::cerr << "Error: " << ex.what() << "\n";
		return 1;
	}

    const char* json_text = "[1,2,3]";

    // copy the code of str_to_json() here9
    rapidjson::Document document;
    document.Parse(json_text);
    //print_json_value(document);  // works

    const rapidjson::Document json_value = str_to_json(json_text);
    assert(json_value.IsArray());
    //print_json_value(json_value);  // Now works

	return 0;
}
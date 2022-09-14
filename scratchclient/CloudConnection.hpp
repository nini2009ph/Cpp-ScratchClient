#pragma once
#include <functional>
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>

#include <ixwebsocket/IXNetSystem.h>
#include <ixwebsocket/IXWebSocket.h>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>

#include "ScratchSession.hpp"

#define CONNECT_TIME std::chrono::milliseconds(600)
#define SEND_TIME std::chrono::milliseconds(100)

namespace scratchclient {
	struct CloudVariable {
		std::string name;
		std::string value;
	};

	class CloudConnection {
  std::function<void(std::string, std::string)> set_cloud_variable_lambda;
		ix::WebSocket websocket;
		int reconnects;
    
    void connect();
	public:
		std::vector<std::function<void(CloudVariable)>> on_set_callbacks;
    std::vector<CloudVariable> cloud_variables;
    
		std::vector<std::thread> callback_threads;
		std::vector<std::thread> set_threads;
    std::thread connection_thread;
    
		ScratchSession client;
		int project_id;
    
    CloudConnection(ScratchSession& client, int project_id, bool async = false);
		~CloudConnection();
    
		void set_cloud_variable(std::string name, std::string value, bool async = false);
		std::string get_cloud_variable(std::string name);
	};
}

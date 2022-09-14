#include "CloudConnection.hpp"

void scratchclient::CloudConnection::connect() {
	auto splitstr = [](std::string str, std::string separator) {
		int start = 0;
		int end = str.find(separator);
		std::vector<std::string> output;
    
		while (end != -1) {
			output.emplace_back(str.substr(start, end - start));
			start = end + separator.size();
			end = str.find(separator, start);
		}
		output.emplace_back(str.substr(start, end - start));
		return output;
	};

#if defined(WIN32)
	ix::initNetSystem();
#endif		
	websocket.setUrl("wss://clouddata.scratch.mit.edu/");
	websocket.setHandshakeTimeout(5);

	websocket.setOnMessageCallback([&](const ix::WebSocketMessagePtr& msg) {
		if (msg->type == ix::WebSocketMessageType::Open) {
			reconnects++;
		}
		if (msg->type == ix::WebSocketMessageType::Close) {
			if (reconnects > 2) {
				std::cout << "Could not reconnect" << std::endl;
				websocket.disableAutomaticReconnection();
			}
		}
		if (msg->type == ix::WebSocketMessageType::Message) {
			websocket.enableAutomaticReconnection(); reconnects = 0;
			
			std::vector<std::string> responce = splitstr(msg->str, "\n");
			if (responce.size() > 2) {
				cloud_variables.clear();
				for (int i = 0; i < responce.size() - 1; i++) {
					responce[i].erase(responce[i].find(u8"☁ "), 4);
					nlohmann::json json = nlohmann::json::parse(responce[i]);
					cloud_variables.emplace_back(CloudVariable{ json["name"], json["value"] });
				}
			}
			else {
				responce[0].erase(responce[0].find(u8"☁ "), 4);
				if (responce[0].find("\"}") == 4294967295) {
					responce[0].insert(responce[0].find("}"), "\"");
					responce[0].insert(responce[0].find("\"value\":") + 8, "\"");			
				}

				nlohmann::json json = nlohmann::json::parse(responce[0]); bool exists = false;
				for (CloudVariable& cloud_variable : cloud_variables) {
					if (cloud_variable.name == json["name"]) {
						cloud_variable.value = json["value"];
            
						for (auto callback : on_set_callbacks)
							callback_threads.emplace_back(std::thread(callback, CloudVariable{ json["name"], json["value"] }));
							
						exists = true; break;
					}		
				}

				if (exists == false) {
					cloud_variables.emplace_back(CloudVariable{ json["name"], json["value"] });
          
					for (auto callback : on_set_callbacks)
						callback_threads.emplace_back(std::thread(callback, cloud_variables[cloud_variables.size() - 1]));
				}	
			}
		}
	});

	ix::WebSocketHttpHeaders extra_headers;
	extra_headers["cookie"] = "scratchsessionsid=" + client.session_id + ";";
	extra_headers["origin"] = "https://scratch.mit.edu";
	extra_headers["enable_multithread"] = true;
	websocket.setExtraHeaders(extra_headers);

	websocket.start();
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	websocket.send("{\"method\":\"handshake\", \"user\":\"" + client.username + "\", \"project_id\":\"" + std::to_string(project_id) + "\" }\n");
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

scratchclient::CloudConnection::~CloudConnection() {
	websocket.stop();
#if defined(WIN32)
	ix::uninitNetSystem();
#endif	
}

void scratchclient::CloudConnection::set_cloud_variable(std::string name, std::string value, bool async) {
	if(async)
		set_threads.emplace_back(std::thread(set_cloud_variable_lambda, name, value));
	else
		set_cloud_variable_lambda(name, value);
}

std::string scratchclient::CloudConnection::get_cloud_variable(std::string name) {
	for (CloudVariable cloud_variable : cloud_variables)
		if (cloud_variable.name == name) return cloud_variable.value;
	std::cout << "Varible " + name + " is not in this project" << std::endl;
	return "NULL";
}

scratchclient::CloudConnection::CloudConnection(ScratchSession& client, int project_id, bool async) : client(client), project_id(project_id), reconnects(0) {
	set_cloud_variable_lambda = [&](std::string name, std::string value) {
		websocket.send(u8"{ \"method\":\"set\", \"name\":\"☁ " + name + "\", \"value\":\"" + value + "\", \"user\":\"" + client.username + "\", \"project_id\":\"" + std::to_string(this->project_id) + "\" }\n");
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	};
	
	if (async)
		connection_thread = std::thread([&]() { connect(); });
	else
		connect();
}

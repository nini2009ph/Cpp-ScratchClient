#pragma once
#include <iostream>
#include <regex>

#include <nlohmann/json.hpp>
#include <cpr/cpr.h>

namespace scratchclient {
	class ScratchSession {
		void login();
		void get_csrf_token();
	public:
		std::string username;
		std::string password;
		std::string session_id;
		std::string x_token;
		std::string csrf_token;
		
		ScratchSession(std::string username = "", std::string password = "", std::string session_id = "", std::string x_token = "");
	};
}

#include "ScratchSession.hpp"

void scratchclient::ScratchSession::login() {
	cpr::Url login_url{ "https://scratch.mit.edu/login/" };
	cpr::Header login_headers{ {"x-csrftoken", "a"},
							   {"x-requested-with", "XMLHttpRequest"},
							   {"Cookie", "scratchcsrftoken=a;scratchlanguage=en;"},
							   {"referer", "https://scratch.mit.edu"},
							   {"user-agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.101 Safari/537.36"} };
	cpr::Body login_body{ "{ \"username\": \"" + username + "\", \"password\": \"" + password + "\" }" };
	cpr::Response login_responce = cpr::Post(login_url, login_headers, login_body);

	if (login_responce.status_code != 200)
		std::cout << "Your password or username is incorrect" << std::endl;

	std::smatch login_smatch;
	std::regex_search(login_responce.raw_header, login_smatch, std::regex("\"(.*)\""));

	session_id = login_smatch[0];
	x_token = nlohmann::json::parse(login_responce.text)[0]["token"];
}

void scratchclient::ScratchSession::get_csrf_token() {
	cpr::Url csrf_token_url{ "https://scratch.mit.edu/csrf_token/" };
	cpr::Response csrf_token_responce = cpr::Get(csrf_token_url);

	std::smatch csrf_token_smatch;
	std::regex_search(csrf_token_responce.raw_header, csrf_token_smatch, std::regex("scratchcsrftoken=(.*?);"));

	csrf_token = csrf_token_smatch[1];
}

scratchclient::ScratchSession::ScratchSession(std::string username, std::string password, std::string session_id, std::string x_token) : username(username), password(password), session_id(session_id), x_token(x_token) {
	if (password != "") login();
	get_csrf_token();
}

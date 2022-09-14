# CPP-ScratchClient
This is a C++ version of <https://github.com/CubeyTheCube/scratchclient> with similar syntax.

This project was made with permission from the creator of scratchclient <https://github.com/CubeyTheCube>.

Note: This project is only in alpha and is far from done.

# Build
Comming Soon!

# Usage

Basic usage:
```cpp

#include "scratchclient.hpp"

//Adds std::this_thread
#include <chrono>

using namespace scratchclient;

int main() {
	//Basics

	ScratchSession session("username", "password");
	CloudConnection connection(session, 123456789);

	connection.set_cloud_variable("name", "value");
	connection.get_cloud_variable("name");

	//Events

	auto on_set = [&](CloudVariable cloud_variable) {
		//CloudVariable
		cloud_variable.name;
		cloud_variable.value;
	};

	connection.on_set_callbacks.emplace_back(on_set);

	//Async

	CloudConnection connection(session, 123456789, true);
	std::this_thread::sleep_for(CONNECT_TIME);

	connection.set_cloud_variable("name", "value", true);
	std::this_thread::sleep_for(SEND_TIME);

	//Multiple connections

	CloudConnection* connections[2];
	for (int i = 0; i < 2; i++)
		connections[i] = new CloudConnection(session, 123456789, true);
	std::this_thread::sleep_for(CONNECT_TIME);

	for (int i = 0; i < 2; i++)
		connections[i]->set_cloud_variable("name", "value", true);
	std::this_thread::sleep_for(SEND_TIME);

	delete[] connections;
}

```

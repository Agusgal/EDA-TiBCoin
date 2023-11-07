#pragma once

#include <string>
#include "../BlockChain/Blocks/json.hpp"
#include <curl\curl.h>




using namespace std;
using json = nlohmann::json;


namespace {
	const char* stdURL = "eda_coins";
}


class Client
{
public:
	
	Client(const string &ip, const unsigned int ownPort, const unsigned int outPort);
	
	virtual ~Client(void);

	virtual bool performRequest(void);
	virtual const json& getAnswer(void);



protected:
	virtual void configureClient(void) = 0;
	CURL* easyHandler, * multiHandler;

	string ip, rawReply, url;
	json data;
	json answer;

	unsigned int ownPort;
	unsigned int outPort;
	int stillRunning;

	static size_t myCallback(char* contents, size_t size, size_t nmemb, void* userData);
	
	
};

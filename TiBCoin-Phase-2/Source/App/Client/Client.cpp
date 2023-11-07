#include "Client.h"
#include <iostream>


Client::Client(const std::string& ip, const unsigned int ownPort, const unsigned int outPort) : ip(ip), ownPort(ownPort),
outPort(outPort), multiHandler(nullptr), easyHandler(nullptr) 
{
	if (ip.length() && ownPort && outPort)
	{
		stillRunning = 1;
	}
	else
	{
		throw std::exception("Wrong input in client.");
	}

	url = ip + '/' + stdURL;
};

//Performs request.
bool Client::performRequest(void) 
{
	bool stillOn = true;
	static bool first = false;
	try 
	{
		if (ip.length() && ownPort && outPort) 
		{
			if (!first) 
			{
				if (!(easyHandler = curl_easy_init()))
				{
					throw std::exception("Failed to initialize easy handler.");
				}
				
				if (!(multiHandler = curl_multi_init()))
				{
					throw std::exception("Failed to initialize multi handler.");
				}

				configureClient();
				first = true;
			}

			if (stillRunning) 
			{
				if (curl_multi_perform(multiHandler, &stillRunning) != CURLM_OK) 
				{
					curl_easy_cleanup(easyHandler);
					curl_multi_cleanup(multiHandler);
					easyHandler = nullptr;
					multiHandler = nullptr;
					throw std::exception("Failed to connect.");
				}
			}
			else 
			{
				//Cleans
				curl_easy_cleanup(easyHandler);
				curl_multi_cleanup(multiHandler);

				first = false;

				stillRunning = 1;

				//Parse reply from rawreply
				try 
				{
					answer = json::parse(rawReply);
				}
				catch (std::exception&) 
				{
					throw std::exception("Invalid Data. Wrong JSON format.");
				}
				
				std::cout << "Received: " << answer << std::endl;

				stillOn = false;
			}
		}
		else
		{
			throw std::exception("Invalid data.");
		}
	}
	catch (std::exception& e) 
	{
		answer = e.what();
		stillOn = false;
		first = false;
		stillRunning = 1;
		std::cout << "Received: " << answer << std::endl;
	}
	return stillOn;
}

size_t Client::myCallback(char* ptr, size_t size, size_t nmemb, void* userData) 
{
	std::string* userDataPtr = (std::string*)userData;

	userDataPtr->append(ptr, size * nmemb);

	return size * nmemb;
}

const json& Client::getAnswer() 
{
	return answer;
}

Client::~Client() {}
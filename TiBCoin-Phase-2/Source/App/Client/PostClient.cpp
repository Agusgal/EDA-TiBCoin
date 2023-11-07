#include "PostClient.h"

using json = nlohmann::json;


//postClient constructor.
PostClient::PostClient(const std::string& ip, const unsigned int ownPort, const unsigned int outPort,
	const json& data) : Client(ip, ownPort, outPort), data(data)
{
}


//Configures client.
void PostClient::configureClient(void)
{
	rawReply.clear();
	struct curl_slist* list = nullptr;
	
	//Sets header with token.
	strData = "Data=" + data.dump();
	list = curl_slist_append(list, strData.c_str());

	if (curl_easy_setopt(easyHandler, CURLOPT_POST, 1) != CURLE_OK)
	{ 
		throw std::exception("Failed to set POST request.");
	}
	else if (curl_multi_add_handle(multiHandler, easyHandler) != CURLM_OK)
	{
		throw std::exception("Failed to set add handler en cURL.");
	}
	else if (curl_easy_setopt(easyHandler, CURLOPT_URL, url.c_str()) != CURLE_OK)
	{
		throw std::exception("Failed to set URL in cURL.");
	}
	else if (curl_easy_setopt(easyHandler, CURLOPT_PROTOCOLS, CURLPROTO_HTTP) != CURLE_OK)
	{
		throw std::exception("Failed to set HTTP protocol.");
	}
	else if (curl_easy_setopt(easyHandler, CURLOPT_PORT, outPort) != CURLE_OK)
	{
		throw std::exception("Failed to set receiving port");
	}
	else if (curl_easy_setopt(easyHandler, CURLOPT_LOCALPORT, ownPort) != CURLE_OK)
	{
		throw std::exception("Failed to set sending port");
	}
	else if (curl_easy_setopt(easyHandler, CURLOPT_HTTPHEADER, list) != CURLE_OK)
	{
		throw std::exception("Failed to set header.");
	}

	//Sets callback and userData.
	using namespace std::placeholders;
	if (curl_easy_setopt(easyHandler, CURLOPT_WRITEFUNCTION, std::bind(&Client::myCallback, this, _1, _2, _3, _4)) != CURLE_OK)
	{
		throw std::exception("Failed to set callback");
	}
	else if (curl_easy_setopt(easyHandler, CURLOPT_WRITEDATA, &rawReply) != CURLE_OK)
	{
		throw std::exception("Failed to set userData");
	}

//#ifdef _DEBUG
//	if (curl_easy_setopt(easyHandler, CURLOPT_VERBOSE, 1L))
//	{
//		throw std::exception("Failed to set verbose mode");
//	}
//#endif // _DEBUG

}

PostClient::~PostClient() {}
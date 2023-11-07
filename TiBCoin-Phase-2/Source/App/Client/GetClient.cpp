#include "GetClient.h"

GetClient::GetClient(const std::string& ip, const unsigned int ownPort, const unsigned int outPort, const std::string& id,
	const unsigned int count) : Client(ip, ownPort, outPort), id(id), count(count)
{
}

//Configures client.
void GetClient::configureClient(void) 
{
	rawReply.clear();


	if (curl_multi_add_handle(multiHandler, easyHandler) != CURLM_OK)
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
		throw std::exception("Failed to set receiving port.");
	}
	else if (curl_easy_setopt(easyHandler, CURLOPT_LOCALPORT, ownPort) != CURLE_OK)
	{
		throw std::exception("Failed to set sending port.");
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

	if (curl_easy_setopt(easyHandler, CURLOPT_FORBID_REUSE, 1L))
	{
		throw std::exception("Failed to set up reuse");
	}

//#ifdef _DEBUG
//	if (curl_easy_setopt(easyHandler, CURLOPT_VERBOSE, 1L))
//	{
//		throw std::exception("Failed to set verbose mode");
//	}
//#endif // _DEBUG
}
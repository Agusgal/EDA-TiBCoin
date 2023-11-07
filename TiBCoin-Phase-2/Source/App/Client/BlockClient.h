#pragma once
#include "PostClient.h"
class BlockClient : public PostClient
{
public:

	BlockClient(const std::string& ip, const unsigned int ownPort, const unsigned int outPort, const json& data) :
		PostClient(ip, ownPort, outPort, data) {
		url += '/' + indURL;
}
	~BlockClient() {};
private:
	const std::string indURL = "send_block";
};
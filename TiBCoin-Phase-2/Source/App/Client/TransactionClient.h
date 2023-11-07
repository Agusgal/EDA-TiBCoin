#pragma once
#include "PostClient.h"

class TransactionClient : public PostClient 
{
public:
	TransactionClient(const std::string& ip, const unsigned int ownPort, const unsigned int outPort,
		const json& data) : PostClient(ip, ownPort, outPort, data) {
		url += '/' + indURL;
	}
private:
	const std::string indURL = "send_tx";
};
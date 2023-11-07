#pragma once
#include "PostClient.h"

class FilterClient : public PostClient {
public:
	FilterClient(const std::string& ip, const unsigned int self_port, const unsigned int out_port, const json& data) :
		PostClient(ip, self_port, out_port, data) {
		url += '/' + indURL;
	}
private:
	const std::string indURL = "send_filter";
};
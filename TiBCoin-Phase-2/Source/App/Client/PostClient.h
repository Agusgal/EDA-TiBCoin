#pragma once

#include "Client.h"
class PostClient : public Client
{
public:
	PostClient(const std::string&, const unsigned int, const unsigned int, const json&);

	virtual ~PostClient(void);

protected:
	virtual void configureClient(void);

	json data;

	std::string strData;
};
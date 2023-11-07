#pragma once
#include "Client.h"

class GetClient : public Client
{
public:
	GetClient(const std::string&, const unsigned int, const unsigned int, const std::string&, const unsigned int);

	virtual ~GetClient(void) {};

protected:
	virtual void configureClient(void);

	std::string id;
	unsigned int count;
};
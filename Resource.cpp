#include "Resource.h"

Resource::Resource()
{
	Resource("Berserk DVD");
}

Resource::Resource(string name)
{
	this -> name = name;
	this -> status = true;
	this -> Requests = queue<int>();
}

Resource& Resource::operator=(const Resource& other)
{
	this -> name = other.name;
	this -> status = other.status;
	this -> Requests = other.Requests;
	return *this;
}

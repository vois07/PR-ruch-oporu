#include "Resource.h"

Resource& Resource::operator=(const Resource& other)
{
	this -> name = other.name;
	return *this;
}

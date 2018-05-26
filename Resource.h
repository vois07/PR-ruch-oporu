#include <string>
#include <queue>

using namespace std;

class Resource
{
	public:
	string name;
    bool status; //0 not available
    queue<int> Requests; //ids of threads that requested the resource
	
	Resource();
    Resource(string);
	Resource& operator=(const Resource&);
};

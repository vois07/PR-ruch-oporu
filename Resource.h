#include <string>
#include <queue>

using namespace std;

class Resource
{
	
	string name;
    bool status; //0 not available
public:
    queue<int> Requests; //ids of threads that requested the resource
	
	Resource();
    Resource(string);
	Resource& operator=(const Resource&);
	string getName();
	void setStatus(bool);
	bool getStatus();
	
};

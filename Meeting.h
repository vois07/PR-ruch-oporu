#include <queue>
#include <random>

using namespace std;

class Meeting
{
	int id;
	queue <int> RSVPs; //people who responded "yes"
	queue <int> Participants; //people who came to the meeting

	Meeting();
	Meeting(int);
};

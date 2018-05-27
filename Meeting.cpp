#include "Meeting.h"

Meeting::Meeting()
{
	Meeting(rand());
}

Meeting::Meeting(int id)
{
	this -> id = id;
	this -> Participants = queue<int>();
	this -> RSVPs = queue<int>();
}

int Meeting::getID()
{
	return this -> id;
}

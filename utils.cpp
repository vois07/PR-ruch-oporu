#include "utils.h"
#include <string>
#include <random>

namespace utils
{
	int getMeetingChannel(int leader)
	{
		string ans = std::to_string(leader) + std::to_string(rand())%(INT_MAX/1000);
		return std::stoi(ans);
	}
}

#include <string>
#include <sstream>

using namespace std;

void clearLogFile();

void printStatus(string text);

template < class T >
string to_string( T t );
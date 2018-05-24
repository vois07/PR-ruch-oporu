#include "Functions.h"
#include <cstdio>
#include <string>
#include <iostream>
#include <sstream>

using namespace std;

template < class T >
string to_string( T t )
{
    stringstream ss;
    ss << t;
    return ss.str();
}

void printStatus(string text){
    cout<<text;

}
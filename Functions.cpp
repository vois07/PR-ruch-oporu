#include "Functions.h"
#include <cstdio>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>

using namespace std;

//int2string
template < class T >
string to_string( T t )
{
    stringstream ss;
    ss << t;
    return ss.str();
}

//clearing saved log
void clearLogFile(){
    ofstream file;
    file.open("log.txt", ios::out | ios::trunc);
    file.close();
}

//print status to console and save in logfile
void printStatus(string text){
    cout<<text;

    ofstream file;
    file.open("log.txt", ios::out | ios::app);
    file<<text;
    file.close();
}
// nativewriter
// Simple native host for Chrome extensions, that easily allows to write some data from Chrome extensions to file.
// It accepts json commands from stdin (one command per launch)
// Json command fomat: { command: "write", file: request.fname, data: request.data }; 
// command - what to do. fo now the only implemented command is "write" that appends file
// file - filename (with path)
// data - what to write
//
// author Nikolay Safronov bfishh@gmail.com

#include "stdafx.h"
#include <windows.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>

using namespace std;

typedef unsigned int       uint32_t;
static const size_t max_message_size = 1024*1024*10; // Max 10MB in one message

#define ERROR_READ -1
#define ERROR_LAGRE_MSG -2
#define ERROR_WRITE -3

// get JSON message from stdin
int getMessage(stringstream &ss) {
    std::istream* is;
    is = &std::cin;

    uint32_t l;
    is->read((char*)&l,4);
    if (!is->good()) return ERROR_READ;
    if (l==0) return 0;
    if (l>max_message_size) return ERROR_LAGRE_MSG;
    
    char* buf = new char[l];
    is->read(buf,l);
    if (!is->good()) return ERROR_READ;
    ss.write(buf,l);
    delete[] buf;
    return 0;
}

void log (string msg)
{
    HMODULE hModule = GetModuleHandle(NULL);
    if (hModule != NULL)
    {
        TCHAR ownPth[MAX_PATH];
        GetModuleFileName(hModule,ownPth, (sizeof(ownPth))); // log file mane will be same as exe + .log
        wstring p = ownPth;

        ofstream log;        
        log.open (p + L".log",ios::app);
        if (log.fail()) { cerr << "Error: cannot open logfile" << endl; return;}
        log << msg << endl;
        log.close ();
    }
}

bool write (string fname, string data)
{
    ofstream myfile;
    myfile.open (fname,ios::app); //appends file
    if (myfile.fail()){
        log ("Error open file "+fname + " : " + strerror(errno));
        return false;
    }
    myfile << data << endl;
    myfile.close();
    return true;
}

int _tmain(int argc, _TCHAR* argv[])
{
    stringstream ss;
    int res = getMessage(ss);
    if (res== ERROR_READ) { log ("error reading input message"); return ERROR_READ; }
    if (res== ERROR_LAGRE_MSG) { log ("input message to large"); return ERROR_LAGRE_MSG; }
    //parse json message
    try {
        boost::property_tree::ptree pt;
        boost::property_tree::read_json(ss, pt);

        string command = pt.get<string>("command");
        if (command == "write")
        {
            string data = pt.get<string>("data");
            string fname = pt.get<string>("file");
            if (write (fname, data)) log ("Writing to file: " + fname);
            else return ERROR_WRITE;
        }
    }
    catch (std::exception const& e)
    {
        log ("error parsing json");
        log (e.what());
        string jstr = ss.str ();
        log ("Json string: " + jstr + " END");
    }
    return 0;
}

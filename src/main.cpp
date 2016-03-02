//#include <iostream>
//#include <string>
//#include <sstream>
//#include <fstream>
//#include <vector>
#include "viewer/viewer.hpp"

#include "main.hpp"


int main()
{
    //initialize viewer object
    viewer* v = new viewer();
    int load = v->loadWindow();
    if(load == -1) return -1;

    return 0;
}

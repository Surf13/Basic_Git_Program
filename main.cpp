#include "basicGit.h"
#include <iostream>

int main() {

    basicGit myGit;
    myGit.init();

    //myGit.add("Test.txt");
    //myGit.add("main.cpp");

    myGit.status();
}

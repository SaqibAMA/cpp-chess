//
// Created by pysaq on 29/11/2019.
//

#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>

using namespace std;

int main(){

    ifstream test;
    test.open("sampleReplay.txt");

    while (!test.eof()){

        char ch = test.get();
        this_thread::sleep_for(chrono::milliseconds(500));
        cout << ch;

    }

}
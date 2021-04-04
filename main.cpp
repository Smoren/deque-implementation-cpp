#include <iostream>
#include "printer.h"
#include "profiler.h"
#include "deque.h"


using namespace std;
using namespace Smoren::Containers;
using namespace Smoren::Tools;

void printDequeVerbose(const Deque<int>& d);
void testMyDeque();
void testMyDequeBench();

int main() {
    testMyDeque();
    testMyDequeBench();

    return 0;
}

void printDequeVerbose(const Deque<int>& d) {
    cout << d << endl;
    cout << "SIZE: " << d.size() << endl;
    d.printData();
    cout << "==========" << endl << endl;
}


void testMyDeque() {
    {
        Deque<int> d(3);
        printDequeVerbose(d);

        d.push_front(-1);
        printDequeVerbose(d);

        d.push_front(-2);
        printDequeVerbose(d);

        d.push_back(1);
        printDequeVerbose(d);

        d.push_back(2);
        printDequeVerbose(d);

        d.push_front(-3);
        printDequeVerbose(d);

        d.push_front(-4);
        printDequeVerbose(d);

        d.push_back(3);
        printDequeVerbose(d);

        d.push_back(4);
        printDequeVerbose(d);

        d.push_back(5);
        printDequeVerbose(d);

        d.push_back(6);
        printDequeVerbose(d);

        for(int i=0; i<5; i++) {
            d.pop_back();
            printDequeVerbose(d);
        }

        for(int i=0; i<2; i++) {
            d.pop_front();
            printDequeVerbose(d);
        }

        for(size_t i=0; i<d.size(); i++) {
            cout << d[i] << ", ";
        }
        cout << endl;

        for(auto it = d.begin(); it != d.end(); it++) {
            cout << *it << ", ";
        }
        cout << endl;

        for(auto it = d.rbegin(); it != d.rend(); it--) {
            cout << *it << ", ";
        }
        cout << endl;

        for(int& x : d) {
            cout << x << ", ";
        }
        cout << endl;
    }
}

void testMyDequeBench() {
    size_t SIZE = 1000000;
    {
        LOG_DURATION("Deque");
        Deque<int> d(100);
        for(size_t i=0; i<SIZE; i++) {
            d.push_back(1);
        }
        for(size_t i=0; i<SIZE; i++) {
            d.push_front(-1);
        }
        for(size_t i=0; i<SIZE*2; i++) {
            int& a = d[i];
        }
        for(auto& x : d) {

        }
        cout << "Deque size: " << d.size() << endl;
        cout << "Deque chunks: " << d.chunksCount() << endl;
        for(size_t i=0; i<SIZE; i++) {
            d.pop_back();
            d.pop_front();
        }
        cout << "Deque size after pop: " << d.size() << endl;
        cout << "Deque chunks after pop: " << d.chunksCount() << endl;
    }
    cout << endl;
    {
        LOG_DURATION("Deque");
        deque<int> d;
        for(size_t i=0; i<SIZE; i++) {
            d.push_back(1);
        }
        for(size_t i=0; i<SIZE; i++) {
            d.push_front(-1);
        }
        for(size_t i=0; i<SIZE*2; i++) {
            int& a = d[i];
        }
        for(auto& x : d) {

        }
        cout << "Deque size: " << d.size() << endl;
        for(size_t i=0; i<SIZE; i++) {
            d.pop_back();
            d.pop_front();
        }
        cout << "Deque size after pop: " << d.size() << endl;
    }
    cout << endl;
}

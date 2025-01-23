#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <condition_variable>

using namespace std;

mutex traffic_mutex;
condition_variable cv;
bool greenForGroupAC = true; 

void trafficLight(const string& roadName, bool isGroupAC, int greenDuration, int yellowDuration, int redDuration) {
    while (true) {
        {
            unique_lock<mutex> lock(traffic_mutex);
            cv.wait(lock, [isGroupAC] { return greenForGroupAC == isGroupAC; });

            cout << roadName << ": GREEN light (Go!)" << endl;
            cout << roadName << ": LEFT-TURN SIGNAL (Turn left if safe!)" << endl;
            lock.unlock();
            this_thread::sleep_for(chrono::seconds(greenDuration));

            lock.lock();
            cout << roadName << ": YELLOW light (Prepare to stop)" << endl;
            lock.unlock();
            this_thread::sleep_for(chrono::seconds(yellowDuration));

            lock.lock();
            cout << roadName << ": RED light (Stop!)" << endl;
        }

        
        if ((isGroupAC && roadName == "Road C") || (!isGroupAC && roadName == "Road D")) {
            {
                lock_guard<mutex> lock(traffic_mutex);
                greenForGroupAC = !greenForGroupAC;
            }
            cv.notify_all();
        }

        this_thread::sleep_for(chrono::seconds(redDuration));
    }
}

int main() {
    const int greenDuration = 5; 
    const int yellowDuration = 2;
    const int redDuration = 7;  
    char initialRoad; 

    
    cout << "Enter the road to start with green light (A, B, C, D): ";
    cin >> initialRoad;

 
    while (initialRoad != 'A' && initialRoad != 'B' && initialRoad != 'C' && initialRoad != 'D') {
        cout << "Invalid input. Please enter A, B, C, or D: ";
        cin >> initialRoad;
    }

 
    if (initialRoad == 'A' || initialRoad == 'C') {
        greenForGroupAC = true;
    } else {
        greenForGroupAC = false;
    }

 
    thread roadA(trafficLight, "Road A", true, greenDuration, yellowDuration, redDuration);
    thread roadB(trafficLight, "Road B", false, greenDuration, yellowDuration, redDuration);
    thread roadC(trafficLight, "Road C", true, greenDuration, yellowDuration, redDuration);
    thread roadD(trafficLight, "Road D", false, greenDuration, yellowDuration, redDuration);

   
    roadA.join();
    roadB.join();
    roadC.join();
    roadD.join();

    return 0;
}

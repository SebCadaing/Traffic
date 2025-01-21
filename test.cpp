#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <condition_variable>

using namespace std;

mutex traffic_mutex;
condition_variable cv;
bool greenForAandC = true;

void trafficLight(const string& roadName, bool isGroupA, int greenDuration, int yellowDuration, int redDuration) {
    while (true) {
        {
            unique_lock<mutex> lock(traffic_mutex);
            cv.wait(lock, [isGroupA] { return greenForAandC == isGroupA; });

            cout << roadName << ": GREEN light (Go!)" << endl;
            lock.unlock();
            this_thread::sleep_for(chrono::seconds(greenDuration));
            
            lock.lock();
            cout << roadName << ": YELLOW light (Prepare to stop)" << endl;
            lock.unlock();
            this_thread::sleep_for(chrono::seconds(yellowDuration));

            lock.lock();
            cout << roadName << ": RED light (Stop!)" << endl;
        }

        if ((isGroupA && roadName == "Road C") || (!isGroupA && roadName == "Road D")) {
            {
                lock_guard<mutex> lock(traffic_mutex);
                greenForAandC = !greenForAandC;
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

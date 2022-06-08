#include <iostream>
#include <thread>
#include <condition_variable>

float temp=18.5;
float target=18;
bool heating_on=false;

std::condition_variable tempCV;
std::condition_variable targetCV;
std::mutex m;

bool stop=false;


void targetTemp(){

    while(!stop){
        std::this_thread::sleep_for (std::chrono::seconds(5));
        targetCV.wait(m);
        std::cin >> target;
        if(target==-1){
            stop=true;
        }

    }
}

void currentTemp(){
    while(!stop){

    }
}

void admin(){

}

int main() {
    std::cout << "Hello, World!" << std::endl;
    return 0;
}

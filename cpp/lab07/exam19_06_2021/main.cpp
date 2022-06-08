#include <iostream>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <thread>

std::mutex m;
std::condition_variable adminCV, adderCV;
std::queue<int> taskQueue;


int var = 0;
void admin_f(){
    std::unique_lock<std::mutex> l{m};

    var=10;
    //after initializing var wake up one adder and wait on adminCV (main will wake up the admin)
    std::cout << "admin " << std::endl;

    adderCV.notify_one();
    adminCV.wait(l);
    /*
    for(int i=0;i<3;i++){
        adderCV.wait(l);

    }*/
    std::cout << "value " << var;
}
void adder_f() {
    std::unique_lock<std::mutex> l{m};
    //if adder arrive first they wait for the admin
   // std::cout << "adder " << std::endl;
    while(var<10)
        adderCV.wait(l);

    int toAdd=rand()%5;

    if(var<15){
        var+=toAdd;
    }
    std::cout << "adder " <<  var << std::endl;
    adderCV.notify_one();
}
int main() {
    std::vector<std::thread> adders;
    std::thread admin(admin_f);
    for(int i = 0; i < 3; i++){
        srand((unsigned)time(NULL)); //makes seed different for calling rand()
        adders.emplace_back(std::thread( adder_f));
    }
    for(auto& i : adders) {
        i.join();
    }
    adminCV.notify_one();
    admin.join();
    return 0;
}

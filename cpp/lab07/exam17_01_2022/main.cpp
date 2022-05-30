#include <iostream>
#include <thread>
#include <future>
#include <vector>
#include <time.h>

int places=3;
int duration=5;

std::mutex m;
int first=0;
std::vector<std::thread> threadPool;

void leave(){
    std::unique_lock<std::mutex> l{m};
    std::cout << "Car leave" << std::endl;

    places++;
    return;

}

void arrive(){
    std::unique_lock<std::mutex> l{m};
    std::cout << "Car arrive" << std::endl;

    if(places==0){
        std::cout << "Places: " << places << std::endl;
        return;
    }
    places--;
    l.unlock();

    //wait random sec and leave
    std::this_thread::sleep_for (std::chrono::seconds(rand()%4+3));
    leave();

    return;
}



void manageCars(){
    //int start,end;
    auto start = time(nullptr);
    do{
        std::this_thread::sleep_for (std::chrono::seconds(rand()%3+1));

        threadPool.emplace_back([] {arrive();});


    }
    while((time(nullptr) - start)<duration);
}

int main(int argc,char *argv[]) {
    manageCars();

    for(int i=0;i<threadPool.size();i++){
        threadPool.at(i).join();
    }

    return 0;
}

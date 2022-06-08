#include <iostream>
#include <thread>
#include <vector>
#include <condition_variable>

#define NTASKS 25
#define BUFFER_SIZE 8
#define filename "tasks-output.txt"

using namespace std;


vector<int> buf(BUFFER_SIZE);
condition_variable canConsume;
condition_variable canProduce;
mutex m;
int size=0,consumerIndex=0;
bool stop= false;


void produces(){
    int i,last=0;
    for(i=0;i<NTASKS;i++,last++){
        unique_lock<mutex> l{m};

        while(size>=BUFFER_SIZE){
            canProduce.wait(l);
        }
        last=last%BUFFER_SIZE;

        buf[last]=i;
        if(i==NTASKS-1){
            buf[last]=-1;
        }

        size++;
        if(size==1)
            canConsume.notify_one();

    }

}

void consumer(){
    int i;

    while(!stop){
        unique_lock<mutex> l{m};
        while(size<=0){
            canConsume.wait(l);
        }
        if( buf[consumerIndex]==-1){
            stop= true;
            canConsume.notify_all();
            break;
        }
        cout << buf[consumerIndex] << endl;
        consumerIndex++;
        consumerIndex%=BUFFER_SIZE;
        size--;
        canProduce.notify_one();
       // l.unlock();
    }
    //canConsume.notify_one();
    //return;

}

int main() {
    vector<thread> consumers;

    thread p=thread(produces);

    int num=thread::hardware_concurrency();

    if(num==1)
        num++;

    for(int i=0;i<num;i++){
        consumers.emplace_back(thread(consumer));
    }

    for(int i=0;i<num;i++){
        consumers[i].join();
    }

    p.join();


    return 0;
}

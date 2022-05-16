#include "Queue.h"
#include <iostream>

Queue::Queue() {
    _size=0;
    _queue= nullptr;
}

enum Client Queue::pop() {

    enum Client val;

    if(_size==0){
        throw QueueEmptyException();
    }
    val=_queue[0];
    _size--;

    enum Client *oldClient=_queue;
    if(_size==0)
        _queue= nullptr;
    else{
        _queue= new enum Client[_size];

        for(int i=0;i<_size;i++){
            _queue[i]=oldClient[i+1];
        }
        delete[] oldClient;
    }

    return val;
}

bool Queue::empty() {

    return (_size==0);
}

void Queue::_pushAt(enum Client value, int position) {
    enum Client* oldQueue = _queue;
    ++_size;
    _queue = new enum Client[_size];
    _queue[position] = value;
    for(int i=0; i<position; i++)
        _queue[i] = oldQueue[i];
    for(int i=position+1; i<_size; i++)
        _queue[i] = oldQueue[i-1];
    delete[] oldQueue;
}

void Queue::printQueue() {
    for(int i=0; i < _size; i++)
        std::cout << i << " " << _queue[i] << std::endl;

}

int Queue::push(enum Client client) {
    int last=0,howManyMoney=0;
    switch (client) {
        case PRIORITY:
            for(int i=0;i<_size;i++){
                if(_queue[i]!=PRIORITY){
                    last=i;
                    break;
                }
            }
            _pushAt(client,last);
            break;
        case POSTAL:
            howManyMoney=0;
            last=_size;
            for(int i=0;i<_size;i++){
                if(_queue[i]==MONEY)
                    howManyMoney++;
                if(howManyMoney>3){
                    //push postal here
                    last=i;
                    break;
                }
            }
            _pushAt(client,last);
            break;
        case MONEY:
            last=_size;
            _pushAt(client,last);
            return MONEY;
    }
    return last;

}





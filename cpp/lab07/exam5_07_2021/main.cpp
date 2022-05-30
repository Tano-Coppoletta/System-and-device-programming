#include <iostream>
#include <future>


using namespace std;

void take_value(promise<int>& p){
    int value;
    cout << "Insert a number..." << endl;
    cin >> value;
    p.set_value(value);
}

void check_if_prime(future<int>& f, promise<int>& p){
    int value=f.get();
    int notPrime=0;
    for(int i=2;i<value;i++){
        if(value%i==0)
            notPrime=1;
    }
    p.set_value(notPrime);
}

void give_answer(future<int>& f){
    int res=f.get();
    if(res==0)
        cout << "The number is prime!" <<endl;
    else
        cout << "The number is NOT prime!" <<endl;

}

int main(int argc, char* argv[]) {
    promise<int> p1,p2;
    future<int> f1=p1.get_future();
    thread t1= thread(take_value,ref(p1));

    future<int> f2=p2.get_future();
    thread t2=thread(check_if_prime,ref(f1),ref(p2));

    thread t3=thread(give_answer,ref(f2));

    t1.join();
    t2.join();
    t3.join();

    return 0;
}

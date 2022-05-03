#include <iostream>
#include <thread>

void helloWorld() {
    std::cout << "Hello from Function.." << std::endl;
}

class hello {
public:
    void operator() () {
        std::cout << "Hello from callable object.." << std::endl;
    }
};

int main(int argc, char const* argv[]) {
    std::thread t_function(helloWorld);
    std::thread t_class((hello()));

    std::thread t_lambda([]() {
        std::cout << "Hello from Lambda.." << std::endl;
        });

    t_lambda.join();
    t_class.join();
    t_function.join();
    return 0;
}

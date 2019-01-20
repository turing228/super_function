#include <iostream>
#include <vector>
#include "function.h"
#include <algorithm>


std::string functionPointer(double n) {
    std::vector<double> v;

    int size = 10000;

    v.assign(size, 0);

    for (int i = 0; i < size; ++i) {
        v[i] += n;
    }

    double ans;
    for (int i = 0; i < size; ++i) {
        ans += v[i];
    }

    return "FunctionPointer is executed.";
};


struct functionObject {
    std::string operator()() const {
        return "FunctionObject is executed.";
    }
};


int main() {

    function<int(int)> functionEasy = [](int n) {
        return n;
    };
    std::cout << functionEasy(5) << std::endl;


    function<std::string()> functionFromFP = std::bind(functionPointer, 10.1);
    std::cout << functionFromFP() << std::endl;


    function<int(int)> functionFromLambda = [](int n) {
        n = n * 2 + 10;
        return n;
    };
    std::cout << functionFromLambda(45) << std::endl;


    function<std::string()> functionFromFO = functionObject();
    std::cout << functionFromFO() << std::endl;


    functionFromFP = std::move(functionFromFO);
    std::cout << functionFromFP() << std::endl;

    try {
        std::cout << functionFromFO();
    }
    catch (std::bad_function_call &e) {
        std::cout << e.what() << std::endl;
    }
}
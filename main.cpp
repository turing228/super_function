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

int a() {
    return 1;
}

double e() {
    return 2.28;
}

struct functionObject {
    std::string s1 = "FunctionObject ";
    std::string s2 = "is executed";
    std::string s3 = "lol";
    int x = 5;
    int y = 10;

    std::string operator()() const {
//        return std::to_string(x) + " " + std::to_string(y);
        return std::to_string(sizeof(x)) + " " + std::to_string(x) + " " + std::to_string(sizeof(s1)) + " " +
               std::to_string(sizeof(s2)) + " " + std::to_string(sizeof(s3)) + " " + s1 + s2;
    }
};


int main() {

    function<int()> fa(a);
    std::cout << fa();
    fa = e;
    std::cout << fa();
    std::cout << e();

    function<std::string()> functionFromFOstd(functionObject());
    std::cout << functionFromFOstd << std::endl;
    function<std::string()> functionFromFO = functionObject();
    std::cout << functionFromFO() << " " << sizeof(functionFromFO) << std::endl;


    srand(time(0));

    //std::vector<int> x(100000, 1);
    function<int(int)> functionEasy = [](int n) {
        std::vector<int> x(100000, 1);
        for (int &i : x) {
            i = rand() % 100;
        }
        int t = 0;
        for (int i : x) {
            t += i;
        }
        return t;
    };
    std::cout << functionEasy(5) << std::endl;
    std::cout << functionEasy(5) << std::endl;

    std::cout << sizeof(functionEasy) << std::endl;
    std::cout << sizeof(functionObject) << std::endl;


    function<std::string()> functionFromFP = std::bind(functionPointer, 10.1);
    std::cout << functionFromFP() << std::endl;


    function<std::string(int)> functionFromLambda = [](int n) {
        n = n * 2 + 10;
        return "FunctionFromLamda executed " + std::to_string(n);
    };
    std::cout << functionFromLambda(45) << std::endl;


//    function<std::string()> functionFromFO = functionObject();
//    std::cout << functionFromFO() << std::endl;



    std::cout << std::endl << "SIZES!!!" << std::endl;
    std::cout << sizeof(functionEasy) << std::endl;
    std::cout << sizeof(functionFromFP) << std::endl;
    std::cout << sizeof(functionFromLambda) << std::endl;
    std::cout << sizeof(functionFromFO) << std::endl;
    std::cout << std::endl;


    functionFromFP = std::move(functionFromFO);
    std::cout << functionFromFP() << std::endl;

    try {
        std::cout << functionFromFO();
    }
    catch (std::bad_function_call &e) {
        std::cout << e.what() << std::endl;
    }
}
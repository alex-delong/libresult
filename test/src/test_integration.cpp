// an example to illustrate error propagation using Result
#include <libresult.hpp>
#include <iostream>
#include <assert.h>
#include <bits/stdc++.h>
#include <exception>
#include <libexception.hpp>
#include <cmath>
using namespace std;

struct DivByZero : public Exception {
    DivByZero() : Exception("Division by zero", "") {}
    DivByZero(const char* l) : Exception("Division by zero", l) {}
    DivByZero(const Exception& other) : Exception("Division by zero", other.where()) {} 
};
struct NegativeLog : public Exception {
    NegativeLog() : Exception("Negative logarithm", "") {}
    NegativeLog(const char* l) : Exception("Negative logarithm", l) {}
    NegativeLog(const Exception& other) : Exception("Negative logarithm", other.where()) {}
};
// we presume the user floatends to work with real numbers only
struct NegativeRoot : public Exception {
    NegativeRoot() : Exception("Negative root", "") {}
    NegativeRoot(const char* l) : Exception("Negative root", l) {}
    NegativeRoot(const Exception& other) : Exception("Negative root", other.where()) {}
};
struct RecievedErr : public Exception {
    RecievedErr() : Exception("Recieved Err value", "") {}
    RecievedErr(const char* l) : Exception("Recieved Err value", l) {}
    RecievedErr(const Exception& other) : Exception("Recieved Err value", other.where()) {}
};

Result<float, Exception>& divide(Result<float, Exception>& a, Result<float, Exception>& b) {
    if (b.is_ok() && a.is_ok()) {
        if (b.unwrap() != 0) {
            Result<float, Exception>& c = *(new Ok<float, Exception>(a.unwrap() / b.unwrap()));
            c.push_back(b);
            c.push_back(a);
            return c;
        } else {
            Result<float, Exception>& c = *(new Err<float, Exception>(new DivByZero("divide")));
            c.push_back(b);
            c.push_back(a);
            return c;
        }
    } else if (a.is_err() && b.is_err()) {
        Result<float, Exception>& c = *(new Err<float, Exception>(new RecievedErr("divide")));
        c.push_back(b);
        c.push_back(a);
        return c;
    } else if (a.is_err()) {       
        Result<float, Exception>& c = *(new Err<float, Exception>(new RecievedErr("divide")));
        c.push_back(a);
        return c;
    } else {
        Result<float, Exception>& c = *(new Err<float, Exception>(new RecievedErr("divide")));
        c.push_back(b);
        return c;
    }
}
Result<float, Exception>& nat_log(Result<float, Exception>& a) {
    if (a.is_err()) {
        Result<float, Exception>& b = *(new Err<float, Exception>(new RecievedErr("nat_log")));
        b.push_back(a);
        return b;
    } else if (a.unwrap() < 0) {
        Err<float, Exception>& b = *(new Err<float, Exception>(new NegativeLog("nat_log")));
        b.push_back(a);
        return b;
    } else {
        Ok<float, Exception>& b = *(new Ok<float, Exception>(log(a.unwrap())));
        b.push_back(a);
        return b;
    } 
}
Result<float, Exception>& square_rt(Result<float, Exception>& a) {
    if (a.is_err()) {
        Result<float, Exception>& b = *(new Err<float, Exception>(new RecievedErr("square_rt")));
        b.push_back(a);
        return b;
    } else if (a.unwrap() < 0) {
        Err<float, Exception>& b = *(new Err<float, Exception>(new NegativeRoot("square_rt")));
        b.push_back(a);
        return b;
    } else {
        Ok<float, Exception>& b = *(new Ok<float, Exception>(sqrt(a.unwrap()))); 
        b.push_back(a);
        return b;
    }
}

int main() {
    Result<float, Exception>& a = *(new Ok<float, Exception>(10));
    Result<float, Exception>& b = *(new Ok<float, Exception>(0.0));
    Result<float, Exception>& c = square_rt(nat_log(divide(a, b)));
    if (c.is_err()) {
        cout << "exception in main: " << endl;
        c.get_trace();
    } else {
        cout << c.unwrap() << endl;
    }
    delete &c;
    cout << "end of example program." << endl;
}

// The result monad allows us to capture errors inside a wrapper
// We can use it to propagate errors
#include <libresult.hpp>
#include <iostream>
#include <exception>
#include <assert.h>
#include <bits/stdc++.h>
#include <limits>
#include <cstdlib>
#include <ctime>

struct TestOk {
    static void constructor() {
        using namespace std;
        cout << "Ok::Ok(T).. ";
        try {
            Ok<int, exception> a = Ok<int, exception>(0); 
            Ok<int, exception>(INT_MIN);
            Ok<int, exception>(INT_MAX);
            srand(time(nullptr));
            for (int i = 0; i < 1000; i++) {
                Ok<int, exception> j = Ok<int, exception>(rand());
                Ok<int, exception> k = Ok<int, exception>(-rand());
            }
            float f = numeric_limits<float>::infinity();
            Ok<float, exception> ok_f = Ok<float, exception>(std::move(f));
            ok_f = Ok<float, exception>(-f);
            Ok<float, exception>(0.0);
            for (int i = 0; i < 1000; i++) {
                Ok<float, exception> j = Ok<float, exception>(rand());
                Ok<float, exception> k = Ok<float, exception>(-rand());
            }
            Ok<double, exception>(0.0);
            double d =  numeric_limits<double>::infinity();
            Ok<double, exception> x = Ok<double, exception>(std::move(d));
            Ok<double, exception> y = Ok<double, exception>(-d);
            for (int i = 0; i < 1000; i++) {
                Ok<double, exception> j = Ok<double, exception>(rand());
                Ok<double, exception> k = Ok<double, exception>(-rand());
            }
            Ok<string, exception>("foo");
            Ok<const char*, exception>("bar");
            Ok<const char*, exception>();
            Ok<char, exception>('a');
            Ok<Ok<int, exception>, exception> ok_ok_i = Ok<Ok<int, exception>, exception>(Ok<int, exception>(5));
        } catch (exception& e) {
            cout << "failed!" << endl;
            return;
        }
        cout << "passed!" << endl;
    }
    static void unwrap() {
        using namespace std;
        cout << "Ok::unwrap().. ";
        try {
            Result<int, exception>* foo = new Ok<int, exception>(1);
            assert(foo->unwrap() == 1);
            assert(foo->unwrap() == 1);
            delete foo;
            int i = Ok<int, exception>(7).unwrap();
            assert(i == 7);
            i = Ok<int, exception>(INT_MAX).unwrap();
            assert(i == INT_MAX);
            i = Ok<int, exception>(INT_MIN).unwrap();
            assert(i == INT_MIN);
            string s = Ok<string, exception>("foo").unwrap();
            assert(s == "foo");
        } catch (exception& e) {
            cout << "failed! " << e.what() << endl;
            return;
        } cout << "passed!" << endl;
    }
    static void expect() {
        using namespace std;
        cout << "Ok::expect(string).. ";
        try {
            Result<int, exception>* foo = new Ok<int, exception>(1);
            assert(foo->expect("foo") == 1);
            delete foo;
        } catch (exception& e) {
            cout << "failed! " << e.what() << endl;
            return;
        } cout << "passed!" << endl;
    }
    static void is_ok() {
        using namespace std;
        cout << "Ok::is_ok().. ";
        try {
            Result<int, exception>* foo = new Ok<int, exception>(1);
            assert(foo->is_ok());
            delete foo;
        } catch (exception& e) {
            cout << "failed! " << e.what() << endl;
            return;
        } cout << "passed!" << endl;
    }
    static void is_err() {
        using namespace std;
        try {
            cout << "Ok::is_err().. ";
            Result<int, exception>* foo = new Ok<int, exception>(1);
            assert(!foo->is_err());
            delete foo;
        } catch (exception& e) {
            cout << "failed! " << e.what() << endl;
            return;
        } cout << "passed!" << endl;
    }
    static void assignment() {
        using namespace std;
        cout << "Ok::operator=().. ";
        try {
            Ok<int, exception> ok_i(3); // = Ok<int, exception>(3);
            ok_i = Ok<int, exception>(5);
            ok_i = Ok<int, exception>(0);
            ok_i = Ok<int, exception>(-1);
            ok_i = ok_i;
            ok_i = Ok<int, exception>(INT_MAX);
            ok_i = Ok<int, exception>(INT_MIN);
            Ok<float, exception> ok_f(3); // = Ok<float, exception>(3.1);
            ok_f = Ok<float, exception>(7.2);
            ok_f = Ok<float, exception>(33.13);
            ok_f = Ok<float, exception>(33.13);
            ok_f = Ok<float, exception>(3.13);
            ok_f = Ok<float, exception>(343.234);
            ok_f = ok_f;
            ok_f = Ok<float, exception>(0.0);
            ok_f = Ok<float, exception>(-100.0);
            ok_f = Ok<float, exception>(-1.00003);
            ok_f = Ok<float, exception>(23423421.1);
        } catch (exception& e) {
            cout << "failed! " << e.what() << endl;
            return;
        } 
        cout << "passed!" << endl;
    }
    static void all() {
        constructor();
        unwrap();
        expect();
        is_ok();
        is_err();
        assignment();
    }
};
struct TestErr {
    static void constructor() {
        using namespace std;
        cout << "Err::Err(E).. ";
        Err<int, exception> e = exception();
        cout << "passed!" << endl;
    }
    static void unwrap() {
        using namespace std;
        cout << "Err::unwrap().. ";
        Result<int, exception>* a = new Err<int, exception>(exception());
        try {
            a->unwrap();
            cout << "failed!" << endl;
        } catch (exception& e) {
            cout << "passed!" << endl;
        }
        delete a;
    }
    static void expect() {
        using namespace std;
        cout << "Err::expect(string).. ";
        Err<int, exception> a = exception();
        try {
            a.expect("passed!");
            cout << "failed!" << endl;
        } catch (exception& e) {}
    }
    static void is_ok() {
        using namespace std;
        cout << "Err::is_ok().. ";
        Err<int, exception> e = exception();
        try {
            assert(!e.is_ok());
            assert(e.is_ok() == false);
        } catch (exception& ee) {
            cout << "failed!" << ee.what() << endl;
        } cout << "passed!" << endl;
    }
    static void is_err() {
        using namespace std;
        cout << "Err::is_err()..";
        Err<int, exception> e = exception();
        try {
            assert(e.is_err() == true);
            assert(e.is_err());
        } catch (exception& ee) {
            cout << "failed! " << ee.what() << endl;
        } cout << "passed!" << endl;
    }
    static void assignment() {
        using namespace std;
        try {
            cout << "Err::operator=().. ";
            Err<int, exception> e = new exception();
            e = new out_of_range("");
            assert(strcmp(e.what(), out_of_range("").what()) == 0);
            e = new logic_error("asdf");
            assert(strcmp(e.what(), logic_error("asdf").what()) == 0);
            e = e;
            e = new length_error("exdgr");
            assert(strcmp(e.what(), length_error("exdgr").what()) == 0);
            e = new invalid_argument("");
            assert(strcmp(e.what(), invalid_argument("").what()) == 0);
            e = new domain_error("jklj");
            assert(strcmp(e.what(), domain_error("jklj").what()) == 0);
        } catch (exception& e) {
            cout << "failed! " << e.what() << endl;
        }
        cout << "passed!" << endl;
    }
    static void all() {
        constructor();
        unwrap();
        expect();
        is_ok();
        is_err();
        assignment();
    }
};

int main() {
    using namespace std;
    cout << "beginning Ok unit test: " << endl;
    TestOk::all();
    cout << "beginning Err unit test: " << endl;
    TestErr::all();
    cout << "All tests complete!" << endl;
    //string* s_ptr = malloc(sizeof(string));
}

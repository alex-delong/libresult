// an implementation of the 'Result' monad from Rust
#include <iostream>
#include <libresult.hpp>
using namespace std;

struct ResultBase::Impl {
    void* v; 
    void* unwrap() {
        return v;
    }
    void set(void* const& v) {
        this->v = v;
    }
    ~Impl() {}//{cout << "Impl destructor called for v @ " << v << endl;}
};

void* ResultBase::unwrap() const {
    return pimpl->unwrap();
}
void ResultBase::set(void* const& v) {
    pimpl->set(v);
}
ResultBase::ResultBase(void* const& v) : pimpl(new Impl) {
    //cout << "ResultBase constructor called for pimpl @ " << pimpl << endl;
    pimpl->set(v);
}
ResultBase::~ResultBase() {
    //cout << "ResultBase destructor called for pimpl @ " << pimpl << endl;
    delete pimpl;
}


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
    ~Impl() {}
};

void* ResultBase::unwrap() const {
    return pimpl->unwrap();
}
void ResultBase::set(void* const& v) {
    pimpl->set(v);
}
ResultBase::ResultBase(void* const& v) : pimpl(new Impl) {
    pimpl->set(v);
}
ResultBase::~ResultBase() {
    delete pimpl;
}


#include <iostream>
#include <stdexcept>
#include <utility>
#include <memory>
#include <assert.h>
#include <cstring>
#include <cstdlib>
class ResultBase;
template<class T, class E> class Result;
template<class T, class E> class Ok;
template<class T, class E> class Err;

class ResultBase {
    struct Impl;
    Impl* pimpl;
  protected:
    void* unwrap() const;
    void set(void* const&);
  public:
    ResultBase(void* const&);
    ~ResultBase();
};

// dummy class that resolves to either an Ok or an Err
// T and E must have a copy constructor
template<class T, class E> class Result : protected ResultBase {
  protected:
    Result* next;
  public:
    Result<T, E>(void* const& v) : next(nullptr), ResultBase(v) {}//{cout << "Result constructor called for v @ " << v << endl;}
    virtual T& unwrap() const = 0;
    virtual T& expect(std::string s) const = 0;
    void push_back(Result& r) {
        if (next == nullptr) {
            next = &r;
        } else {
            next->push_back(r);
        }
    };
    void push_back(const T& t) {
        if (next == nullptr) {
            next = new Ok<T, E>(t);
        } else {
            next->push_back(t);
        }
    };
    void push_back(E e) {
        if (next == nullptr) {
            next = new Err<T, E>(e);
        } else {
            next->push_back(e);
        }
    };
    void get_trace() {
        if (this->is_err()) {
            Err<T, E>* e_ptr = static_cast<Err<T, E>*>(this);
            std::cout << e_ptr->what() << std::endl;
        } else {
            std::cout << this->unwrap() << std::endl;
        }
        if (next != nullptr) {
            next->get_trace();
        }
    }
    virtual bool is_ok() const = 0;
    virtual bool is_err() const = 0;
    virtual ~Result<T, E>() {
        delete next;
    }
};
template<class T, class E> class Ok : public Result<T, E> { 
    T* move_out() { 
        T* this_t_ptr = static_cast<T*>(ResultBase::unwrap());
        T* new_t_ptr = new T;
        std::swap(*new_t_ptr, *this_t_ptr);
        return new_t_ptr;
    }
    T& get_wrapped() const {
        return *static_cast<T*>(ResultBase::unwrap());
    }
  public:
    Ok() : Result<T, E>(new T) {}
    Ok(const Ok& other_ok) : Result<T, E>(other_ok.get_wrapped()) {}
    Ok(const T& other_t) : Result<T, E>(new T(other_t)) {}
    Ok(T* other_t_ptr) : Result<T, E>(other_t_ptr) {}
    Ok(Ok&& other_ok) : Result<T, E>(other_ok.move_out()) {}
    Ok(T&& other_t) : Result<T, E>(new T(std::move(other_t))) {}
    T& unwrap() const final {
        return get_wrapped(); 
    }
    T& expect(std::string) const final {
        return get_wrapped();
    }
    bool is_ok() const final {
        return true;
    }
    bool is_err() const final {
        return false;
    }
    ~Ok() override {
        T* t_ptr = static_cast<T*>(ResultBase::unwrap());
        delete t_ptr;
    }
    Ok& operator=(const T& other_t) {
        T* this_t_ptr = static_cast<T*>(ResultBase::unwrap());
        if (&other_t == this_t_ptr) {
            return *this;
        }
        *this_t_ptr = other_t;
        return *this;
    }
    Ok& operator=(const Ok& other_ok) {
        if (&other_ok == this) {
            return *this;
        }
        T* this_t_ptr = static_cast<T*>(ResultBase::unwrap());
        *this_t_ptr = other_ok.get_wrapped();
        return *this;
    }
    Ok& operator=(T* other_t_ptr) {
        T* this_t_ptr = static_cast<T*>(ResultBase::unwrap());
        if (other_t_ptr == this_t_ptr) {
            return *this;
        }
        delete this_t_ptr;
        ResultBase::set(other_t_ptr);
        return *this;
    }
    Ok& operator=(Ok<T, E>&& other_ok) {
        if (&other_ok == this) {
            return *this;
        }
        T* other_t_ptr = other_ok.move_out();
        T* this_t_ptr = static_cast<T*>(ResultBase::unwrap());
        delete this_t_ptr;
        ResultBase::set(other_t_ptr);
        return *this;
    }
};
template<class T, class E> class Err : public Result<T, E> {
    // moves exception out of this and returns it
    E* move_out() {
        E* this_e_ptr = static_cast<E*>(ResultBase::unwrap());
        E* new_e_ptr = new E;
        std::swap(*this_e_ptr, *new_e_ptr);
        return new_e_ptr;        
    }
    E& get_wrapped() const {
        return *static_cast<E*>(ResultBase::unwrap());
    }
  public:
    Err() : Result<T, E>(new E) {}
    Err(const Err& other_err) : Result<T, E>(new E(other_err.get_wrapped())) {}
    Err(const E& other_e) : Result<T, E>(new E(other_e)) {}
    Err(E* other_e_ptr) : Result<T, E>(other_e_ptr) {}
    Err(Err&& err) : Result<T, E>(err.move_out()) {}
    Err(E&& other_e) : Result<T, E>(new E(std::move(other_e))) {}
    T& unwrap() const final {
        std::cout << "throwing unwrapped Err " << this->what() << std::endl;
        throw get_wrapped();
    }
    T& expect(std::string s) const final {
        std::cout << "throwing unwrapped Err " << this->what() << ": " << s << std::endl;
        throw get_wrapped();
    }
    bool is_ok() const final {
        return false;
    }
    bool is_err() const final {
        return true;
    }
    const char* what() const {
        E* e_ptr = static_cast<E*>(ResultBase::unwrap());
        return e_ptr->what();
    }
    const char* where() const {
        E* e_ptr = static_cast<E*>(ResultBase::unwrap());
        return e_ptr->where();
    }
    ~Err() override {
        E* this_e_ptr = static_cast<E*>(ResultBase::unwrap());
        delete this_e_ptr;
    }
    Err& operator=(const E& other_e) {
        E* this_e_ptr = static_cast<E*>(ResultBase::unwrap());
        if (&other_e == this_e_ptr) {
            return *this;
        }
        *this_e_ptr = other_e;
        return *this;
    }
    Err& operator=(const Err& other_err) {
        if (&other_err == this) {
            return *this;
        }
        E* this_e_ptr = static_cast<E*>(ResultBase::unwrap());
        *this_e_ptr = other_err.get_wrapped();
        return *this;
    }
    Err& operator=(E* other_e_ptr) {
        E* this_e_ptr = static_cast<E*>(ResultBase::unwrap());
        if (other_e_ptr == this_e_ptr) {
            return *this;
        }
        delete this_e_ptr;
        ResultBase::set(other_e_ptr);
        return *this;
    }
    Err& operator=(E&& other_e) {
        E* this_e_ptr = static_cast<E*>(ResultBase::unwrap());
        if (&other_e == this_e_ptr) {
            return *this;
        }
        delete this_e_ptr;
        ResultBase::set(this->move_in(other_e));
        return *this;
    }
    Err& operator=(Err&& other) {
        if (&other == this) {
            return *this;
        }
        E* other_e_ptr = other.move_out();
        E* this_e_ptr = static_cast<E*>(ResultBase::unwrap());
        delete this_e_ptr;
        ResultBase::set(other_e_ptr);
        return *this;
    }
};



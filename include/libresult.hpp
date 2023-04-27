#include <iostream>
#include <stdexcept>
#include <utility>
#include <memory>
#include <assert.h>
#include <cstring>
#include <cstdlib>
// TODO: rename variables such as 't' and 't' to 'other_t' and 'other_e' to achieve consistency
namespace LibResult {
    class ResultBase;
    template<class T, class E> class Result;
    template<class T, class E> class Ok;
    template<class T, class E> class Err;

    // Base class that stores a generic pointer to the value passed to Result
    class ResultBase {
        struct Impl;
        Impl* pimpl;
      protected:
        // returns the generic pointer to the stored value
        // pre-conditions:
            // ResultBase has been constructed and 
            // stores a valid pointer to a T or E value
        // post-conditions:
            // the generic pointer to the stored value has been returned
        void* unwrap() const;

        // sets the stored generic pointer to v
        // pre-conditions:
            // v is a valid pointer to a T or E value
        // post-conditions:
            // the stored generic pointer has been set to v
        void set(void* const& v);
      public:
        // copy constructor
        // pre-conditions:
            // v is a valid generic pointer to a T or E value
        // post-conditions:
            // pimpl has been allocated
            // the stored generic pointer has been set to v
        ResultBase(void* const& v);

        // pre-conditions:
            // this->pimpl must be delete safe
        // post-conditions
            // this->pimpl has been deleted
        ~ResultBase();
    };

    // abstract base class that resolves to either an Ok or an Err
    template<class T, class E> class Result : protected ResultBase {
      protected:
        // a pointer to the next Result in the trace
        Result* next;
      public:
        // copy constructor:
        // pre-conditions:
            // v is a valid pointer to a T or E object
        // post-conditions:
            // ResultBase has been constructed with v
        Result<T, E>(void* const& v) : next(nullptr), ResultBase(v) {}
        
        // returns the held T or throws the held E
        // pre-conditions:
            // ResultBase is holding a valid pointer to a T or E
        // post-conditions:
            // the held value is returned if Ok or thrown if Err
        virtual T& unwrap() const = 0; // TODO: return as a copy

        // returns the held T or prints the error message s before throwing E
        // pre-conditions:
            // ResultBase is holding a valid pointer to a T or E
        // post-conditions:
            // the held value is returned if Ok or thrown if Err
        virtual T& expect(std::string s) const = 0; // TODO: return as a copy

        // stores the argument at the tail of the list
        // pre-conditions:
            // next is nullptr or holds a valid pointer to a Result
            // r is a valid reference to a Result and has been allocated with new
        // post-conditions:
            // r has been pushed to the tail
        void push_back(Result& r) {
            if (next == nullptr) {
                next = &r;
            } else {
                next->push_back(r);
            }
        };

        // newly allocates an Ok(t) and stores it to the tail of the list
        // pre-conditions:
            // next is nullptr or holds a valid pointer to a Result
            // t is copy constructable
        // post-conditions:
            // new Ok(t) has been pushed back to the tail
        void push_back(const T& t) {
            if (next == nullptr) {
                next = new Ok<T, E>(t);
            } else {
                next->push_back(t);
            }
        };

        // newly allocates an Err(e) and stores it to the tail of the list
        // pre-conditions:
            // next is nullptr or holds a valid pointer to a Result
            // e is copy constructable
        // post-conditions:
            // new Err(e) has been pushed back to the tail
        void push_back(E e) { // TODO: pass in by reference
            if (next == nullptr) {
                next = new Err<T, E>(e);
            } else {
                next->push_back(e);
            }
        };

        // prints a trace for the linked list where this is the head
        // pre-conditions:
            // this is holding either a T or E value
            // if holding E, then what() must be a method of E
            // if holding T, then T must have a "<<" operation
            // next must be nullptr or hold a valid Result pointer
        // post-conditions:
            // a trace has been printed to stdout
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

        // checks if this points to an Ok
        // pre-conditions:
            // this must point to a constructed Ok or Err
        // post-conditions:
            // if Ok, then true has been returned
            // else, false has been returned
        virtual bool is_ok() const = 0;

        // checks if this points to an Err
        // pre-conditions:
            // this must point to a constructed Ok or Err
        // post-conditions:
            // if Err, then true has been returned
            // else, false has been returned
        virtual bool is_err() const = 0;

        // pre-conditions:
            // this->next is a delete-safe pointer
        // post-conditions:
            // this->next has been deleted
        virtual ~Result<T, E>() {
            delete next;
        }
    };
    template<class T, class E> class Ok : public Result<T, E> { 
        // moves T out of this and leaves this holding a default constructed T
        // pre-conditions:
            // this is holding a valid T allocated with new
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
}



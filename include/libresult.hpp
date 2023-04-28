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

        // TODO: add a pure virtual get_wrapped()
        // TODO: add a pure virtual move_out()
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
        // post-conditions:
            // a copy of the held T has been returned 
            // this holds a default constructed T
        T* move_out() { // TODO: remove this method in favor of std::move(this->get_wrapped())
            T* this_t_ptr = static_cast<T*>(ResultBase::unwrap());
            T* new_t_ptr = new T;
            std::swap(*new_t_ptr, *this_t_ptr);
            return new_t_ptr;
        }

        // returns the held T value
        // pre-conditions:
            // ResultBase is holding a valid T ptr
        // post-conditions:
            // the held T value has been returned
        T& get_wrapped() const { // TODO: return by value
            return *static_cast<T*>(ResultBase::unwrap());
        }
      public:
        // default constructor:
        // pre-conditions:
            // T is default constructable
        // post-conditions:
            // this has been constructed with a new T
        Ok() : Result<T, E>(new T) {}

        // copy constructor
        // pre-conditions:
            // the argument is a constructed Ok
        // post-conditions:
            // this->get_wrapped() is a new-allocated copy of the T held by the argument
        Ok(const Ok& other_ok) : Result<T, E>(other_ok.get_wrapped()) {} // TODO: change argument to "new T(other_ok.get_wrapped())"
        
        // copy constructor
        // pre-conditions:
            // the argument is a constructed T
        // post-conditions:
            // this->get_wrapped() is a new-allocated copy of the argument
        Ok(const T& other_t) : Result<T, E>(new T(other_t)) {}
        
        // move semantics:

        // pre-conditions:
            // the argument is a new-allocated pointer to a T
        // post-conditions:
            // this has taken ownership of the argument
        Ok(T* other_t_ptr) : Result<T, E>(other_t_ptr) {}
        
        // pre-conditions:
            // the argument is a constructed Ok
        // post-conditions:
            // this wrapped value is a new-allocated move of the argument's wrapped value
        Ok(Ok&& other_ok) : Result<T, E>(other_ok.move_out()) {} // TODO: use std::forward instead
        
        // pre-conditions:
            // argument is a constructed T
        // post-conditions:
            // this wrapped value is a new-allocated std::move of the argument
        Ok(T&& other_t) : Result<T, E>(new T(std::move(other_t))) {}

        // returns the wrapped value
        // pre-conditions:
            // this wrapped value is a constructed T
        // post-conditions:
            // this wrapped value has been returned
        T& unwrap() const final { // TODO: return by value
            return get_wrapped(); 
        }

        // equivalent to Ok::unwrap()
        // pre-conditions:
            // argument is a constructed std::string
            // this wrapped value is a constructed T
        // post-conditions:
            // this wrapped value has been returned
        T& expect(std::string) const final { // TODO: return by value
            return get_wrapped();
        }

        // checks if this points to an Ok (used by Results)
        bool is_ok() const final {
            return true;
        }

        // checks if this points to an Err (used by Results)
        bool is_err() const final {
            return false;
        }

        // pre-conditions:
            // this wrapped value is new-allocated
        // post-conditions:
            // this wrapped value is deleted 
        ~Ok() override {
            T* t_ptr = static_cast<T*>(ResultBase::unwrap());
            delete t_ptr;
        }

        // copy assignment
        // pre-conditions:
            // argument is a constructed T
            // T is copy assignable
            // this is constructed 
        // post-conditions:
            // this wrapped value == argument
        Ok& operator=(const T& other_t) {
            T* this_t_ptr = static_cast<T*>(ResultBase::unwrap());
            if (&other_t == this_t_ptr) {
                return *this;
            }
            *this_t_ptr = other_t;
            return *this;
        }

        // copy assignment
        // pre-conditions:
            // argument is constructed
            // T is copy assignable
            // this is constructed
        // post-conditions:
            // this wrapped value is a copy of the argument's wrapped value
        Ok& operator=(const Ok& other_ok) {
            if (&other_ok == this) {
                return *this;
            }
            T* this_t_ptr = static_cast<T*>(ResultBase::unwrap());
            *this_t_ptr = other_ok.get_wrapped();
            return *this;
        }

        // pre-conditions:
            // argument is a valid ptr to a new-allocated T
            // this is constructed
        // post-conditions:
            // this wrapped ptr is deleted and replaced with argument
        Ok& operator=(T* other_t_ptr) {
            T* this_t_ptr = static_cast<T*>(ResultBase::unwrap());
            if (other_t_ptr == this_t_ptr) {
                return *this;
            }
            delete this_t_ptr;
            ResultBase::set(other_t_ptr);
            return *this;
        }

        // TODO: add move semantics for other_t

        // pre-conditions:
            // argument is constructed
            // this is constructed
        // post-conditions:
            // this wrapped value is a std::move of argument's wrapped value
        Ok& operator=(Ok<T, E>&& other_ok) { // TODO: replace move_out() with std::move(other_ok.get_wraped())
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
        // moves E out of this and leaves this holding a default constructed T
        // pre-conditions:
            // this is holding a valid E allocated with new
        // post-conditions:
            // a copy of the held E has been returned 
            // this holds a default constructed E
        E* move_out() { // TODO: remove this method in favor of std::move(this->get_wrapped())
            E* this_e_ptr = static_cast<E*>(ResultBase::unwrap());
            E* new_e_ptr = new E;
            std::swap(*this_e_ptr, *new_e_ptr);
            return new_e_ptr;        
        }
        
        // returns the held E value
        // pre-conditions:
            // ResultBase is holding a valid E ptr
        // post-conditions:
            // the held E value has been returned
        E& get_wrapped() const {
            return *static_cast<E*>(ResultBase::unwrap());
        }
      public:
        // default constructor:
        // pre-conditions:
            // E is default constructable
        // post-conditions:
            // this has been constructed with a new E
        Err() : Result<T, E>(new E) {}
        
        // copy constructor
        // pre-conditions:
            // the argument is a constructed Err
        // post-conditions:
            // this->get_wrapped() is a new-allocated copy of the E held by the argument
        Err(const Err& other_err) : Result<T, E>(new E(other_err.get_wrapped())) {}
 
        // copy constructor
        // pre-conditions:
            // the argument is a constructed E
        // post-conditions:
            // this->get_wrapped() is a new-allocated copy of the argument
        Err(const E& other_e) : Result<T, E>(new E(other_e)) {}
        
        // move semantics:

        // pre-conditions:
            // the argument is a new-allocated pointer to an E
        // post-conditions:
            // this has taken ownership of the argument
        Err(E* other_e_ptr) : Result<T, E>(other_e_ptr) {}
        
        // pre-conditions:
            // the argument is a constructed Err
        // post-conditions:
            // this wrapped value is a new-allocated move of the argument's wrapped value 
        Err(Err&& err) : Result<T, E>(err.move_out()) {} // TODO: replace err.move_out() with std::move(err.get_wrapped())
        
        // pre-conditions:
            // argument is a constructed E
        // post-conditions:
            // this wrapped value is a new-allocated std::move of the argument 
        Err(E&& other_e) : Result<T, E>(new E(std::move(other_e))) {}
        
        // throws the wrapped value
        // pre-conditions:
            // this wrapped value is a constructed E
        // post-conditions:
            // this wrapped value has been thrown
        T& unwrap() const final { // TODO: return by value
            std::cout << "throwing unwrapped Err " << this->what() << std::endl;
            throw get_wrapped();
        }
 
        // the same thing as this->unwrap() but also prints the argument to stdout
        // pre-conditions:
            // argument is a constructed std::string
            // this wrapped value is a constructed E
        // post-conditions:
            // this wrapped value has been thrown and s has been printed to stdout
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

        // returns E::what() for the wrapped E
        // pre-conditions:
            // this wrapped E is constructed and has a what() method that returns a cstring
        // post-conditions:
            // for this wrapped E, E::what() has been returned
        const char* what() const {
            E* e_ptr = static_cast<E*>(ResultBase::unwrap());
            return e_ptr->what();
        }
        
        // returns E::where() for the wrapped E (this is meant to be used with LibException)
        // pre-conditions:
            // this wrapped E is constructed and has a where() method that returns a cstring
        // post-conditions:
            // for this wrapped E, E::where() has been returned
        const char* where() const {
            E* e_ptr = static_cast<E*>(ResultBase::unwrap());
            return e_ptr->where();
        }

        // pre-conditions:
            // this wrapped value is new-allocated
        // post-conditions:
            // this wrapped value is deleted 
        ~Err() override {
            E* this_e_ptr = static_cast<E*>(ResultBase::unwrap());
            delete this_e_ptr;
        }

        // copy assignment
        // pre-conditions:
            // argument is a constructed E
            // E is copy assignable
            // this is constructed 
        // post-conditions:
            // this wrapped value == argument
        Err& operator=(const E& other_e) {
            E* this_e_ptr = static_cast<E*>(ResultBase::unwrap());
            if (&other_e == this_e_ptr) {
                return *this;
            }
            *this_e_ptr = other_e;
            return *this;
        }

        // copy assignment
        // pre-conditions:
            // argument is constructed
            // E is copy assignable
            // this is constructed
        // post-conditions:
            // this wrapped value is a copy of the argument's wrapped value 
        Err& operator=(const Err& other_err) {
            if (&other_err == this) {
                return *this;
            }
            E* this_e_ptr = static_cast<E*>(ResultBase::unwrap());
            *this_e_ptr = other_err.get_wrapped();
            return *this;
        }

        // pre-conditions:
            // argument is a valid ptr to a new-allocated E
            // this is constructed
        // post-conditions:
            // this wrapped ptr is deleted and replaced with argument
        Err& operator=(E* other_e_ptr) {
            E* this_e_ptr = static_cast<E*>(ResultBase::unwrap());
            if (other_e_ptr == this_e_ptr) {
                return *this;
            }
            delete this_e_ptr;
            ResultBase::set(other_e_ptr);
            return *this;
        }

        // pre-conditions:
            // argument is constructed
            // this is constructed
        // post-conditions:
            // this wrapped value is a std::move of the argument
        Err& operator=(E&& other_e) {
            E* this_e_ptr = static_cast<E*>(ResultBase::unwrap());
            if (&other_e == this_e_ptr) {
                return *this;
            }
            delete this_e_ptr;
            ResultBase::set(this->move_in(other_e));
            return *this;
        }

        // pre-conditions:
            // argument is constructed
            // this is constructed
        // post-conditions:
            // this wrapped value is a std::move of argument's wrapped value
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



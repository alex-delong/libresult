#include <libexception.hpp>
using namespace LibException;
// sets the location and message_location variables
// pre-conditions:
    // l is a valid cstring
    // this->location is delete[] safe
    // this->message is a valid cstring
    // this->message_location is delete[] safe
// post-conditions:
    // l has been copied to this->location
    // if location != "" then message_location holds message + " in " + location
    // else message_location == message
void Exception::set_location(const char* l) {
    delete[] this->location;
    this->location = new char[strlen(l) + 1];
    strcpy(this->location, l);
    
    delete[] this->message_location;
    if (strcmp(this->location, "") == 0) {
        this->message_location = new char[strlen(message) + 1];
        strcpy(message_location, message);
    } else {
        this->message_location = new char[strlen(message) + strlen(l) + 5];
        strcpy(this->message_location, this->message);
        strcat(this->message_location, " in ");
        strcat(this->message_location, this->location);
    }
}

// copy constructors:

// pre-conditions:
    // m and l are non-null ptrs 
// post-conditions:
    // the contents of m has been copied to this->message
    // the contents of l has been copied to this->location
Exception::Exception(const char* m, const char* l) : message(m) {
    this->set_location(l);    
};
// pre-conditions:
    // other is constructed or otherwise has a non-null location
// post-conditions:
    // the contents of other.location has been copied to this->location
    // this->message is default
Exception::Exception(const Exception& other) {
    this->set_location(other.location);
}
// pre-conditions:
    // l is a non-null ptr 
// post-conditions
    // the contents of l is copied to this->location
    // this->message is default
Exception::Exception(const char* l) {
    this->set_location(l);
}

// default constructor
// pre-conditions:
    // this->location == nullptr or is otherwise delete[] safe
// post-conditions:
    // "" has been copied to this->location
    // this->message is default
Exception::Exception() {
    this->set_location("");
}

// returns the error message + location where the error occured
// pre-conditions:
    // this has been constructed with a valid location and message
    // if location != "" then message_location holds message + " in " + location
    // else message_location == message
// post-conditions:
    // the error message + location has been returned in the appropriate format
const char* Exception::what() const noexcept {
    return message_location;
};

// returns the location
// pre-conditions:
    // this has been constructed with a valid location
// post-conditions:
    // the location has been returned
const char* Exception::where() const {
    return this->location;
}

// copy assignment
// pre-conditions:
    // other is constructed and has a valid location and message
// post-conditions:
    // other.location has been copied to this->location
Exception& Exception::operator=(const Exception& other) {
    if (this == &other) {
        return *this;
    }
    set_location(other.location);
    return *this;
}

// pre-conditions:
    // this->location is delete[] safe
    // this->message_location is delete[] safe
// post-conditions:
    // this->location is deleted
    // this->message_location is deleted
Exception::~Exception() {
    delete[] location;
    delete[] message_location;
}


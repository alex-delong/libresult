#include <libexception.hpp>

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
Exception::Exception(const char* m, const char* l) : message(m) {
    this->set_location(l);    
};
Exception::Exception(const Exception& other) {
    this->set_location(other.location);
}
Exception::Exception() {
    this->set_location("");
}
Exception::Exception(const char* l) {
    this->set_location(l);
}
const char* Exception::what() const noexcept {
    return message_location;
};
const char* Exception::where() const {
    return this->location;
}
Exception& Exception::operator=(const Exception& other) {
    if (this == &other) {
        return *this;
    }
    set_location(other.location);
    return *this;
}
Exception::~Exception() {
    delete[] location;
    delete[] message_location;
}


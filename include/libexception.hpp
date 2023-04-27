#include <cstring>
#include <exception>

namespace LibException {
    class Exception : std::exception {
      protected:
        const char* message = "Exception";
        char* location = nullptr;
        char* message_location = nullptr;
        
        // sets the location and message_location variables
        // pre-conditions:
            // l is a valid cstring
            // this->location is delete[] safe
            // this->message is a valid cstring
            // this->message_location is delete[] safe
        // post-conditions:
            // l has been copied to this->location
            // if location != "", then message_location holds: message + " in " + location
            // else, message_location == message
        void set_location(const char* l);

        // copy constructors:

        // pre-conditions:
            // m and l are non-null ptrs 
        // post-conditions:
            // the contents of m has been copied to this->message
            // the contents of l has been copied to this->location
        Exception(const char* m, const char* l);
      public:
        
        // pre-conditions:
            // other is constructed or otherwise has a non-null location
        // post-conditions:
            // the contents of other.location has been copied to this->location
            // this->message is default
        Exception(const Exception& other);
        
        // pre-conditions:
            // l is a non-null ptr 
        // post-conditions
            // the contents of l is copied to this->location
            // this->message is default
        Exception(const char* l);
        
        // default constructor
        // pre-conditions:
            // this->location == nullptr or is otherwise delete[] safe
        // post-conditions:
            // "" has been copied to this->location
            // this->message is default
        Exception(); 
        
        // returns the error message + location where the error occured
        // pre-conditions:
            // this has been constructed with a valid location and message
            // if location != "" then message_location holds message + " in " + location
            // else message_location == message
        // post-conditions:
            // the error message + location has been returned in the appropriate format
        virtual const char* what() const noexcept override; 

        // returns the location
        // pre-conditions:
            // this has been constructed with a valid location
        // post-conditions:
            // the location has been returned
        const char* where() const;

        // copy assignment
        // pre-conditions:
            // other is constructed and has a valid location and message
        // post-conditions:
            // other.location has been copied to this->location
        Exception& operator=(const Exception& other);

        // pre-conditions:
            // this->location is delete[] safe
            // this->message_location is delete[] safe
        // post-conditions:
            // this->location is deleted
            // this->message_location is deleted
        virtual ~Exception(); 
    };
}

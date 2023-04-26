#include <cstring>
#include <exception>

class Exception : std::exception {
  protected:
    const char* message = "Exception";
    char* location = nullptr;
    char* message_location = nullptr;
    void set_location(const char*);
    Exception(const char*, const char*);
  public:
    Exception(const Exception&);
    Exception(); 
    Exception(const char*);
    virtual const char* what() const noexcept override; 
    const char* where() const;
    Exception& operator=(const Exception& other);
    virtual ~Exception(); 
};

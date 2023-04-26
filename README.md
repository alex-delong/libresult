# libresult

This library is an implementation of a "Result" monad. It is used to wrap values that could return an error with methods to provide error handling. It is very similar to the one found in the Rust standard library, but includes additional error tracing functionality.

# libexception

This is an extension of the std::exception class that provides error location details explicitly. Exceptions of this class and derived classes can optionally append the location of the error onto the default error message using a constructor.

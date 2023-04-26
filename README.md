# libresult

This library is an implementation of a "Result" monad. It is used to wrap values that could return an error with methods to provide error handling. It is very similar to the one found in the Rust standard library, but includes additional error tracing functionality.

Results that have been pushed onto other results are stored in a linked list to be used for error tracing by calling get_trace(). Errors in the list will print to stdout the returned value from the what() method of the wrapped exception, while Ok's will print the values they hold.

Thus, exceptions must have a what() method that returns a value that can be used with the "<<" operator, and Ok's must hold a value that does the same.

# libexception

This is an extension of the std::exception class that provides error location details explicitly. Exceptions of this class and derived classes can optionally append the location of the error onto the default error message using a constructor. 

Using exceptions of this class or derived classes with libresult provides error tracing capability. An example of this is shown in the integration test. Traces that use such classes print both the type of error and where the error was created.

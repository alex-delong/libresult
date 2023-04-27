# libresult

This library is an implementation of a "Result" monad. It is used to wrap values that could return an error with methods to provide error handling. It is very similar to the one found in the Rust standard library, but includes additional tracing functionality.

Tracing is achieved by pushing existing results onto new results. Results that have been pushed are stored in a linked list. get_trace() will use the results in the list to print a formatted trace to stdout. This method requires that Errs call the what() method of the wrapped exception, whereas Oks print the values they hold.

Thus, exceptions must have a what() method that returns a value that can be used with the "<<" operator, and Ok's must hold a value that does the same.

# libexception

This is a derivation of the std::exception class that provides additional error location details for more useful tracing. Exceptions of this class and derived classes can optionally append the location of the error onto the default error message using a constructor.  

The integration test gives an example that utilizes both libraries to provide a trace that prints the location and result to stdout.

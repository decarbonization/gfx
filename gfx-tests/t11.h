//
//  t11.h
//  gfx
//
//  Created by Kevin MacWhinnie on 11/24/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

///Indicates whether or not the code currently
///being compiled will be run in a T11 context.
#ifndef T11_Active
#   define T11_Active   1
#endif /* T11_Active */

#ifndef __t11_h__
#define __t11_h__

#include <vector>
#include <string>
#include <functional>


/*!
 T11 is a tiny framework built on the C++11 standard library intended
 to make it possible to write simple test cases for your code.
 
 T11 is made up of two classes:
 - `Test`: Encapsulates a series of assertions, and tracks failures.
 - `Suite`: Encapsulates a collection of `Test` objects.
 */


///T11 is intended to be entirely self-contained, but there may be
///use cases where you wish to run T11 tests within a larger project
///that already has a `::main` function. In that case, either comment
///out this macro, or set its contents to `0`.
#define T11_GenerateMain    1

///If set to `1`, the keywords `protected` and `private` will be
///redefined to `public` to allow easier testing of library code.
///
///Code that uses T11 to test it may prefer to check for the existence
///of the `T11_Active` macro to modify scope of class members. In order
///for this macro to operate as intended, T11 should be included before
///any other files, preferably in a prefix header (`pch`) file.
#define T11_MakeIsPublic    1

#if T11_MakeIsPublic
#   if private
#       undef private
#   endif /* private */
#   if protected
#       undef protected
#   endif /* protected */
#endif /* T11_MakeIsPublic */

#pragma mark -

namespace T11 {
    
    ///The Test class encapsulates a collection of assertions around
    ///given test criteria provided in the form of a function.
    ///
    ///A Test object is constructed with a given test name, and a function
    ///that is used to implement the actual test logic. The Test class
    ///contains several template functions that allow you to write assertions
    ///around simple conditions.
    ///
    ///##Example:
    /// T11::Test t("comparisons", [](Test &t) {
    ///     t.equal(12, 12);
    ///     t.not_equal(12, 13);
    ///
    ///     void *nullValue = nullptr;
    ///     t.null(nullValue);
    ///
    ///     void *notNullValue = &nullValue;
    ///     t.not_null(notNullValue);
    ///
    ///     t.is_true(true);
    ///     t.is_false(false);
    /// });
    ///
    ///Test objects are usually created through the `T11::Suite::test` function,
    ///and not through direction construction as shown in the example.
    ///
    ///Test objects are generally exception safe, however a warning
    ///will be printed if a test raises an unexpected exception.
    class Test
    {
    public:
        
        ///A functor that contains the implementation of a test.
        typedef std::function<void(Test &)> Implementation;
        
    protected:
        
        ///The name of the test.
        std::string mName;
        
        ///The implementation of the test.
        Implementation mImplementation;
        
        ///The number of failures that occurred in the test.
        size_t mFailureCount;
        
    public:
        ///Constructs a test with a given name and implementation functor.
        ///
        /// \param  name            The name of the test.
        /// \param  implementation  The functor that contains the logic of the test.
        ///
        Test(const std::string &name, const Implementation &implementation);
        
        ///Returns the name of the test.
        const std::string &name() const { return mName; }
        
        ///Returns the number of assertions that have failed within the test.
        size_t failure_count() const { return mFailureCount; }
        
        
        ///Runs the test.
        ///
        ///This method is usually called by
        ///`T11::Suite`, and not directly.
        void run();
        
        ///Reports a failure from within the test, printing it to
        ///the console and incrementing the test's failure count.
        ///
        /// \param  reason  The human readable reason the test failed.
        ///
        ///This method does not raise exceptions under normal conditions.
        void report_failure(const std::string &reason);
        
#pragma mark - Comparisons
        
        ///Asserts that two values are equal to each other using the
        ///equality `(==)` operator, reporting a failure if they are unequal.
        ///
        /// \tparam T   The type of the value to compare. Must have a valid equality operator.
        ///
        /// \param  left    The left value to compare.
        /// \param  right   The right value to compare.
        ///
        template<typename T> void equal(T left, T right)
        {
            if(!(left == right))
                report_failure("equality test failed");
        }
        
        ///Asserts that two values are not equal to each other using the
        ///equality `(==)` operator, reporting a failure if they are equal.
        ///
        /// \tparam T   The type of the value to compare. Must have a valid equality operator.
        ///
        /// \param  left    The left value to compare.
        /// \param  right   The right value to compare.
        ///
        template<typename T> void not_equal(T left, T right)
        {
            if(left == right)
                report_failure("inequality test failed");
        }
        
        ///Asserts that a value is NULL.
        ///
        /// \tparam T   The type of the value. Must be a pointer.
        ///
        /// \param  value   The value that must be null.
        ///
        template<typename T> void null(T value)
        {
            static_assert(std::is_pointer<T>::value, "T must be a pointer");
            
            if(value != nullptr)
                report_failure("expected null value");
        }
        
        ///Asserts that a value is not NULL.
        ///
        /// \tparam T   The type of the value. Must be a pointer.
        ///
        /// \param  value   The value that must not be null.
        ///
        template<typename T> void not_null(T value)
        {
            static_assert(std::is_pointer<T>::value, "T must be a pointer");
            
            if(value == nullptr)
                report_failure("expected non_null value");
        }
        
        ///Asserts that a value is true.
        ///
        /// \tparam T   A value that must either be `bool` or be convertable to `bool`.
        ///
        /// \param  value   The value to compare for truthiness.
        ///
        template<typename T> void is_true(T value)
        {
            if(!value)
                report_failure("expected true value");
        }
        
        ///Asserts that a value is false.
        ///
        /// \tparam T   A value that must either be `bool` or be convertable to `bool`.
        ///
        /// \param  value   The value to compare for falseiness.
        ///
        template<typename T> void is_false(T value)
        {
            if(value)
                report_failure("expected false value");
        }
        
#pragma mark - Exceptions
        
        ///Asserts that a given functor throws an exception.
        ///
        /// \param  body    The functor that will throw an exception.
        ///
        void throws(std::function<void()> body)
        {
            bool caught = false;
            try {
                body();
            }
            catch (...) {
                caught = true;
            }
            
            if(!caught)
                report_failure("expected exception");
        }
        
        ///Asserts that a given functor does not throw an exception.
        ///
        /// \param  body    The functor that will not throw an exception.
        ///
        void does_not_throw(std::function<void()> body)
        {
            bool caught = false;
            try {
                body();
            }
            catch (...) {
                caught = true;
            }
            
            if(caught)
                report_failure("expected no exception");
        }
    };
    
#pragma mark -
    
    ///The Suite class encapsulates a collection of `T11::Test` objects,
    ///and is responsible for running them under normal conditions.
    ///
    ///Instaces of the `T11::Suite` class are usually created through the
    ///`T11Suite` function-like macro from within an anonymous namespace.
    ///
    ///Suite objects are generally exception safe, however a warning will
    ///be printed if the setup or teardown function of the suite throws.
    ///
    /// \seealso(T11Suite, T11::Test)
    class Suite
    {
    protected:
        
        ///The name of the suite.
        const std::string mName;
        
        ///The `T11::Test` objects contained in the suite.
        std::vector<Test> mTests;
        
        ///The number of known failed tests in the Suite.
        size_t mFailureCount;
        
        ///The functor to run before the Suite is executed.
        std::function<void()> mSetup;
        
        ///The functor to run after the suite is executed.
        std::function<void()> mTeardown;
        
    public:
        
        ///Constructs the receiver with a given name string, and a functor
        ///that will be invoked in the constructor to allow a client to
        ///associate setup/teardown/test logic with the suite.
        ///
        /// \param  name    The name of the test.
        /// \param  createFunctor   The functor to invoke upon construction. The functor will be
        ///                         passed a fully constructed `T11::Suite` object that it is then
        ///                         expected to associate behavior with through the `T11::Suite:::test`
        ///                         method. Setup and teardown logic may be associated in this functor as well.
        ///
        ///The constructor has the side effect of registering the suite with the global suite pool.
        ///All constructors in the suite pool are executed in an undefined order when `T11::RunAllTestSuites`
        ///is invoked. __Important:__ Suite objects should only be created on a single thread.
        Suite(const std::string &name, const std::function<void(Suite &)> &createFunctor);
        
        ///The destructor of the suite.
        ///
        ///The destructor unregisters the suite with the global suite pool.
        ~Suite();
        
        const std::string &name() const { return mName; }
        
        const std::vector<Test> &tests() const { return mTests; }
        
        size_t failure_count() const { return mFailureCount; }
        
        ///Sets the setup functor of the Suite.
        ///
        /// \param  functor The functor to run before all tests in the suite are executed.
        ///
        ///There is only one setup functor per Suite.
        Suite &setup(const std::function<void()> &functor);
        
        ///Sets the teardown functor of the Suite.
        ///
        /// \param  functor The functor to run after all tests in the suite are executed.
        ///
        ///There is only one teardown functor per Suite.
        Suite &teardown(const std::function<void()> &functor);
        
        ///Creates and adds a new test to the suite with a given name, and implementation functor.
        ///
        /// \param  name            The name of the test.
        /// \param  implementation  The functor that contains the logic of the test.
        ///
        ///This is the preferred way to add tests to Suites.
        void test(const std::string &name, const Test::Implementation &implementation);
        
        ///Adds a given `Test` to the suite's internal collection.
        ///
        /// \param  test    The test that will be run as part of the suite.
        ///
        /// \seealso(T11::Suite::test)
        void add_test(const Test &test);
        
        ///Runs all of the tests contained by the suite,
        ///outputting the results to the console.
        void run();
    };
    
#pragma mark - Standalone Functions
    
    ///Tracks the amount of time that was required to execute
    ///a given functor, returning the value in seconds.
    ///
    /// \param  functor The function to time.
    ///
    extern double time_code(const std::function<void()> &functor);
    
    ///Initializes the `T11` global internal state.
    ///
    /// \param  argc
    /// \param  argv
    ///
    ///This function should be invoked before any T11
    ///classes or functions are run.
    extern void init(int argc, const char *argv[]);
    
    ///Runs all of the currently registered test suites.
    ///
    ///All suites introduced through the `T11Suite`
    ///function-like macro are program scoped, and are
    ///guaranteed to be run by this function.
    ///
    ///This function writes all output to the console.
    extern void run_all();
    
#pragma mark - Macros
    
    ///Introduces a new T11Suite.
    ///
    /// \param  _Name   The name of the suite. Must be a valid identifier. Will be used
    ///                 both as a global variable name, and as the suite's runtime name.
    ///
    /// \param  ...     A functor that takes a `Suite&` parameter that is responsible
    ///                 for introducing logic to the suite.
    ///
    ///The parameters of this macro are equivalent to the `T11::Suite` constructor.
    ///
    ///This macro should always be used within the context of an anonymous namespace.
#define T11Suite(_Name, ...)   static T11::Suite _Name ## __LINE__(#_Name, __VA_ARGS__)
    
#pragma mark -
    
#if T11_MakeIsPublic
#   define protected    public
#   define private      public
#endif /* T11_MakeIsPublic */
}

#endif /* defined(__t11_h__) */

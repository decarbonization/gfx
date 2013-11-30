//
//  t11.cpp
//  gfx
//
//  Created by Kevin MacWhinnie on 11/24/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#include "t11.h"
#include <iostream>
#include <chrono>

namespace T11 {
    
#pragma mark - Utilities
    
    static std::vector<Suite *> gRegisteredTestSuites{};
    
    template<typename T> std::string dumb_pluralize(const std::string &s, T count)
    {
        if(count != T(1)) {
            return s + std::string("s");
        } else {
            return s;
        }
    }
    
#pragma mark - Output
    
    struct {
        bool wantsColorOutput;
        bool ignoreExceptions;
    } gOutputOptions = {};
    
    static void print_started_test(const Test &test)
    {
        std::cout << "Test Case '" << test.name() << "' started." << std::endl;
    }
    
    static void print_finished_test(const Test &test, double runningTime)
    {
        if(test.failure_count() == 0)
            std::cout << "Test Case '" << test.name() << "' passed (" << runningTime << " " << dumb_pluralize("second", runningTime) << ")." << std::endl;
        else
            std::cout << "Test Case '" << test.name() << "' failed." << std::endl;
    }
    
#pragma mark -
    
    static void print_started_suite(const Suite &suite)
    {
        std::cout << "Test Suite '" << suite.name() << "' started" << std::endl;
    }
    
    static void print_finished_suite(const Suite &suite, double runningTime)
    {
        std::cout
            << "Executed " << suite.tests().size()
            << " " << dumb_pluralize("test", suite.tests().size()) << " with " << suite.failure_count() << " " << dumb_pluralize("failure", suite.failure_count())
            << " in " << runningTime << " " << dumb_pluralize("second", runningTime) << "."
        << std::endl;
    }
    
#pragma mark -
    
    static void print_failure(const std::string &failure)
    {
        std::cout << failure << std::endl;
    }
    
#pragma mark - • Test
    
    Test::Test(const std::string &name, const Implementation &implementation) :
        mName(name),
        mImplementation(implementation),
        mFailureCount(0)
    {
    }
    
    void Test::run()
    {
        print_started_test(*this);
        double runningTime = time_code([this]() {
            try {
                mImplementation(*this);
            } catch (std::exception e) {
                if(!gOutputOptions.ignoreExceptions)
                    report_failure(std::string("Unhandled exception caught. Please verify test is written correctly. Reason given: ") + e.what());
            } catch (...) {
                if(!gOutputOptions.ignoreExceptions)
                    report_failure("Unhandled exception caught. Please verify test is written correctly.");
            }
        });
        
        print_finished_test(*this, runningTime);
    }
    
    void Test::report_failure(const std::string &reason)
    {
        mFailureCount++;
        
        std::string failure;
        failure += "Failure reported in '";
        failure += name();
        failure += "' (";
        failure += reason;
        failure += ")";
        
        print_failure(failure);
    }
    
#pragma mark - • Suite
    
    Suite::Suite(const std::string &name, const std::function<void(Suite &)> &createFunctor) :
        mName(name),
        mTests(),
        mSetup(),
        mTeardown()
    {
        createFunctor(*this);
        
        gRegisteredTestSuites.push_back(this);
    }
    
    Suite::~Suite()
    {
        gRegisteredTestSuites.erase(std::remove(gRegisteredTestSuites.begin(), gRegisteredTestSuites.end(), this), gRegisteredTestSuites.end());
    }
    
    Suite &Suite::setup(const std::function<void()> &functor)
    {
        mSetup = functor;
        return *this;
    }
    
    ///Sets the teardown functor of the Suite.
    ///
    /// \param  functor The functor to run after all tests in the suite are executed.
    ///
    ///There is only one teardown functor per Suite.
    Suite &Suite::teardown(const std::function<void()> &functor)
    {
        mTeardown = functor;
        return *this;
    }
    
    ///Creates and adds a new test to the suite with a given name, and implementation functor.
    ///
    /// \param  name            The name of the test.
    /// \param  implementation  The functor that contains the logic of the test.
    ///
    ///This is the preferred way to add tests to Suites.
    void Suite::test(const std::string &name, const Test::Implementation &implementation)
    {
        add_test(Test(name, implementation));
    }
    
    void Suite::add_test(const Test &test)
    {
        mTests.push_back(test);
    }
    
    void Suite::run()
    {
        print_started_suite(*this);
        
        mFailureCount = 0;
        double runningTime = time_code([this]() {
            try {
                mSetup();
            } catch (...) {
                if(!gOutputOptions.ignoreExceptions)
                    std::cerr << "Unexpected exception swallowed from suite's (" << mName << ") setup function." << std::endl;
            }
            
            for (Test &test : mTests) {
                test.run();
                
                if(test.failure_count() > 0)
                    mFailureCount++;
            }
            
            try {
                mTeardown();
            } catch (...) {
                if(!gOutputOptions.ignoreExceptions)
                    std::cerr << "Unexpected exception swallowed from suite's (" << mName << ") teardown function." << std::endl;
            }
        });
        
        print_finished_suite(*this, runningTime);
    }
    
#pragma mark - Standalone Functions
    
    double time_code(const std::function<void()> &functor)
    {
        using namespace std::chrono;
        
        auto startTime = high_resolution_clock::now();
        functor();
        auto endTime = high_resolution_clock::now();
        
        return duration_cast<milliseconds>(endTime - startTime).count() / 1000.0;
    }
    
    void init(int argc, const char *argv[])
    {
        for (int i = 0; i < argc; i++) {
            std::string arg = argv[i];
            if(arg == "--with-color")
                gOutputOptions.wantsColorOutput = true;
            else if(arg == "--ignore-exceptions")
                gOutputOptions.ignoreExceptions = true;
        }
    }
    
    void run_all()
    {
        for (Suite *suite : gRegisteredTestSuites)
            suite->run();
    }
}

#pragma mark - Autogenerated Main

#if T11_GenerateMain

int main(int argc, const char *argv[])
{
    T11::init(argc, argv);
    T11::run_all();
    
    return 0;
}

#endif /* T11_GenerateMain */

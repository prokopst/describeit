/**
 * \file describeit.h
 * \author Stanislav Prokop (prosty)
 * \copyright MIT license.
 */

#ifndef __DESCRIBEIT_H__
#define __DESCRIBEIT_H__

#include <typeinfo>
#include <ostream>
#include <map>
#include <string>
#include <stdexcept>
#include <sstream>

#if __cplusplus < 201103L
#define override
#define noexcept
#define nullptr 0
#endif

namespace describeit {
/// \cond DETAIL
template <int DUMMY>
class ExpectationFailureBase {
public:
    static const char* file;
    static int line;
};

template <int DUMMY>
const char* ExpectationFailureBase<DUMMY>::file = nullptr;

template <int DUMMY>
int ExpectationFailureBase<DUMMY>::line = -1;
/// \endcond

struct ExpectationFailure : ExpectationFailureBase<0> {
    std::string actual;
    std::string expected;
    
    ExpectationFailure(const std::string& actual="", const std::string& expected="") : actual(actual), expected(expected) {
    }
    
    virtual const std::string what() {
        if (actual == "" and expected == "") {
            return "";
        }
    }
};

struct ExceptionNotThrownFailure : ExpectationFailureBase<0> {
    std::string name;
    
    ExceptionNotThrownFailure(const char* name) : name(name) {
    }
    
    virtual const std::string what() {
        std::string result = "Exception " + name + " was not catched.";
        return result;
    }
};

namespace Status {
    enum StatusEnum {OK, FAILURE, ERROR};
}

class AbstractPrinter {
protected:
    std::ostream& output;
public:
    AbstractPrinter(std::ostream& output) : output(output) {}
    /**
     * Called at the very beginning. Number of descriptions.
     * \param descriptionsCount number of descriptions
     */
    virtual void start(size_t descriptionsCount) = 0;
    /**
     * Called before every \a Describe.
     * \param name \a Describe name.
     * \param testCount number of tests.
     */
    virtual void startDescribe(const std::string& name, size_t testCount) = 0;
    /**
     * Called after every test.
     * \param name name of the test.
     * \param status status of the test: 0 for OK, 1 for FAILURE, 2 for ERROR.
     */
    virtual void test(const std::string& name, Status::StatusEnum status) = 0;
    /**
     * Called after all tests in description are performed.
     * \param passedCount number of passed tests.
     * \param failedCount number of errorneous tests.
     */
    virtual void endDescribe(size_t passedCount, size_t failedCount) = 0;
    /**
     * Called at the end.
     */
    virtual void end() = 0;
};

/**
 * \brief Printer for simple yaml based printer similar to one found in RSpec.
 */
class BehavePrinter : public AbstractPrinter {
public:
    BehavePrinter(std::ostream& output) : AbstractPrinter(output) {}
    
    virtual void start(size_t) override {}
    
    virtual void startDescribe(const std::string& name, size_t) override {
        output << name << std::endl;
    }
    
    virtual void endDescribe(size_t, size_t) override {}
    
    virtual void test(const std::string& name, Status::StatusEnum status) override {
        output << "- " << name;
        if (status == Status::OK) {
        }
        else if (status == Status::FAILURE) {
            output << " (FAILED)";
        }
        else if (status == Status::ERROR) {
            output << " (ERROR)";
        }
        output << std::endl;
    }
    
    virtual void end() override {}
};

    template <typename Type>
    struct ExpectationBase {
    protected:
        static std::string toString(Type toConvert) {
            std::ostringstream stream;
            stream << toConvert;
            return stream.str();
        }
    public:
        Type actual;
        
        ExpectationBase(Type actual) : actual(actual) {
        }
        
        template <typename Other>
        bool operator==(const Other& other) {
            bool result = (actual == other);
            if (result == false) {
                throw ExpectationFailure(toString(actual), toString(other));
            }
            return result;
        }
        
        template <typename Other>
        bool operator!=(const Other& other) {
            bool result = (actual == other);
            if (result != false) {
                throw ExpectationFailure(toString(actual), toString(other));
            }
            return result;
        }
        
        template <typename Other>
        bool operator>=(const Other& other) {
            bool result = (actual == other);
            if (result >= false) {
                throw ExpectationFailure(toString(actual), toString(other));
            }
            return result;
        }
        
        template <typename Other>
        bool operator<=(const Other& other) {
            bool result = (actual == other);
            if (result <= false) {
                throw ExpectationFailure(toString(actual), toString(other));
            }
            return result;
        }
        
        template <typename Other>
        bool operator>(const Other& other) {
            bool result = (actual == other);
            if (result > false) {
                throw ExpectationFailure(toString(actual), toString(other));
            }
            return result;
        }
        
        template <typename Other>
        bool operator<(const Other& other) {
            bool result = (actual == other);
            if (result < false) {
                throw ExpectationFailure(toString(actual), toString(other));
            }
            return result;
        }
    };
    
    template <typename Type>
    struct Expectation : public ExpectationBase<Type> {
        Expectation(Type actual) : ExpectationBase<Type>(actual) {
        }
    };
    
    template <typename Type>
    inline Expectation<Type> expectation(const char* file, int line, Type actual) {
        ExpectationFailure::file = file;
        ExpectationFailure::line = line;
        return Expectation<Type>(actual);
    }
    
    namespace detail {
    
    
    class DescribeRegistratorBase {
    public:
        virtual void runTests(AbstractPrinter& printer) = 0;
        virtual ~DescribeRegistratorBase() {}
    };
    
    template <typename Type>
    class Singleton {
    public:
        /**
         * Static member function to create or get singleton.
         * \return singleton instance.
         */
        static Type* getInstance() noexcept {
            if (instance == nullptr) {
                instance = new Type();
            }
            return instance;
        }
    protected:
        static Type* instance;
        struct AutoPtr {
            AutoPtr() {
            }
            
            ~AutoPtr() {
                delete instance;
                instance = nullptr;
            }
        };
        static AutoPtr deleter;
    };
    template <typename Type>
    Type* Singleton<Type>::instance = nullptr;
    
    /**
     * \brief Singleton that provides running tests and information about them.
     * 
     * This class is also used for registration of \a DescribeRegistator instances.
     */
    class DescribeIt : public Singleton<DescribeIt> {
    protected:
        typedef std::map<std::string, DescribeRegistratorBase*> Registrators;
        Registrators registrators;
    public:
        DescribeIt() {}
        
        /**
         * \brief Register registrator.
         * 
         * \param name name of the registrator.
         * \param registrator registrator instance
         */
        void registerDescribe(const std::string& name, DescribeRegistratorBase* registrator) {
            registrators[name] = registrator;
        }
        
        /**
         * \brief Run all tests.
         * 
         * \param printer instance of class derived from \a AbstractPrinter.
         */
        void runAll(AbstractPrinter& printer) {
            printer.start(registrators.size());
            
            for (Registrators::iterator iter = registrators.begin(); iter != registrators.end(); ++iter) {
                DescribeRegistratorBase* registrator = iter->second;
                registrator->runTests(printer);
            }
            printer.end();
        }
        
        /**
         * \brief Return number of all descriptions.
         * 
         * \return number of descriptions.
         */
        size_t descriptionsCount() const {
            return registrators.size();
        }
        
        /**
         * \brief Destructor handles destruction of all registrators.
         */
        ~DescribeIt() {
            for (Registrators::iterator iter = registrators.begin(); iter != registrators.end(); ++iter) {
                DescribeRegistratorBase* registrator = iter->second;
                delete registrator;
            }
        }
    };
    
    template <typename SpecificDescribeClass>
    class DescribeRegistrator : public DescribeRegistratorBase {
    public:
        typedef void (SpecificDescribeClass::*Method)();
        typedef std::map<std::string, Method> Methods;
        Methods methods;
        
        typedef std::map<int, std::string> MethodsOrder;
        MethodsOrder methodsOrder;
        std::string name;
        
        DescribeRegistrator(const std::string& name) : name(name) {
            DescribeIt* instance = DescribeIt::getInstance();
            instance->registerDescribe(name, this);
        }
        
        void registerMethod(const std::string& name, size_t id, Method method) {
            methodsOrder[id] = name;
            methods[name] = method;
        }
        
        virtual void runTests(AbstractPrinter& printer) {
            SpecificDescribeClass describeClass;
            printer.startDescribe(name, methods.size());
            for (MethodsOrder::iterator iter = methodsOrder.begin(); iter != methodsOrder.end(); ++iter) {
                const std::string& testName = iter->second;
                Method method = methods[testName];
                Status::StatusEnum status = Status::OK;
                std::string what;
                
                try {
                    (describeClass.*method)();
                }
                catch (ExceptionNotThrownFailure exception) {
                    status = Status::FAILURE;
                }
                catch (ExpectationFailure exception) {
                    status = Status::FAILURE;
                }
                catch (std::exception exception) {
                    status = Status::ERROR;
                    what = exception.what();
                }
                catch (...) {
                    status = Status::ERROR;
                }
                printer.test(testName, status);
            }
        }
    };
    
    template <typename SpecificDescribeClass, char const* NAME>
    class DescribeClass {
    protected:
        typedef SpecificDescribeClass Class;
        static DescribeRegistrator<SpecificDescribeClass>* registrator;
    public:
        virtual void beforeAllMethod() {}
        virtual void afterAllMethod() {}
        virtual void beforeEachMethod() {}        
        virtual void afterEachMethod() {}
    };
    
    template <typename SpecificDescribeClass, char const* NAME>
    DescribeRegistrator<SpecificDescribeClass>* DescribeClass<SpecificDescribeClass, NAME>::registrator = new DescribeRegistrator<SpecificDescribeClass>(NAME);

    struct DummyEater {
        template <typename Type>
        Type& operator,(Type& value) {
            return value;
        }
    };    
}

typedef detail::DescribeIt DescribeIt;

}

#define expect_exception(what, exception) \
try { \
    what; \
    throw describeit::ExceptionNotThrownFailure(#exception); \
} \
catch (exception ex) {}

#define expect(what) describeit::detail::DummyEater(),  describeit::expectation(__FILE__, __LINE__, what)
//#define expect(what) detail::ExpectationHandler(__FILE__, __LINE__),  detail::expectation(what)

#define beforeAll virtual void beforeAllMethod() override
#define afterAll virtual void afterAllMethod() override
#define beforeEach virtual void beforeEachMethod() override
#define afterEach virtual void afterEachMethod() override

#define __describe_implementation(what, counter) \
char __DESCRIPTION_NAME_##what[] = #what;\
class __Describe##what : public describeit::detail::DescribeClass<__Describe##what, __DESCRIPTION_NAME_##what>

#define __describe(what, counter) __describe_implementation(what, counter)

/**
 * \brief Describe context.
 */
#define describe(what) \
__describe(what, __COUNTER__)

#define __it_implementation(what, counter) \
struct __It_##counter { \
    __It_##counter() { \
       registrator->registerMethod(what, counter, &Class::__it_function_##counter); \
    }\
};\
__It_##counter __it_##counter;\
void __it_function_##counter()

#define __it(what, counter) __it_implementation(what, counter)

/**
 * \brief it in BDD context is a test in xUnit world.
 */
#define it(what) __it(what, __COUNTER__)

#define describeit_main \
int main(int, char**) { \
}

#endif

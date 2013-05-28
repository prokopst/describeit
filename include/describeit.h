/**
 * \file describeit.h
 * \author Stanislav Prokop (prost87)
 * \copyright MIT license.
 */

#ifndef __DESCRIBEIT_H__
#define __DESCRIBEIT_H__

#include <typeinfo>
#include <ostream>
#include <map>
#include <string>
#include <stdexcept>

#if __cplusplus < 201103L
#define override
#define noexcept
#define nullptr 0
#endif

struct ExpectationFailure : public std::exception {
    const char* file;
    int line;

    ExpectationFailure(const char* file, int line)
    : file(file), line(line) {
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
    virtual void start(size_t ) = 0;
    virtual void startDescribe(const std::string& name, size_t testCount) = 0;
    /**
     * \param name name of the test.
     * \param status status of the test: 0 for OK, 1 for FAILURE, 2 for ERROR.
     */
    virtual void test(const std::string& name, Status::StatusEnum status) = 0;
    virtual void endDescribe(size_t passedCount, size_t failedCount) = 0;
    virtual void end() = 0;
};

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

namespace detail {
    template <typename Type>
    struct Expectation {
        const char* file;
        const int line;
        Type actual;
        
        Expectation(const char* file, int line, Type actual) : file(file), line(line), actual(actual) {
        }
        
        template <typename Other>
        bool operator==(const Other& other) {
            bool result = (actual == other);
            if (result == true) {
                return true;
            }
            else {
                throw ExpectationFailure(file, line);
            }
            return false;
        }
    };
    
    template <typename Type>
    inline Expectation<Type> expectation(const char* file, int line, Type actual) {
        return Expectation<Type>(file, line, actual);
    }
    
    
    class DescribeRegistratorBase {
    public:
        virtual void runTests(AbstractPrinter& printer) = 0;
        virtual ~DescribeRegistratorBase() {}
    };
    
    /**
     * \brief DescribeIt class.
     */
    template <int DUMMY>
    class DescribeItManager {
    protected:
        typedef std::map<std::string, DescribeRegistratorBase*> Registrators;
        Registrators registrators;
        DescribeItManager() {}
    public:
        static DescribeItManager<DUMMY>* getInstance() noexcept {
            if (instance == nullptr) {
                instance = new DescribeItManager<DUMMY>();
            }
            return instance;
        }
            
        void registerDescribe(const std::string& name, DescribeRegistratorBase* registrator) {
            registrators[name] = registrator;
        }
        void runAll(AbstractPrinter& printer) {
            printer.start(registrators.size());
            
            for (Registrators::iterator iter = registrators.begin(); iter != registrators.end(); ++iter) {
                DescribeRegistratorBase* registrator = iter->second;
                registrator->runTests(printer);
            }
            printer.end();
        }
        
        size_t descriptionsCount() const {
            return registrators.size();
        }
        
        ~DescribeItManager() {
            for (Registrators::iterator iter = registrators.begin(); iter != registrators.end(); ++iter) {
                DescribeRegistratorBase* registrator = iter->second;
                delete registrator;
            }
        }
    private:
        static DescribeItManager* instance;
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
    template <int DUMMY>
    DescribeItManager<DUMMY>* DescribeItManager<DUMMY>::instance = nullptr;
    typedef DescribeItManager<0> DescribeIt;
    
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
                
                try {
                    (describeClass.*method)();
                }
                catch (ExpectationFailure excpectationFailure) {
                    status = Status::FAILURE;
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
    
}

typedef detail::DescribeIt DescribeIt;

#define expect(what) detail::expectation(__FILE__, __LINE__, what)

#define beforeAll virtual void beforeAllMethod() override
#define afterAll virtual void afterAllMethod() override
#define beforeEach virtual void beforeEachMethod() override
#define afterEach virtual void afterEachMethod() override

#define __describe_implementation(what, counter) \
char __DESCRIPTION_NAME_##what[] = #what;\
class __Describe##what : public detail::DescribeClass<__Describe##what, __DESCRIPTION_NAME_##what>

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

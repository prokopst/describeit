#ifndef __DESCRIBEIT_H__
#define __DESCRIBEIT_H__

#include <functional>

namespace detail {
    template <int DUMMY>
    class DescribeItManager {
    public:
        //void register_it(std::type_info parent_describe, function) {
        //}
    };
    DescribeItManager<0> describeItManager;
    
    template <typename SpecificRegistrator>
    struct DescribeClass {
        typedef SpecificRegistrator Class;
        
    };
    
    struct ItRegistrator {
    };
    
}

#define expect(what)

#define __describe(what, counter) \
class __Describe_##counter : public detail::DescribeClass<__Describe_##counter>

#define describe(what) \
__describe(what, __COUNTER__)

#define __it(what, counter) \
struct __It_##counter { \
    __It_##counter() { \
        auto x = &Class::__it_##counter; \
    }\
};\
void __it_##counter()

#define it(what) __it(what, __COUNTER__)

#define describeit_main \
int main(int count, char** argv) {}

#endif
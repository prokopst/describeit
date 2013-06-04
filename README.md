describeit
==========

Header only C++ library for Behaviour driven development inspired by RSpec, more specifically
its [describe/it](https://www.relishapp.com/rspec/rspec-core/v/2-14/docs/example-groups/basic-structure-describe-it) and [expect](http://myronmars.to/n/dev-blog/2012/06/rspecs-new-expectation-syntax) syntax.

Just a little self-describing sneak-peek:

```cpp
#include <describeit.h>
// hand-made Stack class for storing integers
#include "stack.h"

describe(Stack) {
    it("should pop inserted value while having correct depth") {
        Stack stack;
        
        stack.push(1);
        
        int size = stack.size();
        expect(size) == 1337;
        
        int value = stack.pop();
        expect(value) == 1337;
        
        size = stack.size();
        expect(size) == 0;
    }
    
    it("should raise exception when popping empty") {
        Stack stack;
        
        expect_exception(stack.size(), Stack::IndexError);
    }
};
```

And few facts:
* you **don't need to compile it**, really
** it's header only libary
* describeit is using standard C++98 (in other words not the latest revision C++11) without additional dependencies
** its own tests are compiled with "-pedantic -Wextra -Wall -Werr" in clang++ and g++


Motivation
----------

In my opinion xUnit style is being slowly deprecated. It makes more sense - developer don't just
want to write tests, I want to describe behavior first, then write test covering it. And then
finally implement it.

But it was quite disturbing to me that there is no RSpec-like library for C++. Yes, There is
igloo-testing, but using underscores in Spec and Context, that's far away from usable
([source of example](http://igloo-testing.org/)):

```cpp
...

Context(a_guitar_with_a_fuzzbox)
{
  void SetUp()
  {
    guitar.AddEffect(fuzzbox);
  }

  Spec(fuzzbox_starts_in_clean_mode)
  {
    Assert::That(guitar.Sound(), Equals(Clean));
  }

  Context(a_fuzzbox_in_distorted_mode)
  {
    void SetUp()
    {
      Parent().fuzzbox.Switch();
    }

    Spec(when_I_fret_the_strings_the_sound_is_distorted)
    {
      Assert::That(Parent().guitar.Sound(), Equals(Distorted));
    }

    Spec(when_I_switch_the_fuzzbox_the_sound_is_clean)
    {
      Parent().fuzzbox.Switch();
      Assert::That(Parent().guitar.Sound(), Equals(Clean));
    }
  };
```


There is cppspec with its camelCase approach. And it's not kinda simple ([source of example](https://github.com/tpuronen/cppspec/blob/master/examples/StackSpec.h)):

```cpp
...

class EmptyStackSpec : public CppSpec::Specification<Stack<int>, EmptyStackSpec> {
public:
	EmptyStackSpec() {
		REGISTER_BEHAVIOUR(EmptyStackSpec, poppingShouldRaiseException);
		REGISTER_BEHAVIOUR(EmptyStackSpec, backShouldRaiseException);
		REGISTER_BEHAVIOUR(EmptyStackSpec, stackIsEmpty);
		REGISTER_BEHAVIOUR(EmptyStackSpec, stackEqualsWithEmpty);
		REGISTER_BEHAVIOUR(EmptyStackSpec, countShouldBeZero);
	}

	void poppingShouldRaiseException() {
		specify(invoking(&Stack<int>::pop).should.raise.exception<std::exception>());
	}

	void backShouldRaiseException() {
		specify(invoking(&Stack<int>::top).should.raise.exception<std::exception>());
	}

	void stackIsEmpty() {
		specify(should.be().empty());
	}

	void stackEqualsWithEmpty() {
		Stack<int> anotherStack;
		specify(context(), should.equal(anotherStack));
	}

	void countShouldBeZero() {
		specify(context().count(), should.equal(0));
	}
} emptyStackSpec;
```


There is CBehave written in pure C. As you can see bellow, trying to achieve cucumber-like syntax in C has
its drawbacks. It's ugly ([source of example](http://code.google.com/p/cbehave/wiki/Cbehave_User_Guide)):

```cpp
...

FEATURE(1, "strstr")
    SCENARIO("The strstr finds the first occurrence of the substring in the source string")

        GIVEN("A source string: [Lionel Messi is a great football player]")
            char *str = "Lionel Messi is a great football player";
        GIVEN_END

        WHEN("we use strstr to find the first occurrence of [football]")
            char *p = strstr(str, "football");
        WHEN_END

        THEN("We should get the string: [football player]")
            SHOULD_STR_EQUAL(p, "football player");
        THEN_END

    SCENARIO_END

    SCENARIO("If strstr could not find the first occurrence of the substring, it will return NULL")

        GIVEN("A source string: FC Barcelona is a great football club.")
            char *str = "FC Barcelona is a great football club";
        GIVEN_END

        WHEN("we use strstr to find the first occurrence of [AC Milan]")
            char *p = strstr(buf, "AC Milan");
        WHEN_END

        THEN("We should get no string but a NULL")
            SHOULD_STR_EQUAL(p, NULL);
        THEN_END
    SCENARIO_END
FEATURE_END
```



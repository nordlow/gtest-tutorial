# gtest-tutorial

Tutorial on learning the Google Test (GTest) testing framework.

Here's the tutorial:

# Test Doubles and Google Test (Mock)

We start by giving a short explanation of the terminology used in Google Test.

## Test Double Terminology (according to [Uncle Bob](https://8thlight.com/blog/uncle-bob/))

Is a formal name, analogous with a [stunt
double](https://en.wikipedia.org/wiki/Stunt_double). It is a generic term for any kind of pretend object used in place of a real object during testing.

IMHO, the name is misleading; the word "double" refers to the "other" (mocked) entity, not a pair of entities.

These are defined in Gerard Meszaros's book and used by, for instance, Martin Fowler and Uncle Bob:

## Sample interface

```Java
interface Authorizer
{
  public Boolean authorize(String username, String password);
}
```


### *dummy*:

Sample implementation:

```Java
public class DummyAuthorizer implements Authorizer {
  public Boolean authorize(String username, String password) {
    return null;
  }
}
```

- Only mimics interface
- Only constructed but not used

### *stub*: a kind of *dummy*

Sample implementation:

```Java
public class AcceptingAuthorizerStub implements Authorizer {
  public Boolean authorize(String username, String password) {
    return true;
  }
}
```

- Provide canned (statically predefined) answers (return values) to calls
- Stubs (sometimes) verify *state*

### *spy*: a kind of *stub*

Sample implementation:

```Java
public class AcceptingAuthorizerSpy implements Authorizer {
  public boolean authorizeWasCalled = false;

  public Boolean authorize(String username, String password) {
    authorizeWasCalled = true;
    return true;
  }
}
```

- Logs statistics on behaviour
- Check somewhere (else) that a method, in this case `authorize`, was called.

### *mock*: a kind of *spy*

Sometimes used in an informal way to refer to the whole family of objects that are used in tests.

Sample implementation:

```Java
public class AcceptingAuthorizerVerificationMock implements Authorizer {
  public boolean authorizeWasCalled = false;

  public Boolean authorize(String username, String password) {
    authorizeWasCalled = true;
    return true;
  }

  public boolean verify() {
    return authorizedWasCalled;
  }
}
```

- Mocks always verifies *behaviour*
- Less interested in the return values of functions
- More interested in which function was called, with what arguments, when, and how often
- Used by **mocking tools**

### *fake*: a simulator (like none of the above)

Sample implementation:

```Java
public class AcceptingAuthorizerFake implements Authorizer {
      public Boolean authorize(String username, String password) {
        return username.equals("Bob");
      }
  }
```

- Has simplified business behavior
- An in-memory database is a good example.

### Sum up

- Definition of stub, spy and mock can be auto-generated.
- [See also article](https://martinfowler.com/articles/mocksArentStubs.html#TheDifferenceBetweenMocksAndStubs)

## Google's own classification of test types:

### Small tests (unit tests)
- Level: single function or module.
- Duration: < could of seconds
- Written by:
  - Often: Software Engineer (SWE)
  - Seldom: Software Engineer in Test (SET)
  - Hardly ever: Test Engineer (TE)

### Medium tests (integration tests)

### Large tests (acceptance tests)

## [Assertions](https://github.com/google/googletest/blob/master/googletest/docs/Primer.md#assertions)

- Fatal failure: `ASSERT_[TRUE,FALSE,EQ,NE,LT,LE,GT,GE, STREQ,STRNE]`
- Non-fatal failure: `EXPECT_[TRUE,FALSE,EQ,NE,LT,LE,GT,GE, STRCASEEQ,STRCASENE]`
- Sample:

```Cpp
ASSERT_EQ(x.size(), y.size()) << "Vectors x and y are of unequal length";
```

Values of left-hand and right-hand side expressions are printed upon assertion
failure.

*Question*: How cleverly are, for instance, containers (with many elements)
printed?

## Defining a simple test

Arguments to `TEST` should be valid C++ symbols without underscore.

For instance, a test case for

```Cpp
int Factorial(int n); // Returns the factorial of n
```

might look like


```Cpp
// Tests factorial of 0.
TEST(FactorialTest, HandlesZeroInput) {
  EXPECT_EQ(1, Factorial(0));
}

// Tests factorial of positive numbers.
TEST(FactorialTest, HandlesPositiveInput) {
  EXPECT_EQ(1, Factorial(1));
  EXPECT_EQ(2, Factorial(2));
  EXPECT_EQ(6, Factorial(3));
  EXPECT_EQ(40320, Factorial(8));
}
```

Here the test case named `FactorialTest` contains the two tests
`HandlesZeroInput` and `HandlesPositiveInput`.

Google Test groups the test results by test cases, so logically-related tests
should be in the same test case.

## Google Mock: Example Turtle Graphics

```Cpp
class Turtle {
  ...
  virtual ~Turtle() {} // must be virtual, otherwise memory corruption
  virtual void PenUp() = 0;
  virtual void PenDown() = 0;
  virtual void Forward(int distance) = 0;
  virtual void Turn(int degrees) = 0;
  virtual void GoTo(int x, int y) = 0;
  virtual int GetX() const = 0;
  virtual int GetY() const = 0;
};
```

It's preferred but not required for methods to be virtual.

## Derive MockTurtle from Turtle

```Cpp
#include "gmock/gmock.h"  // Brings in Google Mock

class MockTurtle : public Turtle {
public: // mocks must be public
  ...

  // non-const methods:
  MOCK_METHOD0(PenUp, void());
  MOCK_METHOD0(PenDown, void());
  MOCK_METHOD1(Forward, void(int distance));
  MOCK_METHOD1(Turn, void(int degrees));
  MOCK_METHOD2(GoTo, void(int x, int y));

  // const methods:
  MOCK_CONST_METHOD0(GetX, int());
  MOCK_CONST_METHOD0(GetY, int());
};
```

- Note: Most other languages, both static and dynamic, can completely (and 100 %
  correctly) automate this stage.
- If you're lazy and optimistic you can use `scripts/gen-erator/gmock_gen.py`.
- Mocks should be defined in separate files, that is `MockTurtle` should be placed in `mock_turtle.h`.

## Using Mocks

1. Import
2. Create mock objects
3. Set expectations on behaviour
4. Exercise code
5. Upon destruction, expectations are checked

For instance:

```Cpp
#include "path/to/mock-turtle.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
using ::testing::AtLeast;                     // #1

TEST(PainterTest, CanDrawSomething) {
  MockTurtle turtle;                          // #2
  EXPECT_CALL(turtle, PenDown())              // #3
      .Times(AtLeast(1));

  Painter painter(&turtle);                   // #4

  EXPECT_TRUE(painter.DrawCircle(0, 0, 10));
}                                             // #5

int main(int argc, char** argv) {
  // The following line must be executed to initialize Google Mock
  // (and Google Test) before running the tests.
  ::testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}
```

## Alternative tool: [dextool](https://github.com/joakim-brannstrom/dextool/)

- reuse of Clang's C/Cpp parser enables
  - 100 % correct Cpp parsing
  - fully automated mock generation
- used in Gripen software verification

## Reads (in order of importance)
- [The Little Mocker](https://8thlight.com/blog/uncle-bob/2014/05/14/TheLittleMocker.html)
- [Google test](https://en.wikipedia.org/wiki/Google_Test)
- [Google test Primer](https://github.com/google/googletest/blob/master/googletest/docs/Primer.md)

## Convert this document to HTML on Ubuntu via

```Shell
cat gtest-lecture.md | pandoc -s -f markdown_github > /tmp/foo.html; google-chrome /tmp/foo.html
```

## Various

- Install Google Test/Mock on Ubuntu via the command

```Shell
sudo apt install google-mock
```

or checkout and build via

```Shell
git clone --recurse-submodules https://github.com/google/googletest
cd googletest
cmake .
make
```

# gtest-tutorial

Tutorial on learning the Google Test (GTest) testing framework.

Here's the tutorial:

# Test Doubles and Google Test (Mock)

We start by giving a short explanation of the terminology used in Google Test.

## Test Double Terminology (according to [Uncle Bob](https://8thlight.com/blog/uncle-bob/))

Is a formal name, analogous with a [stunt
double](https://en.wikipedia.org/wiki/Stunt_double). It is a generic term for any kind of pretend object used in place of a real object during testing.

The name might be misleading; note that the word "double" here refers to the
"other" (mocked) entity, not a pair of entities.

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
- Return value is here designed to give error upon use (`NullPointerException`
  in Java)

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

Simples sample implementation:

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

- Mocks always verifies *behaviour*, that is which functions are called (in which
  order)
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
TEST(FactorialTest, HandlesZeroInput) {
  EXPECT_EQ(1, Factorial(0));
}

TEST(FactorialTest, HandlesPositiveInput) {
  EXPECT_EQ(1, Factorial(1));
  EXPECT_EQ(2, Factorial(2));
  EXPECT_EQ(6, Factorial(3));
  EXPECT_EQ(40320, Factorial(8));
}
```

Here the test case named `FactorialTest` contains the two tests
`HandlesZeroInput` and `HandlesPositiveInput`.

Test results are grouped by test cases, so logically-related tests should be
in the same test case.

## Using test fixtures

Tests for

```Cpp
template <typename E> // E is the element type.
class Queue {
 public:
  Queue();
  void Enqueue(const E& element);
  E* Dequeue(); // Returns NULL if the queue is empty.
  size_t size() const;
  ...
};
```

can use the fixture class

```Cpp
class QueueTest : public ::testing::Test {
 protected:
  virtual void SetUp() {
    q1_.Enqueue(1);
    q2_.Enqueue(2);
    q2_.Enqueue(3);
  }

  // virtual void TearDown() {}

  Queue<int> q0_;
  Queue<int> q1_;
  Queue<int> q2_;
};
```

and to test it

```Cpp
TEST_F(QueueTest, IsEmptyInitially) {
  EXPECT_EQ(0, q0_.size());
}

TEST_F(QueueTest, DequeueWorks) {
  int* n = q0_.Dequeue();
  EXPECT_EQ(NULL, n);

  n = q1_.Dequeue();
  ASSERT_TRUE(n != NULL);
  EXPECT_EQ(1, *n);
  EXPECT_EQ(0, q1_.size());
  delete n;

  n = q2_.Dequeue();
  ASSERT_TRUE(n != NULL);
  EXPECT_EQ(2, *n);
  EXPECT_EQ(1, q2_.size());
  delete n;
}
```

Use `EXPECT_*` when you want the test to continue to reveal more errors after the
assertion failure, and use `ASSERT_*` when continuing after failure doesn't make
sense.

## Running the tests

The typical C/C++ `main()` function looks like

```Cpp
// tests and fixtures ...

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
```

- `TEST()` and `TEST_F()` automatically register themselves for execution, which
then happens during `RUN_ALL_TESTS()`.
- `RUN_ALL_TESTS()` runs all tests in the current link unit, and returns 0 upon
success, 1 otherwise.
- GCC forces return value of `RUN_ALL_TESTS` to be handle (not ignored)
- `RUN_ALL_TESTS` shall only be called *once*!

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

If the painter object didn't call this method, your test will fail with a
message like this:

```
path/to/my_test.cc:119: Failure
Actual function call count doesn't match this expectation:
Actually: never called;
Expected: called at least once.
```

The line is clickable in Emacs and other tools that recognize GNU style messages.

**Important!**: Final verification happens in destructors of mock objects => use heap leak checker in test.

**Interesting Fact!**: `EXPECT_CALL()` (opposite to assertions) enables Google
Mock to report a violation as soon as it happens, with context (stack trace,
etc) still being intact. This makes debugging much easier.

## Using Google Mock with Any Testing Framework

```D
int main(int argc, char** argv) {
  // The following line causes Google Mock to throw an exception on failure,
  // which will be interpreted by your testing framework as a test failure.

  ::testing::GTEST_FLAG(throw_on_failure) = true;
  ::testing::InitGoogleMock(&argc, argv);
  ... whatever your testing framework requires ...
}
```

## Setting expectations

Balance between setting too strict and too loose expectations.

`EXPECT_CALL()` sets an expectation on a mock method using a *declarative*
domain-specific language:

```D
...
EXPECT_CALL(mock_object, method(matchers)) // separated by comma instead of dot because of limitations in C++
    .Times(cardinality)
    .WillOnce(action)
    .WillRepeatedly(action);
```

enabling more compact specifications (and automatic error diagnostics) compared
to writing the cardinality loop ourselves.

For instance,

```D
using ::testing::Return;
...
EXPECT_CALL(turtle, GetX())
    .Times(5)                     // called 5 times
    .WillOnce(Return(100))        // return 100 first time,
    .WillOnce(Return(150))        // return 150 second time
    .WillRepeatedly(Return(200)); // return 200 the remaining times
```

If you don't care about exact parameter values use

```D
using ::testing::_;
...
EXPECT_CALL(turtle, Forward(_)); // turtle moved forward by some arbitrary step
```

The underscore `_` here is called a *matcher* (as in Erlang's pattern
matching). [Here's](https://github.com/google/googletest/blob/master/googlemock/docs/CheatSheet.md#matchers)
a list of all the others.

For instance, the `Ge` matcher can be used here as

```D
using ::testing::Ge;
...
EXPECT_CALL(turtle, Forward(Ge(100))); // turtle moved forward at least 100 steps
```

The `cardinality` can be zero, in the case when a member is expected *not* to be
called.

The `cardinality` parameter to `.Times()` can be either

- `AtLeast(n)`
- `AtMost(n)`
- `Between(m, n)`
- `Exactly(n)` or just `n`

**Important!**: If you omit `Times()`, Google Mock will infer the cardinality
for you according to these rules:

- If neither `WillOnce()` nor `WillRepeatedly()` is in the `EXPECT_CALL()`, the
  inferred cardinality is `Times(1)`.
- If there are n `WillOnce()`'s but no `WillRepeatedly()`, where n >= 1, the cardinality is `Times(n)`.
- If there are n `WillOnce()`'s and one `WillRepeatedly()`, where n >= 0, the cardinality is `Times(AtLeast(n))`.

Compare this to a regexp `x? y? ... y*`.

**Question!**: Why is there no `WillN()`?

**Important!**: If you want side-effects in expectations be careful because
evaluation order maybe not be what you expect.

Time for another quiz! What does this mean?:

```D
using ::testing::Return;
...
EXPECT_CALL(turtle, GetY())
.Times(4)
.WillOnce(Return(100));
```

It expectes `GetY` to return 100 first time and 0 (default) the remaining three
times.

## Multiple Expectations

## Alternative tool: [dextool](https://github.com/joakim-brannstrom/dextool/)

- reuse of Clang's C/Cpp parser enables
  - 100 % correct Cpp parsing
  - fully automated mock generation
- used in Gripen software verification

## Reads (in order of importance)
- [The Little Mocker](https://8thlight.com/blog/uncle-bob/2014/05/14/TheLittleMocker.html)
- [Google test on Wikipedia](https://en.wikipedia.org/wiki/Google_Test)
- [Google test Primer](https://github.com/google/googletest/blob/master/googletest/docs/Primer.md)
- [Google Mock Cheat Sheet](https://github.com/google/googletest/blob/master/googlemock/docs/CheatSheet.md)

## Convert this document to HTML on Ubuntu via

```Shell
TMP=`tempfile --suffix=.html` && pandoc -s -f markdown_github -o $TMP README.md && $BROWSER $TMP
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

# gtest-tutorial

Three lines of wisdom.

- You can't stay agile without clean code.
- You can't have clean code without refactoring.
- You can't refactor without good automated tests.

First a short explanation of the terminology used in Google Test.

Compared to other docs, this tutorial has colored code, more compact overviews,
and merges several documents (Google Test, Google Mock and Google Mock Cheat
sheet) into one.

---
# Test Double Terminology (according to [Uncle Bob](https://8thlight.com/blog/uncle-bob/))

Is a formal name, analogous with a [stunt
double](https://en.wikipedia.org/wiki/Stunt_double). It is a generic term for any kind of pretend object used in place of a real object during testing.

The name might be misleading; note that the word "double" here refers to the
"other" (mocked) entity, not a pair of entities.

These are defined in Gerard Meszaros's book and used by, for instance, Martin Fowler and Uncle Bob:

---
## Sample interface

```Java
interface Authorizer
{
  public Boolean authorize(String username, String password);
}
```

Here are sample definitions for the five well-known kinds of test doubles in
increasing complexity:

---
### 1. *dummy*:

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
- Return value is here (in Java) designed to give error upon use (`NullPointerException`)

---
### 2. *stub*: a kind of *dummy*

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

---
### 3. *spy*: a kind of *stub*

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

---
### 4. *mock*: a kind of *spy*

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

---
### 5. *fake*: a simulator (like none of the above)

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

---
### Sum up

- Definition of stub, spy and mock can be auto-generated.
- [See also article](https://martinfowler.com/articles/mocksArentStubs.html#TheDifferenceBetweenMocksAndStubs)

<!-- --- -->
<!-- ## Google's own classification of test types: -->

<!-- --- -->
<!-- ### Small tests (unit tests) -->
<!-- - Level: single function or module. -->
<!-- - Duration: < could of seconds -->
<!-- - Written by: -->
<!--   - Often: Software Engineer (SWE) -->
<!--   - Seldom: Software Engineer in Test (SET) -->
<!--   - Hardly ever: Test Engineer (TE) -->

<!-- --- -->
<!-- ### Medium tests (integration tests) -->

<!-- --- -->
<!-- ### Large tests (acceptance tests) -->

# Google Test and Mock presentation

## [Assertions](https://github.com/google/googletest/blob/master/googletest/docs/Primer.md#assertions)

### Standard assertions

Pattern: `[ASSERT,EXPECT]_`
- `[TRUE,FALSE]` (boolean)
- `[EQ,NE](expected, actual)` (equable (most) types)
- `[LT,LE,GT,GE]` (comparable types)
- `[STREQ,STRNE,STRCASEEQ,STRCASENE]` for C-strings
- `[FLOAT_EQ,DOUBLE_EQ](expected, actual)` floating pointer numbers (with implicit range)
- `[NEAR](expected, actual, absolute_range)` floating pointer numbers (with explicit `absolute_range`)
- `THAT(expression, m)`, generic matcher assertion: true if `expression` matcher matcher `m`

**Important**: In order for diagnostics to be correct for `...EQ/NEQ`-functions
the first parameter should be the expected value (in some cases constant) and
the second should be the one calculated by function under test.

Two kinds of behaviour on failures:

- **Fatal**: starts with `ASSERT` and stops execution
- **Non-fatal**: starts with `EXPECT` and continues execution

Sample:

```Cpp
ASSERT_EQ(x.size(), y.size()) << "Vectors x and y are of unequal length";
```

Values of left-hand and right-hand side expressions are printed upon assertion
failure. For the case of STL-containers the beginning of the containers are
printed (but not necessarily at the element indexes where the two containers
differ).

---
### Death tests with exit assertions

- `ASSERT_DEATH(statement, expected_message)`: assert that a `statement` does a
system call to `exit()` and prints a specific `expected_message` to standard
output

- `ASSERT_EXIT(statement, predicate, expected_message)`: assert that a
`statement` does a system call to `exit()` fulfilling `predicate` and prints a specific
`expected_message` to standard output

For instance:

```Cpp
ASSERT_EXIT(square-root (-22.0), ::testing::ExitedWithCode(-1), "Error: Negative Input");
```

---
## Defining a simple test

Arguments to `TEST` *must* be valid C++ symbols and shouldn't (but can) contain an underscore.

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

Here,

- the **test case** named `FactorialTest` contains
- the two **tests** named `HandlesZeroInput` and `HandlesPositiveInput`.

Test results are grouped by test cases, so logically-related tests should be
in the same test case.

Tests can be disabled by prefixing the symbolic name of either test case or the
test with the string `DISABLE_`. A warning will be printed at the end of the
test execution if there are any disabled tests. The flag
`-gtest_also_run_disabled_tests` overrides this behaviour and runs all the
disabled tests aswell.

---
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

---
## Running tests via manual main()

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

---
## Running tests via manual CMake

Alternatively, CMake provides a simpler interface for running the tests
automatically (through linking with `libgtest_main`). See [this
file](samples/CMakeLists.txt) for details.

---

## Checking behaviour with Google Mock

Now for some testing of behaviour with Google Mock.

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

**Important**: It's *preferred* but not required for methods to be virtual.

---
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

- Mocks should be defined in separate files, that is `MockTurtle` should be placed in `mock_turtle.h`.
- If you're lazy and optimistic you can use `scripts/generator/gmock_gen.py`.
- Note: Many other languages, both static and dynamic, can provides more
  automation of this stage.

To a mock class templates such as

```Cpp
template <typename Elem>
class Stack {
 public:
  ...
  virtual ~Stack();  // must still be virtual
  virtual int GetSize() const = 0;
};
```

(note that `~Stack()` must be virtual) just append `_T` to the `MOCK_*` macros:

```Cpp
template <typename Elem>
class MockStack : public Stack<Elem> {
 public:
  ...
  MOCK_CONST_METHOD0_T(GetSize, int());
};
```

---
## Using Mocks

1. Import
2. Create mock objects
3. Optionally, set default actions on mock objects (using `ON_CALL`)
4. Set expectations on behaviour of mock objects (using `EXPECT_CALL`)
5. Exercise code that use mock objects
6. Upon destruction, Google Mock verifies that expectations have been satisfied

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

---
## Using Google Mock with Any Testing Framework

```Cpp
int main(int argc, char** argv) {
  // The following line causes Google Mock to throw an exception on failure,
  // which will be interpreted by your testing framework as a test failure.

  ::testing::GTEST_FLAG(throw_on_failure) = true;
  ::testing::InitGoogleMock(&argc, argv);
  ... whatever your testing framework requires ...
}
```

---
## Setting Default Action

Customize the default action for (free) *functions* with return type `T` globally:

```Cpp
using ::testing::DefaultValue;

// Sets the default value to be returned. T must be CopyConstructible.
DefaultValue<T>::Set(value);
// Sets a factory. Will be invoked on demand. T must be MoveConstructible.
//   T MakeT();
DefaultValue<T>::SetFactory(&MakeT);
// ... use the mocks ...
// Resets the default value.
DefaultValue<T>::Clear();
```

To customize the default action for a particular *method*, use `ON_CALL()`:

```Cpp
ON_CALL(mock_object, method(matchers))
    .With(multi_argument_matcher)  ?
    .WillByDefault(action);
```

---
### Actions

Actions specify what a mock function should do when invoked, that is it *defines
behaviour*. Can be a

- [Return of a value](https://github.com/google/googletest/blob/master/googlemock/docs/CheatSheet.md#returning-a-value)
- [Side effect](https://github.com/google/googletest/blob/master/googlemock/docs/CheatSheet.md#side-effects)
- [Using a Function or a Functor as an Action](https://github.com/google/googletest/blob/master/googlemock/docs/CheatSheet.md#using-a-function-or-a-functor-as-an-action)
- [Default Action](https://github.com/google/googletest/blob/master/googlemock/docs/CheatSheet.md#default-action)
- [Composite Action](https://github.com/google/googletest/blob/master/googlemock/docs/CheatSheet.md#composite-actions)
- [Defining Action](https://github.com/google/googletest/blob/master/googlemock/docs/CheatSheet.md#defining-actions)

---
## Setting Expectations

Balance between setting too strict and too loose expectations.

`EXPECT_CALL()` sets an expectation (how will it be called? what will it do?) on
a mock method using a *declarative* domain-specific language. It's complete spec is:

```Cpp
...
EXPECT_CALL(mock_object, method(matchers)) // separated by comma (limitations in C/C++-preprocessor)
    .With(multi_argument_matcher)  // ?
    .Times(cardinality)            // ?
    .InSequence(sequences)         // *
    .After(expectations)           // *
    .WillOnce(action)              // *
    .WillRepeatedly(action)        // ?
    .RetiresOnSaturation();        // ?
```

enabling more compact specifications (and automatic error diagnostics) compared
to writing the `cardinality` loop ourselves.

---
### Sample Expectation

For instance,

```Cpp
using ::testing::Return;
...
EXPECT_CALL(turtle, GetX())
    .Times(5)                     // called 5 times
    .WillOnce(Return(100))        // return 100 first time,
    .WillOnce(Return(150))        // return 150 second time
    .WillRepeatedly(Return(200)); // return 200 the remaining times
```

---
### Matchers

A matcher matches a single argument. You can use it inside `ON_CALL()` or
`EXPECT_CALL()`, or use it to validate a value directly using either

- `EXPECT_THAT(value, matcher)`: asserts that value matches matcher
- `ASSERT_THAT(value, matcher)`: the same as `EXPECT_THAT(value, matcher)`,
  except that it generates a fatal failure

If you don't care about exact function parameter values use

```Cpp
using ::testing::_;
...
EXPECT_CALL(turtle, Forward(_)); // turtle moved forward by some arbitrary step
```

The underscore `_` here is called a *matcher* (as in Erlang's pattern
matching). [Here's](https://github.com/google/googletest/blob/master/googlemock/docs/CheatSheet.md#matchers)
a list of all the others.

If you instead want to match a parameter against a specific type `T` use

```Cpp
A<T>()
```

or

```Cpp
An<T>()
```

For more specific comparison matching use either

- `Eq(value)` or `value` : parameter == value
- `Ge(value)` : parameter >= value
- `Gt(value)` : parameter > value
- `Le(value)` : parameter <= value
- `Lt(value)` : parameter < value
- `Ne(value)` : parameter != value
- `IsNull()` : parameter is a `NULL` pointer (raw or smart)
- `NotNull()` : parameter is a non-null pointer (raw or smart)
- `Ref(variable)` : parameter is a reference to variable
- `TypedEq<type>(value)` : parameter has type type and is equal to value. You
  may need to use this instead of `Eq(value)` when the mock function is
  overloaded

For instance,


```Cpp
using ::testing::Ge;
...
EXPECT_CALL(turtle, Forward(Ge(100))); // turtle moved forward at least 100 steps
```

There are more matchers...:

- [Floating-Pointer Matchers](https://github.com/google/googletest/blob/master/googlemock/docs/CheatSheet.md#floating-point-matchers)
- [String Matchers](https://github.com/google/googletest/blob/master/googlemock/docs/CheatSheet.md#string-matchers)
- [STL-Container Matchers](https://github.com/google/googletest/blob/master/googlemock/docs/CheatSheet.md#string-matchers)
- [Member-Matchers](https://github.com/google/googletest/blob/master/googlemock/docs/CheatSheet.md#member-matchers)
- [Matching the Result of a Function or Functor](https://github.com/google/googletest/blob/master/googlemock/docs/CheatSheet.md#matching-the-result-of-a-function-or-functor)
- [Pointer Matchers](https://github.com/google/googletest/blob/master/googlemock/docs/CheatSheet.md#pointer-matchers)
- [Multiargument Matchers](https://github.com/google/googletest/blob/master/googlemock/docs/CheatSheet.md#multiargument-matchers)
- [Composite Matchers](https://github.com/google/googletest/blob/master/googlemock/docs/CheatSheet.md#composite-matchers)
- [Adapters for Matchers](https://github.com/google/googletest/blob/master/googlemock/docs/CheatSheet.md#adapters-for-matchers)
- [Matchers as Predicates](https://github.com/google/googletest/blob/master/googlemock/docs/CheatSheet.md#matchers-as-predicates)
- [Defining Matchers](https://github.com/google/googletest/blob/master/googlemock/docs/CheatSheet.md#defining-matchers)
- [Matchers as Test Assertions](https://github.com/google/googletest/blob/master/googlemock/docs/CheatSheet.md#matchers-as-test-assertions)

---
### Cardinality

The `cardinality` parameter to `.Times()` can be either

- `AtLeast(n)`
- `AtMost(n)`
- `Between(m, n)`
- `Exactly(n)` or just `n`

**Important!**: The `cardinality` can be zero, in the case when a member is
expected *not* to be called.

**Important!**: Omitting `Times()` will infer the `cardinality` accordingly:

- If neither `WillOnce()` nor `WillRepeatedly()` is in the `EXPECT_CALL()`, the
  inferred `cardinality` is `Times(1)`.
- If there are n `WillOnce()`'s but no `WillRepeatedly()`, where n >= 1, the `cardinality` is `Times(n)`.
- If there are n `WillOnce()`'s and one `WillRepeatedly()`, where n >= 0, the `cardinality` is `Times(AtLeast(n))`.

Compare this to a regexp `x? y? ... y*`.

**Important!**: If you want side-effects in expectations be careful because
evaluation order maybe not be what you expect.

**Open question!**: Why is there no `WillN()`?

---
### Quiz

To check that we have understood what does this mean?:

```Cpp
using ::testing::Return;
...
EXPECT_CALL(turtle, GetY())
.Times(4)
.WillOnce(Return(100));
```

Answer: It expects `GetY` to return 100 first time and 0 (default) the remaining
three times.

---
### Default return values

By default, mock object members get inferred return values when using C++11 (for default
constructable types).

```Cpp
using ::testing::Return;
...
EXPECT_CALL(turtle, GetX())
    .Times(10)
    .WillOnce(Return(100))
    .WillOnce(Return(200))
    .WillOnce(Return(300));
    // expected Return(0), 7 times, if that's the default value of the return type
```

after all `WillOnce()`s are used up, Google Mock will do the default action for
the function every time.

---
## Multiple Expectations

Very often you need multiple expectations as in

```Cpp
using ::testing::_;
...
EXPECT_CALL(turtle, Forward(_));  // #1
EXPECT_CALL(turtle, Forward(10))  // #2
    .Times(2);
```

<!-- *Important fact*: Google Mock will search the expectations in the reverse order -->
<!-- they are defined, and stop when an active expectation that matches the arguments -->
<!-- is found (you can think of it as "newer rules override older ones.") Compare -->
<!-- with C++ virtual member overrides. -->

---
## Ordered vs Unordered Calls

By default expections are not checked in ordered. If you want ordering you need
to do this:

```Cpp
using ::testing::InSequence;
...
TEST(FooTest, DrawsLineSegment) {
  ...
  {
    InSequence dummy; // a bit too implicit for my taste..

    EXPECT_CALL(turtle, PenDown());
    EXPECT_CALL(turtle, Forward(100));
    EXPECT_CALL(turtle, PenUp());
  }
  Foo();
}
```

---
## All Expectations are sticky (unless said otherwise)

Check that turtle is asked to go to the origin exactly twice (ignoring all other
instructions it receives)?

```Cpp
using ::testing::_;
...
EXPECT_CALL(turtle, GoTo(_, _))  // #1
    .Times(AnyNumber());
EXPECT_CALL(turtle, GoTo(0, 0))  // #2
    .Times(2);
```

If you don't want this use `RetiresOnSaturation` as in for instance

```Cpp
using ::testing::InSequence;
using ::testing::Return;
...
{
  InSequence s;

  for (int i = 1; i <= n; i++) {
    EXPECT_CALL(turtle, GetX())
        .WillOnce(Return(10*i))
        .RetiresOnSaturation();
  }
}
```

.

---
## Alternative tool: [dextool](https://github.com/joakim-brannstrom/dextool/)

- reuse of Clang's C/Cpp parser enables
  - 100 % correct Cpp parsing
  - fully automated mock generation
- used in Gripen software verification

---
## Reads (in order of importance)
- [The Little Mocker](https://8thlight.com/blog/uncle-bob/2014/05/14/TheLittleMocker.html)
- [Google test on Wikipedia](https://en.wikipedia.org/wiki/Google_Test)
- [Google test Primer](https://github.com/google/googletest/blob/master/googletest/docs/Primer.md)
- [Google Mock Cheat Sheet](https://github.com/google/googletest/blob/master/googlemock/docs/CheatSheet.md)

---
## Convert this document to HTML on Ubuntu via

```Shell
TMP=`tempfile --suffix=.html` && pandoc -s -f markdown_github -o $TMP README.md && $BROWSER $TMP
```

---
## Various

- Install Google Test on Ubuntu 17.04 via the command

```Shell
sudo apt install googletest
```

only installs source under `/usr/src/googletest`. To install the libs build and
install it with Googles own hack

```Shell
cd /usr/src/googletest/googletest
sudo mkdir build
cd build
sudo cmake ..
sudo make
sudo cp libgtest* /usr/lib/
cd ..
sudo rm -rf build
```

Not ideal.

```Shell
git clone --recurse-submodules https://github.com/google/googletest
cd googletest
cmake .
make
```

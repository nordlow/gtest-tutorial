#include <iostream>
#include <string>

#include <gtest/gtest.h>

using std::cout;
using std::endl;
using std::hex;
using std::dec;

int main(int argc,
         char * argv[], char * envp[])
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

#include <iostream>
#include <memory>

#include "TExample1.h"
//-------------------------------------------------------------------------------

using namespace std;
//-------------------------------------------------------------------------------

int main(int argc, char** argv)
{
  cerr << endl << "The program Example1 has started..." << endl << endl;

  unique_ptr<TExample1> Example1( new TExample1() );   // can be used starting from C++11
  //auto Example1 = make_unique<TExample1>();          // can be used starting from C++14

  Example1->Run();

  cerr << endl << "The program Example1 has finished successfully." << endl << endl;
}
//-------------------------------------------------------------------------------

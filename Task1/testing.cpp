#include <algorithm>
#include "Tank.h"
#include <iostream>
template <typename DataType>
void PrintTank(Tank<DataType> v)
{
    for(int i=0; i<v.size(); i++)
    {
        std::cout << v[i] << std::endl;
    }
}

int main() 
{

  using DataType = double;

  Tank<DataType> v;
  std::cout << v.size() << std::endl;
  for(int i=0; i<100; i++){
    v.push_back(7);
  }
  for(int i=0; i<50; i++){
    v.pop_back();
  }
  std::cout << v.size() << std::endl;
  //PrintTank(v);
  std::cout << accumulate(v);
}
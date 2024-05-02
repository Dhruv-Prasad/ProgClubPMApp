// ifndef used with define to create an include guard which prevents errors like "redifinition of..."
// when the header file is included more than once
#ifndef TANK_H 
#define TANK_H

template <typename DataType> //The template is used everywhere to make the class usable for all data types
class Tank {
public:
    Tank(); //constructor 
    ~Tank(); //destructor (frees up the memory once the class is no longer in use)

    void push_back(const DataType& value); //pushes value to the end of the tank
    DataType operator[](int index); //returns (index)th element
    DataType at(int index); //returns (index)th element 
    int size(); //returns the number of elements in the tank
    void pop_back(); //removes the last element of the tank

private:
    void reAllocate(int newCapacity); //Moves the old data to new memory block of size we want

private:
    DataType* Data = nullptr; //Pointer to our first element
    int Size = 0; //No. of elements in the Tank
    int Capacity = 0;//Max Capacity of the Tank
};

#include "Tank.cpp" // Include the implementation file

#endif // for the include guard
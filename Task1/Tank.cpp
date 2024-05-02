#include "Tank.h" //includes header file

template <typename DataType> //constructor
Tank<DataType>::Tank()
{
    reAllocate(1);
}

template <typename DataType> //destructor (frees up the memory once the class is no longer in use)
Tank<DataType>::~Tank()
{
    delete[] Data;
}

template <typename DataType> //pushes value to the end of the tank
void Tank<DataType>::push_back(const DataType& value)
{
    if (Size == Capacity) {
        reAllocate((3* Capacity)/2); //Allocating arbitrarily some extra memory to save time later
    }
    Data[Size] = value;
    Size++;
}

template <typename DataType>
DataType Tank<DataType>::operator[](int index) //returns (index)th element
{
    return Data[index];
}

template <typename DataType> //returns (index)th element
DataType Tank<DataType>::at(int index)
{
    return Data[index];
}

template <typename DataType> //returns the number of elements in the tank
int Tank<DataType>::size()
{
    return Size;
}

template <typename DataType> //removes the last element of the tank
void Tank<DataType>::pop_back()
{
    Size--;
    if (Size*2 < Capacity) {
        reAllocate(Capacity / 2);
    } //As I am just simply decreasing the size, old elements remain in memory, which get freed when
      // I reallocate (only if half the capacity becomes unfilled). Half because it's in the app.
}

template <typename DataType> //Moves the old data to new memory block of size we want
void Tank<DataType>::reAllocate(int newCapacity)
{
    DataType* newBlock = new DataType[newCapacity]; // defines the new block of memory

    for (int i = 0; i < Size; i++) {
        newBlock[i] = Data[i];
    } //copying over the data

    if (Size > newCapacity) {
        Size = newCapacity;
    } //if there are less elemts than targeted size (ideally shouldn't happen)

    delete[] Data; //Deletes old memory
    Data = newBlock;
    Capacity = newCapacity;
}

template <typename DataType>
DataType accumulate(Tank<DataType> v) // sum of all elements in the tank
{
    DataType sum = 0;
    for (int i = 0; i < v.size(); i++)
    {
        sum += v[i];
    }
    return sum;
}
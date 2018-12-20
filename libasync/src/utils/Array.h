/*
* MIT License
*
* Copyright (c) 2018 jipeng wu
* <recvfromsockaddr at gmail dot com>
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/

#pragma once
#include <cassert>
#include <exception>

namespace wjp{

// Container for objects that needs to be constructed from nondefault constructors.
// T does not need a copy assignment constructor or a default constructor.
template < class T >
class Array{
public:    
    using Iterator=T*;
    using ConstIterator=const T*;

    // Allocates memory. It does not call any constructor.
    Array(uint64_t cap) :
        buffer(reinterpret_cast<T*>(new char[sizeof(T)*cap])),   
        cap(cap) 
    {}

    // Destroys array. T must have a public destructor.
    ~Array(){
        for(uint64_t i=0;i<current_size;i++){
            buffer[i].~T();  
        }
        delete [] reinterpret_cast<char*>(buffer);
    }

    // Uses placement new to construct given Type in given buffer using any constructor. 
    template< class... Args > 
    void emplace_back(Args&&... args){
        assert(current_size<cap);
        new(reinterpret_cast<char*>(&buffer[current_size])) T{std::forward<Args>(args)...}; 
        current_size++;
    }

    // Const reference to the requested element with bounds checking.
    const T& at(uint64_t index) const{
        if(index<0||index>=current_size) throw std::out_of_range("index out of range!");
        return buffer[index%cap];
    }

    // Reference to the requested element with bounds checking.
    T& at(uint64_t index){
        if(index<0||index>=current_size) throw std::out_of_range("index out of range!");
        return buffer[index%cap];
    }

    // Const reference to the first element.
    const T& front() const{
        return buffer[0];
    }

    // Reference to the first element.
    T& front(){
        return buffer[0];
    }

    // Const reference to the last element.
    const T& back() const{
        return buffer[current_size-1];
    }

    // Reference to the last element.
    T& back(){
        return buffer[current_size-1];
    }

    // Const reference to the requested element without bounds checking.
    const T& operator[](uint64_t index) const noexcept{
        return buffer[index%cap];
    }

    // Reference to the requested element without bounds checking.
    T& operator[](uint64_t index) noexcept{
        return buffer[index%cap];
    }

    // Returns an iterator to the first element of the container. 
    Iterator begin(){
        return buffer;
    }

    // Returns a const iterator to the first element of the container. 
    ConstIterator cbegin(){
        return buffer;
    }

    // Returns an iterator to the end
    Iterator end(){
        return buffer+current_size;
    }

    // Returns a const iterator to the end. 
    ConstIterator cend(){
        return buffer+current_size;
    }

    // Returns the cap.
    uint64_t capacity() const noexcept{
        return cap;
    }

    // Checks whether the container is empty.
    bool empty() const noexcept{
        return current_size==0;
    }

    // Checks whether the container is full.
    bool full() const noexcept{
        return current_size==cap;
    }

    // Returns const pointer to the underlying array.
    const T* data() const noexcept{
        return buffer;
    }

    // Returns pointer to the underlying array.
    T* data() noexcept{
        return buffer;
    }

    // Removes and destructs all elements.
    void clear(){
        for(uint64_t i=0;i<current_size;i++){
            buffer[i].~T();  
        }
        current_size=0;
    }

    // Swaps the contents.
    void swap(Array& other) noexcept{
        std::swap(buffer,other.buffer);
        std::swap(current_size,other.current_size);
        std::swap(cap,other.cap);
    }

    // Returns number of elements.
    uint64_t size() const noexcept{
        return current_size;
    }

private:
    T* buffer;   
    uint64_t cap;     
    uint64_t current_size=0;
};




}
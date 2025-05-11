#include "MUI.hpp"

using namespace UI;

/*
POINTER TREE VERSION
*/




//Helper Data structures
namespace UI
{
    class MemoryPool
    {
        void* pool = nullptr;
        unsigned int capacity = 0; //Never going over 4gb
        unsigned int current_offset = 0; 
    public:
        MemoryPool(void* ptr, unsigned int cap) 
            : pool(ptr), capacity(cap), current_offset(0)
        {

        }
        void* Allocate(unsigned int bytes)
        {

            //(address + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1)
            unsigned int aligned_bytes = (bytes + 7) & ~7;
            unsigned int new_offset = current_offset + aligned_bytes; 
            if(new_offset < capacity)
            {
                void* ptr = ((char*)pool + current_offset);
                current_offset = new_offset;
                return ptr;
            }
            return nullptr;
        }
        template<typename T>
        T* New(unsigned int count)
        {
            return (T*)Allocate(count * sizeof(T));
        }
        void Reset()
        {
            current_offset = 0;
        }
        unsigned int GetCapacity()
        {
            return capacity;
        }
    };

    template<typename T, unsigned int CAPACITY>
    class ArrayStack
    {
        T data[CAPACITY]{};
        unsigned int size = 0;
    public:
        inline void Push(const T& node)
        {
            if(size < CAPACITY)
            {
                data[size] = node;
                size++;
            }
        }
        inline void Pop()
        {
            if(size > 0)
                size--;
        }
        inline T& Peak()
        {
            if(size > 0)
                return data[size - 1];
            throw "UI ArrayStack cant peak size of 0";
        }
        inline void Clear()
        {
            size = 0; 
        }
        inline unsigned int Size() const
        {
            return size;
        }
        inline unsigned int Capacity() const
        {
            return CAPACITY;
        }
        inline T& operator[](unsigned int index) 
        {
            if(index >= CAPACITY)
                throw "UI ArrayStack index out of scope";
            else
                return data[index];
        }
        inline T* Data()
        {
            return data;
        }
    };


}




namespace UI
{
    struct Node
    {
        Node* parent = nullptr;
        Node** children = nullptr;
        const StyleSheet* style_sheet = nullptr;
        short width, height;//Pixel values
        short x, y;
    };

    StyleSheet default_sheet;
    //ArrayStack<char, 100> stack_visual;
    //ArrayStack<Node, 100> stack1;
    alignas(8) char data[1024]; //1kb 8 byte aligned
    MemoryPool memory_pool(data, 1024);
    int* p = memory_pool.New<int>(10);


    
}


//sizes only need to get propogated up for content_percent

#include <iostream>
void UI::BeginDiv(const StyleSheet* sheet, DivMouseInfo* get_info)
{
    //stack_visual.Push('B');
    //Node node;
}
void UI::EndDiv()
{
    //stack_visual.Push('E');



}
void UI::Draw()
{
    //stack_visual.Push('\0');
    //std::cout<<stack_visual.Data()<<'\n';
    //stack_visual.Clear();



}











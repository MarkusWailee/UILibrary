#pragma once

#include <stdint.h>
#include <cassert>


namespace UI
{
    template<typename T, unsigned int CAPACITY>
    class Array;

    template<typename T, unsigned int CAPACITY>
    class FixedStack;

    template<typename T>    
    class Stack;

    class MemoryArena;

    template<typename T>
    class ObjectPool;


    template<typename T>
    class ArenaLL;

    template<typename T>
    class Map;

    void StringCopy(char* dst, const char* src, uint32_t size)
    {
        if(!size || !src || !dst) return;
        uint32_t i;
        for(i = 0; i<size-1 && src[i] != '\0'; i++)
            dst[i] = src[i];
        dst[i] = '\0';
    }
    bool StringCompare(const char* s1, const char* s2)
    {
        if(s1 == nullptr || s2 == nullptr)
            return false;
        while(*s1 && *s2)
        {
            if(*s1 != *s2)
                return false;
            s1++;
            s2++;
        }
        return *s1 == *s2;
    }


}

//Stack allocated data structures
namespace UI
{



    template<typename T, unsigned int CAPACITY>
    class Array
    {
        T data[CAPACITY];
        public:
        inline unsigned int Capacity() const;
        inline T& operator[](unsigned int index);
        inline T* Data();
    };




    template<typename T, unsigned int CAPACITY>
    class FixedStack
    {
        T data[CAPACITY];
        unsigned int size = 0;
    public:
        inline void Push(const T& value);
        inline void Pop();
        inline T& Peek();
        inline void Clear();
        inline bool IsEmpty() const;
        inline unsigned int Size() const;
        inline unsigned int Capacity() const;
        inline T& operator[](unsigned int index) ;
        inline T* Data();
    };



    template<typename T>    
    class Stack
    {
        T* data = nullptr;
        uint32_t size = 0;
        uint32_t capacity = 0;
        bool has_arena = false;
    public:
        Stack(uint32_t capacity, MemoryArena* arena = nullptr);
        ~Stack();
        void Push(const T& value);
        void Pop();
        T& Peek();
        uint32_t Size() const;
        uint32_t Capacity() const;
        bool IsEmpty() const;
        bool HasArena() const;
        T& operator[](uint32_t index);
    };





    class MemoryArena
    {
        char* data = nullptr;
        uint64_t capacity = 0;
        uint64_t current_offset = 0; 
    public:
        inline MemoryArena(uint64_t cap);
        inline ~MemoryArena();
        inline void ResizeAndReset(uint64_t bytes);
        inline void* Allocate(uint64_t bytes);

        template<typename T>
        inline T* New(uint64_t count);

        template<typename T>
        inline T* New();

        inline void Rewind(void* ptr);

        inline void Reset();
        inline uint64_t Capacity() const;
    };


    template<typename T>
    class ObjectPool
    {
        Stack<uint32_t> free_list;
        T* data = nullptr;
    public:
        ObjectPool(uint32_t obj_count, MemoryArena* arena = nullptr);
        ~ObjectPool();
        T* New();
        void Free(T* obj);
        uint32_t Size() const;
        uint32_t Capacity() const;
        bool IsExhausted() const;

    };


    template<typename T>
    class ArenaLL
    {
    public:
        struct Node 
        {
            Node* next = nullptr;
            T value;
        };
    private:
        Node* head = nullptr;
        Node* tail = nullptr;
    public:
        //returns address or nullptr if arena is out of space
        T* Add(const T& value, MemoryArena* arena);
        bool IsEmpty() const;
        //Just sets head/tail to nullptr
        void Clear();
        Node* GetHead();
    };



    template<typename T>
    class Map
    {
    public:
        struct Item
        {
            uint64_t key = 0;
            int next = -1;
            T value;
        };
        bool AllocateCapacity(uint32_t count, MemoryArena* arena);
        bool Insert(uint64_t key, const T& value);
        T* GetValue(uint64_t key);
        void Reset();
        bool ShouldResize() const;
        uint32_t Capacity() const;
        float GetLoadFactor() const;
    private:
        Item* data = nullptr;
        uint32_t cap1 = 0;
        uint32_t cap2 = 0;
        uint32_t size1 = 0;
        uint32_t size2 = 0;
    };


    template<typename T>
    class DoubleBufferMap
    {
    
        Map<T> front;
        Map<T> back;
    public:
        uint32_t Capacity() const;
        bool ShouldResize() const;
        bool AllocateBufferCapacity(uint32_t capacity, MemoryArena* arena);
        bool Insert(uint64_t key, const T& value);
        T* BackValue(uint64_t key);
        T* FrontValue(uint64_t key);
        void SwapBuffer();
    };
}



//Array
namespace UI
{
    template<typename T, unsigned int CAPACITY>
    inline unsigned int Array<T, CAPACITY>::Capacity() const
    {
        return CAPACITY;
    }
    template<typename T, unsigned int CAPACITY>
    inline T& Array<T, CAPACITY>::operator[](unsigned int index) 
    {
        assert(index < CAPACITY);
        return data[index];
    }
    template<typename T, unsigned int CAPACITY>
    inline T* Array<T, CAPACITY>::Data()
    {
        return data;
    }
}




//Fixed Stack
namespace UI
{
    template<typename T, unsigned int CAPACITY>
    inline void FixedStack<T, CAPACITY>::Push(const T& value)
    {
        assert(size < CAPACITY);
        data[size] = value;
        size++;
    }
    template<typename T, unsigned int CAPACITY>
    inline void FixedStack<T, CAPACITY>::Pop()
    {
        assert(size > 0);
        size--;
    }
    template<typename T, unsigned int CAPACITY>
    inline T& FixedStack<T, CAPACITY>::Peek()
    {
        assert(size > 0);
        return data[size - 1];
    }
    template<typename T, unsigned int CAPACITY>
    inline void FixedStack<T, CAPACITY>::Clear()
    {
        size = 0; 
    }
    template<typename T, unsigned int CAPACITY>
    inline unsigned int FixedStack<T, CAPACITY>::Size() const
    {
        return size;
    }
    template<typename T, unsigned int CAPACITY>
    inline unsigned int FixedStack<T, CAPACITY>::Capacity() const
    {
        return CAPACITY;
    }
    template<typename T, unsigned int CAPACITY>
    inline T& FixedStack<T, CAPACITY>::operator[](unsigned int index) 
    {
        assert(index < size);
        return data[index];
    }
    template<typename T, unsigned int CAPACITY>
    inline T* FixedStack<T, CAPACITY>::Data()
    {
        return data;
    }
    template<typename T, unsigned int CAPACITY>
    inline bool FixedStack<T, CAPACITY>::IsEmpty() const
    {
        return size == 0;
    }

}




//Stack
namespace UI
{

    template<typename T>
    Stack<T>::Stack(uint32_t capacity, MemoryArena* arena)
        : size(0), capacity(capacity), data(nullptr), has_arena(arena)
    {
        if(!arena)
        {
            data = new T[capacity];
        }
        else
        {
            data = arena->New<T>(capacity);
        }

        //Should always contain memory
        assert(data);
    }
    template<typename T>
    Stack<T>::~Stack()
    {
        if(!has_arena)
            delete[] data;

    }
    template<typename T>
    void Stack<T>::Push(const T& value)
    {
        assert(size < capacity);
        data[size] = value;
        size++;
    }
    template<typename T>
    void Stack<T>::Pop()
    {
        assert(size > 0);
        size--;
    }
    template<typename T>
    uint32_t Stack<T>::Size() const
    {
        return size;
    }

    template<typename T>
    uint32_t Stack<T>::Capacity() const
    {
        return capacity;
    }
    template<typename T>
    T& Stack<T>::Peek()
    {
        assert(size > 0);
        return data[size - 1];
    }

    template<typename T>
    T& Stack<T>::operator[](uint32_t index)
    {
        assert(index < size);
        return data[index];
    }
    template<typename T>
    bool Stack<T>::HasArena() const
    {
        return has_arena;
    }
    template<typename T>
    bool Stack<T>::IsEmpty() const
    {
        return size == 0;
    }

    //MemoryArena Implementation
    inline MemoryArena::MemoryArena(uint64_t bytes) 
        : capacity(bytes), current_offset(0), data(new char[bytes])
    {
        assert(data); //Over Capacity
    }
    inline MemoryArena::~MemoryArena()
    {
        delete[] data;
    }
    inline void MemoryArena::ResizeAndReset(uint64_t bytes)
    {
        if(data)
            delete[] data;
        data = new char[bytes];
        capacity = bytes;
        current_offset = 0;
        assert(data); //should not happen
    }
    inline void* MemoryArena::Allocate(uint64_t bytes)
    {
        uint64_t aligned_bytes = (bytes + 7) & ~7;
        uint64_t new_offset = current_offset + aligned_bytes; 
        if(new_offset <= capacity)
        {
            void* ptr = (data + current_offset);
            current_offset = new_offset;
            return ptr;
        }
        return nullptr;
    }
    template<typename T>
    inline T* MemoryArena::New(uint64_t count)
    {
        T* temp = (T*)Allocate(count * sizeof(T)); 
        if(!temp) return nullptr;
        //initialize
        for(uint64_t i = 0; i<count; i++)
            temp[i] = T();
        return temp;
    }
    template<typename T>
    inline T* MemoryArena::New()
    {
        T* temp = (T*)Allocate(sizeof(T)); 
        if(!temp) return nullptr;
        //initialize;
        *temp = T();
        return temp;
    }
    inline void MemoryArena::Rewind(void* ptr)
    {
        if(ptr == nullptr)
            return;
        assert(ptr >= data && ptr <= data + capacity);
        assert((uintptr_t)ptr % 8 == 0);
        uint64_t new_offset = ((char*)ptr - data);
        current_offset = new_offset; 
    }
    inline void MemoryArena::Reset()
    {
        current_offset = 0;
    }
    inline uint64_t MemoryArena::Capacity() const
    {
        return capacity;
    }



    //ObjectPool Implementation
    template<typename T>
    inline ObjectPool<T>::ObjectPool(uint32_t obj_count, MemoryArena* arena): free_list(obj_count, arena)
    {
        if(arena)
        {
            data = arena->New<T>(obj_count);
        }
        else
        {
            data = new T[obj_count]{};
        }

        //Fill the free list
        for(uint32_t i = 0; i < free_list.Capacity(); i++)
            free_list.Push(i);

        //Should contain data
        assert(data);
    }
    template<typename T>
    inline ObjectPool<T>::~ObjectPool()
    {
        if(!free_list.HasArena())
            delete[] data;
    }
    template<typename T>
    inline T* ObjectPool<T>::New()
    {
        if(!free_list.IsEmpty())
        {
            uint32_t index = free_list.Peek();
            free_list.Pop();
            return (data + index);
        }
        return nullptr;
    }
    template<typename T>
    inline void ObjectPool<T>::Free(T* ptr)
    {
        assert(ptr);
        assert((uintptr_t)data <= (uintptr_t)ptr);
        uintptr_t index = ((uintptr_t)ptr - (uintptr_t)data) / sizeof(T);
        assert(index < free_list.Capacity());
        free_list.Push((uint32_t)index);
    }
    template<typename T>
    inline bool ObjectPool<T>::IsExhausted() const
    {
        return free_list.IsEmpty();
    }
    template<typename T>
    inline uint32_t ObjectPool<T>::Size() const
    {
        return free_list.Size();
    }
    template<typename T>
    inline uint32_t ObjectPool<T>::Capacity() const
    {
        return free_list.Capacity();
    }
    

    //ArenaLL Implementation
    template<typename T>
    inline T* ArenaLL<T>::Add(const T& value, MemoryArena* arena)
    {
        assert(arena);
        Node* temp = arena->New<ArenaLL<T>::Node>();
        if(!temp)
            return nullptr;
        temp->value = value;
        if(head == nullptr)
        {
            head = temp;
            tail = temp;
        }
        else
        {
            tail->next = temp;
            tail = temp;
        }
        return &temp->value;
    }

    template<typename T>
    inline typename ArenaLL<T>::Node* ArenaLL<T>::GetHead()
    {
        return head; 
    }
    template<typename T>
    inline void ArenaLL<T>::Clear()
    {
        head = nullptr;
        tail = nullptr;
    }
    template<typename T>
    inline bool ArenaLL<T>::IsEmpty() const
    {
        return head == nullptr;
    }



    //Map Implementation
    template<typename T>
    bool Map<T>::AllocateCapacity(uint32_t capacity, MemoryArena* arena)
    {
        assert(arena);
        data = arena->New<Item>(capacity);
        if(data == nullptr)
        {
            cap1 = 0;
            cap2 = 0;
            size1 = 0;
            size2 = 0;
            return false;
        }
        cap2 = capacity * 37 / 100;
        cap1 = capacity - cap2;
        return true;
    }
    template<typename T>
    bool Map<T>::Insert(uint64_t key, const T& value)
    {
        if(!key || data == nullptr)
            return false; //key should never be 0
        
        int index = key % cap1;
        if(data[index].key == 0)
        {
            data[index] = Item{key, -1, value};
            size1++;
            return true;
        }
        int prev = index;
        for(;index != -1; index = data[index].next)
        {
            if(data[index].key == key)
            {
                data[index].value = value;
                return true;
            }
            prev = index;
        }
        if(size2 < cap2)
        {
            int index2 = size2 + cap1;
            data[prev].next = index2;
            data[index2] = Item{key, -1, value};
            size2++;
            return true;
        }
        return false;
    }
    template<typename T>
    T* Map<T>::GetValue(uint64_t key)
    {
        if(data == nullptr)
            return nullptr;
        for(int index = key % cap1; index != -1; index = data[index].next)
        {
            if(data[index].key == key)
                return &data[index].value;
        }
        return nullptr;
    }
    template<typename T>
    void Map<T>::Reset()
    {
        for(uint32_t i = 0; i<cap1 + cap2; i++)
            data[i] = Item();
        size1 = 0;
        size2 = 0;
    }
    template<typename T>
    bool Map<T>::ShouldResize() const
    {
        return size2 >= cap2;
    }
    template<typename T>
    uint32_t Map<T>::Capacity() const
    {
        return cap1 + cap2;
    }
    template<typename T>
    float Map<T>::GetLoadFactor() const
    {
        return (float)size1/cap1;
    }


    template<typename T>
    bool DoubleBufferMap<T>::AllocateBufferCapacity(uint32_t capacity, MemoryArena* arena)
    {
        return front.AllocateCapacity(capacity, arena) && back.AllocateCapacity(capacity, arena);
    }
    template<typename T>
    bool DoubleBufferMap<T>::Insert(uint64_t key, const T& value)
    {
        return back.Insert(key, value);
    }
    template<typename T>
    T* DoubleBufferMap<T>::FrontValue(uint64_t key)
    {
        return front.GetValue(key);
    }
    template<typename T>
    T* DoubleBufferMap<T>::BackValue(uint64_t key)
    {
        return back.GetValue(key);
    }
    template<typename T>
    void DoubleBufferMap<T>::SwapBuffer()
    {
        Map<T> temp = back;
        back = front;
        front = temp;
        back.Reset();
    }
    template<typename T>
    bool DoubleBufferMap<T>::ShouldResize() const
    {
        return front.ShouldResize() || back.ShouldResize();
    }
    template<typename T>
    uint32_t DoubleBufferMap<T>::Capacity() const
    {
        return front.Capacity();
    }
}
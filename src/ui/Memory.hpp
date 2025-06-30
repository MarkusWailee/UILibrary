#pragma once

#include <stdint.h>
#include <cstring>
#include <cassert>


namespace UI::Internal
{
    template<typename T, unsigned int CAPACITY>
    class Array;

    template<typename T, unsigned int CAPACITY>
    class FixedStack;

    template<typename T>
    class Map;

    class MemoryArena;

    template<typename T>
    class ArenaLL;

    template<typename T>
    class ArenaMap;

    template<typename T>
    class ArenaDoubleBufferMap;
}

//Stack allocated data structures
namespace UI::Internal
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
    class Map
    {
    public:
        struct Item
        {
            uint64_t key = 0;
            T value;
        };
        ~Map();
        void Free();
        void Clear();
        void Grow();
        uint32_t Capacity() const;
        uint32_t Size() const;
        bool IsEmpty() const;
        T* Insert(uint64_t key, const T& value);
        T* GetValue(uint64_t key);
        void Remove(uint64_t key);
        bool ShouldResize() const;
    private:
        Item* data = nullptr;
        uint32_t capacity = 0;
        uint32_t size = 0;
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
        bool PopHead();
        Node* GetHead();
    };



    template<typename T>
    class ArenaMap
    {
    public:
        struct Item
        {
            uint64_t key = 0;
            int next = -1;
            T value;
        };
        bool AllocateCapacity(uint32_t count, MemoryArena* arena);
        void RewindArena(MemoryArena* arena); 
        T* Insert(uint64_t key, const T& value);
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
    class ArenaDoubleBufferMap
    {
    
        ArenaMap<T> front;
        ArenaMap<T> back;
    public:
        uint32_t Capacity() const;
        bool ShouldResize() const;
        bool AllocateBufferCapacity(uint32_t capacity, MemoryArena* arena);
        void RewindArena(MemoryArena* arena);
        T* Insert(uint64_t key, const T& value);
        T* BackValue(uint64_t key);
        T* FrontValue(uint64_t key);
        void SwapBuffer();
    };
}


namespace UI::Internal
{
    //Array
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

    //Fixed Stack
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

    template<typename T>
    inline Map<T>::~Map()
    {
        Free();
    }
    template<typename T>
    inline void Map<T>::Free()
    {
        if(data)
            delete[] data;
        data = nullptr;
        size = 0;
        capacity = 0;
    }
    template<typename T>
    inline void Map<T>::Clear()
    {
        for(uint32_t i = 0; i<capacity; i++)
            data[i] = Item();
        size = 0;
        capacity = 0;
    }
    template<typename T>
    inline void Map<T>::Grow()
    {
        uint32_t old_cap = capacity;
        Item* old_data = data;
        size = 0;
        capacity = IsEmpty()? 64: capacity * 2;
        data = new Item[capacity]{};
        if(old_data)
        {
            for(uint32_t i = 0; i<old_cap; i++)
            {
                Item& item = old_data[i];
                if(item.key != 0)
                {
                    Insert(item.key, item.value);
                }
            }
            delete[] old_data;
        }
    }
    template<typename T>
    inline uint32_t Map<T>::Size() const
    {
        return size;
    }
    template<typename T>
    inline uint32_t Map<T>::Capacity() const
    {
        return capacity;
    }
    template<typename T>
    inline bool Map<T>::IsEmpty() const
    {
        return capacity == 0;
    }
    template<typename T>
    inline bool Map<T>::ShouldResize() const
    {
        return size >= capacity * 70 / 100;
    }
    template<typename T>
    inline T* Map<T>::Insert(uint64_t key, const T& value)
    {
        if(ShouldResize())
            Grow();
        for(uint32_t i = 0; i < capacity; i++)
        {
            uint32_t index = (key + i) % capacity;
            Item& item = data[index];
            if(item.key == key)
            {
                item = Item{key, value};
                return &item.value;
            }
            else if(item.key == 0)
            {
                size++;
                item = Item{key, value};
                return &item.value;
            }
        }
        return nullptr; //most likely wont happen since we should have grown
    }
    template<typename T>
    inline T* Map<T>::GetValue(uint64_t key)
    {
        for(uint32_t i = 0; i < capacity; i++)
        {
            uint32_t index = (key + i) % capacity;
            Item& item = data[index];
            if(item.key == key)
            {
                return &item.value;
            }
            else if(item.key == 0)
            {
                return nullptr;
            }
        }
        return nullptr;
    }
    template<typename T>
    inline void Map<T>::Remove(uint64_t key)
    {
        for(uint32_t i = 0; i < capacity; i++)
        {
            uint32_t index = (key + i) % capacity;
            if(data[index].key == 0)
            {
                return;
            }
            else if(data[index].key == key)
            {
                size--;
                uint32_t prev = index;
                for(uint32_t j = 1; j < capacity; j++)
                {
                    uint32_t index2 = (index + j) % capacity;
                    if(data[index2].key == 0 || index2 == data[index2].key % capacity)
                    {
                        data[index] = data[prev];
                        data[prev] = Item();
                        break;
                    }
                    prev = index2;
                }
                return;
            }
        }
        return;
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
        {
            delete[] data;
            data = nullptr;
        }
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
    inline bool ArenaLL<T>::PopHead()
    {
        if(head == nullptr)
            return false;
        head = head->next;
        return true;
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
    bool ArenaMap<T>::AllocateCapacity(uint32_t capacity, MemoryArena* arena)
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
    void ArenaMap<T>::RewindArena(MemoryArena* arena)
    {
        assert(arena && "No Arena passed");
        if(arena)
        {
            size1 = size2 = cap1 = cap2 = 0;
            arena->Rewind(data);
            data = nullptr;
        }
    }
    template<typename T>
    T* ArenaMap<T>::Insert(uint64_t key, const T& value)
    {
        if(!key || data == nullptr)
            return nullptr; //key should never be 0
        
        int index = key % cap1;
        if(data[index].key == 0)
        {
            data[index] = Item{key, -1, value};
            size1++;
            return &data[index].value;
        }
        int prev = index;
        for(;index != -1; index = data[index].next)
        {
            if(data[index].key == key)
            {
                data[index].value = value;
                return &data[index].value;
            }
            prev = index;
        }
        if(size2 < cap2)
        {
            int index2 = size2 + cap1;
            data[prev].next = index2;
            data[index2] = Item{key, -1, value};
            size2++;
            return &data[index2].value;
        }
        return nullptr;
    }
    template<typename T>
    T* ArenaMap<T>::GetValue(uint64_t key)
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
    void ArenaMap<T>::Reset()
    {
        for(uint32_t i = 0; i<cap1 + cap2; i++)
            data[i] = Item();
        size1 = 0;
        size2 = 0;
    }
    template<typename T>
    bool ArenaMap<T>::ShouldResize() const
    {
        return size2 >= cap2;
    }
    template<typename T>
    uint32_t ArenaMap<T>::Capacity() const
    {
        return cap1 + cap2;
    }
    template<typename T>
    float ArenaMap<T>::GetLoadFactor() const
    {
        return (float)size1/cap1;
    }


    template<typename T>
    bool ArenaDoubleBufferMap<T>::AllocateBufferCapacity(uint32_t capacity, MemoryArena* arena)
    {
        return front.AllocateCapacity(capacity, arena) && back.AllocateCapacity(capacity, arena);
    }
    template<typename T>
    void ArenaDoubleBufferMap<T>::RewindArena(MemoryArena* arena)
    {
        assert(arena && "No arena sent");
        back.RewindArena(arena);
        front.RewindArena(arena);
    }
    template<typename T>
    T* ArenaDoubleBufferMap<T>::Insert(uint64_t key, const T& value)
    {
        return back.Insert(key, value);
    }
    template<typename T>
    T* ArenaDoubleBufferMap<T>::FrontValue(uint64_t key)
    {
        return front.GetValue(key);
    }
    template<typename T>
    T* ArenaDoubleBufferMap<T>::BackValue(uint64_t key)
    {
        return back.GetValue(key);
    }
    template<typename T>
    void ArenaDoubleBufferMap<T>::SwapBuffer()
    {
        ArenaMap<T> temp = back;
        back = front;
        front = temp;
        back.Reset();
    }
    template<typename T>
    bool ArenaDoubleBufferMap<T>::ShouldResize() const
    {
        return front.ShouldResize() || back.ShouldResize();
    }
    template<typename T>
    uint32_t ArenaDoubleBufferMap<T>::Capacity() const
    {
        return front.Capacity();
    }
}
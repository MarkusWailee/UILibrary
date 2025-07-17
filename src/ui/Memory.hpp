#pragma once

#include <cstdint>
#include <cstring>
#include <cassert>
#include <type_traits>

#include <chrono>
class StopWatch
{
public:
    void Start()
    {
        start_time = std::chrono::high_resolution_clock::now();
    }

    double Stop()
    {
        auto end_time = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> duration_ms = end_time - start_time;
        return duration_ms.count(); // milliseconds as double
    }

private:
    std::chrono::high_resolution_clock::time_point start_time;
};

namespace UI::Internal
{
    


    template<typename T, unsigned int CAPACITY>
    class FixedArray;

    template<typename T, unsigned int CAPACITY>
    class FixedStack;

    template<typename T, uint32_t CAPACITY>
    class FixedQueue;

    template<typename T>
    class ArrayView;

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

namespace UI::Internal
{
    // ========== Useful Hash Functions ==========
    constexpr uint64_t HashCombine(uint64_t seed, uint64_t value);
    template<typename T>
    constexpr typename std::enable_if<std::is_enum<T>::value, uint64_t>::type 
    CastToU64(T value);
    template<typename T>
    constexpr typename std::enable_if<std::is_integral<T>::value, uint64_t>::type
    CastToU64(T value);
    template<typename T>
    typename std::enable_if<!std::is_integral<T>::value && !std::is_enum<T>::value, uint64_t>::type
    CastToU64(T value);

    template<typename T>
    uint64_t Hash(T value);

    uint64_t HashBytes(const void* src, uint64_t byte_count);
    // ===========================================




    //Stack allocated data structures
    template<typename T, unsigned int CAPACITY>
    class FixedArray
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
        T data[CAPACITY]{};
        uint32_t size = 0;
    public:
        void Push(const T& value);
        void Pop();
        T& Peek();
        void Clear();
        bool IsEmpty() const;
        bool IsFull() const;
        uint32_t Size() const;
        uint32_t Capacity() const;
        T& operator[](unsigned int index) ;
        T* Data();
    };

    template<typename T, uint32_t CAPACITY>
    class FixedQueue
    {
        T data[CAPACITY]{};
        uint32_t size = 0;
        uint32_t front = 0;
        uint32_t back = 0;
    public:
        void Push(const T& value);
        void Pop();
        T& Front();
        T& Back();
        void Clear();
        bool IsFull() const;
        bool IsEmpty() const;
        uint32_t Size() const;
        uint32_t Capacity() const;
    };

    template<typename T>
    class ArrayView
    {
    public:
        bool IsEmpty() const;
        uint64_t Size() const; 
        T& operator[](uint64_t index);
        const T& operator[](uint64_t index) const;
        T* data = nullptr;
        uint64_t size = 0;
    };

    template<typename T>
    class ArrayViewConst
    {
    public:
        bool IsEmpty() const;
        uint64_t Size() const; 
        T operator[](uint64_t index) const;
        const T* data = nullptr;
        uint64_t size = 0;
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
        MemoryArena(uint64_t cap);
        ~MemoryArena();
        void ResizeAndReset(uint64_t bytes);
        void* Allocate(uint64_t bytes, uint8_t alignment = 8);

        template<typename T>
        T* NewArray(uint64_t count);

        template<typename T>
        T* NewArrayZero(uint64_t count);

        template<typename T>
        T* New();
        template<typename T>
        T* New(const T& value);

        void Rewind(void* ptr);

        void Reset();
        uint64_t GetOffset() const;
        uint64_t Capacity() const;
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
        Node* GetTail();
    };



    template<typename T>
    class ArenaMap
    {
        friend class ArenaDoubleBufferMap<T>;
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
        void Reset();
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
    // ============= Useful Hash functions ======================
    template<typename T>
    inline constexpr typename std::enable_if<std::is_enum<T>::value, uint64_t>::type
    CastToU64(T value)
    {
        return (uint64_t)value;
    }
    template<typename T>
    inline constexpr typename std::enable_if<std::is_integral<T>::value, uint64_t>::type
    CastToU64(T value)
    {
        return (uint64_t)value;
    }
    template<typename T>
    inline typename std::enable_if<!std::is_integral<T>::value && !std::is_enum<T>::value, uint64_t>::type
    CastToU64(T value)
    {
        static_assert(sizeof(T) <= sizeof(uint64_t) && "Type cannot be larger than uint64_t");
        uint64_t result = 0;
        std::memcpy(&result, &value, sizeof(T));
        return result;
    }

    template<typename T>
    inline uint64_t Hash(T value)
    {
        assert(0 && "No hash function provided");
        return 0;
    }
    inline constexpr uint64_t HashCombine(uint64_t seed, uint64_t value) 
    {
        return seed ^ (value + 0x9e3779b9 + (seed << 6) + (seed >> 2));
    }
    inline uint64_t HashBytes(const void* src, uint64_t byte_count)
    {
        constexpr uint64_t FNV1_PRIME = 1099511628211ULL;
        uint64_t size1 = byte_count & ~7;
        uint64_t size2 = byte_count - size1;

        const uint8_t* ptr = (const uint8_t*)src;
        const uint8_t* ptr2 = ptr + size1;

        uint64_t hash = 14695981039346656037ULL;
        for(uint64_t i = 0; i < size1; i += 8)
        {
            uint64_t x = 0;
            std::memcpy(&x, ptr + i, 8);
            hash ^= x;
            hash *= FNV1_PRIME;
        }
        for(uint64_t i = 0; i < size2; i++)
        {
            hash ^= ptr2[i];
            hash *= FNV1_PRIME;
        }
        return hash;
    }
    // ========================================================



    //Array
    template<typename T, unsigned int CAPACITY>
    inline unsigned int FixedArray<T, CAPACITY>::Capacity() const
    {
        return CAPACITY;
    }
    template<typename T, unsigned int CAPACITY>
    inline T& FixedArray<T, CAPACITY>::operator[](unsigned int index) 
    {
        assert(index < CAPACITY);
        return data[index];
    }
    template<typename T, unsigned int CAPACITY>
    inline T* FixedArray<T, CAPACITY>::Data()
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
    inline uint32_t FixedStack<T, CAPACITY>::Size() const
    {
        return size;
    }
    template<typename T, unsigned int CAPACITY>
    inline uint32_t FixedStack<T, CAPACITY>::Capacity() const
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
    template<typename T, unsigned int CAPACITY>
    inline bool FixedStack<T, CAPACITY>::IsFull() const
    {
        return size == CAPACITY;
    }


    template<typename T, uint32_t CAPACITY>
    void FixedQueue<T, CAPACITY>::Push(const T& value)
    {
        assert(!IsFull() && "Queue is full");
        data[back] = value;
        back = (back + 1) % CAPACITY;
        size++;
    }
    template<typename T, uint32_t CAPACITY>
    void FixedQueue<T, CAPACITY>::Pop()
    {
        assert(!IsEmpty() && "Queue is empty");
        front = (front + 1) % CAPACITY;
        size--;
    }
    template<typename T, uint32_t CAPACITY>
    T& FixedQueue<T, CAPACITY>::Front()
    {
        assert(!IsEmpty() && "Queue is empty");
        return data[front];
    }
    template<typename T, uint32_t CAPACITY>
    T& FixedQueue<T, CAPACITY>::Back()
    {
        assert(!IsEmpty() && "Queue is empty");
        uint32_t i = back == 0? CAPACITY - 1: back - 1;
        return data[i];
    }
    template<typename T, uint32_t CAPACITY>
    void FixedQueue<T, CAPACITY>::Clear()
    {
        size = 0;
        front = 0;
        back = 0;
    }
    template<typename T, uint32_t CAPACITY>
    bool FixedQueue<T, CAPACITY>::IsFull() const
    {
        return size >= CAPACITY;
    }
    template<typename T, uint32_t CAPACITY>
    bool FixedQueue<T, CAPACITY>::IsEmpty() const
    {
        return size == 0;
    }
    template<typename T, uint32_t CAPACITY>
    uint32_t FixedQueue<T, CAPACITY>::Size() const
    {
        return size;
    }
    template<typename T, uint32_t CAPACITY>
    uint32_t FixedQueue<T, CAPACITY>::Capacity() const
    {
        return CAPACITY;
    }

    template<typename T>
    inline uint64_t ArrayView<T>::Size() const
    {
        return size;
    }
    template<typename T>
    inline bool ArrayView<T>::IsEmpty() const
    {
        return size == 0 || data == nullptr;
    }
    template<typename T>
    inline T& ArrayView<T>::operator[](uint64_t index)
    {
        assert(index < size && "ArrayView out of scope");
        return data[index];
    }
    template<typename T>
    inline const T& ArrayView<T>::operator[](uint64_t index) const
    {
        assert(index < size && "ArrayView out of scope");
        return data[index];
    }

    template<typename T>
    inline uint64_t ArrayViewConst<T>::Size() const
    {
        return size;
    }
    template<typename T>
    inline bool ArrayViewConst<T>::IsEmpty() const
    {
        return size == 0 || data == nullptr;
    }
    template<typename T>
    inline T ArrayViewConst<T>::operator[](uint64_t index) const
    {
        assert(index < size && "ArrayView out of scope");
        return data[index];
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
    inline void* MemoryArena::Allocate(uint64_t bytes, uint8_t alignment)
    {
        assert(bytes && "0 byte");
        assert((alignment & (alignment - 1)) == 0 && "Alignment must be power of 2");
        alignment--;
        current_offset = (current_offset + alignment) & ~alignment;
        uint64_t new_offset = current_offset + bytes; 
        if(new_offset <= capacity)
        {
            void* ptr = (data + current_offset);
            current_offset = new_offset;
            return ptr;
        }
        return nullptr;
    }
    template<typename T>
    inline T* MemoryArena::NewArray(uint64_t count)
    {
        assert(count && "0 count");
        T* temp = (T*)Allocate(count * sizeof(T), alignof(T)); 
        if(!temp) return nullptr;
        //initialize
        for(uint64_t i = 0; i<count; i++)
            temp[i] = T();
        return temp;
    }
    template<typename T>
    inline T* MemoryArena::NewArrayZero(uint64_t count)
    {
        assert(count && "0 count");
        T* temp = (T*)Allocate(count * sizeof(T), alignof(T)); 
        if(!temp) return nullptr;
        //initialize
        memset(temp, 0, count * sizeof(T));
        return temp;
    }
    template<typename T>
    inline T* MemoryArena::New()
    {
        T* temp = (T*)Allocate(sizeof(T), alignof(T)); 
        if(!temp) return nullptr;
        *temp = T();
        return temp;
    }
    template<typename T>
    inline T* MemoryArena::New(const T& value)
    {
        T* temp = (T*)Allocate(sizeof(T), alignof(T)); 
        if(!temp) return nullptr;
        *temp = value;
        return temp;
    }
    inline void MemoryArena::Rewind(void* ptr)
    {
        if(ptr == nullptr)
            return;
        assert(ptr >= data && ptr < data + capacity);
        uint64_t new_offset = ((char*)ptr - data);
        if(new_offset < current_offset)
            current_offset = new_offset; 
    }
    inline void MemoryArena::Reset()
    {
        current_offset = 0;
    }
    inline uint64_t MemoryArena::GetOffset() const
    {
        return current_offset;
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
    inline typename ArenaLL<T>::Node* ArenaLL<T>::GetTail()
    {
        return tail; 
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
        data = arena->NewArray<Item>(capacity);
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
        //These buffers get swapped so we must rewind them in order
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
    template<typename T>
    void ArenaDoubleBufferMap<T>::Reset()
    {
        front.Reset();
        back.Reset();
    }
}
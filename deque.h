#pragma once

#include <iostream>
#include <vector>
#include <list>
#include "printer.h"

namespace Smoren::Containers {
    template <typename T>
    class Chunk {
    public:
        explicit Chunk(size_t capacity):
            capacity(capacity),
            _head(nullptr),
            _end(nullptr)
        {
            data = new T[capacity];
        }

        Chunk(const Chunk& chunk): Chunk(chunk.capacity) {
            if(!chunk.empty()) {
                _head = data+(chunk._head-chunk.data);
                _end = data+(chunk._end-chunk.data);

                std::copy(chunk._head, chunk._end, _head);
            }
        }

        ~Chunk() {
            delete[] data;
        }

        bool empty() const { return _head == _end; }
        bool full() const { return _head == &data[0] && _end == &data[capacity]; }
        bool full_left() const { return _head == &data[0]; }
        bool full_right() const { return _end == &data[capacity]; }

        T* begin() { return _head; }
        T* end() { return _end; }

        const T* begin() const { return _head; }
        const T* end() const { return _end; }

        T* rbegin() { return _end-1; }
        T* rend() { return _head-1; }

        const T* rbegin() const { return _end-1; }
        const T* rend() const { return _head-1; }

        void push_front(const T& value) {
            if(empty()) {
                _head = &data[0];
                _end = _head+1;
            } else {
                _head--;
            }
            *_head = value;
        }

        void push_back(const T& value) {
            if(empty()) {
                _head = &data[capacity-1];
                _end = &data[capacity];
                *_head = value;
            } else {
                *_end = value;
                _end++;
            }
        }

        void pop_front() {
            _head++;
        }

        void pop_back() {
            _end--;
        }

        T& operator [](size_t i) {
            return data[i];
        }

        size_t size() const {
            return _end - _head;
        }

        T* getData() const {
            return data;
        }

        void printData() const {
            std::cout << "<";

            for(size_t i=0; i<capacity; i++) {
                std::cout << data[i] << ", ";
            }

            std::cout << "> (";

            if(full()) {
                std::cout << "full, ";
            } else {
                std::cout << "not full, ";
            }

            if(empty()) {
                std::cout << "empty";
            } else {
                std::cout << "not empty";
            }

            std::cout << ")" << std::endl;
        }

        friend std::ostream& operator <<(std::ostream& stream, const Chunk& item) {
            return stream << "<" << Smoren::Tools::join(item, ", ") << ">";
        }

    protected:
        size_t capacity;
        T* data;
        T* _head;
        T* _end;
    };

    template <typename T>
    class ChunkPtrVector {
    public:
        ChunkPtrVector(): shiftLeft(0) {

        }

        Chunk<T>* operator [](size_t index) const {
            return data[index+shiftLeft];
        }

        Chunk<T>* front() {
            return data[shiftLeft];
        }

        Chunk<T>* back() {
            return data.back();
        }

        void push_back(Chunk<T>* chunk) {
            data.push_back(chunk);
        }

        void push_front(Chunk<T>* chunk) {
            if(shiftLeft == 0) {
                throw std::out_of_range("...");
            }
            data[--shiftLeft] = chunk;
        }

        bool can_push_front() const {
            return shiftLeft != 0;
        }

        void pop_back() {
            data.pop_back();
        }

        void pop_front() {
            ++shiftLeft;
        }

        size_t shift() const {
            return shiftLeft;
        }

        size_t size() const {
            return data.size()-shiftLeft;
        }

        bool empty() const {
            return !size();
        }

        friend std::ostream& operator <<(std::ostream& stream, const ChunkPtrVector<T>& d) {
            return stream << "SIZE: " << d.size() << ", FRONT: " << d.indexFront;
        }

    protected:
        std::vector< Chunk<T>* > data;
        size_t shiftLeft;
    };

    template <typename T>
    class Deque {
    public:
        class iterator;
        friend class iterator;
        class const_iterator;
        friend class const_iterator;

        explicit Deque(size_t chunkCapacity):
            chunkCapacity(chunkCapacity),
            leftShift(0),
            _size(0)
        {}

        iterator begin() {
            return iterator(data.begin()->begin(), data.begin(), this, 0);
        }
        iterator end() {
            return iterator(data.rbegin()->end(), --data.end(), this, size());
        }

        const const_iterator begin() const {
            return const_iterator(data.begin()->begin(), data.begin(), this, 0);
        }
        const const_iterator end() const {
            return const_iterator(data.rbegin()->end(), --data.end(), this, size());
        }

        iterator rbegin() {
            return iterator(data.rbegin()->rbegin(), --data.end(), this, size()-1);
        }
        iterator rend() {
            return iterator(data.begin()->rend(), data.begin(), this, -1);
        }

        const iterator rbegin() const {
            return iterator(data.rbegin()->rbegin(), --data.end(), this, size()-1);
        }
        const iterator rend() const {
            return iterator(data.rbegin()->rbegin(), --data.end(), this, -1);
        }

        void push_front(const T& value) {
            if(empty() || chunkLeft->full_left()) {
                addChunkToFront(createChunk(data.begin()));
                chunkLeft->push_back(value);
                leftShift = chunkCapacity-1;
            } else {
                chunkLeft->push_front(value);
                leftShift--;
            }
            _size++;
        }

        void push_back(const T& value) {
            if(empty() || chunkRight->full_right()) {
                addChunkToBack(createChunk(data.end()));
                chunkRight->push_front(value);
            } else if(chunkRight->empty()) {
                chunkRight->push_front(value);
            } else {
                chunkRight->push_back(value);
            }
            _size++;
        }

        void pop_front() {
            chunkLeft->pop_front();
            if(chunkLeft->empty()) {
                removeChunkFromFront();
                leftShift = 0;
            } else {
                ++leftShift;
            }
            _size--;
        }

        void pop_back() {
            chunkRight->pop_back();
            if(chunkRight->empty()) {
                removeChunkFromBack();
            }
            _size--;
        }

        T& operator [](const size_t& i) const {
            return getElementByIndex(i);
        }

        size_t size() const {
            return _size;
        }

        bool empty() const {
            return !_size;
        }

        size_t chunksCount() const {
            return data.size();
        }

        void printClusterSizes() {
            std::cout << "CLUSTER SIZES: ";
            for(const auto& c : data) {
                std::cout << c.size() << ", ";
            }
            std::cout << std::endl;
        }

        void printData() const {
            std::cout << std::endl;

            long int i = getLeftChunkIndex();
            for(auto& chunk : data) {
                std::cout << i++ << ": ";
                chunk.printData();
            }

            std::cout << std::endl;
        }

        void printDataVerbose() const {
            std::cout << "SIZE: " << size() << std::endl;
            std::cout << "LEFT SHIFT: " << getLeftShift() << std::endl;
            std::cout << "CLUSTERS COUNT: " << data.size() << " | " << (partLeft.size()+partRight.size()) << std::endl;
            std::cout << "{";
            for(size_t i=0; i<size(); i++) {
                std::cout << (*this)[i] << ", ";
            }
            std::cout << "}" << std::endl;
            printData();
            std::cout << "==========" << std::endl << std::endl;
        }

        size_t getLeftShift() const {
            return leftShift;
        }

        friend std::ostream& operator <<(std::ostream& stream, Deque<T> d) {
            return stream << d.data;
        }

    protected:
        size_t chunkCapacity;
        size_t leftShift;
        size_t _size;

        std::list< Chunk<T> > data;

        ChunkPtrVector<T> partLeft;
        ChunkPtrVector<T> partRight;

        Chunk<T>* chunkLeft = nullptr;
        Chunk<T>* chunkRight = nullptr;

        /**
         * @brief возвращает индекс самого левого чанка
         */
        int getLeftChunkIndex() const {
            if(partLeft.empty()) {
                return static_cast<long int>(partRight.shift());
            }
            return -static_cast<long int>(partLeft.size());
        }

        T& getElementByIndex(const size_t& index) const {
            int leftChunkIndex = getLeftChunkIndex();
            int chunkShift = static_cast<long int>((index+leftShift)/chunkCapacity) + leftChunkIndex;
            size_t chunkIndex;
            size_t itemIndex = (index+leftShift)%chunkCapacity;

            if(chunkShift >= 0) {
                chunkIndex = static_cast<size_t>(chunkShift);
                return (*(partRight[chunkIndex-partRight.shift()]))[itemIndex];
            }

            chunkIndex = static_cast<size_t>(-chunkShift-1);
            return (*(partLeft[chunkIndex-partLeft.shift()]))[itemIndex];
        }

        void addChunkToFront(Chunk<T>* chunk) {
            if(partRight.can_push_front()) {
                partRight.push_front(chunk);
            } else {
                partLeft.push_back(chunk);
            }
            chunkLeft = chunk;
            if(chunkRight == nullptr) {
                chunkRight = chunk;
            }
        }

        void addChunkToBack(Chunk<T>* chunk) {
            if(partLeft.can_push_front()) {
                partLeft.push_front(chunk);
            } else {
                partRight.push_back(chunk);
            }
            chunkRight = chunk;
            if(chunkLeft == nullptr) {
                chunkLeft = chunk;
            }
        }

        void removeChunkFromFront() {
            data.pop_front();
            if(!partLeft.empty()) {
                partLeft.pop_back();
            } else {
                partRight.pop_front();
            }

            if(!partLeft.empty()) {
                chunkLeft = partLeft.back(); // было front
            } else if(!partRight.empty()) {
                chunkLeft = partRight.front();
            } else {
                chunkLeft = chunkRight = nullptr;
            }
        }

        void removeChunkFromBack() {
            data.pop_back();
            if(!partRight.empty()) {
                partRight.pop_back();
            } else {
                partLeft.pop_front();
            }

            if(!partRight.empty()) {
                chunkRight = partRight.back();
            } else if(!partLeft.empty()) {
                chunkRight = partLeft.front(); // было back
            } else {
                chunkLeft = chunkRight = nullptr;
            }
        }

        Chunk<T>* createChunk(const typename std::list< Chunk<T> >::iterator position) {
            const auto& p = data.insert(position, std::move(Chunk<T>(chunkCapacity)));
            return &(*p);
        }
    };

    template <typename T>
    class Deque<T>::iterator : public std::iterator<std::random_access_iterator_tag, T, ptrdiff_t> {
    public:
        /**
         * @brief конструктор по умолчанию
         */
        iterator(T* ptr, const typename std::list< Chunk<T> >::iterator& chunkIt, Deque<T>* container, size_t index):
            ptr(ptr), chunkIt(chunkIt), container(container), index(index) {}

        bool operator==(const iterator& x) const {
            return ptr == x.ptr;
        }
        bool operator!=(const iterator& x) const {
            return !(*this == x);
        }
        T& operator*() {
            return *ptr;
        }
        iterator& operator++() {
            ptr++;
            index++;

            if(ptr == getEndPtr()) {
                return *this;
            }

            if(ptr == chunkIt->end()) {
                chunkIt++;
                ptr = chunkIt->begin();
            }

            return *this;
        }
        iterator operator++(int) {
            iterator tmp = *this;
            ++(*this);
            return tmp;
        }
        iterator& operator--() {
            ptr--;
            index--;

            if(ptr == getRendPtr()) {
                return *this;
            }

            if(ptr == chunkIt->rend()) {
                chunkIt--;
                ptr = chunkIt->rbegin();
            }

            return *this;
        }
        iterator operator--(int) {
            iterator tmp = *this;
            --(*this);
            return tmp;
        }
        T& operator[] (const size_t i) const {
            return (*container)[i];
        }
    //    iterator& operator+=(int n) {
    //        return this;
    //    }
    private:
        T* ptr;
        typename std::list< Chunk<T> >::iterator chunkIt;
        Deque<T>* container;
        size_t index;

        T* endPtr = nullptr;
        T* rendPtr = nullptr;

        T* getEndPtr() {
            if(endPtr == nullptr) {
                endPtr = container->end().ptr;
            }

            return endPtr;
        }

        T* getRendPtr() {
            if(rendPtr == nullptr) {
                rendPtr = container->data.begin()->rend();
            }

            return rendPtr;
        }
    };

    template <typename T>
    class Deque<T>::const_iterator : public std::iterator<std::random_access_iterator_tag, T, ptrdiff_t> {
    public:
        /**
         * @brief конструктор по умолчанию
         */
        const_iterator(T* ptr, const typename std::list< Chunk<T> >::iterator& chunkIt, Deque<T>* container):
            ptr(ptr), chunkIt(chunkIt), container(container) {}

        const_iterator(const iterator& it): const_iterator(it.ptr, it.chunkIt, it.container) {}

        bool operator==(const const_iterator& x) const {
            return ptr == x.ptr;
        }
        bool operator!=(const const_iterator& x) const {
            return !(*this == x);
        }
        const T& operator*() const {
            return *ptr;
        }
        const_iterator& operator++() {
            ptr++;

            if(ptr == getEndPtr()) {
                return *this;
            }

            if(ptr == chunkIt->end()) {
                chunkIt++;
                ptr = chunkIt->begin();
            }

            return *this;
        }
        const_iterator operator++(int) {
            const_iterator tmp = *this;
            ++(*this);
            return tmp;
        }
        const_iterator& operator--() {
            ptr--;

            if(ptr == getRendPtr()) {
                return *this;
            }

            if(ptr == chunkIt->rend()) {
                chunkIt--;
                ptr = chunkIt->rbegin();
            }

            return *this;
        }
        const_iterator operator--(int) {
            const_iterator tmp = *this;
            --(*this);
            return tmp;
        }
    private:
        T* ptr;
        typename std::list< Chunk<T> >::iterator chunkIt;
        Deque<T>* container;

        T* endPtr = nullptr;
        T* rendPtr = nullptr;

        T* getEndPtr() {
            if(endPtr == nullptr) {
                endPtr = container->end().ptr;
            }

            return endPtr;
        }

        T* getRendPtr() {
            if(rendPtr == nullptr) {
                rendPtr = container->data.begin()->rend();
            }

            return rendPtr;
        }
    };
}

#ifndef SJTU_VECTOR_HPP
#define SJTU_VECTOR_HPP

#include "exceptions.hpp"

#include <climits>
#include <cstddef>

namespace sjtu {
/**
 * a data container like std::vector
 * store data in a successive memory and support random access.
 */

template<typename T>
class simple_allocator {
public:
  T* allocate(size_t n) {
    if (n == 0) return nullptr;
    return static_cast<T*>(::operator new(n * sizeof(T)));
  }

  void deallocate(T* p, size_t) {
    ::operator delete(p);
  }

  template<typename... Args>
  void construct(T* p, Args&&... args) {
    new (p) T(std::forward<Args>(args)...);
  }

  void destroy(T* p) {
    p->~T();
  }
};

template<typename T>
class vector {
private:
  T* _start;
  T* _finish;
  T* _end_of_memory;
  simple_allocator<T> _alloc;

public:
  /**
   * a type for actions of the elements of a vector, and you should write
   *   a class named const_iterator with same interfaces.
   */
  /**
   * you can see RandomAccessIterator at CppReference for help.
   */
  class const_iterator;
  class iterator {
  // The following code is written for the C++ type_traits library.
  // Type traits is a C++ feature for describing certain properties of a type.
  // For instance, for an iterator, iterator::value_type is the type that the
  // iterator points to.
  // STL algorithms and containers may use these type_traits (e.g. the following
  // typedef) to work properly. In particular, without the following code,
  // @code{std::sort(iter, iter1);} would not compile.
  // See these websites for more information:
  // https://en.cppreference.com/w/cpp/header/type_traits
  // About value_type: https://blog.csdn.net/u014299153/article/details/72419713
  // About iterator_category: https://en.cppreference.com/w/cpp/iterator
    friend class const_iterator;
    friend class vector;

  public:
    using difference_type = std::ptrdiff_t;
    using value_type = T;
    using pointer = T*;
    using reference = T&;
    using iterator_category = std::random_access_iterator_tag;

  private:
    pointer _ptr;
    vector * _vec;
  
  public:
    iterator(pointer ptr = nullptr, vector * vec = nullptr) : _ptr(ptr), _vec(vec) {}
    iterator(const iterator &other) : _ptr(other._ptr), _vec(other._vec) {}
    /**
     * return a new iterator which pointer n-next elements
     * as well as operator-
     */
    iterator operator+(const int &n) const {
      return iterator(_ptr + n, _vec);
    }
    iterator operator-(const int &n) const {
      return iterator(_ptr - n, _vec);
    }
    // return the distance between two iterators,
    // if these two iterators point to different vectors, throw invaild_iterator.
    int operator-(const iterator &rhs) const {
      if (_vec != rhs._vec) {
        throw invalid_iterator();
      }
      return _ptr - rhs._ptr;
    }
    iterator& operator+=(const int &n) {
      _ptr += n;
      return *this;
    }
    iterator& operator-=(const int &n) {
      _ptr -= n;
      return *this;
    }
    // iter++
    iterator operator++(int) {
      iterator temp = *this;
      _ptr++;
      return temp;
    }
    // ++iter
    iterator& operator++() {
      _ptr++;
      return *this;
    }
    // iter--
    iterator operator--(int) {
      iterator temp = *this;
      _ptr--;
      return temp;
    }
    // --iter
    iterator& operator--() {
      _ptr--;
      return *this;
    }
    // *it
    T& operator*() const{
      if (_ptr < _vec->_start || _ptr >= _vec->_finish) {
        throw invalid_iterator();
      }
      return *_ptr;
    }
    /**
     * a operator to check whether two iterators are same (pointing to the same memory address).
     */
    bool operator==(const iterator &rhs) const {
      return _ptr == rhs._ptr && _vec == rhs._vec;
    }
    bool operator==(const const_iterator &rhs) const {
      return _ptr == rhs._ptr && _vec == rhs._vec;
    }
    /**
     * some other operator for iterator.
     */
    bool operator!=(const iterator &rhs) const {
      return !(*this == rhs);
    }
    bool operator!=(const const_iterator &rhs) const {
      return !(*this == rhs);
    }
    bool operator<(const iterator &rhs) const {
      return _ptr < rhs._ptr && _vec == rhs._vec;
    }
    bool operator>(const iterator &rhs) const {
      return _ptr > rhs._ptr && _vec == rhs._vec;
    }
    bool operator<=(const iterator &rhs) const {
      return !(*this > rhs);
    }
    bool operator>=(const iterator &rhs) const {
      return !(*this < rhs);
    }
  };

  /**
   * has same function as iterator, just for a const object.
   */
  class const_iterator {
    friend class iterator;
    friend class vector;

  public:
    using difference_type = std::ptrdiff_t;
    using value_type = T;
    using pointer = T*;
    using reference = T&;
    using iterator_category = std::random_access_iterator_tag;

  private:
    /*TODO*/
    pointer _ptr;
    const vector * _vec;
  public:
    const_iterator(pointer ptr = nullptr, const vector * vec = nullptr) : _ptr(ptr), _vec(vec) {}
    const_iterator(const const_iterator &other) : _ptr(other._ptr), _vec(other._vec) {}
    
    iterator operator+(const int &n) const {
      return const_iterator(_ptr + n, _vec);
    }
    iterator operator-(const int &n) const {
      return const_iterator(_ptr - n, _vec);
    }

    int operator-(const const_iterator &rhs) const {
      if (_vec != rhs._vec) {
        throw invalid_iterator();
      }
      return _ptr - rhs._ptr;
    }

    const_iterator& operator+=(const int &n) {
      _ptr += n;
      return *this;
    }
    const_iterator& operator-=(const int &n) {
      _ptr -= n;
      return *this;
    }

    const_iterator operator++(int) {
      const_iterator temp = *this;
      _ptr++;
      return temp;
    }
    const_iterator& operator++() {
      _ptr++;
      return *this;
    }
    const_iterator operator--(int) {
      const_iterator temp = *this;
      _ptr--;
      return temp;
    }
    const_iterator& operator--() {
      _ptr--;
      return *this;
    }

    const T& operator*() const{
      if (_ptr < _vec->_start || _ptr >= _vec->_finish) {
        throw invalid_iterator();
      }
      return *_ptr;
    }

    bool operator==(const iterator &rhs) const {
      return _ptr == rhs._ptr && _vec == rhs._vec;
    }
    bool operator==(const const_iterator &rhs) const {
      return _ptr == rhs._ptr && _vec == rhs._vec;
    }
    bool operator!=(const iterator &rhs) const {
      return !(*this == rhs);
    }
    bool operator!=(const const_iterator &rhs) const {
      return !(*this == rhs);
    }
    bool operator<(const iterator &rhs) const {
      return _ptr < rhs._ptr && _vec == rhs._vec;
    }
    bool operator>(const iterator &rhs) const {
      return _ptr > rhs._ptr && _vec == rhs._vec;
    }
    bool operator<=(const iterator &rhs) const {
      return !(*this > rhs);
    }
    bool operator>=(const iterator &rhs) const {
      return !(*this < rhs);
    }
  };

  // Constructs
  vector() : _start(nullptr), _finish(nullptr), _end_of_memory(nullptr) {}
  vector(const vector &other) {
    size_t size = other.size();
    _start = _alloc.allocate(size);
    _finish = _start + size;
    _end_of_memory = _finish;
    for (size_t i = 0; i < size; ++i) {
      _alloc.construct(_start + i, other._start[i]);
    }
  }
  // Destructor
  ~vector() {
    for (size_t i = 0; i < size(); ++i) {
      _alloc.destroy(_start + i);
    }
    _alloc.deallocate(_start, capacity());
  }
  // Assignment operator
  vector &operator=(const vector &other) {
    if (this != &other) {
      for (size_t i = 0; i < size(); ++i) {
        _alloc.destroy(_start + i);
      }
      _alloc.deallocate(_start, capacity());
      size_t size = other.size();
      _start = _alloc.allocate(size);
      _finish = _start + size;
      _end_of_memory = _finish;
      for (size_t i = 0; i < size; ++i) {
        _alloc.construct(_start + i, other._start[i]);
      }
    }
    return *this;
  }
  /**
   * assigns specified element with bounds checking
   * throw index_out_of_bound if pos is not in [0, size)
   */
  T & at(const size_t &pos) {
    if (pos >= size()) {
      throw index_out_of_bound();
    }
    return _start[pos];
  }
  const T & at(const size_t &pos) const {
    if (pos >= size()) {
      throw index_out_of_bound();
    }
    return _start[pos];
  }
  /**
   * assigns specified element with bounds checking
   * throw index_out_of_bound if pos is not in [0, size)
   * !!! Pay attentions
   *   In STL this operator does not check the boundary but I want you to do.
   */
  T & operator[](const size_t &pos) {
    if (pos >= size()) {
      throw index_out_of_bound();
    }
    return _start[pos];
  }
  const T & operator[](const size_t &pos) const {
    if (pos >= size()) {
      throw index_out_of_bound();
    }
    return _start[pos];
  }
  /**
   * access the first element.
   * throw container_is_empty if size == 0
   */
  const T & front() const {
    if (size() == 0) {
      throw container_is_empty();
    }
    return _start[0];
  }
  /**
   * access the last element.
   * throw container_is_empty if size == 0
   */
  const T & back() const {
    if (size() == 0) {
      throw container_is_empty();
    }
    return *(_finish - 1);
  }
  /**
   * returns an iterator to the beginning.
   */
  iterator begin() {
    return iterator(_start, this);
  }
  const_iterator begin() const {
    return const_iterator(_start, this);
  }
  const_iterator cbegin() const {
    return const_iterator(_start, this);
  }
  /**
   * returns an iterator to the end.
   */
  iterator end() {
    return iterator(_finish, this);
  }
  const_iterator end() const {
    return const_iterator(_finish, this);
  }
  const_iterator cend() const {
    return const_iterator(_finish, this);
  }
  /**
   * checks whether the container is empty
   */
  bool empty() const {
    return size() == 0;
  }
  /**
   * returns the number of elements
   */
  size_t size() const {
    return _finish - _start;
  }
  size_t capacity() const {
    return _end_of_memory - _start;
  }
  /**
   * clears the contents
   */
  void clear() {
    for (size_t i = 0; i < size(); ++i) {
      _alloc.destroy(_start + i);
    }
    _finish = _start;
  }
  /**
   * increases the capacity of the vector
   */
  void grow() {
    size_t old_size = size();
    size_t new_capacity = old_size == 0 ? 1 : old_size * 2;
    T* new_start = _alloc.allocate(new_capacity);
    for (size_t i = 0; i < old_size; ++i) {
      _alloc.construct(new_start + i, _start[i]);
    }
    for (size_t i = 0; i < old_size; ++i) {
      _alloc.destroy(_start + i);
    }
    _alloc.deallocate(_start, capacity());
    _start = new_start;
    _finish = _start + old_size;
    _end_of_memory = _start + new_capacity;
  }
  /**
   * inserts value before pos
   * returns an iterator pointing to the inserted value.
   */
  iterator insert(iterator pos, const T &value) {
    if (pos < begin() || pos > end()) {
      throw index_out_of_bound();
    }
    if (size() == capacity()) {
      size_t ind = pos - begin();
      grow();
      pos = begin() + ind;
    }

    if (size() == 0) {
      _alloc.construct(_finish, value);
      ++_finish;
      return begin();
    }

    if (pos == end()) {
      _alloc.construct(_finish, value);
      ++_finish;
      return iterator(_finish - 1, this);
    }

    _alloc.construct(_finish, *(_finish - 1));
    for (T* p = _finish - 1; p != pos._ptr; --p) {
      *p = *(p - 1);
    }
    *pos._ptr = value;
    ++_finish;
    return pos;
  }
  /**
   * inserts value at index ind.
   * after inserting, this->at(ind) == value
   * returns an iterator pointing to the inserted value.
   * throw index_out_of_bound if ind > size (in this situation ind can be size because after inserting the size will increase 1.)
   */
  iterator insert(const size_t &ind, const T &value) {
    if (ind > size()) {
      throw index_out_of_bound();
    }
    iterator pos = begin() + ind;
    return insert(pos, value);
  }
  /**
   * removes the element at pos.
   * return an iterator pointing to the following element.
   * If the iterator pos refers the last element, the end() iterator is returned.
   */
  iterator erase(iterator pos) {
    if (pos < begin() || pos >= end()) {
      throw index_out_of_bound();
    }
    for (iterator it = pos + 1; it < end(); ++it) {
      *(it - 1) = *it;
    }
    _alloc.destroy(_finish - 1);
    --_finish;
    return pos;
  }
  /**
   * removes the element with index ind.
   * return an iterator pointing to the following element.
   * throw index_out_of_bound if ind >= size
   */
  iterator erase(const size_t &ind) {
    if (ind >= size()) {
      throw index_out_of_bound();
    }
    iterator pos = begin() + ind;
    return erase(pos);
  }
  /**
   * adds an element to the end.
   */
  void push_back(const T &value) {
    if (size() == capacity()) {
      grow();
    }
    _alloc.construct(_finish, value);
    ++_finish;
  }
  /**
   * remove the last element from the end.
   * throw container_is_empty if size() == 0
   */
  void pop_back() {
    if (size() == 0) {
      throw container_is_empty();
    }
    --_finish;
    _alloc.destroy(_finish);
  }
};

}

#endif // SJTU_VECTOR_HPP

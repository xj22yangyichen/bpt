#ifndef SJTU_LIST_HPP
#define SJTU_LIST_HPP

#include "exceptions.hpp"
#include "algorithm.hpp"

#include <climits>
#include <cstddef>

namespace sjtu {
/**
 * a data container like std::list
 * allocate random memory addresses for data and they are doubly-linked in a list.
 */
template<typename T>
class list {
protected:
  class node {
  public:
    /**
     * add data members and constructors & destructor
     */
    T *data;
    node *prev;
    node *next;

    node() : data(nullptr), prev(nullptr), next(nullptr) {}
    node(const T &data) : data(new T(data)), prev(nullptr), next(nullptr) {}
    ~node() {
      delete data;
    }
  };

protected:
  /**
   * add data members for linked list as protected members
   */
  node *head;
  node *tail;
  size_t size_;
  /**
   * insert node cur before node pos
   * return the inserted node cur
   */
  node *insert(node *pos, node *cur) {
    if (!cur) throw invalid_iterator();
    if (!pos) {
      if (!head) {
        head = tail = cur;
      } else {
        tail->next = cur;
        cur->prev = tail;
        tail = cur;
      }
      size_++;
      return cur;
    }
    cur->prev = pos->prev;
    cur->next = pos;
    if (pos->prev) pos->prev->next = cur;
    pos->prev = cur;
    if (pos == head) head = cur;
    size_++;
    return cur;
  }
  /**
   * remove node pos from list (no need to delete the node)
   * return the removed node pos
   */
  node *erase(node *pos) {
    if (!pos) return nullptr;
    if (size_ == 1) {
      head = tail = nullptr;
      size_--;
      return pos;
    }
    if (pos == head) {
      head = head->next;
      if (head) head->prev = nullptr;
    } else {
      pos->prev->next = pos->next;
    }
    if (pos == tail) {
      tail = tail->prev;
      if (tail) tail->next = nullptr;
    } else {
      pos->next->prev = pos->prev;
    }
    size_--;
    return pos;
  }

public:
  class const_iterator;
  class iterator {
  private:
    /**
     * TODO add data members
     *   just add whatever you want.
     */
    node *ptr;
    list *container;

    friend class list;
    friend class const_iterator;
  public:
    iterator() : ptr(nullptr), container(nullptr) {}
    iterator(node *ptr, list *container) : ptr(ptr), container(container) {}
    iterator(const iterator &other) : ptr(other.ptr), container(other.container) {}
    iterator(const const_iterator &other) : ptr(other.ptr), container(other.container) {}
    /**
     * iter++
     */
    iterator operator++(int) {
      if (!ptr) throw invalid_iterator();
      iterator ret = *this;
      ptr = ptr->next;
      return ret;
    }
    /**
     * ++iter
     */
    iterator & operator++() {
      if (!ptr) throw invalid_iterator();
      ptr = ptr->next;
      return *this;
    }
    /**
     * iter--
     */
    iterator operator--(int) {
      if (ptr == container->head) throw invalid_iterator();
      iterator ret = *this;
      if (ptr == nullptr) {
        ptr = container->tail;
      } else {
        ptr = ptr->prev;
      }
      return ret;
    }
    /**
     * --iter
     */
    iterator & operator--() {
      if (ptr == container->head) throw invalid_iterator();
      if (ptr == nullptr) {
        ptr = container->tail;
      } else {
        ptr = ptr->prev;
      }
      return *this;
    }
    /**
     * TODO *it
     * remember to throw if iterator is invalid
     */
    T & operator *() const {
      if (!ptr) throw invalid_iterator();
      return *ptr->data;
    }
    /**
     * TODO it->field
     * remember to throw if iterator is invalid
     */
    T * operator ->() const {
      if (!ptr) throw invalid_iterator();
      return ptr->data;
    }
    /**
     * a operator to check whether two iterators are same (pointing to the same memory).
     */
    bool operator==(const iterator &rhs) const {
      return ptr == rhs.ptr && container == rhs.container;
    }
    bool operator==(const const_iterator &rhs) const {
      return ptr == rhs.ptr && container == rhs.container;
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
  };
  /**
   * TODO
   * has same function as iterator, just for a const object.
   * should be able to construct from an iterator.
   */
  class const_iterator {
  private:
    node *ptr;
    const list *container;

    friend class list;
    friend class iterator;
  public:
    const_iterator() : ptr(nullptr), container(nullptr) {}
    const_iterator(node *ptr, const list *container) : ptr(ptr), container(container) {}
    const_iterator(const const_iterator &other) : ptr(other.ptr), container(other.container) {}
    const_iterator(const iterator &other) : ptr(other.ptr), container(other.container) {}
    /**
    * iter++
    */
    const_iterator operator++(int) {
      if (!ptr) throw invalid_iterator();
      const_iterator ret = *this;
      ptr = ptr->next;
      return ret;
    }
    /**
     * ++iter
     */
    const_iterator & operator++() {
      if (!ptr) throw invalid_iterator();
      ptr = ptr->next;
      return *this;
    }
    /**
     * iter--
     */
    const_iterator operator--(int) {
      if (ptr == container->head) throw invalid_iterator();
      const_iterator ret = *this;
      if (ptr == nullptr) {
        ptr = container->tail;
      } else {
        ptr = ptr->prev;
      }
      return ret;
    }
    /**
     * --iter
     */
    const_iterator & operator--() {
      if (ptr == container->head) throw invalid_iterator();
      if (ptr == nullptr) {
        ptr = container->tail;
      } else {
        ptr = ptr->prev;
      }
      return *this;
    }
    /**
     * TODO *it
     * remember to throw if iterator is invalid
     */
    const T & operator *() const {
      if (!ptr) throw invalid_iterator();
      return *ptr->data;
    }
    /**
     * TODO it->field
     * remember to throw if iterator is invalid
     */
    const T * operator ->() const {
      if (!ptr) throw invalid_iterator();
      return ptr->data;
    }
     /**
     * a operator to check whether two iterators are same (pointing to the same memory).
     */
    bool operator==(const iterator &rhs) const {
      return ptr == rhs.ptr && container == rhs.container;
    }
    bool operator==(const const_iterator &rhs) const {
      return ptr == rhs.ptr && container == rhs.container;
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
  };
  /**
   * TODO Constructs
   * Atleast two: default constructor, copy constructor
   */
  list() : head(nullptr), tail(nullptr), size_(0) {}
  list(const list &other) : head(nullptr), tail(nullptr), size_(0) {
    for (node *cur = other.head; cur; cur = cur->next) {
      push_back(*cur->data);
    }
  }
  /**
   * TODO Destructor
   */
  virtual ~list() {
    clear();
  }
  /**
   * TODO Assignment operator
   */
  list &operator=(const list &other) {
    if (this != &other) {
      clear();
      for (node *cur = other.head; cur; cur = cur->next) {
        push_back(*cur->data);
      }
    }
    return *this;
  }
  /**
   * access the first / last element
   * throw container_is_empty when the container is empty.
   */
  const T & front() const {
    if (empty()) throw container_is_empty();
    return *head->data;
  }
  const T & back() const {
    if (empty()) throw container_is_empty();
    return *tail->data;
  }
  /**
   * returns an iterator to the beginning.
   */
  iterator begin() {
    return iterator(head, this);
  }
  const_iterator begin() const {
    return const_iterator(head, this);
  }
  const_iterator cbegin() const {
    return const_iterator(head, this);
  }
  /**
   * returns an iterator to the end.
   */
  iterator end() {
    return iterator(nullptr, this);
  }
  const_iterator end() const {
    return const_iterator(nullptr, this);
  }
  const_iterator cend() const {
    return const_iterator(nullptr, this);
  }
  /**
   * checks whether the container is empty.
   */
  virtual bool empty() const {
    return size_ == 0;
  }
  /**
   * returns the number of elements
   */
  virtual size_t size() const {
    return size_;
  }

  /**
   * clears the contents
   */
  virtual void clear() {
    while (!empty()) {
      pop_back();
    }
  }
  /**
   * insert value before pos (pos may be the end() iterator)
   * return an iterator pointing to the inserted value
   * throw if the iterator is invalid
   */
  virtual iterator insert(iterator pos, const T &value) {
    if (pos.container != this) throw invalid_iterator();
    node *cur = new node(value);
    if (!head) {
      head = tail = cur;
      size_++;
      return iterator(cur, this);
    } else {
      insert(pos.ptr, cur);
    }
    return iterator(cur, this);
  }
  /**
   * remove the element at pos (the end() iterator is invalid)
   * returns an iterator pointing to the following element, if pos pointing to the last element, end() will be returned.
   * throw if the container is empty, the iterator is invalid
   */
  virtual iterator erase(iterator pos) {
    if (empty()) throw container_is_empty();
    if (pos.container != this || pos.ptr == nullptr) throw invalid_iterator();
    node *next = pos.ptr->next;
    erase(pos.ptr);
    delete pos.ptr;
    return iterator(next, this);
  }
  /**
   * adds an element to the end
   */
  void push_back(const T &value) {
    node *cur = new node(value);
    if (!head) {
      head = tail = cur;
    } else {
      tail->next = cur;
      cur->prev = tail;
      tail = cur;
    }
    ++size_;
  }
  /**
   * removes the last element
   * throw when the container is empty.
   */
  void pop_back() {
    if (empty()) throw container_is_empty();
    node *cur = tail;
    if (size_ == 1) {
      head = tail = nullptr;
    } else {
      tail = tail->prev;
      if (tail) tail->next = nullptr;
    }
    delete cur;
    --size_;
  }
  /**
   * inserts an element to the beginning.
   */
  void push_front(const T &value) {
    node *cur = new node(value);
    if (!head) {
      head = tail = cur;
    } else {
      cur->next = head;
      head->prev = cur;
      head = cur;
    }
    ++size_;
  }
  /**
   * removes the first element.
   * throw when the container is empty.
   */
  void pop_front() {
    if (empty()) throw container_is_empty();
    node *cur = head;
    if (size_ == 1) {
      head = tail = nullptr;
    } else {
      head = head->next;
      if (head) head->prev = nullptr;
    }
    delete cur;
    --size_;
  }
  /**
   * sort the values in ascending order with operator< of T
   */
  void sort() {
    if (size_ <= 1) return;
    int mid = size_ / 2;
    list left, right;
    node *cur = head;
    for (int i = 0; i < mid; ++i) {
      left.push_back(*cur->data);
      cur = cur->next;
    }
    for (int i = mid; i < size_; ++i) {
      right.push_back(*cur->data);
      cur = cur->next;
    }
    left.sort();
    right.sort();
    *this = left;
    merge(right);
  }
  /**
   * merge two sorted lists into one (both in ascending order)
   * compare with operator< of T
   * container other becomes empty after the operation
   * for equivalent elements in the two lists, the elements from *this shall always precede the elements from other
   * the order of equivalent elements of *this and other does not change.
   * no elements are copied or moved
   */
  void merge(list &other) {
    if (this == &other) return;
    node *cur1 = head;
    node *cur2 = other.head;
    while (cur1 && cur2) {
      if (*cur2->data < *cur1->data) {
        node *next = cur2->next;
        other.erase(cur2);
        insert(cur1, cur2);
        cur2 = next;
      } else {
        cur1 = cur1->next;
      }
    }
    while (cur2) {
      node *next = cur2->next;
      other.erase(cur2);
      insert(nullptr, cur2);
      cur2 = next;
    }
    other.head = other.tail = nullptr;
    other.size_ = 0;
  }
  /**
   * reverse the order of the elements
   * no elements are copied or moved
   */
  void reverse() {
    node *cur = head;
    while (cur) {
      std::swap(cur->prev, cur->next);
      cur = cur->prev;
    }
    std::swap(head, tail);
  }
  /**
   * remove all consecutive duplicate elements from the container
   * only the first element in each group of equal elements is left
   * use operator== of T to compare the elements.
   */
  void unique() {
    if (empty()) return;
    node *cur = head;
    while (cur->next) {
      if (*cur->data == *cur->next->data) {
        node *to_delete = cur->next;
        cur->next = to_delete->next;
        if (to_delete == tail) {
          tail = cur;
        } else {
          to_delete->next->prev = cur;
        }
        delete to_delete;
        --size_;
      } else {
        cur = cur->next;
      }
    }
  }
};

}

#endif //SJTU_LIST_HPP

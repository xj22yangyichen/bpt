// 用斜堆实现的 priority_queue

#ifndef SJTU_PRIORITY_QUEUE_HPP
#define SJTU_PRIORITY_QUEUE_HPP

#include <cstddef>
#include <functional>
#include "exceptions.hpp"

namespace sjtu {
/**
 * @brief a container like std::priority_queue which is a heap internal.
 * **Exception Safety**: The `Compare` operation might throw exceptions for certain data.
 * In such cases, any ongoing operation should be terminated, and the priority queue should be restored to its original state before the operation began.
 */
template<typename T, class Compare = std::less<T>>
class priority_queue {
private:
    struct Node {
    T value;
    Node *left;
    Node *right;

    Node(const T &val) : value(val), left(nullptr), right(nullptr) {}

    bool operator<(const Node &other) const {
      Compare comp;
      return comp(other.value, value); // max-heap
    }
  };

  size_t _size;
  Node *root;

  void copy_node(Node *&this_node, Node *other_node) {
    if (!other_node) {
      this_node = nullptr;
      return;
    }
    this_node = new Node(other_node->value);
    copy_node(this_node->left, other_node->left);
    copy_node(this_node->right, other_node->right);
  }

  Node *merge_nodes(Node *a, Node *b) {
    if (!a) return b;
    if (!b) return a;
    if (*b < *a) {
      std::swap(a, b);
    }
    a->right = merge_nodes(a->right, b);
    std::swap(a->left, a->right);
    return a;
  }

  void dfs_clear(Node *node) {
    if (node) {
      dfs_clear(node->left);
      dfs_clear(node->right);
      delete node;
    }
  }
public:
  /**
   * @brief default constructor
   */
  priority_queue() : _size(0), root(nullptr) {}

  /**
   * @brief copy constructor
   * @param other the priority_queue to be copied
   */
  priority_queue(const priority_queue &other) : _size(other._size), root(nullptr) {
    copy_node(root, other.root);
  }

  /**
   * @brief deconstructor
   */
  ~priority_queue() {
    dfs_clear(root);
  }

  /**
   * @brief Assignment operator
   * @param other the priority_queue to be assigned from
   * @return a reference to this priority_queue after assignment
   */
  priority_queue &operator=(priority_queue other) {
    std::swap(root, other.root);
    std::swap(_size, other._size);
    return *this;
  }

  /**
   * @brief get the top element of the priority queue.
   * @return a reference of the top element.
   * @throws container_is_empty if empty() returns true
   */
  const T & top() const {
    if (empty()) {
      throw container_is_empty();
    }
    return root->value;
  }

  /**
   * @brief push new element to the priority queue.
   * @param e the element to be pushed
   */
  void push(const T &e) {
    Node *new_node = new Node(e);
    try {
      Node *new_root = merge_nodes(root, new_node);
      root = new_root;
      _size++;
    } catch (...) {
      delete new_node;
      throw;
    }
  }

  /**
   * @brief delete the top element from the priority queue.
   * @throws container_is_empty if empty() returns true
   */
  void pop() {
    if (empty()) {
      throw container_is_empty();
    }
    Node *old_root = root;
    root = merge_nodes(root->left, root->right);
    delete old_root;
    _size--;
  }

  /**
   * @brief clear the nodes recursively.
   */
  void clear() {
    dfs_clear(root);
    root = nullptr;
    _size = 0;
  }

  /**
   * @brief return the number of elements in the priority queue.
   * @return the number of elements.
   */
  size_t size() const {
    return _size;
  }

  /**
   * @brief check if the container is empty.
   * @return true if it is empty, false otherwise.
   */
  bool empty() const {
    return _size == 0;
  }

  /**
   * @brief merge another priority_queue into this one.
   * The other priority_queue will be cleared after merging.
   * The complexity is at most O(logn).
   * @param other the priority_queue to be merged.
   */
  void merge(priority_queue &other) {
    if (other.empty()) return;
    root = merge_nodes(root, other.root);
    _size += other._size;
    other.root = nullptr;
    other._size = 0;
  }
};

}

#endif
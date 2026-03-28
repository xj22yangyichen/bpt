#ifndef SJTU_MAP_HPP
#define SJTU_MAP_HPP

// only for std::less<T>
#include <functional>
#include <cstddef>
#include "utility.hpp"
#include "exceptions.hpp"

namespace sjtu {

template<
    class Key,
    class T,
    class Compare = std::less <Key>
> class map {
 public:
  typedef pair<const Key, T> value_type;

 private:
  struct Node {
    Node *fa, *lc, *rc;
    value_type *data;
    bool color;  // red = true, black = false
    bool is_right_child() const {
      return fa != nullptr && fa->rc == this;
    }
    bool is_red() const {
      return color;
    }
    Node(Node *fa, Node *lc, Node *rc, value_type *data, bool color) :
        fa(fa), lc(lc), rc(rc), data(data), color(color) {}
  };

  Node *header;
  // Node *nil;
  Compare comp;
  size_t size_;

  /**
   * copy helper function for copy constructor and assignment operator
   */
  void copy_node(Node *&dest, const Node *src, Node *fa) {
    if (src == nullptr) {
      dest = nullptr;
      return;
    }
    dest = new Node(fa, nullptr, nullptr, new value_type(*(src->data)), src->color);
    copy_node(dest->lc, src->lc, dest);
    copy_node(dest->rc, src->rc, dest);
  }

  /**
   * clear helper function for destructor and assignment operator
   */
  void clear_node(Node *&node) {
    if (node == nullptr) return;
    clear_node(node->lc);
    clear_node(node->rc);
    delete node->data;
    delete node;
    node = nullptr;
  }
  
  /**
   * find the minimum/maximum node in the subtree rooted at node.
   * return nullptr if the subtree is empty.
   */
  Node* find_min(Node* node) const {
    if (node == nullptr || node == header) return nullptr;
    while (node->lc != nullptr) node = node->lc;
    return node;
  }

  Node* find_max(Node* node) const {
    if (node == nullptr || node == header) return nullptr;
    while (node->rc != nullptr) node = node->rc;
    return node;
  }

  /**
   * rotate left/right at node.
   */
  void rotate_left(Node *node) {
    Node *child = node->rc;
    node->rc = child->lc;
    if (child->lc != nullptr) child->lc->fa = node;
    child->fa = node->fa;
    if (node->fa == header) {
      header->fa = child;
    } else if (node->is_right_child()) {
      node->fa->rc = child;
    } else {
      node->fa->lc = child;
    }
    child->lc = node;
    node->fa = child;
  }

  void rotate_right(Node *node) {
    Node *child = node->lc;
    node->lc = child->rc;
    if (child->rc != nullptr) child->rc->fa = node;
    child->fa = node->fa;
    if (node->fa == header) {
      header->fa = child;
    } else if (node->is_right_child()) {
      node->fa->rc = child;
    } else {
      node->fa->lc = child;
    }
    child->rc = node;
    node->fa = child;
  }

  /**
   * rotate left/right at node to fix the red-black tree.
   * the direction of rotation is determined by the position of node to its parent.
   */
  void fix_insert(Node *node) {
    while (node->fa != header && node->fa->is_red()) {
      Node *grand = node->fa->fa;
      if (node->fa == grand->lc) {
        Node *uncle = grand->rc;
        if (uncle != nullptr && uncle->is_red()) {
          node->fa->color = false;
          uncle->color = false;
          grand->color = true;
          node = grand;
        } else {
          if (node == node->fa->rc) {
            node = node->fa;
            rotate_left(node);
          }
          node->fa->color = false;
          grand->color = true;
          rotate_right(grand);
        }
      } else {
        Node *uncle = grand->lc;
        if (uncle != nullptr && uncle->is_red()) {
          node->fa->color = false;
          uncle->color = false;
          grand->color = true;
          node = grand;
        } else {
          if (node == node->fa->lc) {
            node = node->fa;
            rotate_right(node);
          }
          node->fa->color = false;
          grand->color = true;
          rotate_left(grand);
        }
      }
    }
    header->fa->color = false;
  }

  void fix_erase(Node *node, Node *p) {
    while (node != header->fa && (node == nullptr || !node->is_red())) {
      if (node == p->lc) {
        Node *brother = p->rc;
        if (brother->is_red()) {
          brother->color = false;
          p->color = true;
          rotate_left(p);
          brother = p->rc;
        }
        if ((brother->lc == nullptr || !brother->lc->is_red()) &&
            (brother->rc == nullptr || !brother->rc->is_red())) {
          brother->color = true;
          node = p;
          p = node->fa;
        } else {
          if (brother->rc == nullptr || !brother->rc->is_red()) {
            brother->lc->color = false;
            brother->color = true;
            rotate_right(brother);
            brother = p->rc;
          }
          brother->color = p->color;
          p->color = false;
          if (brother->rc != nullptr) brother->rc->color = false;
          rotate_left(p);
          node = header->fa;
          break;
        }
      } else {
        Node *brother = p->lc;
        if (brother->is_red()) {
          brother->color = false;
          p->color = true;
          rotate_right(p);
          brother = p->lc;
        }
        if ((brother->lc == nullptr || !brother->lc->is_red()) &&
            (brother->rc == nullptr || !brother->rc->is_red())) {
          brother->color = true;
          node = p;
          p = node->fa;
        } else {
          if (brother->lc == nullptr || !brother->lc->is_red()) {
            brother->rc->color = false;
            brother->color = true;
            rotate_left(brother);
            brother = p->lc;
          }
          brother->color = p->color;
          p->color = false;
          if (brother->lc != nullptr) brother->lc->color = false;
          rotate_right(p);
          node = header->fa;
          break;
        }
      }
    }
    if (node != nullptr) {
      node->color = false;
    }
  }

  void replace_node(Node *u, Node *v) {
    if (u->fa == header) {
      header->fa = v;
    } else if (u->is_right_child()) {
      u->fa->rc = v;
    } else {
      u->fa->lc = v;
    }
    if (v != nullptr) {
      v->fa = u->fa;
    }
  }

 public:
  /**
   * if there is anything wrong throw invalid_iterator.
   *     like it = map.begin(); --it;
   *       or it = map.end(); ++it;
   */
  class const_iterator;
  class iterator {
    friend class const_iterator;
    friend class map;
   private:
    Node *node_;
    map *map_;
   public:
    iterator() : node_(nullptr), map_(nullptr) {}

    iterator(Node *node, map *map) : node_(node), map_(map) {}

    iterator(const iterator &other) : node_(other.node_), map_(other.map_) {}

    iterator &operator=(const iterator &other) {
      node_ = other.node_;
      map_ = other.map_;
      return *this;
    }

    iterator operator++(int) {
      iterator res = *this;
      ++(*this);
      return res;
    }

    iterator &operator++() {
      if (node_ == map_->header) throw invalid_iterator();
      if (node_->rc != nullptr) {
        node_ = map_->find_min(node_->rc);
      } else {
        Node *p = node_->fa;
        while (p != map_->header && node_ == p->rc) {
          node_ = p;
          p = p->fa;
        }
        node_ = p;
      }
      return *this;
    }

    iterator operator--(int) {
      iterator res = *this;
      --(*this);
      return res;
    }

    iterator &operator--() {
      if (node_ == map_->header->lc) throw invalid_iterator();
      if (node_ == map_->header) {
        node_ = map_->header->rc;
      } else if (node_->lc != nullptr) {
        node_ = map_->find_max(node_->lc);
      } else {
        Node *p = node_->fa;
        while (p != map_->header && node_ == p->lc) {
          node_ = p;
          p = p->fa;
        }
        node_ = p;
      }
      return *this;
    }

    /**
     * a operator to check whether two iterators are same (pointing to the same memory).
     */
    value_type &operator*() const {
      if (node_ == map_->header) throw invalid_iterator();
      return *(node_->data);
    }

    bool operator==(const iterator &rhs) const {
      return node_ == rhs.node_ && map_ == rhs.map_;
    }

    bool operator==(const const_iterator &rhs) const {
      return node_ == rhs.node_ && map_ == rhs.map_;
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

    /**
     * for the support of it->first.
     * See <http://kelvinh.github.io/blog/2013/11/20/overloading-of-member-access-operator-dash-greater-than-symbol-in-cpp/> for help.
     */
    value_type *operator->() const
    noexcept {
      if (node_ == map_->header) throw invalid_iterator();
      return node_->data;
    }
  };

  class const_iterator {
    friend class iterator;
    friend class map;
   private:
    Node *node_;
    const map *map_;
   public:
    const_iterator() : node_(nullptr), map_(nullptr) {}

    const_iterator(Node *node, const map *map) : node_(node), map_(map) {}

    const_iterator(const const_iterator &other) : node_(other.node_), map_(other.map_) {}

    const_iterator(const iterator &other) : node_(other.node_), map_(other.map_) {}

    const_iterator &operator=(const const_iterator &other) {
      node_ = other.node_;
      map_ = other.map_;
      return *this;
    }

    const_iterator &operator=(const iterator &other) {
      node_ = other.node_;
      map_ = other.map_;
      return *this;
    }

    const_iterator operator++(int) {
      const_iterator res = *this;
      ++(*this);
      return res;
    }

    const_iterator &operator++() {
      if (node_ == map_->header) throw invalid_iterator();
      if (node_->rc != nullptr) {
        node_ = map_->find_min(node_->rc);
      } else {
        Node *p = node_->fa;
        while (p != map_->header && node_ == p->rc) {
          node_ = p;
          p = p->fa;
        }
        node_ = p;
      }
      return *this;
    }

    const_iterator operator--(int) {
      const_iterator res = *this;
      --(*this);
      return res;
    }

    const_iterator &operator--() {
      if (node_ == map_->header->lc) throw invalid_iterator();
      if (node_ == map_->header) {
        node_ = map_->header->rc;
      } else if (node_->lc != nullptr) {
        node_ = map_->find_max(node_->lc);
      } else {
        Node *p = node_->fa;
        while (p != map_->header && node_ == p->lc) {
          node_ = p;
          p = p->fa;
        }
        node_ = p;
      }
      return *this;
    }

    const value_type &operator*() const {
      if (node_ == map_->header) throw invalid_iterator();
      return *(node_->data);
    }

    bool operator==(const iterator &rhs) const {
      return node_ == rhs.node_ && map_ == rhs.map_;
    }

    bool operator==(const const_iterator &rhs) const {
      return node_ == rhs.node_ && map_ == rhs.map_;
    }

    bool operator!=(const iterator &rhs) const {
      return !(*this == rhs);
    }

    bool operator!=(const const_iterator &rhs) const {
      return !(*this == rhs);
    }

    value_type *operator->() const
    noexcept {
      if (node_ == map_->header) throw invalid_iterator();
      return node_->data;
    }
  };

  map() : size_(0) {
    header = new Node(nullptr, nullptr, nullptr, nullptr, false);
    header->lc = header->rc = header;
    header->fa = nullptr;
    comp = Compare();
  }

  map(const map &other) : size_(other.size_) {
    header = new Node(nullptr, nullptr, nullptr, nullptr, false);
    copy_node(header->fa, other.header->fa, header);
    comp = other.comp;
    header->lc = size_ == 0 ? header : find_min(header->fa);
    header->rc = size_ == 0 ? header : find_max(header->fa);
  }

  map &operator=(const map &other) {
    if (this == &other) return *this;
    clear();
    comp = other.comp;
    size_ = other.size_;
    copy_node(header->fa, other.header->fa, header);
    header->lc = size_ == 0 ? header : find_min(header->fa);
    header->rc = size_ == 0 ? header : find_max(header->fa);
    return *this;
  }

  ~map() {
    clear();
    delete header;
  }

  /**
   * access specified element with bounds checking
   * Returns a reference to the mapped value of the element with key equivalent to key.
   * If no such element exists, an exception of type `index_out_of_bound'
   */
  T &at(const Key &key) {
    iterator it = find(key);
    if (it == end()) throw index_out_of_bound();
    return it->second;
  }

  const T &at(const Key &key) const {
    const_iterator it = find(key);
    if (it == cend()) throw index_out_of_bound();
    return it->second;
  }

  /**
   * access specified element
   * Returns a reference to the value that is mapped to a key equivalent to key,
   *   performing an insertion if such key does not already exist.
   */
  T &operator[](const Key &key) {
    iterator it = find(key);
    if (it == end()) {
      pair<iterator, bool> res = insert(value_type(key, T()));
      return res.first->second;
    }
    return it->second;
  }

  /**
   * behave like at() throw index_out_of_bound if such key does not exist.
   */
  const T &operator[](const Key &key) const {
    const_iterator it = find(key);
    if (it == cend()) throw index_out_of_bound();
    return it->second;
  }

  /**
   * return a iterator to the beginning
   */
  iterator begin() {
    return iterator(header->lc, this);
  }

  const_iterator begin() const {
    return const_iterator(header->lc, this);
  }

  const_iterator cbegin() const {
    return const_iterator(header->lc, this);
  }

  /**
   * return a iterator to the end
   * in fact, it returns past-the-end.
   */
  iterator end() {
    return iterator(header, this);
  }

  const_iterator end() const {
    return const_iterator(header, this);
  }

  const_iterator cend() const {
    return const_iterator(header, this);
  }

  /**
   * checks whether the container is empty
   * return true if empty, otherwise false.
   */
  bool empty() const {
    return size_ == 0;
  }

  /**
   * returns the number of elements.
   */
  size_t size() const {
    return size_;
  }

  /**
   * clears the contents
   */
  void clear() {
    clear_node(header->fa);
    header->lc = header->rc = header;
    size_ = 0;
  }

  /**
   * insert an element.
   * return a pair, the first of the pair is
   *   the iterator to the new element (or the element that prevented the insertion),
   *   the second one is true if insert successfully, or false.
   */
  pair<iterator, bool> insert(const value_type &value) {
    Node *p = header, *curr = header->fa;
    while (curr != nullptr) {
      p = curr;
      if (comp(value.first, curr->data->first)) {
        curr = curr->lc;
      } else if (comp(curr->data->first, value.first)) {
        curr = curr->rc;
      } else {
        return pair<iterator, bool>(iterator(curr, this), false);
      }
    }

    Node *new_node = new Node(p, nullptr, nullptr, new value_type(value), true);
    if (p == header) {
      header->fa = new_node;
    } else if (comp(value.first, p->data->first)) {
      p->lc = new_node;
    } else {
      p->rc = new_node;
    }
    fix_insert(new_node);
    size_++;
    header->lc = find_min(header->fa);
    header->rc = find_max(header->fa);
    return pair<iterator, bool>(iterator(new_node, this), true);
  }

  /**
   * erase the element at pos.
   * throw if pos pointed to a bad element (pos == this->end() || pos points an element out of this)
   */
  void erase(iterator pos) {
    if (pos == end() || pos.map_ != this) throw invalid_iterator();
    Node *node = pos.node_, *child = node, *replace = nullptr, *replace_p = nullptr;
    bool original_color = node->color;

    if (node->lc == nullptr) {
      replace = node->rc;
      replace_p = node->fa;
      replace_node(node, replace);
    } else if (node->rc == nullptr) {
      replace = node->lc;
      replace_p = node->fa;
      replace_node(node, replace);
    } else {
      child = find_min(node->rc);
      original_color = child->color;
      replace = child->rc;
      if (child->fa == node) {
        replace_p = child;
      } else {
        replace_p = child->fa;
         replace_node(child, replace);
         child->rc = node->rc;
         child->rc->fa = child;
      }
      replace_node(node, child);
      child->lc = node->lc;
      child->lc->fa = child;
      child->color = node->color;
    }

    delete node->data;
    delete node;
    if (!original_color) {
      fix_erase(replace, replace_p);
    }
    size_--;
    header->lc = size_ == 0 ? header : find_min(header->fa);
    header->rc = size_ == 0 ? header : find_max(header->fa);
  }

  /**
   * Returns the number of elements with key
   *   that compares equivalent to the specified argument,
   *   which is either 1 or 0
   *     since this container does not allow duplicates.
   * The default method of check the equivalence is !(a < b || b > a)
   */
  size_t count(const Key &key) const {
    return find(key) != end() ? 1 : 0;
  }

  /**
   * Finds an element with key equivalent to key.
   * key value of the element to search for.
   * Iterator to an element with key equivalent to key.
   *   If no such element is found, past-the-end (see end()) iterator is returned.
   */
  iterator find(const Key &key) {
    Node *curr = header->fa;
    while (curr != nullptr) {
      if (comp(key, curr->data->first)) {
        curr = curr->lc;
      } else if (comp(curr->data->first, key)) {
        curr = curr->rc;
      } else {
        return iterator(curr, this);
      }
    }
    return end();
  }

  const_iterator find(const Key &key) const {
    Node *curr = header->fa;
    while (curr != nullptr) {
      if (comp(key, curr->data->first)) {
        curr = curr->lc;
      } else if (comp(curr->data->first, key)) {
        curr = curr->rc;
      } else {
        return const_iterator(curr, this);
      }
    }
    return cend();
  }
};

}

#endif

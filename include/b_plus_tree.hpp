#ifndef B_PLUS_TREE_HPP
#define B_PLUS_TREE_HPP

#include "utility.hpp"
#include "memory_river.hpp"

namespace sjtu {

template<class Key, class T, int order = 32>
class b_plus_tree {
public:
  typedef sjtu::pair<Key, T> value_type;
private:
  struct Node {
    bool is_leaf;
    int size;
    value_type keys[order];
    int parent;
    int children[order + 1];
    int next;
    Node() : is_leaf(false), size(0), parent(-1), next(-1) {}
    Node(bool is_leaf) : is_leaf(is_leaf), size(0), parent(-1), next(-1) {}
  };

  MemoryRiver<Node> node_pool;

  Node extract_node(int pos) {
    Node node;
    node_pool.read(node, pos);
    return node;
  }
  void write_node(const Node &node, int pos) {
    node_pool.update(node, pos);
  }

  int extract_root() {
    int root_pos;
    node_pool.get_info(root_pos, 1);
    return root_pos;
  }
  void write_root(int root_pos) {
    node_pool.write_info(root_pos, 1);
  }

  int create_node(const Node &node) {
    return node_pool.write(node);
  }

  int find_leaf(const value_type &key) {
    int pos = extract_root();
    while (true) {
      Node node = extract_node(pos);
      if (node.is_leaf) return pos;
      int i = 0;
      while (i < node.size && key >= node.keys[i]) ++i;
      pos = node.children[i];
    }
  }

  void insert_leaf(int leaf_pos, const value_type &kv) {
    Node leaf = extract_node(leaf_pos);
    int i = leaf.size;
    while (i > 0 && leaf.keys[i - 1] > kv) {
      leaf.keys[i] = leaf.keys[i - 1];
      --i;
    }
    leaf.keys[i] = kv;
    ++leaf.size;
    write_node(leaf, leaf_pos);
    if (leaf.size == order) split_leaf(leaf_pos);
  }

  void split_leaf(int leaf_pos) {
    Node leaf = extract_node(leaf_pos);
    Node new_leaf(true);
    new_leaf.parent = leaf.parent;
    int mid = order / 2;
    for (int i = mid; i < order; ++i) {
      new_leaf.keys[i - mid] = leaf.keys[i];
    }
    new_leaf.size = order - mid;
    leaf.size = mid;
    new_leaf.next = leaf.next;
    leaf.next = create_node(new_leaf);
    write_node(leaf, leaf_pos);
    write_node(new_leaf, leaf.next);
    insert_parent(leaf_pos, new_leaf.keys[0], leaf.next);
  }

  void insert_parent(int left_pos, const value_type &kv, int right_pos) {
    if (left_pos == extract_root()) {
      Node new_root;
      new_root.keys[0] = kv;
      new_root.children[0] = left_pos;
      new_root.children[1] = right_pos;
      new_root.size = 1;
      int new_root_pos = create_node(new_root);

      Node left_node = extract_node(left_pos);
      left_node.parent = new_root_pos;
      write_node(left_node, left_pos);
      Node right_node = extract_node(right_pos);
      right_node.parent = new_root_pos;
      write_node(right_node, right_pos);

      write_root(new_root_pos);
      return;
    }

    int parent_pos = extract_node(left_pos).parent;
    Node parent = extract_node(parent_pos);
    int i = parent.size;
    while (i > 0 && parent.keys[i - 1] > kv) {
      parent.keys[i] = parent.keys[i - 1];
      parent.children[i + 1] = parent.children[i];
      --i;
    }
    parent.keys[i] = kv;
    parent.children[i + 1] = right_pos;
    ++parent.size;
    Node right_node = extract_node(right_pos);
    right_node.parent = parent_pos;
    write_node(right_node, right_pos);
    write_node(parent, parent_pos);
    if (parent.size == order) split_internal(parent_pos);
  }

  void split_internal(int internal_pos) {
    Node internal = extract_node(internal_pos);
    Node new_internal;
    new_internal.parent = internal.parent;
    int mid = order / 2;
    for (int i = mid + 1; i < order; ++i) {
      new_internal.keys[i - mid - 1] = internal.keys[i];
      new_internal.children[i - mid] = internal.children[i];
    }
    new_internal.size = order - mid - 1;
    internal.size = mid;
    int new_internal_pos = create_node(new_internal);
    for (int i = 0; i <= new_internal.size; ++i) {
      Node child = extract_node(new_internal.children[i]);
      child.parent = new_internal_pos;
      write_node(child, new_internal.children[i]);
    }
    write_node(internal, internal_pos);
    write_node(new_internal, new_internal_pos);
    insert_parent(internal_pos, internal.keys[mid], new_internal_pos);
  }

  void fix_leaf(int leaf_pos) {
    Node leaf = extract_node(leaf_pos);
    if (leaf.size >= (order + 1) / 2) return;
    if (leaf.parent == -1) {
      // Root leaf is allowed to be empty; keep root position valid.
      return;
    }
    int parent_pos = leaf.parent;
    Node parent = extract_node(parent_pos);
    int idx = 0;
    while (idx <= parent.size && parent.children[idx] != leaf_pos) ++idx;

    if (idx > 0) {
      Node left_sibling = extract_node(parent.children[idx - 1]);
      if (left_sibling.size > (order + 1) / 2) {
        for (int i = leaf.size; i > 0; --i) {
          leaf.keys[i] = leaf.keys[i - 1];
        }
        leaf.keys[0] = left_sibling.keys[left_sibling.size - 1];
        ++leaf.size;
        --left_sibling.size;
        parent.keys[idx - 1] = left_sibling.keys[left_sibling.size - 1];
        write_node(left_sibling, parent.children[idx - 1]);
        write_node(leaf, leaf_pos);
        write_node(parent, parent_pos);
        return;
      }
    }

    if (idx < parent.size) {
      Node right_sibling = extract_node(parent.children[idx + 1]);
      if (right_sibling.size > (order + 1) / 2) {
        leaf.keys[leaf.size] = right_sibling.keys[0];
        ++leaf.size;
        for (int i = 0; i < right_sibling.size - 1; ++i) {
          right_sibling.keys[i] = right_sibling.keys[i + 1];
        }
        --right_sibling.size;
        parent.keys[idx] = right_sibling.keys[0];
        write_node(right_sibling, parent.children[idx + 1]);
        write_node(leaf, leaf_pos);
        write_node(parent, parent_pos);
        return;
      }
    }
    if (idx > 0) {
      Node left_sibling = extract_node(parent.children[idx - 1]);
      if (left_sibling.size > (order + 1) / 2) {
        for (int i = leaf.size; i > 0; --i) {
          leaf.keys[i] = leaf.keys[i - 1];
        }
        leaf.keys[0] = left_sibling.keys[left_sibling.size - 1];
        ++leaf.size;
        --left_sibling.size;
        parent.keys[idx - 1] = left_sibling.keys[left_sibling.size - 1];
        write_node(left_sibling, parent.children[idx - 1]);
        write_node(leaf, leaf_pos);
        write_node(parent, parent_pos);
        return;
      }
    }

    if (idx > 0) {
      merge_leaf(parent.children[idx - 1], leaf_pos, idx - 1);
    } else {
      merge_leaf(leaf_pos, parent.children[idx + 1], idx);
    }
  }

  void merge_leaf(int left_pos, int right_pos, int parent_idx) {
    Node left = extract_node(left_pos);
    Node right = extract_node(right_pos);
    for (int i = left.size; i < left.size + right.size; ++i) {
      left.keys[i] = right.keys[i - left.size];
    }
    left.size += right.size;
    left.next = right.next;

    Node parent = extract_node(left.parent);
    for (int i = parent_idx; i < parent.size - 1; ++i) {
      parent.keys[i] = parent.keys[i + 1];
      parent.children[i + 1] = parent.children[i + 2];
    }
    --parent.size;
    write_node(parent, left.parent);
    write_node(left, left_pos);
    
    if (parent.parent != -1 && parent.size < (order + 1) / 2) {
      fix_internal(left.parent);
    } else if (parent.size == 0) {
      write_root(left_pos);
      left.parent = -1;
      write_node(left, left_pos);
    }
  }

  void fix_internal(int internal_pos) {
    Node internal = extract_node(internal_pos);
    if (internal.size >= (order + 1) / 2) return;
    if (internal.parent == -1) {
      if (internal.size == 0) write_root(internal.children[0]);
      return;
    }
    int parent_pos = internal.parent;
    Node parent = extract_node(parent_pos);
    int idx = 0;
    while (idx <= parent.size && parent.children[idx] != internal_pos) ++idx;

    if (idx > 0) {
      Node left_sibling = extract_node(parent.children[idx - 1]);
      if (left_sibling.size > (order + 1) / 2) {
        for (int i = internal.size; i > 0; --i) {
          internal.keys[i] = internal.keys[i - 1];
        }
        for (int i = internal.size + 1; i > 0; --i) {
          internal.children[i] = internal.children[i - 1];
        }
        internal.keys[0] = parent.keys[idx - 1];
        internal.children[0] = left_sibling.children[left_sibling.size];
        ++internal.size;
        --left_sibling.size;
        parent.keys[idx - 1] = left_sibling.keys[left_sibling.size - 1];
        write_node(left_sibling, parent.children[idx - 1]);
        write_node(internal, internal_pos);
        write_node(parent, parent_pos);
        return;
      }
    }

    if (idx < parent.size) {
      Node right_sibling = extract_node(parent.children[idx + 1]);
      if (right_sibling.size > (order + 1) / 2) {
        internal.keys[internal.size] = parent.keys[idx];
        internal.children[internal.size + 1] = right_sibling.children[0];
        ++internal.size;
        for (int i = 0; i < right_sibling.size - 1; ++i) {
          right_sibling.keys[i] = right_sibling.keys[i + 1];
          right_sibling.children[i] = right_sibling.children[i + 1];
        }
        right_sibling.children[right_sibling.size - 1] = right_sibling.children[right_sibling.size];
        --right_sibling.size;
        parent.keys[idx] = right_sibling.keys[0];
        write_node(right_sibling, parent.children[idx + 1]);
        write_node(internal, internal_pos);
        write_node(parent, parent_pos);
        return;
      }
    }

    if (idx > 0) {
      merge_internal(parent.children[idx - 1], internal_pos, idx - 1);
    } else {
      merge_internal(internal_pos, parent.children[idx + 1], idx);
    }
  }

  void merge_internal(int left_pos, int right_pos, int parent_idx) {
    Node left = extract_node(left_pos);
    Node right = extract_node(right_pos);
    Node parent = extract_node(left.parent);
    left.keys[left.size] = parent.keys[parent_idx];
    for (int i = left.size + 1; i < left.size + 1 + right.size; ++i) {
      left.keys[i] = right.keys[i - left.size - 1];
      left.children[i] = right.children[i - left.size - 1];
      Node child = extract_node(left.children[i]);
      child.parent = left_pos;
      write_node(child, left.children[i]);
    }
    left.children[left.size + right.size + 1] = right.children[right.size];
    left.size += right.size + 1;

    for (int i = parent_idx; i < parent.size - 1; ++i) {
      parent.keys[i] = parent.keys[i + 1];
      parent.children[i + 1] = parent.children[i + 2];
    }
    --parent.size;
    write_node(parent, left.parent);
    write_node(left, left_pos);
    
    if (parent.parent != -1 && parent.size < (order + 1) / 2) {
      fix_internal(left.parent);
    } else if (parent.parent == -1 && parent.size == 0) {
      write_root(left_pos);
      left.parent = -1;
      write_node(left, left_pos);
    }
  }

public:
  b_plus_tree(std::string filename = "b_plus_tree.dat") : node_pool(filename) {
    node_pool.initialise(filename);
    if (node_pool.size() == 0) {
      Node root(true);
      int root_pos = create_node(root);
      write_root(root_pos);
    }
  }

  ~b_plus_tree() = default;

  void insert(const Key &key, const T &value) {
    value_type kv(key, value);
    int leaf_pos = find_leaf(kv);
    Node leaf = extract_node(leaf_pos);
    for (int i = 0; i < leaf.size; ++i) {
      if (leaf.keys[i] == kv) {
        return;
      }
    }
    insert_leaf(leaf_pos, kv);
  }

  void erase(const Key &key, const T &value) {
    value_type kv(key, value);
    int leaf_pos = find_leaf(kv);
    Node leaf = extract_node(leaf_pos);
    int i = 0;
    while (i < leaf.size && leaf.keys[i] != kv) ++i;
    if (i == leaf.size) return;
    for (int j = i; j < leaf.size - 1; ++j) {
      leaf.keys[j] = leaf.keys[j + 1];
    }
    --leaf.size;
    write_node(leaf, leaf_pos);
    fix_leaf(leaf_pos);
  }

  sjtu::vector<T> find(const Key &key) {
    value_type kv(key, std::numeric_limits<T>::min());
    int leaf_pos = find_leaf(kv);
    sjtu::vector<T> result;
    while (leaf_pos != -1) {
      Node leaf = extract_node(leaf_pos);
      for (int i = 0; i < leaf.size; ++i) {
        if (leaf.keys[i].first == key) {
          result.push_back(leaf.keys[i].second);
        } else if (leaf.keys[i].first > key) {
          return result;
        }
      }
      leaf_pos = leaf.next;
    }
    return result;
  }
};
}


#endif // B_PLUS_TREE_HPP

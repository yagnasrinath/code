#include <pair>
struct TreeNode {
  int value;
  TreeNode *left;
  TreeNode *right;
  TreeNode(int value) : value(value), left(nullptr), right(nullptr) {}
};

class BinarySearchTree {
private:
  TreeNode *root;
  std::pair<TreeNode *, TreeNode *> getParentAndNode(int target) {
    auto parent = nullptr;
    auto curr = root;
    while (curr) {
      if (curr->value == target) {
        return {parent, root};
      } else if (target < curr->value) {
        parent = curr;
        curr = curr->left;
      } else {
        parent = curr;
        curr = curr->right;
      }
    }
    return {parent, root};
  }

public:
  BinarySearchTree() : root(nullptr) {}
  void insert(int val) {}

  TreeNode *find(int target) {
    auto [parent, root] = getParentAndNode(target);
    if (root && root->value == target) {
      return root;
    }
    return nullptr;
  }

  bool remove(int target) {
    auto [parent, root] = getParentAndNode(target);
    if (root && root->value == target) {
      auto nextRoot = nullptr;
      if (!root->left && !root->right) {
      } else if (!root->left) {
        nextRoot = root->right;
      } else if (!root->right) {
        nextRoot = root->left;
      } else {
        auto newParent = root;
        auto curr = root->left;
        while(curr->right) {
            parent = curr;
            curr = curr->right;
        }
        if(curr->left) {
            nextRootValue = curr->value;
            curr->value = curr->left->value;
            delete curr->left;
            
        }
      }

    }
    return nullptr;
  }
};
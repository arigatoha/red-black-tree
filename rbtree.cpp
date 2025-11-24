#include "rbtree.hpp"

template <typename T>
RedBlackTree<T>::RedBlackTree() {
    _tnull = new Node;
    _tnull->color = BLACK;
    _tnull->right = nullptr;
    _tnull->left = nullptr;
    _root = _tnull;
}

template <typename T>
RedBlackTree<T>::~RedBlackTree() {}

template <typename T>
RedBlackTree<T>::RedBlackTree(const RedBlackTree<T> &other) {}

template <typename T>
RedBlackTree<T> &RedBlackTree<T>::operator=(const RedBlackTree<T> &other) {
    if (this != &other) {
        this._
    }
    return *this;
}

template <typename T>
void RedBlackTree<T>::copyDeep(Node<T> *otherNode, Node<T> *otherTnull) {
    if (otherNode == otherTnull)
        return this->_tnull;
    
    copyDeep(otherNode->_left, otherTnull);
    copyDeep(otherNode->_right, otherTnull);

    
}



template <typename T>
void RedBlackTree<T>::deleteTree(Node<T> *node) {
    if (node == _tnull || node == nullptr) return;

    deleteTree(node->_left);
    deleteTree(node->_right);

    delete node;
}

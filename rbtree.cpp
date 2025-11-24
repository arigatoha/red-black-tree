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
RedBlackTree<T>::~RedBlackTree() {
    deleteTree(_root);
    delete _tnull;
}

template <typename T>
RedBlackTree<T>::RedBlackTree(const RedBlackTree<T> &other) {
    this->_tnull = new Node;
    this->_tnull->color = BLACK;
    this->_tnull->_left = nullptr;
    this->_tnull->_right = nullptr;

    this->_root = copyHelper(other._root, other._tnull);
    this->_root->_parent = nullptr;
}

template <typename T>
RedBlackTree<T> &RedBlackTree<T>::operator=(const RedBlackTree<T> &other) {
    if (this != &other) {
        this->clear();
        this->_root = copyHelper(other._root, other._tnull);
        this->_root->_parent = nullptr;
    }
    return *this;
}

template <typename T>
Node<T>	*RedBlackTree<T>::copyHelper(Node<T> *otherNode, Node<T> *otherTnull) {
    if (otherNode == otherTnull)
        return this->_tnull;
    
    Node<T> *node = new Node;

    node->_left = copyHelper(otherNode->_left, otherTnull);
    node->_right = copyHelper(otherNode->_right, otherTnull);

    if (otherNode->_left != otherTnull) node->_left->_parent = node;
    if (otherNode->_right != otherTnull) node->_right->_parent = node;

    return node;
}

template <typename T>
void RedBlackTree<T>::clear() {
    if (_root != _tnull)
        deleteTree(_root);
    _root = _tnull;
}

template <typename T>
void RedBlackTree<T>::deleteTree(Node<T> *node) {
    if (node == _tnull || node == nullptr) return;

    deleteTree(node->_left);
    deleteTree(node->_right);

    delete node;
}

template <typename T>
void RedBlackTree<T>::insert(Node<T> *node) {

}

template <typename T>
void RedBlackTree<T>::find(Node<T> *node) {

}

template <typename T>
void RedBlackTree<T>::deleteNode(Node<T> *node) {

}

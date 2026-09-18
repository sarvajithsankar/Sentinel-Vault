#ifndef THREATENGINE_H
#define THREATENGINE_H

#include <iostream>
#include <algorithm>
#include <string>

using namespace std;

struct node{
    string ipAdress;
    node* left;
    node* right;
    int height;

    node (string ip){
        ipAdress = ip;
        left = nullptr;
        right = nullptr;
        height = 1;
    }
};

inline int getheight(node* n){
    if (n == nullptr){
        return 0;
    }
    return n->height;
}

inline int get_bf(node *n){ 
    if (n == nullptr){
        return 0;
    }
    return getheight(n->left) - getheight(n->right);
}

inline node* right_rotate(node* y){
    node* x = y->left;
    node* T2 = x->right;

    x->right = y;
    y->left = T2;

    y->height = 1 + max(getheight(y->left), getheight(y->right));
    x->height = 1 + max(getheight(x->left), getheight(x->right));

    return x;
}

inline node* left_rotate(node* x){
    node* y = x->right;
    node* T2 = y->left;

    y->left = x;
    x->right = T2;

    x->height = 1 + max(getheight(x->left), getheight(x->right));
    y->height = 1 + max(getheight(y->left), getheight(y->right));

    return y;
}

inline node* insert(node* root,string ipAddress){
    if (root == nullptr){
        return new node(ipAddress);
    }
    if (ipAddress < root->ipAdress){
        root->left = insert(root->left,ipAddress);
    }
    else if (ipAddress > root->ipAdress){
        root->right = insert(root->right,ipAddress);
    }
    else{
        return root;
    }
    root->height = 1 + max(getheight(root->left), getheight(root->right));
    
    int balance = get_bf(root);

    if (balance > 1 && ipAddress < root->left->ipAdress){
        return right_rotate(root);
    }
    if (balance <-1 && ipAddress > root->right->ipAdress){
        return left_rotate(root);
    }

    if (balance > 1 && ipAddress > root->left->ipAdress){
        root->left = left_rotate(root->left);
        return right_rotate(root);
    }

    if (balance < -1 && ipAddress < root->right->ipAdress){
        root->right = right_rotate(root->right);
        return left_rotate(root);
    }
    return root;
}

inline bool isblacklisted (node* root,string ipAddress){
    if (root == nullptr){
        return false;
    }
    else if (ipAddress == root->ipAdress){
        return true;
    }
    else if (ipAddress < root->ipAdress){
        return isblacklisted(root->left,ipAddress);
    }
    return isblacklisted(root->right,ipAddress);
}

#endif




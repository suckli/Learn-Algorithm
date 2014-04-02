#include<string.h>
#include<stdio.h>
#include<stdlib.h>
#include<assert.h>
#include"b+tree.h"

int _get_index_in_parent(Node* node) {
    int i;
    Node* p = node->parent;
    int count = p->count;
    for (i = 0; i < count; i++) {
        if(p->children[i] == node)
            return i;
    }
	return -1;
}
int _insert(int item, int **k, int count) {
    int *keys;
	int i;
    for (i = 0; i < count; i++) {
        if (*k[i] == item)
            return 0;
        if (*k[i] > item) {
            keys = (int *)calloc(count+1, sizeof(int));
            memcpy(keys, *k, sizeof(int)*(i-1));
            keys[i] = item;
            memcpy(keys+i+1, *k+i+1, sizeof(int)*(count-i));
            free(*k);
            *k = keys;
            return 0;
        }
    }
    return -1;
}

/*
 *insert node into leaf's parent
 */
void _insert2parent(Node* leaf, Node* node) {
    Node **children;
    int index;
    Node * parent = leaf->parent;
    int count = parent->count;

    index = _get_index_in_parent(leaf);
    children = (Node**)calloc(count + 1, sizeof(Node*));
    memcpy(children, parent->children,  sizeof(Node*) * index);
    children[index+1] = node;
    memcpy(children+index+1+1, parent->children+index+1, count-index);
    free(parent->children);
    parent->children = children;
    
    _insert(leaf->keys[leaf->count-1], &parent->keys, parent->count);
}

void _split(Node* lleaf, BPtree*tree) {
    int i;
    Node* new_parent;
    int count = lleaf->count;
    int half = count/2;
    

    Node* node = (Node*)calloc(1,sizeof(Node));
    node->keys = (int*)calloc(half, sizeof(int));
    node->count = half;
    node->next = lleaf->next;
    node->prev = lleaf;
    node->parent = lleaf->parent;

    for (i = 0; i < half; i++) {
        node->keys[i] = lleaf->keys[i+half];
    }

    lleaf->next = node;
    lleaf->count = lleaf->count-half;

    if (lleaf->parent != NULL) {
        _insert2parent(lleaf, node);
         if (lleaf->parent->count > tree->m) 
            _split(lleaf->parent, tree);
    } else {
        new_parent = (Node*)calloc(1, sizeof(Node*));
        new_parent->count = 2;
        new_parent->children = (Node**)calloc(2, sizeof(Node*));
        new_parent->keys = (int*)calloc(2, sizeof(int));
        new_parent->children[0] = lleaf;
        new_parent->children[1] = node;
        
        new_parent->keys[0] = lleaf->keys[lleaf->count - 1];
        new_parent->keys[1] = node->keys[node->count - 1];
        tree->root = new_parent;
    }

}



void _append(int item, int **k, int count) {
    int * keys = (int*)calloc(count+1,sizeof(int));
    memcpy(keys, *k, sizeof(int)*count);
    keys[count] = item;
    free(*k);
    *k = keys;
}

int insert(int item, BPtree* tree) {
    int min;
	int count;
    Node * lleaf;
    lleaf = tree->lleaf;
    count = lleaf->count;

    if (tree->m%2 != 0)
        min = tree->m/2+1;

    while(1) {
        if (_insert(item, &lleaf->keys, count) == 0) {
            lleaf->count+=1;
            break;
        }

        if (lleaf->next == NULL) {
            _append(item,&lleaf->keys,count);
            lleaf->count+=1;
            break;
        }
    }

    if (lleaf->count > tree->m) {
        _split(lleaf, tree);
    } else if(lleaf->count < min) {
        assert(lleaf == tree->root);
    }

    return 0;
}
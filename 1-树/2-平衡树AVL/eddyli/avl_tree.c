#include<stdio.h>
#include<stdlib.h>
#include<assert.h>

#define MAX_COUNT 512
typedef int KEY_TYPE;
typedef struct AVL{
    KEY_TYPE key;
    int height;
    struct AVL* lchild;
    struct AVL* rchild;
}AVL;

AVL* RR_Rotate(AVL* k2);
AVL* LL_Rotate(AVL* k2);
AVL* LR_Rotate(AVL* k3);
AVL* RL_Rotate(AVL* k3);

AVL* avl_insert(AVL* node, int num);

int get_nums_input(int *p, int max_len) {
	char str[MAX_COUNT] = { 0 };
	char* p_end = str;
	char* p_pre = NULL;
	int count = 0;
	int num;

	printf("please input some numbers separate by space \n>");
	if (fgets(str, max_len, stdin) == NULL) {
		printf("read input error!\n");
		return 0;
	}
	
	while( count < max_len ) {
		p_pre = p_end;
		num = strtol(p_end, &p_end, 10);
		if (p_end == p_pre)
			break;
		p[count] = num;
		count += 1;
	}

	return count;
}

AVL * create_avl_tree(int* num, int len) {
	int i = 1;
	AVL * root = (AVL*)calloc(1, sizeof(AVL));
	if (root == NULL)
		return NULL;
	root->key = num[0];
	for (;i < len; i++) {
		//root maybe changed
		avl_insert(&root, num[i]);
	}
	return root;
}

void level_order_traverse(AVL *T) {
	AVL * node_list[MAX_COUNT] = { {0} };
	AVL * node = T;
	int index = 0;
	int length = 1;
	int flag = 0;
	node_list[0] = T;
	
	while(index < MAX_COUNT) {
		AVL * node = node_list[index];
		if (node != NULL) {

			printf("%d", node->key);
			if (node->lchild != NULL) {
				length = 2 * index + 1;
				node_list[2*index + 1] = node->lchild;
			}
			if (node->rchild != NULL) {
				length = 2 * index + 2;
				node_list[length] = node->rchild;
			}
			
		} 
		printf(" ");
		if (index == length)
				break;

		if ( index == flag ) {
			printf("\n");
			flag = 2 * index + 2;
		}
		index += 1;
	}
}

int get_tree_height(AVL* node) {
	int lheight,rheight,height;
	if (node == NULL)
		return -1;
	lheight = get_tree_height(node->lchild);
	rheight = get_tree_height(node->rchild);
	height = lheight > rheight ? lheight : rheight;
	return height + 1;
}

void set_node_height(AVL* node) {
	int lheight,rheight;
	if (node == NULL)
		return;
	lheight = get_tree_height(node->lchild);
	rheight = get_tree_height(node->rchild);
	node->height = lheight - rheight;	
}


void set_parent(AVL* new_node, AVL*old_node, AVL* parent) {
	assert(new_node != NULL 
		&& old_node != NULL 
		&& parent != NULL);
	if (parent->lchild == old_node) {
		parent->lchild = new_node;
	} else if (parent->rchild == old_node){
		parent->rchild = new_node;
	} else {
		assert(0);
	}
}


AVL*  check_and_rorate(AVL * rnode, AVL* parent) {
	AVL * rret = rnode;
	if (rnode == NULL)
		return;
	if (rnode->height >= 2) { 
		//left tree height than right
		if(rnode->lchild != NULL 
			&& rnode->lchild->lchild != NULL) {
			rret = LL_Rotate(rnode);
		} else if(rnode->lchild != NULL 
			&& rnode->lchild->rchild != NULL) {
			rret = LR_Rotate(rnode);
		}

		if (parent != NULL) {
			set_parent(rret, rnode, parent);
		}
		
	} else if (rnode->height <= -2) {
		//right tree height than left
		if (rnode->rchild != NULL 
			&& rnode->rchild->rchild != NULL) {
			rret = RR_Rotate(rnode);
		} else if (rnode->rchild != NULL
			&& rnode->rchild->lchild != NULL) {
			rret = RL_Rotate(rnode);
		}
		if (parent != NULL) {
			set_parent(rret, rnode, parent);
		}
	}
	return rret;
}

AVL* _avl_insert(AVL* node, int num) {
	AVL **next_node = NULL;
	AVL * ret = NULL;   //inserted node
	AVL * rnode = NULL; //next_node

	if (node->key == num) {
		return node;
	}

	if (num < node->key) 
		next_node = &node->lchild;
	else
		next_node = &node->rchild;

	if (*next_node == NULL) {
		*next_node = (AVL*)calloc(1, sizeof(AVL));
		(*next_node)->key = num;
		(*next_node)->height = 0;
		ret = *next_node;
	} else {
		ret = _avl_insert(*next_node, num);
	}
	rnode = *next_node;
	set_node_height(rnode);
	check_and_rorate(rnode, node);
	return ret;
}

AVL* avl_insert(AVL** node, int num) {
	AVL *ret = NULL;
	AVL *rret = NULL;
	ret = _avl_insert(*node, num);
	set_node_height(*node);
	rret = check_and_rorate(*node, NULL);
	*node = rret;
	return ret;
}

/**
 *       A
 *      /
 *     B    =>    B
 *    /          / \
 *   C          C   A
**/
AVL * LL_Rotate(AVL * A) {
	AVL * B = NULL;
	B = A->lchild;
	assert(A != NULL && B != NULL);
	A->lchild = B->rchild;
	B->rchild = A;
	set_node_height(A);
	set_node_height(B);
	return B;
}

/**
 *  A
 *   \
 *     B    =>      B
 *      \          / \
 *		 C        A   C
**/
AVL * RR_Rotate(AVL * A) {
	AVL * B = NULL;
	B = A->rchild;
	assert(A != NULL && B != NULL);
	A->rchild = B->lchild;
	B->lchild = A;
	set_node_height(A);
	set_node_height(B);
	return B;
}

/**
 *  A
 *   \
 *    B    =>   C
 *   /         / \
 *  C         A   B
**/
AVL* RL_Rotate(AVL* node) {
	AVL * A, *B;
	A = node;
	B = node->rchild;
	assert(A != NULL && B != NULL);
	A->rchild = LL_Rotate(B);
	return RR_Rotate(A);
}

/**
 *   A
 *  / 
 * B     =>    C
 *  \         / \
 *   C       B   A
**/
AVL* LR_Rotate(AVL* node) {
	AVL * A, *B;
	A = node;
	B = node->lchild;
	assert(A != NULL && B != NULL);
	A->lchild = RR_Rotate(B);
	return LL_Rotate(A);
}


int main() {
	int nums[MAX_COUNT];
	int count = 0;
	AVL head;
	count = get_nums_input(nums, MAX_COUNT);
	head = *create_avl_tree(nums, count);
	level_order_traverse(&head);
	return 0;
}
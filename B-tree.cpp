#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <stack>
using namespace std;

/* 
B-tree Implementation

https://www.youtube.com/watch?v=ZiXKWN4LzZs&list=LLnBKgYoOZ0WUVrEaXeYPrUw
 */
 
 
 
/**
 * BTNode represents a node in B-tree.
 */
typedef struct BTNode
{
  int n, *K, *A;//n : number of keys in node, K: key value, A: pointer to records 
  struct BTNode **P;//P: pointer to subtree
} BTNode;

/**
 * getBTNode returns a new node.
 * @param m: fanout of B-tree
 * @return a new node
 */
BTNode *getBTNode(int m)
{
  BTNode *node = (BTNode *)malloc(sizeof(BTNode));
  node->n = 0;
  node->K = (int *)malloc(sizeof(int) * (m - 1));
  node->A = NULL;
  node->P = (BTNode **)calloc(m, sizeof(BTNode *));
  return node;
}

typedef BTNode *BTree;







/**
 * binarySearch returns index i where K[i-1] < key <= K[i].
 * @param K: an array
 * @param n: size of array
 * @param key: a key to search
 * @return index of key in K
 */
int keyIndexSearch(int K[], int n, int key){
	if (n == 0) {
		return -1;
  	}
  	for (int i = 0; i < n; i++){
  		if (key<= K[i]){
  			return i;	
		}
	}
	return n;

}

/**
 * insertBT inserts newKey into T.
 * @param T: a B-tree
 * @param m: fanout of B-tree
 * @param newKey: a key to insert
 */
void insertBT(BTree* T, int m , int newKey){
	cout << newKey << "삽입 -> " << endl;
	BTree x = *T;
	BTree y = NULL;
	stack<BTree> NodeStack;
	stack<int> iStack;
	while (x != NULL){
		int i = keyIndexSearch(x->K, x->n, newKey);
		if ((i < x->n) && newKey == x->K[i])//newKey already exists in T
			return;
		NodeStack.push(x);
		iStack.push(i);
		x = x->P[i];
	}
	//insert Key and node while popping parent node from stack
	while (!NodeStack.empty()){
		x = NodeStack.top();
		NodeStack.pop();
		int i = iStack.top();
		iStack.pop();
		if (x->n < m - 1){
			for (int idx = x->n-1; idx >= i; idx--){
				x->K[idx+1] = x->K[idx];
			}
			for (int idx = x->n; idx >= i+1; idx--){
				x->P[idx+1] = x->P[idx];
			}
			x->K[i] = newKey;
			x->P[i+1] = y;
			x->n = x->n + 1;
			return;
		}
		//case of Overflow
		BTNode *tempNode= getBTNode(m + 1);
		tempNode->P[0] = x->P[0];
		for (int idx = 0; idx < i; idx++){
			tempNode->P[idx+1] = x->P[idx+1];
			tempNode->K[idx] = x->K[idx];
		}
		tempNode->P[i+1] = y;
		tempNode->K[i] = newKey;
		for (int idx = i + 1; idx < m; idx++){
			tempNode->P[idx+1] = x->P[idx];
			tempNode->K[idx] = x->K[idx-1]; 
		}
		
		y = getBTNode(m);
		if (m == 3){
			x->K[0] = tempNode->K[0];
			x->K[1] = NULL;
			x->P[0] = tempNode->P[0];
			x->P[1] = tempNode->P[1];
			x->P[2] = NULL;
			y->K[0] = tempNode->K[2];
			y->K[1] = NULL;
			y->P[0] = tempNode->P[2];
			y->P[1] = tempNode->P[3];
			y->P[2] = NULL;
			
			
			//x와 y로 split된 노드 n값 최신화.
			int cnt = 0;
			for (int j = 0; j < m -1; j++){
				if (x->K[j] != 0){
					cnt++;
				}
			}
			x->n = cnt;
			
			cnt = 0;
			for (int j = 0; j < m -1; j++){
				if (y->K[j] != 0){
					cnt++;
				}
			}
			y->n = cnt;
		}
		else if (m == 4){
			x->K[0] = tempNode->K[0];
			x->K[1] = tempNode->K[1];
			x->K[2] = NULL;
			x->P[0] = tempNode->P[0];
			x->P[1] = tempNode->P[1];
			x->P[2] = tempNode->P[2];
			x->P[3] = NULL;
			y->K[0] = tempNode->K[3];
			y->K[1] = NULL;
			y->K[2] = NULL;
			y->P[0] = tempNode->P[3];
			y->P[1] = tempNode->P[4];
			y->P[2] = NULL;
			y->P[3] = NULL;
			
			//x와 y로 split된 노드 n값 최신화.
			int cnt = 0;
			for (int j = 0; j < m -1; j++){
				if (x->K[j] != 0){
					cnt++;
				}
			}
			x->n = cnt;
			
			cnt = 0;
			for (int j = 0; j < m -1; j++){
				if (y->K[j] != 0){
					cnt++;
				}
			}
			y->n = cnt;
		}
		newKey = tempNode->K[m/2];
		delete tempNode;
	}
	//now root has been splited
	*T = getBTNode(m);
	(*T)->K[0] = newKey;
	(*T)->P[0] = x;
	(*T)->P[1] = y;
	(*T)->n = 1;
}

/**
 * deleteBT deletes oldKey from T.
 * @param T: a B-tree
 * @param m: fanout of B-tree
 * @param oldKey: a key to delete
 */
void deleteBT(BTree *T, int m, int oldKey){	
	cout << "delete -> " << oldKey << endl;
	BTree x = *T;
	stack<BTree> NodeStack;
	stack<int> iStack;
	int i;
	//find position of oldKey while storing parent node on stack
	while (x != NULL){
		i = keyIndexSearch(x->K, x->n, oldKey);
		if ((i < x->n) && oldKey == x->K[i])
			break;
		NodeStack.push(x);
		iStack.push(i);
		x = x->P[i];
	}
	if (x == NULL){return;}//oldKey was not found
	//if oldKey was found in internal node,
	//exchange key with successor and delete successor
	BTNode* internalNode = NULL;
	if (x->P[i+1] != NULL){
		internalNode = x;
		NodeStack.push(x);
		iStack.push(i+1);
		x = x->P[i+1];
		while (x != NULL){
			NodeStack.push(x);
			iStack.push(0);
			x = x->P[0];
		}
	}
	//exchange key with successor.
	if (x == NULL){
		x = NodeStack.top();
		NodeStack.pop();
		internalNode->K[i] = x->K[0];
		x->K[0] = oldKey;
		i = iStack.top();
		iStack.pop();
	}
	
	//delete oldkey from x;
	//두가지 경우 : x가 internal node의 P[i+1]에 이어진 subtree의 minnode인 경우 / x가 internal node와 같을 경우(x가 단말일 경우)  
	//first case : i는 0 or other. in this case K[i+n] is null; also P[i+n]
	// second case : i = 0 
	if (x == internalNode){
		for (int idx = i; idx < m -2; idx++){
			x->K[idx] = x->K[idx+1];
			x->P[idx+1] = x->P[idx+2];
		}
		x->K[m-2] = NULL;
		x->P[m-1] = NULL;
	}
	else{
		for (int idx = i; idx < m-2;idx++){
			x->K[idx] = x->K[idx+1];
			x->P[idx] = x->P[idx+1];
		}
		x->K[m-2] = NULL;
		x->P[m-2] = x->P[m-1];
		x->P[m-1] = NULL; 
	}
	x-> n = x->n - 1;
	
	while (!NodeStack.empty()){
		if (x->n >= (m-1)/2){
			return;
		}
		BTNode* y;
		y = NodeStack.top();
		NodeStack.pop();
		i = iStack.top();
		iStack.pop();
		
		BTNode* bestSibling;
		int flag;//if bestSibling is right side of x -> flag = 1; if bestSibling is left side x <- 0;
		//bestSibling = best sibling of x;
		if (i == 0){
			bestSibling = y->P[i+1];
			flag = 1;
		}
		else if (i == m-1){
			bestSibling = y->P[i - 1];
			flag = 0;
		}
		else {
			if (y->P[i-1] == NULL){
				bestSibling = y->P[i+1];
				flag = 1;
			}
			else if (y->P[i+1] == NULL){
				bestSibling = y->P[i-1];
				flag = 0;
			}
			else if (y->P[i-1] -> n >= y->P[i+1]->n){
				bestSibling = y ->P[i-1];
				flag = 0;
			}
			else{
				bestSibling = y->P[i+1];
				flag = 1;
			}
		}
		//case of key redistribution
		if (bestSibling->n > (m-1) / 2){
			//bestSibling is on a leftSide of x in Y
			if (flag == 0){
				for (int idx = m-2; idx >=0; idx--){
					x->P[idx+1] = x->P[idx];
				}
				for (int idx = m-3; idx>= 0; idx--){
					x->K[idx+1] = x->K[idx];
				}
				x->K[0] = y->K[i-1];
				y->K[i-1] = bestSibling->K[bestSibling->n-1];
				x->P[0] = bestSibling->P[bestSibling->n];
				bestSibling->K[bestSibling->n-1] = NULL;
				bestSibling->P[bestSibling->n] = NULL;
				bestSibling->n = bestSibling->n - 1;
				x -> n = x->n + 1;
				break;
			} 
			else if (flag == 1){//bestSibling is on a right Side of x in Y
				x->K[0] = y->K[i];
				y->K[i] = bestSibling->K[0];
				x->P[1] = bestSibling->P[0];
				for (int idx = 0; idx < m-2; idx++){
					bestSibling->K[idx] = bestSibling->K[idx+1];
				} 
				for (int idx = 0; idx < m-1; idx++){
					bestSibling->P[idx] = bestSibling->P[idx+1];
				}
				bestSibling->P[m-1] = NULL;
				bestSibling->n = bestSibling->n - 1;
				x -> n = x->n + 1;
				break;
			}
		}
		//case of node merge
		if (flag == 0){//bestSibling is on a left side of x. 
			if (i == y->n){
				bestSibling->K[bestSibling->n] = y->K[i-1];
				y->K[i-1] = NULL;
				y->P[i] = NULL;
			}
			else if ((y->P[i - 1] != NULL && y->P[i + 1] == NULL) || y->P[i - 1]->n >= y->P[i + 1]->n){
				bestSibling->K[bestSibling->n] = y->K[i-1];
				for (int idx = i; idx < y->n; idx++){
					y->K[idx - 1] = y->K[idx];
				}
				y->K[y->n - 1] = NULL;
				
				for (int idx = i; idx < y->n; idx++){
					y->P[idx] = y->P[idx+1];
				}
				y->P[y->n] = NULL;
			}
			
			bestSibling ->n = bestSibling->n + 1;
			y->n = y->n -1;
			
			//merge x.K and bestSibling.K
			for (int idx = 0; idx < x->n; idx++){
				bestSibling->K[bestSibling->n + idx] = x->K[idx];
			}
			//merge x.P and bestSibling.P
			for (int idx = 0; idx <= x->n; idx++){
				bestSibling->P[bestSibling->n + idx] = x->P[idx];
			}
			bestSibling->n += x->n;
			
			//delete right node
			delete x;
		}
		else{//bestSibling is on a right side of x. 
			if (i == 0){
				x->K[x->n] = y->K[i];
				for (int idx = i+1; idx < y->n; idx++){
					y->K[idx-1] = y->K[idx];
				}
				y->K[y->n - 1] = NULL;
				
				for (int idx = i+1; idx < y->n ; idx++){
					y->P[idx] = y->P[idx + 1];
				}
				y->P[y->n] = NULL;
			}
			else if ((y->P[i - 1] == NULL && y->P[i + 1] != NULL) || y->P[i - 1]->n < y->P[i + 1]->n) {

            	x->K[x->n] = y->K[i];
            	for (int idx = i + 1; idx < y->n; idx++) {
                	y->K[idx - 1] = y->K[idx];
            	}
            	y->K[y->n - 1] = NULL;

            	for (int idx = i + 1; idx < y->n; idx++) {
            	    y->P[idx] = y->P[idx + 1];
            	}
            	y->P[y->n] = NULL;
        	}
			x->n += 1;
			y->n -= 1;
			
			//merge x.K and bestSibling.K
			for (int idx = 0; idx < bestSibling->n; idx++){
				x->K[x->n + idx] = bestSibling->K[idx];
			}
			
			//merge x.P and bestSibling.P
			for (int idx = 0; idx <= bestSibling->n; idx++){
				x->P[x->n + idx] = bestSibling->P[idx];
			}
			x->n += bestSibling->n;
			
			delete bestSibling;
		}
		
		x = y;	
	}
	//now root has been merged
	//level of tree decreases
	if (x->n == 0){
		*T = x->P[0];
		delete x;
	}
	
}

/**
 * inorderBT implements inorder traversal in T.
 * @param T: a B-tree
 */
void inorderBT(BTree T)
{
	if (T == NULL) return;

    for (int i = 0; i < T->n; i++) {
        inorderBT(T->P[i]);
        cout << T->K[i] << " ";
    }
    inorderBT(T->P[T->n]);

}

int main()
{	
//dev c++ 환경에서 실행했습니다.  
  /* DO NOT MODIFY CODE BELOW */
  FILE *f;
  for(int m = 3; m <= 4; m++){
    BTree T = NULL;
	
	cout << "m = " << m << ", insert 실행."<<endl;
    f = fopen("./insertSequence.txt", "r");
    for(int n; !feof(f);){
      fscanf(f, "%d", &n);
      insertBT(&T, m, n);
      inorderBT(T);
      printf("\n");
    }
    fclose(f);
    cout << "---------------------------------------------" << endl;
    cout << "m = " << m << ", delete 실행." <<endl;
    f = fopen("./deleteSequence.txt", "r");
    for(int n; !feof(f);){
      fscanf(f, "%d", &n);
      deleteBT(&T, m, n);
      inorderBT(T);
      printf("\n");
    }
    fclose(f);
    cout << "---------------------------------------------" << endl;
    
  }
  return 0;
  
}

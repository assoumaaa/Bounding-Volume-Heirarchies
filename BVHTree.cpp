#include "BVHTree.h"
// OMAR ASSOUNMA
// 28556


bool leaf = false;

void BVHTree::printNode(std::ostream &out, BVHTreeNode *node, int level) {
	if (root == nullptr) return;
	for (int i = 0; i < level; i++) {
		out << "  ";
	}
	if (!node->isLeaf) {
		out << "+ branch || ";
		node->aabb.printAABB(out);
		out << std::endl;
		printNode(out, node->rightChild, level + 1);
		printNode(out, node->leftChild, level + 1);
	}
	else {
		out << "- ";
		if (node->parent) {
			if (node->parent->rightChild == node)
				out << "R ";
			else
				out << "L ";
		}
		out << "- leaf: " << node->name << " || ";
		node->aabb.printAABB(out);
		out << std::endl;
	}
}

BVHTree::BVHTree()
{
	root = NULL;
}

void makeEmpty(BVHTreeNode *&root)
{
	if ( root != NULL )
	{
		makeEmpty(root->rightChild);
		makeEmpty(root->leftChild);
		delete root;
	}
	root = NULL;
}


BVHTree::~BVHTree()
{
	makeEmpty(root);                 
}


void BVHTree::addBVHMember(AABB object, std::string name)
{
	if ( map.find(name) == map.end() ) {
		if ( root == NULL){
			root = new BVHTreeNode(object,name,leaf = true);
			map[name] = root;
		}	
		else if ( root->rightChild == NULL && root->leftChild == NULL && root->isLeaf == true){
			BVHTreeNode *oldRoot;
			oldRoot = root;


			AABB object2(0,0,0,0);
			object2  = oldRoot->aabb + object;
			BVHTreeNode *branch;
			branch	= new BVHTreeNode(object2,"branch",leaf = false);
			root = branch;

			root->rightChild = oldRoot;
			root->rightChild->parent = root;



			BVHTreeNode *newRoot;
			newRoot = new BVHTreeNode(object,name,leaf = true);
			root->leftChild = newRoot;
			root->leftChild->parent = root;


			map[name] = newRoot;
		}
		else{

			BVHTreeNode *newNode = new BVHTreeNode(object,name,leaf = true);
			BVHTreeNode *itr;
			itr = root;
			while ( itr->rightChild != NULL && itr->leftChild != NULL )
			{
				int increaseInRightTreeSize = AABB::unionArea(newNode->aabb,
				itr->rightChild->aabb) - itr->rightChild->aabb.getArea();
				int increaseInLeftTreeSize = AABB::unionArea(newNode->aabb,
				itr->leftChild->aabb) - itr->leftChild->aabb.getArea();

				if ( increaseInLeftTreeSize > increaseInRightTreeSize )
					itr = itr->rightChild;
			
				else
					itr = itr->leftChild;

			}
			if ( itr->parent->leftChild->name == itr->name ){
				BVHTreeNode *left_child;
				BVHTreeNode *tempBranch;
				BVHTreeNode *parentt = itr->parent;

				left_child = parentt->leftChild;
		
				parentt->leftChild = new BVHTreeNode(object,"branch", leaf = false);
				parentt->leftChild->parent = parentt;
		

				tempBranch = parentt->leftChild;
				tempBranch->parent = parentt;
				tempBranch->leftChild = newNode;
				tempBranch->rightChild = left_child;

				tempBranch->leftChild->parent = tempBranch;
				tempBranch->rightChild->parent = tempBranch;

				map[name] = newNode;
			}
			else if ( itr->parent->rightChild->name == itr->name ){
				BVHTreeNode *right_child;
				BVHTreeNode *tempBranch;
				BVHTreeNode *parentt = itr->parent;

				right_child = parentt->rightChild;

				parentt->rightChild = new BVHTreeNode(object,"branch", leaf = false);
				parentt->rightChild->parent = parentt;
		
				tempBranch = parentt->rightChild;
				tempBranch->parent = parentt;
				tempBranch->rightChild = right_child;
				tempBranch->leftChild = newNode;

				tempBranch->rightChild->parent = tempBranch;
				tempBranch->leftChild->parent = tempBranch;

				map[name] = newNode;
			}
			while ( itr != root )
			{
				BVHTreeNode *ptr;
				ptr = itr->parent;

				ptr->aabb = ptr->rightChild->aabb + ptr->leftChild->aabb;
				itr = itr->parent;
			}	
		}
	}
}

void BVHTree::removeBVHMember(std::string name){

	if ( root == NULL )
		root = NULL;
	else if ( root->isLeaf == true )
	{
		BVHTreeNode *temp = root;
		root = NULL;
		delete temp;
	}
	else if ( root->rightChild->isLeaf == true && root->leftChild->isLeaf == true )
	{
		BVHTreeNode *temp = map[name];
		if ( temp->parent->rightChild->name == temp->name )
		{
			root = temp->leftChild;
			delete temp->parent;
			delete temp;
			map.erase(name);
		}
		else{
			root = temp->rightChild;
			delete temp->leftChild;
			delete temp;
			map.erase(name);
		}
	}
	else{
		BVHTreeNode *temp = map[name];
		map.erase(name);
		if ( temp->parent == root )
		{
			BVHTreeNode *child;
			if ( temp->parent->rightChild->name == temp->name ){
				BVHTreeNode * other_child  = temp->parent->leftChild;

				root = other_child;
				
				delete temp->parent;
				delete temp;

			}
			else{
				BVHTreeNode * other_child = temp->rightChild;

				root = other_child;

				delete temp->parent;
				delete temp;
			}
		}
		else{
			BVHTreeNode *grandparent = temp->parent->parent;
			BVHTreeNode *parentt = temp->parent;
			BVHTreeNode *other_child;

			if ( temp->parent->rightChild->name == temp->name ){

				other_child = temp->parent->leftChild;

				delete temp;
				if ( grandparent->rightChild == parentt ){
					delete parentt;
					grandparent->rightChild = other_child;
					other_child->parent = grandparent;
				}
				else{
					delete parentt;
					grandparent->leftChild = other_child;
					other_child->parent = grandparent;
				}

			}
			else{
				other_child = temp->parent->rightChild;

				delete temp;

				if ( grandparent->rightChild == parentt ){
					delete parentt;
					grandparent->rightChild = other_child;
					other_child->parent = grandparent;
				}
				else{
					delete parentt;
					grandparent->leftChild = other_child;
					other_child->parent = grandparent;
				}
			}
		}		
	}
	
}

bool fitsOrNo (AABB newLocation,BVHTreeNode * temp,BVHTreeNode *parentt)
{
	if ( newLocation.minX >= parentt->aabb.minX &&
	     newLocation.minY >= parentt->aabb.minY &&
	     newLocation.maxY <= parentt->aabb.maxY &&
	     newLocation.maxX <= parentt->aabb.maxX  )
		return true;
	
	return false;
}


void BVHTree::moveBVHMember(std::string name, AABB newLocation)
{
	BVHTreeNode *temp = map[name];
	BVHTreeNode *parentt = temp->parent;
	AABB object = temp->aabb;

	if ( fitsOrNo(newLocation,temp,parentt) == true ){
		temp->aabb = newLocation;
	}
	else{
		removeBVHMember(name);
		addBVHMember(newLocation,name);
	}		
}


std::vector<std::string> checkCollide(BVHTreeNode *root,AABB object,std::vector<std::string> & names)
{
	if ( root->aabb.collide(object) == true && root->isLeaf == true && root != NULL)
		names.push_back(root->name);
	else{
		if ( root->rightChild != NULL ) 
			checkCollide(root->rightChild,object,names);
		if ( root->leftChild != NULL )
			checkCollide(root->leftChild,object,names);
	}
	return names;
}


std::vector<std::string> BVHTree::getCollidingObjects(AABB object)
{
	std::vector <std::string> names;
	checkCollide(root, object,names);
	return names;
}


std::ostream &operator<<(std::ostream &out, BVHTree &tree) {
	tree.printNode(out, tree.root, 0);
	return out;
}

// CREATED BY EGE ATA CEYLAN
#include "BVHTree.h"

using namespace std;

//CONSTRUCTOR OF BVHTREE THAT INITIALIZES ROOT TO NULL AND CREATES AND EMPTY UNORDERED MAP
BVHTree::BVHTree()
{
    root=NULL;
    std::unordered_map<std::string, BVHTreeNode *> map;
}

//ADDBVH MEMBER FUNCTION FOR BVHTREE, DETAILS ARE EXPLAINED BELOW
void BVHTree::addBVHMember(AABB objectArea, std::string name)
{
//  IF THE BVHTREE IS EMPTY WE CREATE A NEW TREE NODE, MAKE IT THE ROOT OF THE TREE AND ADD THE NODE TO THE HASHMAP
    if (root==NULL)
    {
        BVHTreeNode *newNode = new BVHTreeNode(objectArea,name,true);
        map[name]= newNode;
        root= newNode;
    }
    
//  IF THE TREE HAS EXACTLY ONE NODE AND IF IT IS THE ROOT OF THE TREE
    else if (root!=NULL && root->leftChild==NULL && root->rightChild==NULL)
    {
//      CREATING A NEW TREE NODE
        BVHTreeNode *newNode = new BVHTreeNode(objectArea,name,true);
        
//      GETTING THE OLD ROOT OF THE TREE, CREATING A NEW BRANCH NODE THAT WILL COVER BOTH THE NEW NODE AND THE OLD ROOT
        BVHTreeNode *oldRoot = root;
        AABB newBranchArea = newNode->aabb + oldRoot->aabb;
        BVHTreeNode *newBranchNode = new BVHTreeNode(newBranchArea,"branch",false);
        
//      SETTING THE NEW BRANCH NODE'S CHILDS AND SETTING IT AS THE ROOT OF THE TREE
        newBranchNode->leftChild= newNode;
        newBranchNode->rightChild = oldRoot;
        root = newBranchNode;
        
//      SETTING THE PARENT OF THE NEW NODE AND THE OLD ROOT AS THE NEW BRANCH NODE
        newNode->parent= root;
        oldRoot->parent= root;
        
//      ADDING THE NEW NODE TO THE HASH TABLE
        map[name]= newNode;
    }
    
//  IF THERE IS MORE MORE THAN ONE NODE IN THE TREE
    else
    {
//      CREATING A NEW TREE NODE FOR NEW NODE, SETTING THE LOOP VARIABLES EXISTING LEAF AND BRANCH NODE AS THE ROOT OF THE TREE
        BVHTreeNode *newNode = new BVHTreeNode(objectArea,name,true);
        BVHTreeNode *existingLeaf = root;
        BVHTreeNode *branchNode = existingLeaf;
        
//      GOING OVER THE NODES IN THE TREE UNTIL FINDING A LEAF NODE
        while (existingLeaf->isLeaf == false)
        {
//          CHECKING THE INCREASE IN TREE'S SIZE FOR BOTH SIDES OF THE BRANCH NODE IF WE WERE TO ADD THE NEW NODE
            int increaseInRightTreeSize = AABB::unionArea(newNode->aabb, branchNode->rightChild->aabb) - branchNode->rightChild->aabb.getArea();
            int increaseInLeftTreeSize = AABB::unionArea(newNode->aabb, branchNode->leftChild->aabb) - branchNode->leftChild->aabb.getArea();
            
//          IF INCREASE IN RIGHT SIDE IS SMALLER CONTIUNING WITH THE RIGHT SIDE
            if (increaseInRightTreeSize < increaseInLeftTreeSize)
            {
                existingLeaf= existingLeaf->rightChild;
                branchNode = existingLeaf;
            }
//          IF INCREASE IN LEFT SIDE IS SMALLER CONTIUNING WITH THE RIGHT SIDE
            else
            {
                existingLeaf= existingLeaf->leftChild;
                branchNode = existingLeaf;
            }
        }
//      WHEN THE LEAF NODE IS FOUND, CREATING A NEW BRANCH NODE THAT WILL COVER THAT LEAF AND THE NEW NODE
        AABB newBranchArea = newNode->aabb + existingLeaf->aabb;
        BVHTreeNode *newBranchNode = new BVHTreeNode(newBranchArea,"branch",false);
        
//      IF THE EXISTING LEAF IS THE LEFT CHILD OF IT'S PARENT, WE REPLACE THE PARENT'S LEFT CHILD WITH THE NEW BRANCH NODE
        if (existingLeaf->parent->leftChild == existingLeaf)
        {
            existingLeaf->parent->leftChild = newBranchNode;
        }
//      IF THE EXISTING LEAF IS THE RIGHT CHILD OF IT'S PARENT, WE REPLACE THE PARENT'S RIGHT CHILD WITH THE NEW BRANCH NODE
        else
        {
            existingLeaf->parent->rightChild = newBranchNode;
        }
        
//      SETTING THE NEW BRANCH NODE'S CHILD AND ITS PARENT AS THE LEAF'S PARENT
        newBranchNode->leftChild = newNode;
        newBranchNode->rightChild = existingLeaf;
        newBranchNode->parent = existingLeaf->parent;
        
//      SETTING THE NEW NODE'S AND LEAF'S PARENT AS NEW BRANCH NODE
        newNode->parent = newBranchNode;
        existingLeaf->parent = newBranchNode;
        
//      STARTING FROM THE NEWLY ADDED NODE GOING OVER THE PARENT'S OF IT UNTIL WE REACH TO THE ROOT OF TREE IN ORDER TO ADJUST THE PARENT AND PARENT'S ANCESTORS AREA TO FIT THE NEWLY ADDED NODE
        BVHTreeNode *currentNode = newNode;
        while (currentNode->parent != NULL)
        {
            currentNode->parent->aabb = currentNode->parent->leftChild->aabb + currentNode->parent->rightChild->aabb;
            currentNode= currentNode->parent;
        }
        
//      ADDING THE NEW NODE TO THE HASH TABLE
        map[name]= newNode;
    }
    
}

//REMOVEBVH MEMBER FUNCTION FOR BVHTREE, DETAILS ARE EXPLAINED BELOW
void BVHTree::removeBVHMember(std::string name)
{
//  GETTING THE NODE OF TO BE DELETED MEMBER FROM THE HASH TABLE AND GETTING IT'S PARENT BRANCH IN ORDER TO REPLACE IT WITH TO BE DELETED'S SIBLING
    BVHTreeNode *toRemove = map[name];
    BVHTreeNode *parentBranchOftoRemove = toRemove->parent;
    BVHTreeNode *toRemovesSibling;
    
//  IF THE PARENT BRANCH OF TO BE DELETED'S LEFT CHILD IS EQUAL TO BE DELETED, THE SIBLING OF TO BE REMOVED IS THE RIGHT CHILD
    if (parentBranchOftoRemove->leftChild == toRemove)
    {
        toRemovesSibling = parentBranchOftoRemove->rightChild;
    }
//  IF THE PARENT BRANCH OF TO BE DELETED'S RIGHT CHILD IS EQUAL TO BE DELETED, THE SIBLING OF TO BE REMOVED IS THE LEFT CHILD
    else
    {
        toRemovesSibling = parentBranchOftoRemove->leftChild;
    }
    
//  MAKING THE PARENT OF TO BE REMOVED SIBLING AS THE PARENT OF THE PARENT BRANCH
    toRemovesSibling->parent = parentBranchOftoRemove->parent;
    
//  IF THE PARENT BRANCH OF TO BE DELETED'S IS THE LEFT CHILD OF IT'S PARENT, SETTING THE THE PARENT BRANCH'S PARENT'S LEFT CHILD AS THE SIBLING
    if (parentBranchOftoRemove->parent->leftChild == parentBranchOftoRemove)
    {
        parentBranchOftoRemove->parent->leftChild = toRemovesSibling;
    }
//  IF THE PARENT BRANCH OF TO BE DELETED'S IS THE RIGHT CHILD OF IT'S PARENT, SETTING THE PARENT BRANCH'S PARENT'S RIGHT CHILD AS THE SIBLING
    else
    {
        parentBranchOftoRemove->parent->rightChild = toRemovesSibling;
    }
    
//  DELETING THE PARENT BRANCH
    delete parentBranchOftoRemove;
    
//  ADDING THE NEW NODE TO THE HASH TABLE
    map.erase(name);
}

//MOVEBVH MEMBER FUNCTION FOR BVHTREE, DETAILS ARE EXPLAINED BELOW
void BVHTree::moveBVHMember(std::string name, AABB newLocation)
{
//  GETTING THE MOVED NODE FROM THE HASH TABLE
    BVHTreeNode * movedNode = map[name];


    if (movedNode->parent == NULL) //NEW CODE TO CHECK IF THE MOVED NODE IS THE ROOT OF THE TREE
    {
        movedNode->aabb = newLocation; //NEW CODE TO ASSIGN THE NEW AABB OF THE ROOT OF THE TREE
    }
    else//NEW CODE THE ELSE OF THE UPPER IF STATEMENT - REST IS THE SAME
    {
        //  CALCULATING THE TEMP ARE WHICH IS EQUAL TO MOVED NODE'S NEW AREA PLUS IT'S PARENT BRANCHES'S AREA
        AABB tempArea = movedNode->parent->aabb + newLocation;
        
        //  IF THE NEW AREA ISN'T COVERED BY THE PARENT OF THE MOVED NODE
        if (tempArea != movedNode->parent->aabb )
        {
            //REMOVING THE MOVED NODE AND RE ADDING IT TO THE TREE
            removeBVHMember(name);
            addBVHMember(newLocation, name);
        }
        
        // IF THE NEW AREA IS COVERED BY THE PARENT OF THE MOVED NODE
        else
        {
            //UPDATING THE AREA OF THE MOVED NODE
            movedNode->aabb = newLocation;
        }
    }
}

//GET COLLIDING OBJECTS RECURSIVE FUNCTION, DETAILS ARE EXPLAINED BELOW
std::vector<std::string> BVHTree::getCollidingObjectsRecursive(BVHTreeNode *currentRootNode,AABB checkedObject)
{
//  CREATING THREE VECTORS TO STORE THE NAMES OF THE COLLIDING OBJECT, ONE FOR THE FINAL VECTOR, ONE FOR EACH SUBTREE( FOR LEFT AND RIGHT SUBTREE)
    vector<string> collidingObjectsFinal;
    vector<string> collidingObjectsLeftTree;
    vector<string> collidingObjectsRightTree;
    
//  SETTING THE LOOP VARIABLE CURRENTNODE AS THE FUNCTION PAREMETER CURRENTROOTNODE
    BVHTreeNode *currentNode = currentRootNode;
    
//  IF THE CURRENT NODE IS NOT EQUAL TO NULL
    if (currentNode != NULL)
    {
//      IF THE CURRENT NODE AND THE OBJECT IS COLLIDING
        if (currentNode->aabb.collide(checkedObject)==true)
        {
//          IF THE COLLIDING NODE IS A LEAF NODE, WE ADD THE NODE'S NAME TO THE VECTOR
            if (currentNode->isLeaf == true)
            {
                collidingObjectsFinal.push_back(currentNode->name);
            }
            
//          IF THE COLLIDNG NODE IS A BRANCH NODE, RECURSIVLY GETTING THE COLLIDNG OBJECTS FOR THE RIGHT AND LEFT SUBTREES OF THE CURRENTLY COLLIDED BRANCH, STORING THEM IN RESPECTIVE VECTORS
            else
            {
                collidingObjectsLeftTree= getCollidingObjectsRecursive(currentNode->leftChild, checkedObject);
                collidingObjectsRightTree = getCollidingObjectsRecursive(currentNode->rightChild, checkedObject);
            }
        }
    }

//  ADDING THE ELEMENTS OF THE RIGHT AND LEFT SUBTREE'S VECTORS TO THE FINAL VECTOR
    collidingObjectsFinal.insert(collidingObjectsFinal.end(),collidingObjectsRightTree.begin(),collidingObjectsRightTree.end());
    collidingObjectsFinal.insert(collidingObjectsFinal.end(),collidingObjectsLeftTree.begin(),collidingObjectsLeftTree.end());
    
//  RETURNING THE FINAL VECTOR WHICH CONTAINS ALL THE NAMES OF THE COLLIDNG OBJECTS
    return collidingObjectsFinal;
}

//GETCOLLIDINGOBJECTS MEMBER FUNCTION FOR BVHTREE, DETAILS ARE EXPLAINED BELOW
std::vector<std::string> BVHTree::getCollidingObjects(AABB object)
{
//  CREATING A VECTOR OF STRINGS TO STORE THE COLLIDING OBJECTS'S NAMES
    vector<string> collidingObjects;
    
//  SETTING THE CURRENT NODE VARIABLE AS THE ROOT OF THE TREE IN ORDER TO PASS THE TREE TO THE RECURSIVE FUNCTION
    BVHTreeNode *currentNode = root;

//  CALLING THE GET COLLIDING OBJECTS RECURSIVE FUNCTION AND STORING IT'S RESULT IN THE COLLIDING OBJECTS VECTOR, THE DETAILS OF GET COLLIDING OBJECTS RECURSIVE FUNCTION IS EXPLAINED ABOVE
    collidingObjects = getCollidingObjectsRecursive(currentNode, object);
    
//  RETURNING THE COLLIDING OBJECTS VECTOR WHICH CONTAINS ALL THE NAMES OF THE COLLIDNG OBJECTS
    return collidingObjects;
}

//RECURSIVE DELETION FUNCTION WHICH DELETES THE NODES IN A TREE RECURSIVLY
void  BVHTree::recursiveDeletion(BVHTreeNode * currentNode)
{
//  IF THE CURRENT NODE IS NOT EQUAL TO NULL
    if (currentNode!=NULL)
    {
//      RECURSIVLY DELETING THE RIGHT AND THE LEFT SUBTREE'S OF THE CURRENT NODE
        recursiveDeletion(currentNode->leftChild);
        recursiveDeletion(currentNode->rightChild);
        
//      DELETING THE CURRENT NODE
        delete currentNode;
    }
}

//DESTRUCTOR FOR BVHTREE
BVHTree::~BVHTree()
{
//  IF ROOT IS NOT EQUAL TO NULL
    if (root!=NULL)
    {
//      CALLING THE RECURSIVE DELETION FUNCTION TO DELETE THE TREE, THE DETAILS OF RECURSIVE DELETION FUNCTION IS EXPLAINED ABOVE
        recursiveDeletion(root);
    }
}
 

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
std::ostream &operator<<(std::ostream &out, BVHTree &tree) {
	tree.printNode(out, tree.root, 0);
	return out;
}

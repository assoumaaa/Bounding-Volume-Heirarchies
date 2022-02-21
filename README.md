# Bounding-Volume-Heirarchies


## Introduction
Game development is a very challenging process involving many different problems. This includes everything from rendering graphics, carrying out game mechanics, responding to user input, and creating the AI. I will explore a very common problem in game development, and physics engines in general; the problem of collision detection. Let’s look at the following screenshot:

<p align="center">
<img width="415" alt="Screen Shot 2022-02-21 at 4 05 18 PM" src="https://user-images.githubusercontent.com/94231603/154960872-89504832-34e4-40c4-8921-aa69771bcc6f.png">
</p>

We can see there are many characters on the screen and many projectiles flying around. The question is, how does the game figure out that two objects have touched each other? Resolving the problem of two objects “touching” in a physical space is called collision detection. One way to handle this problem is to split each object into smaller “member” shapes that are easier to check than weird (very curvy) shapes. These shapes are often referred to as hitboxes. For example, the previous frame can be seen by the physics engine as the following collection of members:

<p align="center">
<img width="417" alt="Screen Shot 2022-02-21 at 4 07 43 PM" src="https://user-images.githubusercontent.com/94231603/154961267-ec38f347-a93e-4db8-bb9e-0f6c908ff053.png">
</p>

Now, when we wish to check whether two objects are touching, we need to compare the locations of the objects and see if their shapes are colliding. However, looking at this screenshot, we can see that this process can be costly. In the worst case, for each shape shown, we need to check every other object on the screen to see if a collision has happened. This can become costly fast. But there is a way to make this process less costly!
A suitable solution for this problem is to use Bounding Volume Hierarchies (BVH) to represent objects. Basically, with this approach, we represent each object as a tree of boxes that hierarchically cover the object, with the root node covering the entire object, and children covering smaller and smaller parts. The leaves of this tree are the original members of the object (its hitboxes), while non-leaf nodes are “branch” nodes whose job is to contain their children. Take for example the problem of collision detection demonstrated in Figure 3. We wish to find the objects that the blue projectile (a.k.a hadouken) is colliding with at this exact instance.

<p align="center">
<img width="450" alt="Screen Shot 2022-02-21 at 4 08 38 PM" src="https://user-images.githubusercontent.com/94231603/154961431-6afe3d23-8041-4ca3-a0e1-45abbec69d6c.png">
</p>

Each one of these objects (Ryu, Honda, and the hadouken) is an object. The characters are BVH trees (represented by a hierarchy of boxes) while the blue projectile is a single box. Figure 4 shows the objects on the screen. The boxes we see around the characters represent the root node of their BVHTrees

<p align="center">
<img width="488" alt="Screen Shot 2022-02-21 at 4 09 12 PM" src="https://user-images.githubusercontent.com/94231603/154961537-bcdd4f80-1074-4fce-b325-a2269b677ad2.png">
</p>

Now, to see which objects the blue projectile (inside the red box) is colliding with, we only need to check two other boxes (the boxes at the root of the two characters’ trees). When we do this check, we find that the projectile is colliding with the root of the tree of the character to the right only. Now, we begin searching for the exact collision point (if there is one) within the tree of that character. We do so by iteratively exploring the tree starting at the root. At each level, if we find that the projectile collides with a box, we will explore its children. If the box doesn’t have children, it means that it’s a leaf, and that it’s one of the object’s members.

<p align="center">
<img width="781" alt="Screen Shot 2022-02-21 at 4 10 20 PM" src="https://user-images.githubusercontent.com/94231603/154961697-0f7e2d7e-9b5b-4665-b13f-7e05890cfff5.png">
</p>
As shown in Figure 5, we iteratively explore the tree by checking all the boxes that collide with the projectile. During our exploration, when we find a collision with a leaf node (without any children), this indicates that a collision between the projectile and the character has happened at that member (hitbox).

## Solution
- This problem is solved through a Trees as explained above each object is a graph and will be made up of different body parts. My job first is to add those BVHMember in an addBVHMember(AABB objectArea,string name) function. However when adding a node to the tree we will have 3 possible cases:
1) The Tree is empty
2) There is exactly one node in the tree
3) There is more than one node in the tree

### Tree is empty
If the tree is empty i will set the root of the tree to be the newly created node. Example shown below.
<p align="center">
<img width="498" alt="Screen Shot 2022-02-21 at 4 20 11 PM" src="https://user-images.githubusercontent.com/94231603/154963170-56bd7a36-7c62-4179-b14f-32583496e1fd.png">
</p>


### There is exactly one node in the tree
Let’s call the current node at the root oldRoot. I must create a new branch node (non-leaf), which you will set as the root of the tree. In addition, i will set the left child of the branch node as newNode and the right child as oldRoot. And, importantly, i will set the area of the branch node to be as big as both children. In other words, the area of the branch must cover its childrens areas. This is an important insight: anytime a node is added as a child of another node, the parent node as well as all of the parent’s ancestors must adjust their area to fit the new child node. Example shown below:
<p align="center">
<img width="525" alt="Screen Shot 2022-02-21 at 4 19 53 PM" src="https://user-images.githubusercontent.com/94231603/154963124-46d4f51c-4562-4a9f-88c4-a884d4378fb1.png">
</p>


### There is more than one node in the tree

In this case, i must find a location for this node somewhere in the tree. Recall from the invariants mentioned earlier that, in our tree, member (hitbox) nodes cannot have children and branch nodes always have two children. So, if i wanted to add a new node to an existing tree, we cannot add it as a child of an existing branch node since all branch nodes already have two children, and we cannot make it a child of a leaf node since it will no longer be a leaf. So, what we must do, is:
  1. Pick a leaf node in the tree, call it existingLeaf
  2. Create a new branch node that will take its position
  3. Set newNode as the left child of the new branch node and existingLeaf as its
  right child.
Notice that this operation will maintain our tree invariants. The leaf nodes newNode and existingLeaf are still leaves (have no children,) and the new branch node has exactly two children.
An important question arises at this point; how do we pick existingLeaf? Unlike binary search trees, there is no ordering property between sibling nodes. The only condition we must maintain is that this node must be a leaf node. So then how should we proceed? Where do we place the node? Turns out there are many different algorithms for finding an appropriate location for new nodes in the tree, and each comes with its own benefits. However, what i will do is place new nodes in locations that will minimize the increase in other nodes’ areas.

We do so by iterating down the tree starting at root until we reach a leaf. At each branch node, we will go in the direction of the child whose area would increase the least if newNode becomes one of its children. More precisely, given that we wish to add newNode to the tree, and given that we are at the branch node branchNode, we will calculate the following two values


  `int increaseInRightTreeSize = AABB::unionArea(newNode->aabb branchNode->rightChild->aabb)-branchNode->rightChild->aabb.getArea();`
  
  `int increaseInLeftTreeSize = AABB::unionArea(newNode->aabb, branchNode->leftChild->aabb)-branchNode->leftChild->aabb.getArea();`

And if increaseInRightTreeSize < increaseInLeftTreeSize, we will go to the right child. Otherwise, we will go to the left child.
<p align="center">
<img width="510" alt="Screen Shot 2022-02-21 at 4 23 22 PM" src="https://user-images.githubusercontent.com/94231603/154963632-f3ffbe19-68e5-4e97-8603-783ca0a5d7f1.png">
</p>



- removeBVHMember(string name)
When we remove a node from the tree, we must make sure to maintain the invariants of the data structure: leaves have no children, branches have exactly two children, members are leaves and branches aren’t. When we want to remove a member node, we will leave its parent branch node with only a single child, which will break the invariant. For this reason, when we remove a node, call it toRemove, we will replace the branch node that is the parent of toRemove by the sibling of toRemove. This way, the grand parent of toRemove will have two children and the leaves will have no children.

- moveBVHMember(string name, AABB newLocation)
This function will change the location and dimension of a member node. When this function is called, there are two possibilities:
  1. The new location is covered by the parent of the node, in which case you will only change the location (AABB) of the node itself (not its parents).
  2. The new location of the node is not covered by its parent. In this case, you will remove the node and reinsert it in the tree.

- getCollidingObjects(AABB location)
This function takes an AABB object and will store the names of all the members that collide with this object. You must start a search from the root and go to its children. At each node, if the node’s AABB overlaps with location (you can check this using the overlap() function in the AABB struct), then:
  1. If it’s a leaf, you will add it to the vector of names of collided members
  2. If it’s a branch, you will check its children.


- Main program
The main program will read two text files, “agent.txt” and “actions.txt”. “agent.txt” will contain the member data of the agent, and the “actions.txt” file contains the actions that must be done on the BVH. The main program will use the functions that you will implement to build the BVH that satisfies “agent.txt”. Afterwards, “actions.txt” will be read and its actions will be performed on the BVH that you created. The actions that will be performed are the following:


### SYNTAX
- Collision detection with single AABB:
  c minX minY maxX maxY

- Moving a member inside the BVH to a new location:
  m member_name minX minY maxX maxY

- Print out the tree:
  p
  
### Sample Run

agent.txt
left-leg 0 0 1 3
right-leg 2 0 3 3
torso 0 3 3 7
head 1 7 3 8

actions.txt
c 1 2 2 3
c 10 10 11 11 c0318
m left-leg 1 0 2 3 c0318
p
  

We will get the following output (bolded text is user input):
Projectile (1, 2), (2, 3)
Collides with: left-leg, right-leg, torso Projectile (10, 10), (11, 11)
Collides with:
Projectile (0, 3), (1, 8)
Collides with: head, left-leg, torso
Moved the left-leg to the location (1, 0), (2, 3) Projectile (0, 3), (1, 8)
Collides with: head, left-leg, torso


Current tree:
+ branch || min = (0, 0), max = (3, 8), Area = 24
  - R - leaf: left-leg || min = (1, 0), max = (2, 3), Area = 3 
  + branch || min = (0, 0), max = (3, 8), Area = 24
  -  R - leaf: right-leg || min = (2, 0), max = (3, 3), Area = 3 
    + branch || min = (0, 3), max = (3, 8), Area = 15
    - R - leaf: torso || min = (0, 3), max = (3, 7), Area = 12
    - L - leaf: head || min = (1, 7), max = (3, 8), Area = 2


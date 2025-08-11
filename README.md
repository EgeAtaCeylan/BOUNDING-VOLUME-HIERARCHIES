# Bounding Volume Hierarchies (BVH) Collision Detection

## Overview
This project implements a **Bounding Volume Hierarchy (BVH)** for efficient collision detection in 2D space using **Axis-Aligned Bounding Boxes (AABB)**.  
The BVH is a binary tree where:
- **Leaf nodes** represent the smallest bounding boxes (hitboxes) of an object.
- **Branch nodes** represent bounding boxes that encompass their child nodes.

This technique is widely used in **game development** and **physics engines** to reduce the computational cost of collision detection.

---

## How It Works
1. Each object is broken into smaller **AABBs**.
2. The BVH tree’s root covers the entire object.
3. Collision detection begins at the root:
   - If a projectile doesn’t collide with the root, no further checks are needed.
   - If it collides, we descend into the relevant children.
4. Collision checks stop at **leaf nodes**, which correspond to actual object hitboxes.

---

## Class Structure

### `BVHTree`
The main class representing the Bounding Volume Hierarchy for a single agent (object).

**Private Members:**
- `BVHTreeNode* root` — pointer to the root of the BVH.
- `std::unordered_map<std::string, BVHTreeNode*> map` — maps member names to their corresponding tree nodes.

**Public Methods:**
- **`BVHTree()`**  
  Constructor that initializes an empty BVH.

- **`~BVHTree()`**  
  Destructor that cleans up all dynamically allocated nodes.

- **`void addBVHMember(AABB objectArea, std::string name)`**  
  Adds a new hitbox (leaf node) to the BVH.  
  - **Case 1:** Tree is empty → new node becomes the root.  
  - **Case 2:** Tree has one node → create a new branch node as root, set old root and new node as children.  
  - **Case 3:** Tree has more than one node → descend the tree choosing the branch whose area would increase the least if the new node were added.

- **`void moveBVHMember(std::string name, AABB newLocation)`**  
  Moves an existing member to a new position:  
  - If the new position fits within the current parent’s AABB → update the node’s AABB only.  
  - Otherwise → remove and reinsert the node.

- **`void removeBVHMember(std::string name)`**  
  Removes a member node and restructures the tree to maintain invariants:  
  - Replace the removed node’s parent branch with its sibling node.

- **`std::vector<std::string> getCollidingObjects(AABB object)`**  
  Returns a list of member names colliding with the given AABB:  
  - Start at root; if AABB overlaps a node’s area:  
    - If leaf → add name to result list.  
    - If branch → recurse into both children.

---

## Key Data Structures

### `BVHTreeNode`
Represents a node in the BVH tree.
- **Members:**
  - `BVHTreeNode* parent, *leftChild, *rightChild`
  - `AABB area`
  - `std::string name`
  - `bool isLeaf`
- **Invariants:**
  1. Members are always **leaves**.
  2. Leaves have **no children**.
  3. Branch nodes have **exactly two children**.

### `AABB`
Represents an Axis-Aligned Bounding Box with:
- `(minX, minY)` — bottom-left corner.
- `(maxX, maxY)` — top-right corner.
- Methods for computing area, checking collision, and creating a bounding box that contains two AABBs.

---

## Program Flow

The program uses two input files:
- **`agent.txt`** — describes the bounding boxes of each part of an agent.
- **`actions.txt`** — specifies actions to perform on the BVH.

**Main Program Steps:**
1. **Read `agent.txt`**  
   - Each line: `<name> <minX> <minY> <maxX> <maxY>`  
   - For each line, create an `AABB` and add it to the BVH using `addBVHMember`.

2. **Read and execute `actions.txt`**  
   Each line starts with a character indicating the action:
   - `c minX minY maxX maxY` →  
     - Create a temporary `AABB` for the projectile.  
     - Call `getCollidingObjects` to find all colliding members.  
     - Print projectile details and collision results.
   - `m name minX minY maxX maxY` →  
     - Create a new `AABB` for the updated position.  
     - Call `moveBVHMember` to update the BVH.
   - `p` →  
     - Print the current BVH structure using the `operator<<`.

3. **Repeat until all actions are processed**.

---

## Sample Run
actions.txt  
p   
m A1 5 1 8 4  
p  
m A3 0 0 2 1  
p  
m A2 5 1 8 2  
p  
---
agent.txt  
A1 2 3 5 6  
A2 0 2 3 5  
A3 4 2 6 5  
A4 1 0 4 1  
A5 3 6 5 7 
---
Program Output
Current tree:
+ branch || min = (0, 0), max = (6, 7), Area = 42
  + branch || min = (2, 2), max = (6, 7), Area = 20
    + branch || min = (2, 3), max = (5, 7), Area = 12
      - R - leaf: A1 || min = (2, 3), max = (5, 6), Area = 9
      - L - leaf: A5 || min = (3, 6), max = (5, 7), Area = 2
    - L - leaf: A3 || min = (4, 2), max = (6, 5), Area = 6
  + branch || min = (0, 0), max = (4, 5), Area = 20
    - R - leaf: A2 || min = (0, 2), max = (3, 5), Area = 9
    - L - leaf: A4 || min = (1, 0), max = (4, 1), Area = 3

Moved the A1 to the location (5, 1), (8, 4)
Current tree:
+ branch || min = (0, 0), max = (8, 7), Area = 56
  + branch || min = (3, 1), max = (8, 7), Area = 30
    - R - leaf: A5 || min = (3, 6), max = (5, 7), Area = 2
    + branch || min = (4, 1), max = (8, 5), Area = 16 
      - R - leaf: A3 || min = (4, 2), max = (6, 5), Area = 6
      - L - leaf: A1 || min = (5, 1), max = (8, 4), Area = 9
  + branch || min = (0, 0), max = (4, 5), Area = 20
    - R - leaf: A2 || min = (0, 2), max = (3, 5), Area = 9
    - L - leaf: A4 || min = (1, 0), max = (4, 1), Area = 3

Moved the A3 to the location (0, 0), (2, 1)
Current tree:
+ branch || min = (0, 0), max = (8, 7), Area = 56
  + branch || min = (3, 1), max = (8, 7), Area = 30
    - R - leaf: A5 || min = (3, 6), max = (5, 7), Area = 2
    - L - leaf: A1 || min = (5, 1), max = (8, 4), Area = 9
  + branch || min = (0, 0), max = (4, 5), Area = 20
    - R - leaf: A2 || min = (0, 2), max = (3, 5), Area = 9
    + branch || min = (0, 0), max = (4, 1), Area = 4
      - R - leaf: A4 || min = (1, 0), max = (4, 1), Area = 3
      - L - leaf: A3 || min = (0, 0), max = (2, 1), Area = 2

Moved the A2 to the location (5, 1), (8, 2)
Current tree:
+ branch || min = (0, 0), max = (8, 7), Area = 56
  + branch || min = (3, 1), max = (8, 7), Area = 30
    - R - leaf: A5 || min = (3, 6), max = (5, 7), Area = 2
    + branch || min = (5, 1), max = (8, 4), Area = 9
      - R - leaf: A1 || min = (5, 1), max = (8, 4), Area = 9
      - L - leaf: A2 || min = (5, 1), max = (8, 2), Area = 3
  + branch || min = (0, 0), max = (4, 1), Area = 4
    - R - leaf: A4 || min = (1, 0), max = (4, 1), Area = 3
    - L - leaf: A3 || min = (0, 0), max = (2, 1), Area = 2



## Usage
1. Prepare `agent.txt` describing the agent’s hitboxes.
2. Prepare `actions.txt` containing actions (`c`, `m`, `p`).
3. Compile and run the program with both files in the working directory.



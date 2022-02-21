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



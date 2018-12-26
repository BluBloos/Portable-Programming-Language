from tree import Tree
from tree import TreePointer

tree = Tree(3, 0)
tree.GiveBirth(4)
print("Depth: " + str(tree.depth))
tree2 = Tree(6,0)
tree2.GiveBirth(6)
tree2.GiveBirth(7)
tree.Adopt(tree2)
tree.Print(0)
print("Depth: " + str(tree.depth))

'''
families = tree.GetFamilies(treeOG.depth - 1)
for parent, siblings in families:
    print("Parent:")
    parent.Print(0)
    child1, child2 = siblings
    print("Child1:")
    child1.Print(0)
    if child2:
        print("Child2:")
        child2.Print(0)
'''

tree.PrintWeights(0)

#TODO(Noah): Make stripping remove the depth, and weights, because we have modified the tree!
tree.Strip()
tree.Print(0)
tree.PrintWeights(0)
print("Depth: " + str(tree.depth))

#pointer = TreePointer(treeOG, 4, 0)
#print(pointer.Get())

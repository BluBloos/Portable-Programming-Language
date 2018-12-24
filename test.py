from tree import Tree
from tree import TreePointer

treeOG = Tree(3, 0)
treeOG.GiveBirth(4)
treeOG.GiveBirth(5)
tree = treeOG.RandomDescent()
tree.GiveBirth(4)
tree.GiveBirth(5)
treeOG.Print(0)
treeOG.PrintWeights(0)

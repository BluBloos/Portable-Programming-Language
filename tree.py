import sys

class Tree:
    def __init__(self, data, children=None):
        self.children = children
        self.data = data
    def Print(tree, indentation):
        print(" " * indentation +  str(tree.data))
        if tree.children:
            for child in tree.children:
                child.Print(indentation + 2)
    def Node(self, data):
        if not self.children:
            self.children = [Tree(data)]
        else:
            self.children.append(Tree(data))
    def Len(self):
        len = 1
        if self.children:
            nextNode = self.children[0]
            while nextNode:
                len += 1
                if nextNode.children:
                    nextNode = nextNode.children[0]
                else:
                    nextNode = False
        return len
    def Strip(self):
        #go to all children Trees at bottom layer - 1 and set their children to None
        treePointer = TreePointer(self.Len - 1, 0)
        tree = treePointer.Advance()
        while tree:
            tree.children = None
            tree = treePointer.Advance()

class TreePointer:
    def __init__(self, layer, index):
        pass
    def Advance(self):
        pass

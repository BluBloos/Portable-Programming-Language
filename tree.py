import sys
import random

class Tree:
    def __init__(self, data, number, children=None, parent=None):
        self.parent = parent
        self.children = children
        self.data = data
        self.weights = []
        self.number = number
        if children:
            for i in range(len(children)):
                self.weights.append(1)

    def Print(tree, indentation):
        print(" " * indentation +  str(tree.data) + ", " + str(tree.number))
        if tree.children:
            for child in tree.children:
                child.Print(indentation + 2)

    def PrintWeights(tree, indentation):
        print(" " * indentation +  str(tree.weights))
        if tree.children:
            for child in tree.children:
                child.PrintWeights(indentation + 2)

    def GiveBirth(self, data):
        if not self.children: #If I am childless
            self.children = [Tree(data, 0, parent=self)]
        else:
            number = len(self.children)
            self.children.append(Tree(data, number, parent=self))

        #update the weights
        self.weights.append(1)

        #traverse up the tree and update the weights!
        parent = self.parent
        me = self
        while parent:
            parent.weights[me.number] += 1
            me, parent = parent, parent.parent

    def RandomDescent(self):
        range = len(self.children)
        return self.children[int(random.random() * range)]

    '''
    def Node(self, data):
        if not self.children:
            self.children = [Tree(data)]
        else:
            self.children.append(Tree(data))
    '''

    #TODO(Noah): Make this function more elegant.
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
        #Go to all children Trees at bottom layer - 1 and set their children to None
        treePointer = TreePointer(self, self.Len() - 1, 0)
        tree = treePointer.Advance()
        while tree:
            tree.children = None
            tree = treePointer.Advance()



class TreePointer:
    def __init__(self, tree, layer, index):
        self.tree = tree
        self.layer = layer
        self.index = index

    def Advance(self):
        self.index += 1
        #check whether there is a thing at this index
        return self.Get()

    def Get(self):
        root = self.tree.ToBinaryTree()
        currentLayer = 0
        leftRelative = 0
        result = False

        while(1):
            normalizedLayer = (layer - currentLayer)
            #compute max nodes at layer for left side
            maxNodesRemaining = root.leftWeight - (normalizedLayer - 1)
            maxNodesLeft = min( (2**normalizedLayer) / 2, maxNodesRemaining)

            if (index < leftRelative + maxNodesLeft - 1) and (index > leftRelative - 1):
                #index is down the left path
                currentLayer += 1 #step down
                root = root.left #go down the left path
                continue

            #compute max nodes at layer for right side
            maxNodesRemaining = root.rightWeight - (normalizedLayer - 1)
            maxNodesRight = min( (2**normalizedLayer) / 2, maxNodesRemaining)

            if (index < leftRelative + maxNodesLeft + maxNodesRight - 1) and (index > leftRelative + maxNodesLeft - 1):
                #index is down the right path
                currentLayer += 1
                root = root.right
                continue

            break

        return result

    def Set(self, data):
        tree = self.Get()
        tree.data = data

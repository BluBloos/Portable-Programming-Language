import sys
import random

class Tree:
    def __init__(self, data, number, children=None, parent=None):
        self.parent = parent
        self.children = children
        self.data = data
        self.weights = []
        self.number = number
        self.depth = 1
        self.numberOfNodes = 1
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

    def GetRootAndLayer(self):
        parent = self.parent
        previousParent = self
        layer = 0
        while parent:
            layer += 1
            previousParent, parent = parent, parent.parent
        return (previousParent, layer)

    def GiveBirth(self, data):
        if not self.children: #If I am childless
            self.children = [Tree(data, 0, parent=self)]
            self.depth += 1

            #traverse up the tree and update the depth
            parent = self.parent
            while parent:
                parent.depth += 1
                parent = parent.parent

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

        self.numberOfNodes += 1

    def Adopt(self, tree):
        tree.number = 0
        tree.parent = self
        numberOfNodes = tree.numberOfNodes
        depthAddition = tree.depth
        root, layer = self.GetRootAndLayer()
        deltaDepth = root.depth - (layer + 1)

        if not self.children: #If I am childless
            self.children = [tree]
        else:
            tree.number = len(self.children)
            self.children.append(tree)

        #traverse up the tree and update the depth
        depthAddition -= deltaDepth
        self.depth += depthAddition
        parent = self.parent
        while parent:
            parent.depth += depthAddition
            parent = parent.parent

        #update the weights
        self.weights.append(numberOfNodes)

        #traverse up the tree and update the weights!
        parent = self.parent
        me = self
        while parent:
            parent.weights[me.number] += numberOfNodes
            me, parent = parent, parent.parent

        self.numberOfNodes += numberOfNodes

    def RandomDescent(self):
        range = len(self.children)
        return self.children[int(random.random() * range)]

    def Strip(self):
        #Go to all children Trees at bottom layer - 1 and set their children to None
        treePointer = TreePointer(self, self.depth - 2, 0)
        #print("Layer: " + str(treePointer.layer))
        tree = treePointer.Get()
        while tree:
            numberOfChildren = 0
            if tree.children:
                numberOfChildren = len(tree.children)

            tree.children = None
            if numberOfChildren:
                tree.depth -= 1
            tree.weights = []
            #print("Me: " + str(tree.data))

            #traverse up the tree and update the depth and weights!
            parent = tree.parent
            me = tree
            while parent:
                if numberOfChildren:
                    parent.depth -= 1
                parent.weights[me.number] -= numberOfChildren
                me, parent = parent, parent.parent

            tree = treePointer.Advance()

    def GetFamilies(self, layer):
        families = []
        treePointer = TreePointer(self, layer, 0)

        tree = treePointer.Get()
        while tree:
            print("GETTING FAMILY")
            #construct a family
            child1 = tree
            print("Child1: " + child1.data)
            child2 = None
            parent = None
            if tree.parent:
                parent = tree.parent
                print("Parent: " + parent.data)
            tree = treePointer.Advance()
            if tree and tree.parent == parent:
                child2 = tree
                print("final advance")
                tree = treePointer.Advance()
            families.append((parent, (child1, child2)))

        return families

class TreePointer:
    def __init__(self, tree, layer, index):
        self.tree = tree
        self.layer = layer
        self.index = index

    def Advance(self):
        self.index += 1
        #check whether there is a thing at this index
        return self.Get()

    #TODO(Noah): Make this function more rigorous
    def Get(self):
        root = self.tree
        currentLayer = 0
        leftRelative = 0
        result = False

        while(1):
            normalizedLayer = (self.layer - currentLayer)
            print("Index: " + str(self.index))
            print("Normalized Layer: " + str(normalizedLayer))
            print("LEFT:")
            #compute max nodes at layer for left side
            maxNodesRemaining = root.weights[0] - (normalizedLayer - 1)
            print("maxNodesRemaining: " + str(maxNodesRemaining))
            maxNodesLeft = min( (2**normalizedLayer) / 2, maxNodesRemaining)
            print("maxNodesLeft: " + str(maxNodesLeft))
            print("lowerBound: " + str(leftRelative))
            print("upperBound: "  + str(leftRelative + maxNodesLeft - 1))
            if (self.index <= leftRelative + maxNodesLeft - 1) and (self.index >= leftRelative):
                #index is down the left path
                if normalizedLayer == 1:
                    result = root.children[0]
                    break
                else:
                    currentLayer += 1 #step down
                    root = root.children[0] #go down the left path
                    print("went down left path")
                continue

            print("RIGHT:")
            #compute max nodes at layer for right side
            if len(root.weights) == 2:
                maxNodesRemaining = root.weights[1] - (normalizedLayer - 1)
                print("maxNodesRemaining: " + str(maxNodesRemaining))
                maxNodesRight = min( (2**normalizedLayer) / 2, maxNodesRemaining)
                print("maxNodesRight: " + str(maxNodesRight))
                print("lowerBound: " + str(leftRelative + maxNodesLeft))
                print("upperBound: "  + str(leftRelative + maxNodesLeft + maxNodesRight - 1))
                if (self.index <= leftRelative + maxNodesLeft + maxNodesRight - 1) and (self.index >= leftRelative + maxNodesLeft):
                    #index is down the right path
                    if normalizedLayer == 1:
                        result = root.children[1]
                        break
                    else:
                        currentLayer += 1 #step down
                        root = root.children[1] #go down the left path
                        leftRelative += maxNodesLeft
                        print("went down right path")
                    continue

            break

        return result

    def Set(self, data):
        tree = self.Get()
        tree.data = data

class Tree:

    def __init__(self, data, parent=None, modifier=None):
        self.parent = parent
        self.children = []
        self.data = data
        self.modifier = modifier
        # self.weights = [] # what is this advanced stuff?
        # self.number = num # also not sure what this is
        
        # depth is the amount of layers beneath this node.
        self.depth = 0
        
        # self.numberOfNodes = 1 # also was is this used for?
        '''
        if children:
            # my guess is that weights is the amount of 
            # nodes if we chose a specific path down the tree?
            for i in range(len(children)):
                self.weights.append(1)
        '''

    def __str__(self):
        return self.data

    def Print(tree, indentation, logger):
        # print(" " * indentation +  str(tree.data) + ", " + str(tree.number))
        logger.Log(" " * indentation +  str(tree.data))
        if tree.children:
            for child in tree.children:
                child.Print(indentation + 2, logger)
    
    def UpdateDepth(self, delta):
        self.depth += delta
        parent = self.parent
        while parent:
            parent.depth += delta
            parent = parent.parent

    # For creating leaf nodes.
    def GiveBirth(self, data):
        if not self.children: # Tree currently has no children
            self.children = [ Tree(data, parent=self) ]
            self.UpdateDepth(1)
        else:
            self.children.append( Tree(data, parent=self) )
    
    # Add a tree as a child to this tree.
    def Adopt(self, tree):
        if not self.children: # Tree currently has no children
            self.children = [tree]
        else:
            self.children.append(tree)
        maxCurrentDepth = self.depth
        # NOTE(Noah): depthDelta calc is ridiculously simple. 
        # Do not know what is going on with the stuff in PlasmaCompiler.
        depthDelta = max(0, (tree.depth + 1) - self.depth)
        self.UpdateDepth(depthDelta)

        

            
            

            

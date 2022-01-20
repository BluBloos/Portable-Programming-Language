#ifndef TREE_H
#define TREE_H
enum tree_type {
    TREE_ROOT = 0,
    TREE_REGEX_STR,
    TREE_REGEX_ANY,
    TREE_REGEX_GROUP,
    TREE_REGEX_CHAR
};

struct tree_metadata {
    char regex_mod;
    union { // kind of like the data storage for tree.
        char c;
        char *str;
    };
};

/*
Okay, this is the thing that I did not want to do. but I have been forced by to
do this because, well....I do not know what else to do...

So here is what must be done:
    - Make a struct for a tree_node (this guy is the full deal man...the full tree).
    - This struct contains inside it a list of children (each of this are dynamic lists).
    - And every element of children is another tree_node.
    - Okay and that's it!
*/

struct tree_node {
    public:
    
    /* psuedo-linked list style */
    struct tree_node *children;
    unsigned int childrenCount;
    unsigned int childrenContainerCount;
    /* linked list style */
    
    enum tree_type type;
    struct tree_metadata metadata;
};

struct tree_node CreateTree(enum tree_type type) {
    
    struct tree_node tn = {}; // initialize everything to zero for sanity purposes.
    tn.type = type;
    tn.children = NULL;
    tn.childrenCount = 0;
    tn.childrenContainerCount = 0;
    tn.metadata.regex_mod = 0; // null character (null-terminator).
    return tn;
}

struct tree_node CreateTree(enum tree_type type, char c) {
    struct tree_node tn = CreateTree(type);
    tn.metadata.c = c;
    return tn;
}

struct tree_node CreateTree(enum tree_type type, const char *str) {
    struct tree_node tn = CreateTree(type);
    tn.metadata.str = MEMORY_ARENA.StringAlloc((char *)str);
    return tn;
}

// Needs to be a full-blown tree that gets adopted. Not just a leaf node.
void TreeAdoptTree(struct tree_node &tn, struct tree_node child) {
    // TODO(Noah): Maybe abstract stuff and make a dynamic list type :)
    // because this same type of pattern is also done in tokenContainer.
    if (!(tn.childrenCount < tn.childrenContainerCount)) {
        if (tn.childrenCount > 0) {
            tn.childrenContainerCount += 100;
            struct tree_node* _trees = (struct tree_node *)realloc(tn.children, tn.childrenContainerCount * sizeof(struct tree_node));
            Assert(_trees != NULL);
            tn.children = _trees;
        } else {
            // doesn't make sense to realloc memory that has not been alloc yet.
            // so here we do a malloc to init the array.
            tn.childrenContainerCount = 100;
            struct tree_node* _trees = (struct tree_node *)malloc(tn.childrenContainerCount * sizeof(struct tree_node));
            Assert(_trees != NULL);
            tn.children = _trees;
        }
    }
    memcpy(&tn.children[tn.childrenCount++], &child, sizeof(struct tree_node)); 
}

// TODO(Noah): test this function
void DeallocTree(struct tree_node &tn) {
    if (tn.children == NULL) {
        return;
    } else {
        for (int i = 0; i < tn.childrenCount; i++) {
            DeallocTree(tn.children[i]);
        }
        free(tn.children);
    }
}

void PrintTree(struct tree_node &tn, unsigned int indentation) {
    
    // NOTE(Noah): Max indentation is 80 ' ' . Any deeper AST get like, "truncated", or whatever.
    char sillyBuff[81]; // 1 extra than 80 for the null-terminator.
    int i;
    for (i = 0; i < indentation && i < 80; i++) {
        sillyBuff[i] = ' ';
    }
    sillyBuff[i] = 0; // null terminator.

    switch(tn.type) {
        case TREE_REGEX_STR:
        Assert(tn.metadata.str != NULL);
        LOGGER.Min("%s%s", sillyBuff, tn.metadata.str);
        break;
        case TREE_REGEX_ANY:
        LOGGER.Min("%sAny", sillyBuff);
        break;
        case TREE_REGEX_GROUP:
        LOGGER.Min("%sGroup", sillyBuff);
        break;
        case TREE_REGEX_CHAR:
        LOGGER.Min("%s%c", sillyBuff, tn.metadata.c);
        break;
    }

    if (tn.metadata.regex_mod > 0) {
        LOGGER.Min("%c\n", tn.metadata.regex_mod);
    } else {
        LOGGER.Min("\n");
    }

    if (tn.children != NULL) {
        for (int i = 0; i < tn.childrenCount; i++) {
            PrintTree(tn.children[i], indentation + 2);
        }
    }

}

#endif TREE_H
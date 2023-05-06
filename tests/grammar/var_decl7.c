SnakeBody :: struct {

    make_default :: static () -> SnakeBody
    {
        return {-10; -10};
    }

    x : int = 0;
    y : int = 0;
    
}
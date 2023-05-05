SnakeBody :: struct {
    x : int = 0;
    y : int = 0;
    // so the function below goes as a statement_noend.
    make_default := static () -> SnakeBody
    {
        return {-10; -10};
    }
}
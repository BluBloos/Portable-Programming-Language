SnakeBody :: struct {
    x : int = 0;
    y : int = 0;
    // so the function below goes as a statement_noend,
    // but since it is a runtime var decl, normally it requires
    // `;`.
    make_default := static fn() -> SnakeBody
    {
        return {-10; -10};
    }
}
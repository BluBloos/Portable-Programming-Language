int main() {
    switch (ppl::getchar()) {            
        case 'D': 
            fallthrough;
        case 'd': if (DIRECTION != DIRECTION_LEFT)  DIRECTION = DIRECTION_RIGHT;

        case 'S': 
            fallthrough;
        case 's': if (DIRECTION != DIRECTION_UP)    DIRECTION = DIRECTION_DOWN;

        case 'A': 
            fallthrough;
        case 'a': if (DIRECTION != DIRECTION_RIGHT) DIRECTION = DIRECTION_LEFT;

        case 'W': 
            fallthrough;
        case 'w': if (DIRECTION != DIRECTION_DOWN)  DIRECTION = DIRECTION_UP;

        case 27: // ESC key
        exit(); 
        return 0;
        case:
        return 1;
    }
}
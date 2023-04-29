switch (ppl.getchar()) {            
    case 'D': 
    case 'd':
        if (DIRECTION != DIRECTION_LEFT)  DIRECTION = DIRECTION_RIGHT;
        break;
    case 'S': 
    case 's':
        if (DIRECTION != DIRECTION_UP)    DIRECTION = DIRECTION_DOWN;
        break;
    case 'A': 
    case 'a':
        if (DIRECTION != DIRECTION_RIGHT) DIRECTION = DIRECTION_LEFT;
        break;
    case 'W': 
    case 'w':
        if (DIRECTION != DIRECTION_DOWN)  DIRECTION = DIRECTION_UP;
        break;
    case 27: // ESC key
        exit(); 
        return 0;
    case:
        return 1;
}
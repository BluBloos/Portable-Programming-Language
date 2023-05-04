switch //ppl.getchar() {
    ppl {           
    case 'D': fall;
    case 'd':
        if DIRECTION != DIRECTION_LEFT then  DIRECTION = DIRECTION_RIGHT;
    case 'S': fall;
    case 's':
        if DIRECTION != DIRECTION_UP   then  DIRECTION = DIRECTION_DOWN;
    case 'A': fall;
    case 'a':
        if DIRECTION != DIRECTION_RIGHT then DIRECTION = DIRECTION_LEFT;
    case 'W': fall;
    case 'w':
        if DIRECTION != DIRECTION_DOWN then DIRECTION = DIRECTION_UP;
    case 27: // ESC key
        exit(); 
        return 0;
    default:
        return 1;
}
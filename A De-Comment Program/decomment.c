#include <stdio.h>
#include <stdlib.h>

/* Defines constants representing each state in the DFA */
enum Statetype {NORMAL, SLASH, IN_COMMENT, STAR, IN_STRING, ESCAPED_IN_STRING, IN_CHAR, ESCAPED_IN_CHAR};

int comment_start_line = 0;  
int line = 1;


/* Implements the NORMAL state of the DFA. c is the current DFA character. Writes c to stdout (except for when c == '/'). 
Returns the next DFA state. */
enum Statetype handleNormalState (int c) {
    if (c == '/') return SLASH;
    else if (c == '\"') { putchar(c); return IN_STRING; }
    else if (c == '\'') { putchar(c); return IN_CHAR; }
    else if (c == '\n') { putchar('\n'); return NORMAL; }
    putchar(c);
    return NORMAL;
}

/* Implements the SLASH state of the DFA. c is the current DFA character. Writes c to stdout. Returns the next DFA state. 
Keeps track of the comment start line. */
enum Statetype handleSlashState (int c) {
    if (c == '*') { comment_start_line = line; putchar(' '); return IN_COMMENT; }
    else if (c == '/') { putchar(c); return SLASH; }
    else if (c == '\"') { putchar('/'); putchar(c); return IN_STRING; }
    else if (c == '\'') { putchar('/'); putchar(c); return IN_CHAR; }
    putchar('/');
    putchar(c);
    return NORMAL; 
}

/* Implements the IN_COMMENT state of the DFA. c is the current DFA character. Write c to stdout (except for when c == '*'). 
Return the next DFA state. */
enum Statetype handleInCommentState (int c) {
    if (c == '*') return STAR; 
    if (c == '\n') putchar('\n');
    return IN_COMMENT;
}

/* Implements the STAR state of the DFA. c is the current DFA character. Write c to stdout (except for when c == '*' or '/'). 
Return the next DFA state. */
enum Statetype handleStarState (int c) {
    if (c == '*') return STAR;
    else if (c == '/') return NORMAL;
    else if (c == '\n') { putchar('\n'); return IN_COMMENT; }
    return IN_COMMENT; 
}

/* Implements the IN_STRING state of the DFA. c is the current DFA character. Write c to stdout. 
Return the next DFA state. */
enum Statetype handleInStringState (int c) {
    if (c == '\\') { putchar(c); return ESCAPED_IN_STRING; }
    else if (c == '\"') { putchar(c); return NORMAL; }
    putchar(c);
    return IN_STRING; 
}

/* Implements the ESCAPED_IN_STRING state of the DFA. c is the current DFA character. Write c to stdout. 
Return the next DFA state. */
enum Statetype handleEscapedInStringState (int c) {
    putchar(c);
    return IN_STRING; 
}

/* Implements the IN_CHAR state of the DFA. c is the current DFA character. Write c to stdout. 
Return the next DFA state. */
enum Statetype handleInCharState (int c) {
    if (c == '\\') { putchar(c); return ESCAPED_IN_CHAR; }
    else if (c == '\'') { putchar(c); return NORMAL; }
    putchar(c);
    return IN_CHAR; 
}

/* Implements the ESCAPED_IN_CHAR state of the DFA. c is the current DFA character. Write c to stdout. 
Return the next DFA state. */
enum Statetype handleEscapedInCharState (int c) {
    putchar(c);
    return IN_CHAR; 
}

/* Read text from Stdin. Replace all comments with a space. Account for all the exception cases. 
Write the result to stdout. Return EXIT_FAILURE if unterminated comments exist but return EXIT_SUCCESS otherwise. */
int main(void) {
    int c;
    enum Statetype state = NORMAL;

    while ((c = getchar()) != EOF) {
        switch (state) {
            case NORMAL: 
                state = handleNormalState(c);
                break;
            case SLASH: 
                state = handleSlashState(c);
                break;
            case IN_COMMENT: 
                state = handleInCommentState(c);
                break;
            case STAR: 
                state = handleStarState(c);
                break;
            case IN_STRING: 
                state = handleInStringState(c);
                break;
            case ESCAPED_IN_STRING: 
                state = handleEscapedInStringState(c);
                break;
            case IN_CHAR: 
                state = handleInCharState(c);
                break;
            case ESCAPED_IN_CHAR: 
                state = handleEscapedInCharState(c);
                break;
        }

        if (c == '\n') line++; 
    }
    if (state == SLASH) putchar('/');
    if (state == IN_COMMENT || state == STAR) {
        fprintf(stderr, "Error: line %d: unterminated comment\n", comment_start_line);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

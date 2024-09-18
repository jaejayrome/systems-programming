#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

#define NORMAL_TEXT_STATE 0
#define PRE_COMMENT_STATE 1
#define SINGLE_LINE_COMMENT_STATE 2
#define MULTI_LINE_COMMENT_STATE 3
#define MULTI_LINE_COMMENT_END_STATE 4
#define STRING_STATE 5
#define CHAR_CONSTANT_STATE 6

// Function to handle state transitions
int stateMachine(char input, int state) {
    switch (state) {
        case NORMAL_TEXT_STATE:
            if (input == '/') return PRE_COMMENT_STATE;
            if (input == '"') return STRING_STATE;
            if (input == '\'') return CHAR_CONSTANT_STATE;
            return NORMAL_TEXT_STATE;

        case PRE_COMMENT_STATE:
            if (input == '/') return SINGLE_LINE_COMMENT_STATE;
            if (input == '*') return MULTI_LINE_COMMENT_STATE;
            return NORMAL_TEXT_STATE;

        case SINGLE_LINE_COMMENT_STATE:
            return (input == '\n') ? NORMAL_TEXT_STATE : SINGLE_LINE_COMMENT_STATE;

        case MULTI_LINE_COMMENT_STATE:
            return (input == '*') ? MULTI_LINE_COMMENT_END_STATE : MULTI_LINE_COMMENT_STATE;

        case MULTI_LINE_COMMENT_END_STATE:
            if (input == '/') return NORMAL_TEXT_STATE;
            if (input == '*') return MULTI_LINE_COMMENT_END_STATE;
            return MULTI_LINE_COMMENT_STATE;

        case STRING_STATE:
            return (input == '"') ? NORMAL_TEXT_STATE : STRING_STATE;

        case CHAR_CONSTANT_STATE:
            return (input == '\'') ? NORMAL_TEXT_STATE : CHAR_CONSTANT_STATE;

        default:
            return state;
    }
}

// Function to concatenate two strings
char *concatString(char *str1, const char *str2) {
    size_t finalSize = strlen(str1) + strlen(str2) + 1;
    char *newStr = (char *)calloc(finalSize, sizeof(char));
    strcpy(newStr, str1);
    strcat(newStr, str2);

    free(str1);
    return newStr;
}

// Function to handle line number updates
void handleLineNumber(int *lineNumber, char ch) {
    if (ch == '\n') {
        (*lineNumber)++;
    }
}

int main(void) {
    int ich;
    int lineNumber = 1;
    int commentStartLine = 0; // To track the line number where a comment starts
    char ch[2] = {0}; // Buffer for reading characters
    int currentState = NORMAL_TEXT_STATE;
    char *result = (char *)calloc(1, sizeof(char));
    result[0] = '\0';
    setlocale(LC_ALL, "");

    while ((ich = getchar()) != EOF) {
        ch[0] = (char)ich;
        ch[1] = '\0';

        int prevState = currentState;
        currentState = stateMachine(ch[0], currentState);

        // Handle single-line comments
        if (prevState == PRE_COMMENT_STATE && currentState == SINGLE_LINE_COMMENT_STATE) {
            while ((ich = getchar()) != EOF && ich != '\n');
            currentState = NORMAL_TEXT_STATE;
            result = concatString(result, "\n");
            lineNumber++;
            continue;
        }

        // Handle multi-line comments
        if (prevState == PRE_COMMENT_STATE && currentState == MULTI_LINE_COMMENT_STATE) {
            commentStartLine = lineNumber; // Record the line number where the multi-line comment starts
            int unterminatedComment = 1; // Flag for detecting unterminated comment
            while ((ich = getchar()) != EOF) {
                if (ich == '\n') {
                    result = concatString(result, "\n");
                    lineNumber++;
                }
                if (ich == '*') {
                    if ((ich = getchar()) == '/') {
                        unterminatedComment = 0;
                        break;
                    } else {
                        ungetc(ich, stdin);
                    }
                }
            }
            result = concatString(result, " ");
            if (unterminatedComment) {
                printf("%s", result);
                fprintf(stderr, "Error: line %d: unterminated comment\n", commentStartLine);
                free(result);
                return EXIT_FAILURE;
            }
            currentState = NORMAL_TEXT_STATE;
            continue;
        }

        // Handle '/' not followed by a comment
        if (prevState == PRE_COMMENT_STATE && currentState == NORMAL_TEXT_STATE) {
            result = concatString(result, "/");
        }

        // Append normal characters, strings, and char constants
        if (currentState == NORMAL_TEXT_STATE || currentState == STRING_STATE || currentState == CHAR_CONSTANT_STATE) {
            result = concatString(result, ch);
        }

        // Update line number for any newline character
        handleLineNumber(&lineNumber, ch[0]);
    }

    printf("%s", result);
    free(result);
    return EXIT_SUCCESS;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

// State Machine States
// 0: Normal Code State
// 1: Pre-Comment Code State (encountered /)
// 2: Single-Line Comment Code State
// 3: String State (Inside a string literal)

// 6: Multi-Line Comment End State (encountered * after /*)
// 7: Multi-Line Comment State

/* Method would remove the preceding '/' whenever there is a transition */
char *removeLastCharacterOfResult(char *prevResult)
{
  if (strlen(prevResult) > 0)
    prevResult[strlen(prevResult) - 1] = '\0';
  return prevResult;
}

int stateMachine(char prevCharacter, char incomingCharacter, int incomingState)
{
  switch (incomingCharacter)
  {
  case '/':
    if (incomingState == 3 || incomingState == 4)
    {
      return incomingState; // string state would override
    }
    if (incomingState == 1 && prevCharacter == '/') // `//`
      return 2;                                     // Single-line comment state
    if (incomingState == 1 && prevCharacter == '*') // `/* ... /`
      return 0;                                     // Back to normal code
    if (incomingState == 0)
      return 1;                                     // Pre-comment state
    if (incomingState == 6 && prevCharacter == '*') // Exiting multi-line comment `*/`
      return 0;                                     // Return to normal code state
    break;
  case '*':
    if (incomingState == 3 || incomingState == 4)
    {
      return incomingState; // string state would override
    }
    if (prevCharacter == '/' && incomingState == 1) // `/*`
      return 7;                                     // Multi-line comment state
    if (incomingState == 7 && prevCharacter == '*') // `*` inside multi-line comment
      return 6;                                     // Potential end of multi-line comment
    break;
  case '\n':
    if (incomingState == 2) // Newline ends single-line comment
      return 0;             // Back to normal code state
    break;
  case '\'':
    if (incomingState == 4)
    {
      return 0;
    }
    if (incomingState == 2 || incomingState == 7) // String literals inside comments
      return incomingState;
    if (incomingState == 0)
    {
      return 4;
    }
  case '"':
    if (incomingState == 3)
    {
      return 0;
    }
    if (incomingState == 2 || incomingState == 7) // String literals inside comments
      return incomingState;                       // Remain in the same state
    if (incomingState == 0)                       // Enter string literal state from normal code
      return 3;                                   // String state
    break;
  default:
    if (incomingState == 1) // If in pre-comment state and the character is not `/` or `*`
      return 0;             // Back to normal code
    if (incomingState == 2)
    {
      return 2;
    }
    if (incomingState == 6) // If `*/` end of multi-line comment not confirmed
      return 7;             // Stay in multi-line comment
    break;
  }
  return incomingState;
}

char *concatString(char *str1, const char *str2)
{
  int str1Length = strlen(str1);
  int str2Length = strlen(str2);
  size_t finalSize = str1Length + str2Length + 1;

  // Allocate new memory for the concatenated result
  char *appendResult = (char *)calloc(finalSize, sizeof(char));

  // Copy str1 and str2 into the new memory
  strcpy(appendResult, str1);
  strcat(appendResult, str2);

  free(str1);          // Free the old memory
  return appendResult; // Return the new concatenated result
}

int main(void)
{
  int ich;
  int lineNumber = 1;
  char ch[2] = {0};          // Buffer for reading characters
  int currentState = 0;      // Start in normal code state
  char prevCharacter = '\0'; // Used to store the previous character
  char *result = (char *)calloc(1, sizeof(char));
  result[0] = '\0';
  setlocale(LC_ALL, "");

  while (1)
  {
    ich = getchar();

    if (ich == EOF)
    {
      break;
    }
    int temp = currentState;
    ch[0] = (char)ich;
    ch[1] = '\0';

    currentState = stateMachine(prevCharacter, ch[0], currentState);

    // Case 1: Handle single-line comments
    if (temp == 1 && currentState == 2)
    {
      while ((ich = getchar()) != EOF && ich != '\n')
        ;
      currentState = 0;
      removeLastCharacterOfResult(result);
      result = concatString(result, " ");
      lineNumber++;
      char prevCharacter = '\0';
      continue;
    }

    // Case 2: Handle multi-line comments
    if (temp == 1 && currentState == 7)
    {
      int isUnterminatedComment = 1;
      // Skip characters until the end of multi-line comment
      while ((ich = getchar()) != EOF)
      {
        if (ich == '\n')
        {
          lineNumber++;
        }
        if (ich == '*')
        {
          if ((ich = getchar()) == '/')
          {
            isUnterminatedComment = 0;
            break;
          }
          else
          {
            ungetc(ich, stdin);
          }
        }
      }
      removeLastCharacterOfResult(result);
      result = concatString(result, " ");
      if (isUnterminatedComment)
      {
        printf("%s", result);
        fprintf(stderr, "Error: Unterminated comment detected at line %d\n", lineNumber);
        // Exit with failure
        return (EXIT_FAILURE);
      }
      currentState = 0;
      char prevCharacter = '\0';
      continue;
    }

    // Case 3: (Default Case) Normal character addition
    if (currentState == 0 || currentState == 3 || currentState == 4 || currentState == 1)
    {
      result = concatString(result, ch);
    }

    // Update previous character
    prevCharacter = ch[0];
  }

  printf("%s", result);
  free(result);
  return (EXIT_SUCCESS);
}

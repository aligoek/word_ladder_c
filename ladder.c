#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct stack
{
    int top;
    char **word;
} stack;

typedef struct queue
{
    int front, rear, size;
    int capacity;
    stack **stacks;
} queue;

stack *createStack(int capacity);

queue *createQueue(int capacity)
{
    queue *q = (queue *)malloc(sizeof(queue));
    q->capacity = capacity;
    q->front = q->size = 0;
    q->rear = capacity - 1;
    q->stacks = (stack **)malloc(capacity * sizeof(stack *));
    return q;
}

int isQueueEmpty(queue *q)
{
    return q->size == 0;
}

int enqueue(queue *q, stack *s)
{
    if (q->size == q->capacity)
    {
        return 0;
    }
    q->rear = (q->rear + 1) % q->capacity;
    q->stacks[q->rear] = s;
    q->size = q->size + 1;
    return 1;
}

stack *dequeue(queue *q)
{
    if (isQueueEmpty(q))
    {
        return NULL;
    }
    stack *s = q->stacks[q->front];
    q->front = (q->front + 1) % q->capacity;
    q->size = q->size - 1;
    return s;
}

stack *createStack(int capacity)
{
    stack *s = (stack *)malloc(sizeof(stack));
    s->top = -1;
    s->word = (char **)malloc(capacity * sizeof(char *));
    return s;
}

int isEmpty(stack *s)
{
    return s->top == -1;
}

int push(stack *s, char *word)
{
    s->top++;
    s->word[s->top] = word;
    return 1;
}

char *pop(stack *s)
{
    if (isEmpty(s))
    {
        return NULL;
    }
    return s->word[s->top--];
}

char *peek(stack *s)
{
    if (isEmpty(s))
    {
        return NULL;
    }
    return s->word[s->top];
}

char **readWords(char *fileName, int *wordCount)
{
    FILE *file = fopen(fileName, "r");
    if (file == NULL)
    {
        printf("Error: Failed to open the file.\n");
        return NULL;
    }

    char line[50];
    int wordCounter = 0;
    char **words = NULL;

    while (fgets(line, sizeof(line), file) != NULL)
    {
        char *word = strtok(line, " \t\n");
        while (word != NULL)
        {
            words = realloc(words, (wordCounter + 1) * sizeof(*words));
            words[wordCounter] = malloc(strlen(word) + 1);
            strcpy(words[wordCounter], word);
            wordCounter++;
            word = strtok(NULL, " \t\n");
        }
    }

    fclose(file);
    *wordCount = wordCounter;
    return words;
}

int checkIfWordExists(char *word, char **words, int wordCount)
{
    int i;
    for (i = 0; i < wordCount; i++)
    {
        if (strcmp(word, words[i]) == 0)
        {
            return 1;
        }
    }
    return 0;
}

stack *copyStack(stack *s)
{
    stack *newStack = createStack(100);
    int i;
    for (i = 0; i <= s->top; i++)
    {
        push(newStack, s->word[i]);
    }
    return newStack;
}

int checkDifference(char *word1, char *word2)
{
    if (strlen(word1) != strlen(word2))
    {
        return -1;
    }
    int i;
    int difference = 0;
    for (i = 0; i < strlen(word1); i++)
    {
        if (word1[i] != word2[i])
        {
            difference++;
        }
    }
    return difference;
}


int main()
{
    int wordCount = 0;
    int i;

    clock_t start, end;
    double total_time = 0;

    char startingWord[50];
    char endingWord[50];

    int usedWordCount = 0;

    char **words = readWords("dictionary.txt", &wordCount);
    if (words == NULL) {
        return 1;
    }


    char **usedWords = (char **)malloc(wordCount * sizeof(char *));
    for (i = 0; i < wordCount; i++)
    {
        usedWords[i] = NULL;
    }

    printf("Enter starting word: ");
    scanf("%s", startingWord);

    printf("Enter ending word: ");
    scanf("%s", endingWord);

    if (strlen(startingWord) != strlen(endingWord))
    {
        printf("Error: Starting and ending words must have the same length.\n");
        for(i = 0; i < wordCount; i++) {
            if (words[i] != NULL) free(words[i]);
        }
        free(words);
        free(usedWords);
        return 1;
    }

    start = clock();

    queue *q = createQueue(10000);
    stack *s = createStack(100);

    push(s, startingWord);
    enqueue(q, s);

    while (!isQueueEmpty(q))
    {

        stack *currentStack = dequeue(q);
        char *currentWord = peek(currentStack);

        if (strcmp(currentWord, endingWord) == 0)
        {
            printf("\n\nThe path found is: ");
            for (i = 0; i <= currentStack->top; i++)
            {
                printf("%s ", currentStack->word[i]);
            }
            printf("\n");

            end = clock();
            total_time = ((double)(end - start)) / CLOCKS_PER_SEC;
            printf("Time taken: %f seconds\n", total_time);

            while(!isQueueEmpty(q)) {
                stack* temp_s = dequeue(q);
                free(temp_s->word);
                free(temp_s);
            }
            free(q->stacks);
            free(q);

            free(currentStack->word);
            free(currentStack);

            for(i = 0; i < wordCount; i++) {
                if (words[i] != NULL) free(words[i]);
            }
            free(words);
            free(usedWords);


            return 0;
        }

        for (i = 0; i < wordCount; i++)
        {
            if (checkDifference(currentWord, words[i]) == 1)
            {
                if (checkIfWordExists(words[i], usedWords, usedWordCount) == 0)
                {
                    stack *newStack = copyStack(currentStack);
                    push(newStack, words[i]);
                    enqueue(q, newStack);

                    usedWords[usedWordCount] = words[i];
                    usedWordCount++;
                }
            }
        }
        free(currentStack->word);
        free(currentStack);
    }

    printf("\nNo path found between the starting and ending words.\n");

    while(!isQueueEmpty(q)) {
        stack* temp_s = dequeue(q);
        free(temp_s->word);
        free(temp_s);
    }
    free(q->stacks);
    free(q);

    for(i = 0; i < wordCount; i++) {
        if (words[i] != NULL) free(words[i]);
    }
    free(words);
    free(usedWords);

    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include<string.h>
#define MAX_CARDS 100
#define MAX_HISTORY 200
#define MAX_NAME_LENGTH 50
#define MAX_PLAYERS 2

// Card structure
typedef struct {
    int id;
    int attack;
    int defense;
} Card;

// Node structure for deck tree
typedef struct Node {
    Card card;
    struct Node *left;
    struct Node *right;
} Node;

// Stack structure for completed tasks
typedef struct {
    char actions[MAX_HISTORY][MAX_NAME_LENGTH];
    int top;
} HistoryStack;

// Queue structure for player turns
typedef struct {
    int front, rear, size;
    unsigned capacity;
    char players[MAX_PLAYERS][MAX_NAME_LENGTH];
} Queue;

// Function prototypes
Node* createDeckTree(int depth);
void printCard(Card card);
void initializeHistoryStack(HistoryStack *stack);
void pushHistory(HistoryStack *stack, const char *action);
Node* removeTopCard(Node* root, Card *removedCard);
void battle(Card *attacker, Card *defender, int *opponentHealth);
void playerTurn(Node **deck, const char *playerName, int *opponentHealth, bool *hasDrawnCard, Card *currentCard, Card *opponentCard, HistoryStack *history);
Queue* createQueue(unsigned capacity);
void enqueue(Queue* queue, const char* playerName);
char* dequeue(Queue* queue);

int main() {
    srand(time(NULL)); // Seed for randomness

    Node *player1Deck = createDeckTree(3);
    Node *player2Deck = createDeckTree(3);
    
    int health[MAX_PLAYERS] = {20, 20}; // Players start with health points

    bool player1HasDrawnCard = false;
    bool player2HasDrawnCard = false;

    Card player1CurrentCard = {0, 0, 0};
    Card player2CurrentCard = {0, 0, 0};

    HistoryStack history;
    initializeHistoryStack(&history);

    Queue* playerQueue = createQueue(MAX_PLAYERS);
    enqueue(playerQueue, "Player 1");
    enqueue(playerQueue, "Player 2");

    printf("=== Card Battle Game ===\n");
    printf("Objective: Deplete the opponent's health to zero by attacking with cards.\n");

    while (1) {
        char* currentPlayer = dequeue(playerQueue);
        if (strcmp(currentPlayer, "Player 1") == 0) {
            printf("\n-- Player 1's Turn --\n");
            playerTurn(&player1Deck, "Player 1", &health[1], &player1HasDrawnCard, &player1CurrentCard, &player2CurrentCard, &history);
            enqueue(playerQueue, "Player 1");
            if (health[1] <= 0) {
                printf("Player 2's health is zero! Player 1 wins!\n");
                break;
            }
        } else {
            printf("\n-- Player 2's Turn --\n");
            playerTurn(&player2Deck, "Player 2", &health[0], &player2HasDrawnCard, &player2CurrentCard, &player1CurrentCard, &history);
            enqueue(playerQueue, "Player 2");
            if (health[0] <= 0) {
                printf("Player 1's health is zero! Player 2 wins!\n");
                break;
            }
        }
    }

    // Print history of moves
    printf("\n=== Game Over ===\n");
    printf("History of Moves:\n");
    for (int i = 0; i <= history.top; i++) {
        printf("%s\n", history.actions[i]);
    }

    return 0;
}

// Function to create a binary tree for the deck of cards
Node* createDeckTree(int depth) {
    if (depth <= 0) return NULL;

    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->card.id = rand() % 100;
    newNode->card.attack = rand() % 10 + 1;
    newNode->card.defense = rand() % 5 + 1; // Limit defense value to 5
    newNode->left = createDeckTree(depth - 1);
    newNode->right = createDeckTree(depth - 1);

    return newNode;
}

// Function to print card details
void printCard(Card card) {
    printf("Card ID: %d | Attack: %d | Defense: %d\n", card.id, card.attack, card.defense);
}

// Initialize the history stack
void initializeHistoryStack(HistoryStack *stack) {
    stack->top = -1;
}

// Push an action onto the history stack
void pushHistory(HistoryStack *stack, const char *action) {
    if (stack->top == MAX_HISTORY - 1) {
        printf("History stack is full!\n");
        return;
    }
    strcpy(stack->actions[++stack->top], action);
}

// Function to remove the top card from the deck (tree)
Node* removeTopCard(Node* root, Card *removedCard) {
    if (root == NULL) return NULL;

    if (root->left == NULL) {
        *removedCard = root->card;
        Node *temp = root->right;
        free(root);
        return temp;
    }

    root->left = removeTopCard(root->left, removedCard);
    return root;
}

// Battle function between two cards
void battle(Card *attacker, Card *defender, int *opponentHealth) {
    printf("Attacker - ");
    printCard(*attacker);
    printf("Defender - ");
    printCard(*defender);

    int damage = attacker->attack - defender->defense;
    if (damage < 0) damage = 0;

    printf("Opponent health: %d\n", *opponentHealth);
    *opponentHealth -= damage;  // Decrease opponent's health based on attacker's attack value
    printf("Opponent health after attack: %d\n", *opponentHealth);
}

// Function to handle a player's turn
void playerTurn(Node **deck, const char *playerName, int *opponentHealth, bool *hasDrawnCard, Card *currentCard, Card *opponentCard, HistoryStack *history) {
    char choice;

    while (1) {
        printf("\n%s, choose your action:\n", playerName);
        if (!*hasDrawnCard) {
            printf("[D]raw card: ");
        } else {
            printf("[D]raw card, [A]ttack opponent: ");
        }
        scanf(" %c", &choice);

        if ((choice == 'D' || choice == 'd') && !*hasDrawnCard) {
            if (*deck != NULL) {
                *deck = removeTopCard(*deck, currentCard);
                printf("%s drew a card:\n", playerName);
                printCard(*currentCard);
                char action[MAX_NAME_LENGTH];
                sprintf(action, "%s drew a card", playerName);
                pushHistory(history, action);
                *hasDrawnCard = true;
            } else {
                printf("Deck is empty! Cannot draw more cards.\n");
            }
            break;
        } else if ((choice == 'D' || choice == 'd') && *hasDrawnCard) {
            if (*deck != NULL) {
                *deck = removeTopCard(*deck, currentCard);
                printf("%s drew a new card:\n", playerName);
                printCard(*currentCard);
                char action[MAX_NAME_LENGTH];
                sprintf(action, "%s drew a new card", playerName);
                pushHistory(history, action);
            } else {
                printf("Deck is empty! Cannot draw more cards.\n");
            }
            break;
        } else if ((choice == 'A' || choice == 'a') && *hasDrawnCard) {
            battle(currentCard, opponentCard, opponentHealth);
            char action[MAX_NAME_LENGTH];
            sprintf(action, "%s attacked", playerName);
            pushHistory(history, action);
            break;
        } else {
            printf("Invalid choice! Please enter D or A.\n");
        }
    }
}

// Function to create a queue
Queue* createQueue(unsigned capacity) {
    Queue* queue = (Queue*)malloc(sizeof(Queue));
    queue->capacity = capacity;
    queue->front = queue->size = 0;
    queue->rear = capacity - 1;
    return queue;
}

// Function to enqueue a player into the queue
void enqueue(Queue* queue, const char* playerName) {
    if (queue->size == queue->capacity) {
        printf("Queue is full!\n");
        return;
    }
    queue->rear = (queue->rear + 1) % queue->capacity;
    strcpy(queue->players[queue->rear], playerName);
    queue->size++;
}

// Function to dequeue a player from the queue
char* dequeue(Queue* queue) {
    if (queue->size == 0) {
        printf("Queue is empty!\n");
        return NULL;
    }
    char* playerName = queue->players[queue->front];
    queue->front = (queue->front + 1) % queue->capacity;
    queue->size--;
    return playerName;
}

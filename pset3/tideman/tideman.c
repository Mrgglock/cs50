#include <cs50.h>
#include <stdio.h>
#include <string.h>

// Max number of candidates
#define MAX 9

// preferences[i][j] is number of voters who prefer i over j
int preferences[MAX][MAX];

// locked[i][j] means i is locked in over j
bool locked[MAX][MAX];

// Each pair has a winner, loser
typedef struct
{
    int winner;
    int loser;
}
pair;

// Array of candidates
string candidates[MAX];
pair pairs[MAX * (MAX - 1) / 2];

int pair_count;
int candidate_count;

// Function prototypes
bool vote(int rank, string name, int ranks[]);
void record_preferences(int ranks[]);
void add_pairs(void);
void sort_pairs(void);
void lock_pairs(void);
void print_winner(void);

// prepare traversed history for graph
static int traversed[MAX];

int main(int argc, string argv[])
{
    // clear traversed history
    for (int i = 0; i < MAX; i++) {
        traversed[i] = -1;
    }
    // Check for invalid usage
    if (argc < 2)
    {
        printf("Usage: tideman [candidate ...]\n");
        return 1;
    }

    // Populate array of candidates
    candidate_count = argc - 1;
    if (candidate_count > MAX)
    {
        printf("Maximum number of candidates is %i\n", MAX);
        return 2;
    }
    for (int i = 0; i < candidate_count; i++)
    {
        candidates[i] = argv[i + 1];
    }

    // Clear graph of locked in pairs
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            locked[i][j] = false;
            // reset preferences 2d array
            preferences[i][j] = 0;
        }
    }

    pair_count = 0;
    int voter_count = get_int("Number of voters: ");

    // Query for votes
    for (int i = 0; i < voter_count; i++)
    {
        // ranks[i] is voter's ith preference
        int ranks[candidate_count];

        // Query for each rank
        for (int j = 0; j < candidate_count; j++)
        {
            string name = get_string("Rank %i: ", j + 1);

            if (!vote(j, name, ranks))
            {
                printf("Invalid vote.\n");
                return 3;
            }
        }

        record_preferences(ranks);

        printf("\n");
    }
    add_pairs();
    sort_pairs();
    lock_pairs();
    print_winner();
    return 0;
}

// Update ranks given a new vote
bool vote(int rank, string name, int ranks[])
{
    // Search for name to get candidateIndex
    for (int i = 0; i < candidate_count; i++) {
        // If the match is found, put it in ranks
        if (strcmp(name, candidates[i]) == 0) {
            ranks[rank] = i;
            return true;
        }
    }
    return false;
}

// Update preferences given one voter's ranks
void record_preferences(int ranks[])
{
    for (int i = 0; i < candidate_count; i++) {
        for (int j = 0; j < candidate_count; j++) {
            if (i == j) continue;
            // Check if i beats j
            for (int k = 0; k < candidate_count; k++) {
                if (ranks[k] == j) break;
                if (ranks[k] == i) preferences[i][j]++;
            }
        }
    }
    return;
}

// Record pairs of candidates where one is preferred over the other
void add_pairs(void)
{
    int pairIndex = 0;
    // Go through every unordered pair, non-repeat
    for (int row = 0; row < candidate_count; row++) {
        for (int col = row + 1; col < candidate_count; col++) {
            int rowColPair = preferences[row][col];
            int colRowDual = preferences[col][row];
            if (rowColPair == colRowDual) {
                // tie
                continue;
            } else if (rowColPair > colRowDual) {
                // [row][col] is greater and row is the winner
                pairs[pairIndex].winner = row;
                pairs[pairIndex].loser = col;
            } else {
                // [col][row] is greater and col is the winner
                pairs[pairIndex].winner = col;
                pairs[pairIndex].loser = row;
            }
            pairIndex++;
        }
    }
    pair_count = pairIndex;
    return;
}

// Sort pairs in decreasing order by strength of victory
void sort_pairs(void)
{
    // Perform Selection Sort
    for (int i = 0; i < pair_count; i++) {
        int indexAtMax = -2;
        int max = -2;
        // Look for max value
        for (int j = i; j < pair_count; j++) {
            // store winning candidate
            int winningCandidate = pairs[j].winner;
            int losingCandidate = pairs[j].loser;
            // check how many times they won over the other dude
            int winningAmount = preferences[winningCandidate][losingCandidate];
            if (winningAmount > max) {
               max = winningAmount;
               indexAtMax = j;
            }
        }
        // Swap with start index.
        pair temp = pairs[i];
        pairs[i] = pairs[indexAtMax];
        pairs[indexAtMax] = temp;
    }
    return;
}

void tree(int node);

// Lock pairs into the candidate graph in order, without creating cycles
void lock_pairs(void)
{
    // Go through each pair item
    for (int i = 0; i < pair_count; i++) {
        // add a edge via locked
        locked[pairs[i].winner][pairs[i].loser] = true;
        // clear traversed history
        for (int t = 0; t < MAX; t++) {
            traversed[t] = -1;
        }

        // did that edge just create a cyclic graph
        tree(pairs[i].loser);

        // Check if the traversed history contains the winner, i.e. the loser managed to route to the winner.
        for (int t = 0; t < MAX; t++) {
            if (traversed[t] == pairs[i].winner) {
                // if yes, reset what i did
                locked[pairs[i].winner][pairs[i].loser] = false;
                break;
            }
        }
    }
    return;
}

void tree(int node) {
    // Is node in traversed nodes? If it is, stop.
    for (int i = 0; i < MAX; i++) {
        if (traversed[i] == node) {
            return;
        }
    }
    // else, add to traversed log
    for (int i = 0; i < MAX; i++) {
        if (traversed[i] == -1) {
            traversed[i] = node;
            break;
        }
    }
    // and call the nodes that can be called from this node
    for (int i = 0; i < MAX; i++) {
        if (locked[node][i]) {
            tree(i);
        }
    }
    return;
}

// Print the winner of the election
void print_winner(void)
{
    // Check who has all edges pointing outwards
    // i.e. for locked[x][person], all of them should be = false.
    for (int possibleWinner = 0; possibleWinner < candidate_count; possibleWinner++) {
        bool isWinner = true;
        for (int i = 0; i < candidate_count; i++) {
            isWinner = isWinner && !locked[i][possibleWinner];
        }
        // If it got through all of that without breaking, i.e. all of them are false, then they are a winner.
        if (isWinner) {
            printf("%s\n", candidates[possibleWinner]);
            return;
        }
    }
}


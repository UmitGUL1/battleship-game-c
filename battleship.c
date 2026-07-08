#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define SIZE 10
#define NUM_SHIPS 4
#define MAX_SHIP_LENGTH 4

char board[SIZE][SIZE];
int ships[NUM_SHIPS][MAX_SHIP_LENGTH][2];
int ship_lengths[NUM_SHIPS] = {4, 3, 3, 2};
int hits[SIZE][SIZE];

void initialize_board()
{
    int i, j;
    for (i = 0; i < SIZE; i++)
    {
        for (j = 0; j < SIZE; j++)
        {
            board[i][j] = '-';
            hits[i][j] = 0;
        }
    }
}

void place_ships_randomly()
{
    int placed = 0;
    while (placed < NUM_SHIPS)
    {
        int len = ship_lengths[placed];
        int row = rand() % SIZE;
        int col = rand() % SIZE;
        int dir = rand() % 2;
        int fits = 1;
        int i;

        for (i = 0; i < len; i++)
        {
            int r = row + (dir == 1 ? i : 0);
            int c = col + (dir == 0 ? i : 0);
            if (r >= SIZE || c >= SIZE || board[r][c] != '-')
            {
                fits = 0;
                break;
            }
        }

        if (fits)
        {
            for (i = 0; i < len; i++)
            {
                int r = row + (dir == 1 ? i : 0);
                int c = col + (dir == 0 ? i : 0);
                board[r][c] = '#';
                ships[placed][i][0] = r;
                ships[placed][i][1] = c;
            }
            placed++;
        }
    }
}

void print_board()
{
    int i, j;
    printf("  ");
    for (j = 0; j < SIZE; j++)
    {
        printf("%d ", j);
    }
    printf("\n");

    for (i = 0; i < SIZE; i++)
    {
        printf("%d ", i);
        for (j = 0; j < SIZE; j++)
        {
            if (hits[i][j] == 1)
            {
                printf("X ");
            }
            else if (hits[i][j] == -1)
            {
                printf("O ");
            }
            else
            {
                printf("- ");
            }
        }
        printf("\n");
    }
}

int check_if_sank_ship(int row, int col)
{
    int s, i, j;
    for (s = 0; s < NUM_SHIPS; s++)
    {
        for (i = 0; i < ship_lengths[s]; i++)
        {
            if (ships[s][i][0] == row && ships[s][i][1] == col)
            {
                int sank = 1;
                for (j = 0; j < ship_lengths[s]; j++)
                {
                    int r = ships[s][j][0];
                    int c = ships[s][j][1];
                    if (hits[r][c] != 1)
                    {
                        sank = 0;
                        break;
                    }
                }
                if (sank)
                {
                    return ship_lengths[s];
                }
            }
        }
    }
    return 0;
}

void save_log(int row, int col, int hit)
{
    FILE *log = fopen("battleship_log.txt", "a");
    if (log != NULL)
    {
        fprintf(log, "Shot: %d %d - %s\n", row, col, hit ? "HIT" : "MISS");
        fclose(log);
    }
}

void save_ships_to_file()
{
    FILE *file;
    int i, j;
    file = fopen("ships.txt", "w");
    if (file == NULL)
    {
        printf("Error opening ships.txt!\n");
        return;
    }

    for (i = 0; i < NUM_SHIPS; i++)
    {
        for (j = 0; j < ship_lengths[i]; j++)
        {
            fprintf(file, "%d %d\n", ships[i][j][0], ships[i][j][1]);
        }
        fprintf(file, "\n");
    }

    fclose(file);
}

void clear_input_buffer()
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int read_valid_integer()
{
    char input[100];
    int number, i;
    int valid = 0;
    
    if (scanf("%99s", input) != 1)
    {
        clear_input_buffer();
        return -1;
    }
    
    if (toupper(input[0]) == 'X' && input[1] == '\0')
    {
        return -2;
    }
    
    valid = 1;
    for (i = 0; input[i] != '\0'; i++)
    {
        if (!isdigit(input[i]))
        {
            valid = 0;
            break;
        }
    }
    
    if (!valid)
    {
        printf("Invalid input. Please enter a number.\n");
        return -1;
    }
    
    number = atoi(input);
    return number;
}

void play_game()
{
    int row, col;
    int running = 1;
    int total_hits = 0;

    initialize_board();
    place_ships_randomly();
    save_ships_to_file();

    while (running)
    {
        print_board();
        printf("Enter row coordinate (0-9, X to quit): ");
        
        row = read_valid_integer();
        
        if (row == -2)
        {
            printf("Exiting the current game.\n");
            break;
        }
        
        if (row < 0 || row >= SIZE)
        {
            printf("Invalid row. Please enter a number between 0 and %d.\n", SIZE - 1);
            continue;
        }
        
        printf("Enter column coordinate (0-9): ");
        col = read_valid_integer();
        
        if (col < 0 || col >= SIZE)
        {
            printf("Invalid column. Please enter a number between 0 and %d.\n", SIZE - 1);
            continue;
        }

        if (hits[row][col] != 0)
        {
            printf("Already tried that one!\n");
            continue;
        }

        if (board[row][col] == '#')
        {
            hits[row][col] = 1;
            printf("HIT!\n");
            save_log(row, col, 1);
            total_hits++;

            {
                int sunk = check_if_sank_ship(row, col);
                if (sunk)
                {
                    printf("Congratulations! You sank a %d-cell ship!\n", sunk);
                }
            }

            if (total_hits == 12)
            {
                printf("All ships sunk! You win!\n");
                break;
            }
        }
        else
        {
            hits[row][col] = -1;
            printf("MISS!\n");
            save_log(row, col, 0);
        }
    }
}

int main()
{
    char choice;
    int play_again = 1;
    
    srand(42);
    
    printf("\n===== WELCOME TO BATTLESHIP =====\n\n");
    
    while (play_again)
    {
        play_game();
        
        printf("Game over!\n");
        printf("Press 'n' to play again or 'x' to quit: ");
        
        clear_input_buffer();
        
        choice = getchar();
        
        if (toupper(choice) == 'N')
        {
            printf("\n--- Starting a new game ---\n\n");
        }
        else
        {
            play_again = 0;
            printf("Thanks for playing Battleship!\n");
        }
    }
    
    return 0;
}

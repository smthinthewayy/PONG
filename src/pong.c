#include <stdio.h>
#include <termios.h>
#include <unistd.h>

// Input without pressing enter
void turn_off_canon(void);

// Display the entire content on the screen
void draw(int WIDTH,
    int HEIGHT,
    int left_racket_x,
    int left_racket_y,
    int right_racket_x,
    int right_racket_y,
    int ball_x,
    int ball_y,
    int score_1,
    int score_2);

// Check for crossing the ball with rackets
int check_coincide_with_rackets(int ball_y,
    int ball_x,
    int dy,
    int left_racket_x,
    int left_racket_y,
    int right_racket_x,
    int right_racket_y);

// Check for crossing the ball with the top/bottom wall
int check_coincide_with_top_bottom_wall(int ball_x, int dx);

// Ball movements along the x-axis
int move_ball_x(int ball_x, int dx);

// Ball movements along the y-axis
int move_ball_y(int ball_y, int dy);

// Adds points to 2 player
int add_point_2_player(int ball_y, int score_2);

// Adds points to 1 player
int add_point_1_player(int ball_y, int score_1);

// Determines which player won
int who_win(int score_1, int score_2);

int main() {
    turn_off_canon();  // Input without pressing enter

    const int WIDTH = 80;
    const int HEIGHT = 25;
    int ball_x = 12;
    int ball_y = 40;
    int left_racket_x = 12;
    const int left_racket_y = 5;
    int right_racket_x = 12;
    const int right_racket_y = 75;
    int dx = 1;
    int dy = -1;
    int score_1 = 0;
    int score_2 = 0;

    // First frame
    draw(WIDTH,
        HEIGHT,
        left_racket_x,
        left_racket_y,
        right_racket_x,
        right_racket_y,
        ball_x,
        ball_y,
        score_1,
        score_2);

    while (1) {  // Game loop
        // User input
        char c;
        read(0, &c, 1);
        // Move the left racket up
        if (c == 'a' && left_racket_x > 2) {
            left_racket_x--;
        // Move the left racket down
        } else if (c == 'z' && left_racket_x < 22) {
            left_racket_x++;
        // Move the right racket up
        } else if (c == 'k' && right_racket_x > 2) {
            right_racket_x--;
        // Move the right racket down
        } else if (c == 'm' && right_racket_x < 22) {
            right_racket_x++;
        } else if (c == 'q') {  // Quitting
            break;
        }

        // Game logic
        ball_x = move_ball_x(ball_x, dx);
        ball_y = move_ball_y(ball_y, dy);

        dy = check_coincide_with_rackets(ball_y,
            ball_x, dy,
            left_racket_x,
            left_racket_y,
            right_racket_x,
            right_racket_y);

        dx = check_coincide_with_top_bottom_wall(ball_x, dx);

        int tmp_score_1 = add_point_1_player(ball_y, score_1);
        int tmp_score_2 = add_point_2_player(ball_y, score_2);

        // If the score has changed
        if (tmp_score_1 != score_1 || tmp_score_2 != score_2) {
            // Init game
            ball_x = 12;
            ball_y = 40;
            left_racket_x = 12;
            right_racket_x = 12;

            // Updating the score
            score_1 = tmp_score_1;
            score_2 = tmp_score_2;
        }

        // Responsible for the winner
        int king_pong = who_win(score_1, score_2);

        // Game over
        if (king_pong == 1 || king_pong == 2) {
            printf("\e[H\e[2J\e[3J");
            printf("PLAYER %d WIN\n", king_pong);
            printf("GAME OVER\n");
            break;  // End game loop
        }

        usleep(50000);  // Delay

        // Drawing
        draw(WIDTH,
            HEIGHT,
            left_racket_x,
            left_racket_y,
            right_racket_x,
            right_racket_y,
            ball_x,
            ball_y,
            score_1,
            score_2);
    }
    return 0;
}

// Input without pressing enter
void turn_off_canon(void) {
    struct termios uncanon_s;
    tcgetattr(0, &uncanon_s);
    uncanon_s.c_lflag &= ~(ECHO | ICANON | IEXTEN);
    uncanon_s.c_cc[VMIN] = 0;
    uncanon_s.c_cc[VTIME] = 0;
    tcsetattr(0, TCSAFLUSH, &uncanon_s);
}

// Display the entire content on the screen
void draw(int WIDTH,
    int HEIGHT,
    int left_racket_x,
    int left_racket_y,
    int right_racket_x,
    int right_racket_y,
    int ball_x,
    int ball_y,
    int score_1,
    int score_2) {
    printf("\e[H\e[2J\e[3J");  // Clearing the field
    // Score
    printf(" Player 1: %02d", score_1);
    printf("                            ");
    printf("Player 2: %02d", score_2);
    printf("                             \n");
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            if (i == 0 && j == 0) {
                printf("\u250F");  // Top left corner
            } else if (i == 0 && j == WIDTH / 2) {
                printf("\u2533");  // Top cross
            } else if (i == 0 && j == WIDTH - 1) {
                printf("\u2513");  // Top right corner
            } else if (i == HEIGHT - 1 && j == 0) {
                printf("\u2517");  // Bottom left corner
            } else if (i == HEIGHT - 1 && j == WIDTH / 2) {
                printf("\u253B");  // Bottom right corner
            } else if (i == HEIGHT - 1 && j == WIDTH - 1) {
                printf("\u251B");  // Bottom cross
            } else if (i == ball_x && j == ball_y) {
                printf("\u25CF");  // Ball
            } else if (((left_racket_x - 1) <= i && \
                        i <= (left_racket_x + 1)) && \
                        j == left_racket_y) {
                printf("\u2503");  // Left racket
            } else if (((right_racket_x - 1) <= i && \
                        i <= (right_racket_x + 1)) && \
                        j == right_racket_y) {
                printf("\u2503");  // Right racket
            } else if ((j == WIDTH / 2) && (i != 0) && (i != 24)) {
                printf("\u2503");  // Middle
            } else if ((j == 0 || j == WIDTH - 1) && (i != 0) && (i != 24)) {
                printf("\u2503");  // Left/right wall
            } else if ((i == 0 || i == HEIGHT - 1) && \
                (j != 0 && j != WIDTH - 1)) {  // Top/bottom wall
                printf("\u2501");
            } else {
                printf(" ");  // filler
            }
        }
        printf("\n");
    }
}

// Check for crossing the ball with rackets
int check_coincide_with_rackets(int ball_y,
    int ball_x,
    int dy,
    int left_racket_x,
    int left_racket_y,
    int right_racket_x,
    int right_racket_y) {
    if ((ball_y == left_racket_y + 1 && \
            (ball_x == left_racket_x - 1 || \
                ball_x == left_racket_x || \
                ball_x == left_racket_x + 1)) || \
        (ball_y == right_racket_y - 1 && \
            (ball_x == right_racket_x - 1 || \
                ball_x == right_racket_x || \
                ball_x == right_racket_x + 1)))
        return -dy;
    else
        return dy;
}

// Check for crossing the ball with the top/bottom wall
int check_coincide_with_top_bottom_wall(int ball_x, int dx) {
    return ((ball_x < 2 || ball_x > 22) ? -dx : dx);
}

// Ball movements along the x-axis
int move_ball_x(int ball_x, int dx) {
    return ball_x + dx;
}

// Ball movements along the y-axis
int move_ball_y(int ball_y, int dy) {
    return ball_y + dy;
}

// Adds points to 2 player
int add_point_2_player(int ball_y, int score_2) {
    return ((ball_y < 1) ? score_2 + 1 : score_2);
}

// Adds points to 1 player
int add_point_1_player(int ball_y, int score_1) {
    return ((ball_y > 78) ? score_1 + 1 : score_1);
}

// Determines which player won
int who_win(int score_1, int score_2) {
    if (score_1 == 21)
        return 1;
    else if (score_2 == 21)
        return 2;
    return 0;
}

#include <format>
#include <random>
#include <string>

#include <raylib.h>
#include <raymath.h>

std::random_device rd;
std::uniform_int_distribution<int> dist{0, 1};
int directiony[] {-1, 1};

struct GameState {
    Rectangle player1;
    Rectangle player2;
    int player1Score{};
    int player2Score{};
} gamestate;

class Drawable {
  public:
    virtual void update() = 0;
    virtual void render() = 0;
    virtual ~Drawable() = default;
};

class UI : Drawable {
  public:
    void update() {
        // TODO: implemet score and level?
    };
    void render() {
        DrawText("Pingg Pongg", (GetScreenWidth() / 2.0f) - 90, 20, 32,
                 Color{0xb7, 0xbd, 0xf8, 0xff});
        DrawText(std::format("FPS: {}", GetFPS()).c_str(),
                 (GetScreenWidth() - 80), 20, 16,
                 Color{0xff, 0x3f, 0x58, 0xff});
        DrawLineDashed(
            Vector2{GetScreenWidth() / 2.0f, 0},
            Vector2{GetScreenWidth() / 2.0f, (float)GetScreenHeight()}, 20.0f,
            3.0f, Color{0xaa, 0xaa, 0xbb, 0xFF});
        DrawText(std::format("{}", gamestate.player1Score).c_str(),
                 (GetScreenWidth() / 2.0f) - 60, GetScreenHeight() - 60, 32,
                 RAYWHITE);
        DrawText(std::format("{}", gamestate.player2Score).c_str(),
                 (GetScreenWidth() / 2.0f) + 50, GetScreenHeight() - 60, 32,
                 RAYWHITE);
    }
    ~UI() = default;
};

enum class PlayerType { LEFT, RIGHT };

class Player : Drawable {
  private:
    const float width = 40.0f;
    const float height = 300.0f;
    const float radius = 12.0f;
    const float paddingx = 8.0f;
    const float paddingy = 10.0f;
    const float speed = 150;
    Vector2 position;
    PlayerType playertype;

  public:
    Player(PlayerType playertype) : playertype(playertype) {
        switch (playertype) {
        case PlayerType::LEFT:
            this->position = {(width / 2.0f) + paddingx,
                              GetScreenHeight() / 2.0f};
            break;
        case PlayerType::RIGHT:
            this->position = {GetScreenWidth() - ((width / 2.0f) + paddingx),
                              GetScreenHeight() / 2.0f};
            break;
        }
    }
    void update() {
        if (playertype == PlayerType::LEFT) {
            position.x = (width / 2.0f) + paddingx;
            if (IsKeyDown(KEY_W)) {
                position.y -= speed * GetFrameTime() * 10;
            } else if (IsKeyDown(KEY_S)) {
                position.y += speed * GetFrameTime() * 10;
            }
        } else if (playertype == PlayerType::RIGHT) {
            position.x = GetScreenWidth() - ((width / 2.0f) + paddingx);
            if (IsKeyDown(KEY_I)) {
                position.y -= speed * GetFrameTime() * 10;
            } else if (IsKeyDown(KEY_K)) {
                position.y += speed * GetFrameTime() * 10;
            }
        }

        if (position.y < (paddingy + (height / 2.0f))) {
            position.y = paddingy + (height / 2.0f);
        } else if (position.y >
                   (GetScreenHeight() - (paddingy + (height / 2.0f)))) {
            position.y = GetScreenHeight() - (paddingy + (height / 2.0f));
        }
    }
    void render() {
        switch (playertype) {
        case PlayerType::LEFT:
            gamestate.player1 =
                Rectangle{position.x - (width / 2.0f),
                          position.y - (height / 2.0f), width, height};
            DrawRectangleRounded(gamestate.player1, radius, 32,
                                 Color{0xf4, 0xdb, 0xd6, 0xff});
            break;
        case PlayerType::RIGHT:
            gamestate.player2 = {position.x - (width / 2.0f),
                                 position.y - (height / 2.0f), width, height};
            DrawRectangleRounded(gamestate.player2, radius, 32,
                                 Color{0xa6, 0xda, 0x95, 0xff});
            break;
        }
    }
    ~Player() = default;
};

class Ball : Drawable {
  private:
    const int radius = 20.0f;
    Vector2 position;
    Color color{0xe8, 0x84, 0x93, 0xff};
    Vector2 direction{1.0f, 1.0f};
    const float speed = 100.0f;

  public:
    Ball() {
        this->position = {GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f};
    }
    void update() {
        position.y += speed * direction.y * GetFrameTime() * 10;
        position.x += speed * direction.x * GetFrameTime() * 10;
        if (position.x <= (0 + radius)) {
            direction.x = 1;
            gamestate.player2Score += 1;
            position.x = GetScreenWidth() / 2.0f;
        } else if (position.x >= (GetScreenWidth() - radius)) {
            direction.x = -1;
            gamestate.player1Score += 1;
            position.x = GetScreenWidth() / 2.0f;
        }
        if (position.y <= (0 + radius)) {
            direction.y = 1;
        } else if (position.y >= (GetScreenHeight() - radius)) {
            direction.y = -1;
        }

        if (CheckCollisionCircleRec(this->position, this->radius,
                                    gamestate.player1)) {
            direction.x = 1;
            direction.y = directiony[dist(rd)];
        } else if (CheckCollisionCircleRec(this->position, this->radius,
                                           gamestate.player2)) {
            direction.x = -1;
            direction.y = directiony[dist(rd)];
        }
    }
    void render() { DrawCircle(position.x, position.y, radius, color); }
    ~Ball() = default;
};

class Game {
  private:
    const int width;
    const int height;
    std::string title;
    Player *player1;
    Player *player2;
    Ball *ball;
    unsigned int configFlags;
    UI ui;

    void update() {
        ball->update();
        player1->update();
        player2->update();
    }
    void render() {
        ui.render();
        ball->render();
        player1->render();
        player2->render();
    }

  public:
    Game(int width, int height, std::string title, unsigned int configFlags)
        : width(width), height(height), title(title), configFlags(configFlags) {
    }

    Game *init() {
        SetConfigFlags(configFlags);
        InitWindow(width, height, title.c_str());
        SetTargetFPS(144);
        SetWindowMinSize(1000, 600);

        player1 = new Player(PlayerType::LEFT);
        player2 = new Player(PlayerType::RIGHT);
        ball = new Ball();
        return this;
    }
    void mainloop() {
        while (!WindowShouldClose()) {
            update();

            BeginDrawing();
            ClearBackground({0x1E, 0x1E, 0x2E, 0xFF});

            this->render();

            EndDrawing();
        }
    }
    ~Game() {
        CloseWindow();
        delete player1;
        delete player2;
        delete ball;
    }
};

int main() {
    Game game = Game(1200, 800, "pingg pongg", FLAG_WINDOW_RESIZABLE);
    game.init()->mainloop();
    return 0;
}

#include <GL/glew.h>
#include <GLFW/glfw3.h>

// #include <any>
#include <algorithm>
#include <iostream>
#include <functional>
#include <string>
#include <vector>

#include "gfx/mesh2d.hpp"
#include "geometry.hpp"
#include "gfx/gl/types.hpp"

constexpr size_t WINDOW_WIDTH = 640;
constexpr size_t WINDOW_HEIGHT = 640;

enum class FieldState : int {
    NotPlayable,
    Empty,
    Black,
    White,
    SuperBlack,
    SuperWhite
};

constexpr size_t TABLE_DIM = 8;
constexpr size_t TABLE_DATA_LEN = TABLE_DIM*TABLE_DIM;
FieldState g_tableData[TABLE_DATA_LEN];

struct GameState {
    int whiteCount;
    int blackCount;
};

auto accessTableData(size_t x, size_t y) -> FieldState & {
    return g_tableData[y*TABLE_DIM + x];
}

auto generateDefaultTable() -> FieldState {
    static int cnt = 0;

    static bool threshold = 0;
    if (cnt % 8 == 0) {
        threshold = !threshold;
    }
    return (static_cast<int>(threshold) + cnt++) % 2 ? FieldState::Empty : FieldState::NotPlayable;
}

auto drawBuffer() -> std::string {
//    constexpr size_t BUFLINE_LEN = TABLE_DIM + 1;
//    std::string buffer(BUFLINE_LEN * TABLE_DIM, ' ');
//    for (size_t y = 0; y < TABLE_DIM; y++) {
//        for (size_t x = 0; x < TABLE_DIM; x++) {
//            char &curField = buffer[y*BUFLINE_LEN + x];
//            switch (static_cast<int>(accessTableData(x, y))) {
//                case static_cast<int>(FieldState::NotPlayable):
//                    curField = ' ';
//                    break;
//                case static_cast<int>(FieldState::Empty):
//                    curField = '#';
//                    break;
//                case static_cast<int>(FieldState::Black):
//                    curField = '0';
//                    break;
//                case static_cast<int>(FieldState::White):
//                    curField = 'O';
//                    break;
//                case static_cast<int>(FieldState::SuperBlack):
//                    curField = '$';
//                    break;
//                case static_cast<int>(FieldState::SuperWhite):
//                    curField = '%';
//            }
//        }
//        buffer[y*BUFLINE_LEN + TABLE_DIM] = '\n';
//    }
}

struct Position2D;
struct Vec2D {
    int x;
    int y;

    auto operator+(const Vec2D &other) const -> const Vec2D {
        return Vec2D { this->x + other.x, this->y + other.y };
    }
    auto operator-(const Vec2D &other) const -> const Vec2D {
        return Vec2D { this->x - other.x, this->y - other.y };
    }
    auto operator*(int scalar) const -> const Vec2D {
        return Vec2D { this->x*scalar, this->y*scalar };
    }
    friend Position2D operator+(const Position2D &, const Vec2D &);
};

struct Position2D {
    int x;
    int y;

    friend Position2D operator+(const Position2D &pos, const Vec2D &vec);
};

auto operator+(const Position2D &pos, const Vec2D &vec) -> Position2D {
    return { pos.x + vec.x, pos.y + vec.y };
}

auto positionDataToLinearArray(const std::vector<gl::Position2Df> &positions) -> std::vector<GLfloat> {
    std::vector<GLfloat> arr;
    arr.reserve(positions.size() * 2);
    for (size_t i = 0; i < positions.size(); i++) {
        arr[i] = positions[i].x;
        arr[i+1] = positions[i].y;
    }

    return arr;
}

auto generateCheckerMeshInfo() -> MeshInfo {
    static constexpr GLfloat normR = 1.0f / 8 * .9;

    gl::VertexArray va;
    glGenVertexArrays(1, &va);
    glBindVertexArray(va);
    const auto verticesData = positionDataToLinearArray(primitives::circle(normR));
    gl::ArrayBuffer vb(verticesData, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, true, 2 * sizeof(GLfloat), 0);
    glEnableVertexAttribArray(0);
    std::vector<GLuint> checkerIndices;
    checkerIndices.reserve(verticesData.size());
    for (size_t i = 0; i < verticesData.size(); i++)
        checkerIndices[i] = i;
    gl::IndexBuffer ib(checkerIndices, GL_DYNAMIC_DRAW);
    glBindVertexArray(0);

    return MeshInfo(va, vb, ib);
}

// Mesh2D checker(generateCheckerMeshInfo());
// TODO: implement compile-time substitution
// of either std::any or hand-written alternative
//
// using UpdateData = std::any;
// struct UpdateItem {
//     UpdateCause cause;
//     UpdateData data;
// };

// constexpr size_t MAX_UPDATES = 4;
// struct UpdateBlock {
//     UpdateItem updates[MAX_UPDATES];
//
//     class UpdBlockIterator {
//     public:
//         UpdBlockIterator(UpdateBlock &parent)
//             : updBlock_(parent), idx_(0) {}
//         UpdBlockIterator(UpdateBlock &parent, size_t newIndex)
//             : updBlock_(parent), idx_(newIndex) {}
//         auto operator==(const UpdBlockIterator &other) -> bool {
//             return this->idx_ == other.idx_;
//         }
//         auto operator!=(const UpdBlockIterator &other) -> bool {
//             return !this->operator==(other);
//         }
//         auto operator*() -> UpdateItem & {
//             return updBlock_.updates[idx_];
//         }
//         auto operator++(int) -> const UpdBlockIterator {
//             UpdBlockIterator old(this->updBlock_, idx_);
//             idx_++;
//             return old;
//         }
//         auto operator++() -> const UpdBlockIterator & {
//             idx_++;
//             return UpdBlockIterator(updBlock_, idx_);
//         }
//         auto operator->() -> const UpdateItem * {
//             return &updBlock_.updates[idx_];
//         }
// 
//     private:
//         size_t idx_;
//         UpdateBlock &updBlock_;
//     };
// 
//     auto begin() -> UpdBlockIterator {
//         return UpdBlockIterator(*this);
//     }
//     auto end() -> UpdBlockIterator {
//         return UpdBlockIterator(*this, MAX_UPDATES);
//     }
// };

namespace upd_data {
    struct Move {
        Position2D initialPos;
        Vec2D moveDirection;
    };
    struct Destroy {
        Position2D where;
    };
    struct Convert {
        Position2D where;
        FieldState which;
    };
}

void moveChecker(const upd_data::Move &data) {
    const auto movedCheckerType = accessTableData(data.initialPos.x, data.initialPos.y);
    accessTableData(data.initialPos.x, data.initialPos.y) = FieldState::Empty;

    const Position2D dest = { .x = data.initialPos.x + data.moveDirection.x, .y = data.initialPos.y + data.moveDirection.y };
    accessTableData(dest.x, dest.y) = movedCheckerType;
}

enum class Team {
    Black,
    White
};
auto determineTeam(const FieldState &fs) -> Team;
void destroyChecker(const upd_data::Destroy &data, GameState &state) {
    const auto checkerDestroyed = accessTableData(data.where.x, data.where.y);
    accessTableData(data.where.x, data.where.y) = FieldState::Empty;
    if (determineTeam(checkerDestroyed) == Team::White)
        state.whiteCount--;
    else state.blackCount--;
}
void convertChecker(const upd_data::Convert &data) {
    accessTableData(data.where.x, data.where.y) =
        data.which == FieldState::Black
        ? FieldState::SuperBlack
        : FieldState::SuperWhite;
}

// Does not compile on macOS 10.13
// TODO: implement compile-time substitution
// of either std::any or hand-written alternative

// void updateTable(UpdateBlock *upd) {
//     for (auto it = upd->begin(); it != upd->end(); it++) {
//         switch (static_cast<int>(it->cause)) {
//         case static_cast<int>(UpdateCause::Moved):
//             // moveChecker(std::any_cast<upd_data::Move>(it->data));
//             break;
//         case static_cast<int>(UpdateCause::Destroyed):
//             // destroyChecker(std::any_cast<upd_data::Destroy>(it->data));
//             break;
//         case static_cast<int>(UpdateCause::Converted):
//             // convertChecker(std::any_cast<upd_data::Convert>(it->data));
//             break;
//         }
//     }
// }

enum class SpawnSide {
    Upper,
    Lower
};
void spawnTeam(Team t, SpawnSide side) {
    const auto occupationType = t == Team::White
        ? FieldState::White
        : FieldState::Black;
    const size_t spawnThreshold = side == SpawnSide::Upper
        ? 0
        : TABLE_DIM - 3;
    for (size_t y = 0; y < 3; y++) {
        size_t x = (y + spawnThreshold) % 2;
        for ( ; x < TABLE_DIM; x += 2) {
            accessTableData(x, y + spawnThreshold) = occupationType;
        }
    }
}

auto determineTeam(const FieldState &fs) -> Team {
    switch (static_cast<int>(fs)) {
        case static_cast<int>(FieldState::White):
        case static_cast<int>(FieldState::SuperWhite):
            return Team::White;
        case static_cast<int>(FieldState::Black):
        case static_cast<int>(FieldState::SuperBlack):
            return Team::Black;
    }
}

bool fieldFriendlyOrNeutral(FieldState fs, Team t) {
    if (t == Team::White) {
        switch (static_cast<int>(fs)) {
            case static_cast<int>(FieldState::White):
            case static_cast<int>(FieldState::SuperWhite):
                return true;
            case static_cast<int>(FieldState::Black):
            case static_cast<int>(FieldState::SuperBlack):
                return false;
            default:
                return true;
        }
    }
    else {
        switch (static_cast<int>(fs)) {
            case static_cast<int>(FieldState::White):
            case static_cast<int>(FieldState::SuperWhite):
                return false;
            case static_cast<int>(FieldState::Black):
            case static_cast<int>(FieldState::SuperBlack):
                return true;
            default:
                return true;
        }
    }
}

bool posOutOfField(const Position2D &pos) {
    if (pos.x < 0 ||
        pos.x >= TABLE_DIM ||
        pos.y < 0 ||
        pos.y >= TABLE_DIM
       )
        return true;

    return false;
}

auto findAvailableMovesFromPos(const Position2D &pos) -> std::vector<Vec2D> {
    static const std::vector<Vec2D> virtMoves = {
        { .x =  1, .y =  1 },
        { .x =  1, .y = -1 },
        { .x = -1, .y =  1 },
        { .x = -1, .y = -1 }
    };
    const auto team = determineTeam(accessTableData(pos.x, pos.y));
    std::vector<Vec2D> availableMoves;
    availableMoves.reserve(virtMoves.size());
    for (auto &mv : virtMoves) {
        const auto dest = pos + mv;
        if (!posOutOfField(dest)) {
            const auto destFieldState = accessTableData(dest.x, dest.y);
            if (destFieldState == FieldState::Empty)
                availableMoves.push_back(mv);
            if (!fieldFriendlyOrNeutral(destFieldState, team)) {
                const auto behindDest = dest + mv;
                if (accessTableData(behindDest.x, behindDest.y) == FieldState::Empty)
                    availableMoves.push_back(mv*2);
            }
        }
    }
    availableMoves.shrink_to_fit();

    return availableMoves;
}

int main() {
    if (!glfwInit()) {
        std::cerr << "Cannot initialize glfw.\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_CORE_PROFILE, GLFW_TRUE);
    glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    GLFWwindow *wnd = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Checkers", nullptr, nullptr);
    if (!wnd) {
        const char *desc;
        const int errCode = glfwGetError(&desc);
        glfwTerminate();
        std::cerr << "Cannot create window.\n"
            << "Reason: " << desc << '\n';
        return -1;
    }

    glfwSwapInterval(1);
    glfwMakeContextCurrent(wnd);
    if (glewInit() != GLEW_OK) {
        std::cerr << "Could not load glew.\n";
        return -1;
    }

    // Game code start
    std::generate(std::begin(g_tableData), std::end(g_tableData), generateDefaultTable);
//    spawnTeam(Team::Black, SpawnSide::Upper);
//    spawnTeam(Team::White, SpawnSide::Lower);
    const size_t checkerX = 7;
    const size_t checkerY = 3;
    const size_t opponentCheckerX = 6;
    const size_t opponentCheckerY = 4;
    accessTableData(checkerX, checkerY) = FieldState::Black;
    accessTableData(opponentCheckerX, opponentCheckerY) = FieldState::White;
//    const auto buf = drawBuffer();
//    std::cout << buf << "\n\n";

    const auto am = findAvailableMovesFromPos({ .x = checkerX, .y = checkerY });
    for (const auto m : am) {
        std::cout << m.x << ';' << m.y << '\n';
    }
    while(!glfwWindowShouldClose(wnd)) {
        glClear(GL_COLOR_BUFFER_BIT);
        glfwSwapBuffers(wnd);
        glfwPollEvents();
    }
    glfwTerminate();

    return 0;
}

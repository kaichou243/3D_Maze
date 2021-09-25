#include <Siv3D.hpp> // OpenSiv3D v0.6.1
#include <random>
#include <vector>
#include <utility>
#include <stdlib.h>
#define STAGE       3       //ステージ数
#define MAZE_ROW    23      //迷路の行数
#define MAZE_COLUMN 23      //迷路の列数
#define MAZE_FONT   25      //迷路のフォントのサイズ
#define UP_DIG      0
#define DOWN_DIG    1
#define LEFT_DIG    2
#define RIGHT_DIG   3

//プレイヤー
typedef struct
{
  int row;        //プレイヤー位置(行)
  int column;     //プレイヤー位置(列)
}MazePlayer;


//迷路の一ブロック
enum MazeKind {PATH, WALL, START, GOAL};    //ブロックの種類(道、壁、スタート、ゴール)
enum MazeFlag {FALSE, TRUE};                //ブロックが判明しているかどうか

typedef struct
{
  enum MazeKind kind;            //種類(道、壁、スタート、ゴール)
} MazeBlock;


//プレイヤー初期化
int MazePlayerInit(int *playerRow, int *playerColumn, MazeBlock maze[MAZE_ROW][MAZE_COLUMN])
{
  int i, j;

  for(i = 0; i < MAZE_ROW; i++) //行
  {
    for(j = 0;  j < MAZE_COLUMN; j++) //列
    {
      if(maze[i][j].kind == START) //スタート地点ならばプレイヤーの位置に設定する
      {
        *playerRow    = i;
        *playerColumn = j;
        return 0; //関数終了(プレイヤーを設定できれば残りの処理は必要ない)
      }
    }
  }
  return -1;
}
//方向
enum MazeDirection{UP, DOWN, LEFT, RIGHT, Invalid};

//ゴール判定
int MazeGoalCheck(int playerRow, int playerColumn, MazeBlock maze[MAZE_ROW][MAZE_COLUMN])
{
  if(maze[playerRow][playerColumn].kind == GOAL) //プレイヤー位置がゴール地点に等しい
  {
    return 1;
  }
  return 0;
}

std::random_device rnd_dev;
std::mt19937 rnd(rnd_dev());

int random_int(int l, int r) {
  return std::uniform_int_distribution<>(l, r)(rnd);
}

void dig(int i, int j,MazeBlock maze[MAZE_ROW][MAZE_COLUMN]) {
    // どの方向を掘ろうとしたかを覚えておく変数
    int up, down, left, right;

    up = 0;
    down = 0;
    left = 0;
    right = 0;

    // 全方向試すまでループ
    while (up == 0 || down == 0 || left == 0 || right == 0) {
        // 0 - 3 の乱数を取得
        int d = rand() % 4;

        switch(d) {
            case UP:
                // 上方向が掘れるなら掘る
                if (j - 2 >= 0 && j - 2 < MAZE_COLUMN) {
                    if (maze[i][j-2].kind == WALL) {
                        maze[i][j-2].kind = PATH;
                        maze[i][j-1].kind = PATH;
                        dig(i, j-2, maze);
                    }
                }
                up++;
                break;
            case DOWN:
                // 下方向が掘れるなら掘る
                if (j + 2 >= 0 && j + 2 < MAZE_COLUMN) {
                    if (maze[i][j+2].kind == WALL) {
                        maze[i][j+2].kind = PATH;
                        maze[i][j+1].kind = PATH;
                        dig(i, j+2, maze);
                    }
                }
                down++;
                break;
            case LEFT:
                // 左方向が掘れるなら掘る
                if (i - 2 >= 0 && i - 2 < MAZE_ROW) {
                    if (maze[i-2][j].kind == WALL) {
                        maze[i-2][j].kind = PATH;
                        maze[i-1][j].kind = PATH;
                        dig(i-2, j, maze);
                    }
                }
                left++;
                break;
            case RIGHT:
                // 右方向が掘れるなら掘る
                if (i + 2 >= 0 && i + 2 < MAZE_ROW) {
                    if (maze[i+2][j].kind == WALL) {
                        maze[i+2][j].kind = PATH;
                        maze[i+1][j].kind = PATH;
                        dig(i+2, j, maze);
                    }
                }
                right++;
                break;
        }
    }
}

void generate_random_maze(MazeBlock maze[MAZE_ROW][MAZE_COLUMN]) {
  for (int i = 0; i < MAZE_ROW; i++) {
        for (int j = 0; j < MAZE_COLUMN; j++) {
            maze[i][j] = {WALL};
        }
  }
  int start_x = random_int(0, MAZE_ROW - 1);
  int start_y = random_int(0, MAZE_COLUMN - 1);
  maze[start_x][start_y] = {GOAL};
  dig(start_x, start_y, maze);
/*
  std::vector<std::pair<int,int>> candidates;
  while(1){
      for (int i = 0; i < MAZE_ROW-1; i++) {
        for (int j = 0; j < MAZE_COLUMN-1; j++) {
            int maze_path_check = 0;
            if(j < MAZE_COLUMN-1){
                if(maze[i][j].kind != WALL) continue;
                if(maze[i][j+1].kind == PATH||maze[i][j+1].kind == START) maze_path_check++;
            }
            if(j > 0){
                if(maze[i][j].kind != WALL) continue;
                if(maze[i][j-1].kind == PATH||maze[i][j-1].kind == START) maze_path_check++;
            }
            if(i < MAZE_ROW-1){
                if(maze[i][j].kind != WALL) continue;
                if(maze[i+1][j].kind == PATH||maze[i+1][j].kind == START) maze_path_check++;
            }
            if(i > 0){
                if(maze[i][j].kind != WALL) continue;
                if(maze[i-1][j].kind == PATH||maze[i-1][j].kind == START) maze_path_check++;
            }
            if(maze_path_check==1){
                candidates.push_back(std::pair(i,j));
            }
        }
      }
      if (candidates.size() == 0) break;
      std::pair<int,int> path = candidates[random_int(0,candidates.size()-1)];
      int x_path=path.first;
      int y_path=path.second;
      maze[x_path][y_path] = {PATH , FALSE};
      candidates.clear();
  }
 */
  int x, y;
  do{
      x=random_int(0, MAZE_ROW -1);
      y=random_int(0, MAZE_COLUMN -1);
  }while(maze[x][y].kind != PATH);
  maze[x][y] = {START};
}

Vec3 GetDirection(double angle)
{
    const Vec2 dir = Circular{ 1.0, angle };
    return{ dir.x, 0.0, -dir.y };
}

Vec3 GetFocusPosition(const Vec3& eyePosition, double angle)
{
    return (eyePosition + GetDirection(angle));
}

//迷路ゲーム
void MazeGame(int stage)
{
  //プレイヤー
  MazePlayer player;

  //迷路
  MazeBlock maze[STAGE][MAZE_ROW][MAZE_COLUMN] = { { {} } };
  for (int i = 0; i < STAGE; i++) generate_random_maze(maze[i]);
  /*
    {
      { //STAGE0
        { {START, TRUE } , {PATH , FALSE}, {PATH , FALSE}, {PATH , FALSE}, {PATH , FALSE} },
        { {WALL , FALSE} , {WALL , FALSE}, {PATH , FALSE}, {WALL , FALSE}, {WALL , FALSE} },
        { {WALL , FALSE} , {PATH , FALSE}, {PATH , FALSE}, {PATH , FALSE}, {PATH , FALSE} },
        { {PATH , FALSE} , {PATH , FALSE}, {WALL , FALSE}, {WALL , FALSE}, {WALL , FALSE} },
        { {WALL , FALSE} , {PATH , FALSE}, {PATH , FALSE}, {PATH , FALSE}, {GOAL , TRUE } },
      },

      { //STAGE1
        { {PATH , FALSE} , {WALL , FALSE}, {PATH , FALSE}, {PATH , FALSE}, {PATH , FALSE} },
        { {PATH , FALSE} , {WALL , FALSE}, {PATH , FALSE}, {WALL , FALSE}, {PATH , FALSE} },
        { {START, TRUE } , {PATH , FALSE}, {PATH , FALSE}, {WALL , FALSE}, {GOAL , TRUE } },
        { {PATH , FALSE} , {WALL , FALSE}, {WALL , FALSE}, {WALL , FALSE}, {WALL , FALSE} },
        { {PATH , FALSE} , {PATH , FALSE}, {PATH , FALSE}, {PATH , FALSE}, {PATH , FALSE} },
      },

      { //STAGE2
        { {WALL , FALSE} , {WALL , FALSE}, {PATH , FALSE}, {PATH , FALSE}, {PATH , FALSE} },
        { {PATH , FALSE} , {START, TRUE }, {PATH , FALSE}, {WALL , FALSE}, {PATH , FALSE} },
        { {PATH , FALSE} , {PATH , FALSE}, {WALL , FALSE}, {PATH , FALSE}, {PATH , FALSE} },
        { {WALL , FALSE} , {PATH , FALSE}, {WALL , FALSE}, {GOAL , TRUE }, {WALL , FALSE} },
        { {WALL , FALSE} , {PATH , FALSE}, {PATH , FALSE}, {WALL , FALSE}, {WALL , FALSE} },
      }
    };
*/
  //プレイヤー初期化
  if(MazePlayerInit(&player.row, &player.column, maze[stage]) == -1)
  {
    //関数MazePlayerInitが-1を返すとき初期化に失敗している
    //よって、この時点でプログラムを終了し、迷路の表示は行わない
    return;
  }
  
  const ColorF backgroundColor = ColorF{ 0.4, 0.6, 0.8 }.removeSRGBCurve();
  const Texture uvChecker{ U"example/texture/grass.jpg", TextureDesc::MippedSRGB };
  const MSRenderTexture renderTexture{ Scene::Size(),TextureFormat::R8G8B8A8_Unorm_SRGB,HasDepth::Yes };
  Font maze_font(MAZE_FONT);
  double angle=0_deg;
  BasicCamera3D camera{renderTexture.size(),104_deg,Vec3{player.row*4+2,2,player.column*4+2},GetFocusPosition(Vec3{player.row*4+2,2,player.column*4+2},angle)};
  while (System::Update() && maze[stage][player.row][player.column].kind != GOAL) {
    ClearPrint();
    if(KeyLeft.down()){
      angle-=90_deg;
      if(angle<0_deg){
          angle+=360_deg;
      }
    }
    if(KeyRight.down()){
      angle+=90_deg;
      if(angle>=360_deg){
        angle-=360_deg;
      }
    }
      if(KeyW.down()){
          if(angle==0_deg){
              if(player.column+1<MAZE_COLUMN){
                  if(maze[stage][player.row][player.column+1].kind!=WALL) player.column++;
              }
          }else if(angle==90_deg){
              if(player.row+1<MAZE_ROW){
                  if(maze[stage][player.row+1][player.column].kind!=WALL) player.row++;
              }
          }else if(angle==180_deg){
              if(player.column>0){
                  if(maze[stage][player.row][player.column-1].kind!=WALL) player.column--;
              }
          }else if(angle==270_deg){
              if(player.row>0){
                  if(maze[stage][player.row-1][player.column].kind!=WALL) player.row--;
              }
          }
      }
      camera.setView(Vec3{player.row*4+2,2,player.column*4+2},GetFocusPosition(Vec3{player.row*4+2,2,player.column*4+2},angle));
      Graphics3D::SetCameraTransform(camera);
    {
      const ScopedRenderTarget3D target{ renderTexture.clear(backgroundColor) };
      Plane{Vec3{46,0,46},92}.draw(uvChecker);
        for(int i=0;i<MAZE_ROW;i++){
            for(int j=0;j<MAZE_COLUMN;j++){
                if(maze[stage][i][j].kind==WALL){
                    Box::FromPoints(Vec3{i*4,0,j*4},Vec3{(i+1)*4,4,(j+1)*4}).draw(ColorF{ 0.8, 0.6, 0.4 }.removeSRGBCurve());
                }
                if(maze[stage][i][j].kind==GOAL){
                    Sphere{i*4+2,2,j*4+2,2}.draw(ColorF{0.4,0.8,0.6}.removeSRGBCurve());
                }
            }
        }
    }
    {
      Graphics3D::Flush();
      renderTexture.resolve();
      Shader::LinearToScreen(renderTexture);
    }
  }
  //迷路最終表示結果
  while(System::Update()){
      if(maze[stage][player.row][player.column].kind == GOAL) {
          static Font game_clear_font(180);
          static Font enter_font(70);
          game_clear_font(U"Game Clear!").draw(200,150);
          enter_font(U"Press Enter").draw(485,335);
      }else{
          static Font game_over_font(180);
          static Font enter_font(70);
          game_over_font(U"Game Over!").draw(200,200);
          enter_font(U"Press Enter").draw(485,385);
      }
      if(KeyEnter.down()) break;
  }
}


//メニュー
enum MazeMenu {STAGE0, STAGE1, STAGE2, EXIT};

//タイトル
int MazeTitle()
{
    Font title_font(40);
    Font normal_font(30);
    while (System::Update()) {
        title_font(U"＊＊＊迷路ゲーム＊＊＊").draw(520,150);
        title_font(U"メニュー").draw(600,200);
        normal_font(U"1:ステージ1").draw(600,250);
        normal_font(U"2:ステージ2").draw(600,285);
        normal_font(U"3:ステージ3").draw(600,320);
        normal_font(U"4:終了").draw(600,355);
        if (Key1.down()){
            return STAGE0;
        }
        if(Key2.down()){
            return STAGE1;
        }
        if(Key3.down()){
            return STAGE2;
        }
        if(Key4.down()){
            return EXIT;
        }
    }
    return EXIT;
}


void Main(void)
{
  Scene::SetBackground(Palette::Green);
  Window::SetFullscreen(true);
  int menu;
  int stage = -1;

  while(1)
  {
    //メニュー選択
    menu = MazeTitle();

    if(menu == EXIT) //EXITならwhileループを抜けて終了
    {
      break;
    }
    else
    {
      //ステージ設定
      switch(menu)
      {
      case STAGE0:
        stage = 0;
        break;
      case STAGE1:
        stage = 1;
        break;
      case STAGE2:
        stage = 2;
        break;
      }
    }

    //迷路ゲーム
    MazeGame(stage);
    //ゲームが終わるとwhileループの先頭にもどる
  }

}


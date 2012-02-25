// ウィンドウ関連の処理
#include "Window.h"

// 標準ライブラリ
#include <cmath>
#include <memory>

// 光源
const GgSimpleShader::Light light =
{
  { 0.2f, 0.2f, 0.2f, 1.0f },
  { 1.0f, 1.0f, 1.0f, 1.0f },
  { 1.0f, 1.0f, 1.0f, 1.0f },
  { 2.0f, 9.0f, 3.0f, 1.0f }
};

// オブジェクトの材質
const GgSimpleShader::Material material =
{
  { 0.7f, 0.5f, 0.5f, 1.0f },
  { 0.7f, 0.5f, 0.5f, 1.0f },
  { 0.2f, 0.2f, 0.2f, 1.0f },
  50.0f
};

// 床面の材質
const GgSimpleShader::Material tile =
{
  { 0.8f, 0.8f, 0.8f, 0.4f },
  { 0.8f, 0.8f, 0.8f, 0.4f },
  { 0.2f, 0.2f, 0.2f, 0.4f },
  30.0f
};

//
// メインプログラム
//
int main(int argc, const char *argv[])
{
  // ウィンドウを作成する
  Window window("ggsample12");

  // 背景色を指定する
  glClearColor(0.1f, 0.2f, 0.3f, 0.0f);

  // 隠面消去を有効にする
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  glDepthFunc(GL_LEQUAL);

  // アルファブレンディングの設定
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // 正像用のプログラムオブジェクト
  GgSimpleShader shader("simple.vert", "simple.frag");

  // 鏡像用のプログラムオブジェクト
  GgSimpleShader mirror("mirror.vert", "simple.frag");

  // OBJ ファイルの読み込み
  const std::unique_ptr<const GgElements> object(ggElementsObj("bunny.obj"));

  // 床面
  const std::unique_ptr<const GgTriangles> rectangle(ggRectangle(4.0f, 4.0f));

  // ビュー変換行列を mv に求める
  const GgMatrix mv(ggLookat(0.0f, 2.0f, 4.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f));

  // 図形のモデル変換行列を mm に求める
  const GgMatrix mm(ggTranslate(0.0f, 0.7f, 0.0f));

  // 図形の鏡像変換行列を mr に求める
  const GgMatrix mr(ggScale(1.0f, -1.0f, 1.0f));

  // 正像のワールド座標系における光源位置
  GLfloat normalpos[4];
  mv.projection(normalpos, light.position);
  
  // 鏡像のワールド座標系における光源位置
  GLfloat reflected[4];
  mr.projection(reflected, normalpos);

  // ウィンドウが開いている間くり返し描画する
  while (window.shouldClose() == GL_FALSE)
  {
    // 画面消去
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 投影変換行列
    const GgMatrix mp(ggPerspective(0.5f, window.getAspect(), 1.0f, 15.0f));

    // 鏡像用のシェーダの選択
    mirror.use();
    mirror.loadLightMaterial(light);
    mirror.loadLightPosition(reflected);

    // 鏡像の描画
    mirror.loadMaterial(material);
    mirror.loadMatrix(mp, mv * mm * window.getLeftTrackball());
    object->draw();

    // 正像用のシェーダの選択
    shader.use();
    mirror.loadLightMaterial(light);
    mirror.loadLightPosition(normalpos);

    // 床面の描画
    shader.loadMaterial(tile);
    shader.loadMatrix(mp, mv.rotateX(-1.5707963f));
    glEnable(GL_BLEND);
    rectangle->draw();
    glDisable(GL_BLEND);

    // 正像の描画
    shader.loadMaterial(material);
    shader.loadMatrix(mp, mv * mm * window.getLeftTrackball());
    object->draw();

    // カラーバッファを入れ替えてイベントを取り出す
    window.swapBuffers();
  }

  return 0;
}

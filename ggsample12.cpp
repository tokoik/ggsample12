//
// ゲームグラフィックス特論宿題アプリケーション
//
#include "GgApp.h"

// プロジェクト名
#ifndef PROJECT_NAME
#  define PROJECT_NAME "ggsample12"
#endif

// 光源
const GgSimpleShader::Light light
{
  { 0.2f, 0.2f, 0.2f, 1.0f },
  { 1.0f, 1.0f, 1.0f, 1.0f },
  { 1.0f, 1.0f, 1.0f, 1.0f },
  { 2.0f, 9.0f, 3.0f, 1.0f }
};

// オブジェクトの材質
const GgSimpleShader::Material material
{
  { 0.7f, 0.5f, 0.5f, 1.0f },
  { 0.7f, 0.5f, 0.5f, 1.0f },
  { 0.2f, 0.2f, 0.2f, 1.0f },
  50.0f
};

// 床面の材質
const GgSimpleShader::Material tile
{
  { 0.8f, 0.8f, 0.8f, 0.4f },
  { 0.8f, 0.8f, 0.8f, 0.4f },
  { 0.2f, 0.2f, 0.2f, 0.4f },
  30.0f
};

//
// アプリケーション本体
//
int GgApp::main(int argc, const char* const* argv)
{
  // ウィンドウを作成する (この行は変更しないでください)
  Window window{ argc > 1 ? argv[1] : PROJECT_NAME };

  // 背景色を指定する
  glClearColor(0.1f, 0.2f, 0.3f, 0.0f);

  // 隠面消去を有効にする
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  glDepthFunc(GL_LEQUAL);

  // アルファブレンディングの設定
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // 正像用のプログラムオブジェクト
  GgSimpleShader shader{ PROJECT_NAME ".vert", PROJECT_NAME ".frag" };

  // 鏡像用のプログラムオブジェクト
  GgSimpleShader mirror{ PROJECT_NAME "mirror.vert", PROJECT_NAME ".frag" };

  // 図形の読み込み
  const auto object{ ggElementsObj("bunny.obj") };

  // 図形の材質
  const GgSimpleShader::MaterialBuffer materialBuffer{ material };

  // 床面
  const auto rectangle{ ggRectangle(4.0f, 4.0f) };

  // 床面の材質
  const GgSimpleShader::MaterialBuffer tileBuffer{ tile };

  // ビュー変換行列を mv に求める
  const GgMatrix mv(ggLookat(0.0f, 2.0f, 4.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f));

  // 図形のモデル変換行列を mm に求める
  const auto mm{ ggTranslate(0.0f, 0.7f, 0.0f) };

  // 図形の鏡像変換行列を mr に求める
  const auto mr{ ggScale(1.0f, -1.0f, 1.0f) };

  // 光源の材質
  const GgSimpleShader::LightBuffer lightBuffer{ light };

  // 正像のワールド座標系における光源位置
  const auto normalpos{ mv * light.position };
  
  // 鏡像のワールド座標系における光源位置
  const auto reflected{ mr * normalpos };

  // ウィンドウが開いている間くり返し描画する
  while (window)
  {
    // 画面消去
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 投影変換行列
    const auto mp{ ggPerspective(0.5f, window.getAspect(), 1.0f, 15.0f) };

    // 鏡像用のシェーダの選択
    mirror.use(mp, mv * mm * window.getRotationMatrix(), lightBuffer);
    lightBuffer.loadPosition(reflected.data());

    // 鏡像の描画
    materialBuffer.select();
    object->draw();

    // 正像用のシェーダの選択
    shader.use(mp, lightBuffer);
    lightBuffer.loadPosition(normalpos.data());

    // 床面の描画
    shader.loadModelviewMatrix(mv.rotateX(-1.5707963f));
    tileBuffer.select();
    glEnable(GL_BLEND);
    rectangle->draw();
    glDisable(GL_BLEND);

    // 正像の描画
    shader.loadModelviewMatrix(mv * mm * window.getRotationMatrix());
    materialBuffer.select();
    object->draw();

    // カラーバッファを入れ替えてイベントを取り出す
    window.swapBuffers();
  }

  return 0;
}

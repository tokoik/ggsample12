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
// アプリケーションの実行
//
void app()
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
  GgSimpleShader shader("ggsample12.vert", "ggsample12.frag");

  // 鏡像用のプログラムオブジェクト
  GgSimpleShader mirror("ggsample12mirror.vert", "ggsample12.frag");

  // 図形の読み込み
  const std::unique_ptr<const GgElements> object(ggElementsObj("bunny.obj"));

  // 図形の材質
  const GgSimpleShader::MaterialBuffer materialBuffer(material);

  // 床面
  const std::unique_ptr<const GgTriangles> rectangle(ggRectangle(4.0f, 4.0f));

  // 床面の材質
  const GgSimpleShader::MaterialBuffer tileBuffer(tile);

  // ビュー変換行列を mv に求める
  const GgMatrix mv(ggLookat(0.0f, 2.0f, 4.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f));

  // 図形のモデル変換行列を mm に求める
  const GgMatrix mm(ggTranslate(0.0f, 0.7f, 0.0f));

  // 図形の鏡像変換行列を mr に求める
  const GgMatrix mr(ggScale(1.0f, -1.0f, 1.0f));

  // 光源の材質
  const GgSimpleShader::LightBuffer lightBuffer(light);

  // 正像のワールド座標系における光源位置
  const GgVector normalpos(mv * light.position);
  
  // 鏡像のワールド座標系における光源位置
  const GgVector reflected(mr * normalpos);

  // ウィンドウが開いている間くり返し描画する
  while (window)
  {
    // 画面消去
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 投影変換行列
    const GgMatrix mp(ggPerspective(0.5f, window.getAspect(), 1.0f, 15.0f));

    // 鏡像用のシェーダの選択
    mirror.use(mp, mv * mm * window.getTrackball(), lightBuffer);
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
    shader.loadModelviewMatrix(mv * mm * window.getTrackball());
    materialBuffer.select();
    object->draw();

    // カラーバッファを入れ替えてイベントを取り出す
    window.swapBuffers();
  }

  return;
}

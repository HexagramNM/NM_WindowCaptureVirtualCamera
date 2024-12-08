# NM_WindowCaptureVirtualCamera
ウィンドウをキャプチャし、仮想カメラ（MediaFoundation, DirectShow）で映像を送る汎用ツール

現在、工事中です

# ビルド方法

1. このリポジトリをクローンします。

2. クローン後、setupRepository.batを実行してください。

    これにより、以下の処理を実行します。
    
    - サブモジュール内の不要なファイルを削除するように設定

    - DirectShowの古いVisual Studio用プロジェクトをアップグレード

    - DirectShowのビルド（必要なスタティックライブラリのビルド）

3. Visual Studioで`NM_WindowCaptureVirtualCamera.sln`を開き、ビルドします。

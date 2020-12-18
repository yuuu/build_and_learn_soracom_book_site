# 作りながら学ぶSORACOM入門 サポートページ

本リポジトリは技術書典10で頒布した技術書「作りながら学ぶSORACOM入門」のサポートページです。

![カバー]()
![構成図]()

## ディレクトリ構成

本リポジトリのディレクトリ構成は次の通りです。

### backend

「3章 SORACOMをAWSと連携する」で使用するAWSのバックエンドを構築するためのAWS SAM プロジェクトです。

### chapter1

「1章 SORACOMを使うための準備」で使用する、M5Stack用のプログラムです。
ToFセンサーの値を取得して、値に応じた文字をディスプレイへ表示します。

### chapter2

「2章 はじめてのSORACOM」で使用する、M5Stack用のプログラムです。
SORACOMへToFセンサーの値を送信します。

### chapter3

「3章 SORACOMをAWSと連携する」で使用する、M5Stack用のプログラムです。
SORACOM Beam MQTTエンドポイントに接続して、ToFセンサーの値のpublishおよびメッセージのsubscribeを行います。

## 正誤表

書籍の誤植が見つかった場合、こちらに追記予定です。

## 指摘・質問がある場合

[本リポジトリのIssues](https://github.com/yuuu/build_and_learn_soracom_book_site/issues)に書き込みをお願いいたします。

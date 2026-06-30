# 作品説明資料

本プロジェクトは、DirectX（DxLib）と SQLite データベースを利用して開発された、ハック＆スラッシュ（ハクスラ）風の「ガチャ ＆ バトルRPG」ゲームです。  
ゲームの各種パラメータや計算式、セーブデータなどが SQLite を介して完全にデータ駆動で管理されているのが大きな特徴です。

---

## 1. 開発基本情報

		開発にかかった期間 
    	2026年5月12日 〜 2026年6月30日（ゲームループ完成：6月23日）  
    
		所要時間数の概算 
    	約155時間 〜 217時間（1日あたり5〜7時間稼働と想定）
    
		開発メンバー数と構成 
    	1名（個人開発）
    	制作者：市村 義春（アミューズメントメディア総合学院 ゲームクリエイター学科ゲームプログラマー専攻２年）
    
		担当範囲
		個人開発につき全工程を担当
		ゲームデザイン、仕様策定、企画立案
		C++ によるゲームプログラミング（DxLib フレームワーク利用）
    	SQLite データベース設計および SQL 連携ロジックの実装
    	動的数式パースエンジンの設計・実装
    	UIデザイン・レイアウト設計
    	3Dアセット（モデル・モーション）の組み込み・アニメーション制御 (モデルは過去の共同制作で使用したものを流用)

---

## 2. 動作環境・動作仕様

		OS：Windows 10 / 11
		グラフィックスAPI：DirectX 9.0c 互換（DxLib を使用）
		データベース：SQLite3 (`sqlite3.dll` を同梱)
		画面解像度：1280 × 720 (ウィンドウモード)
		開発環境：Visual Studio 2026
		開発言語：C++(C++20推奨)

---

## 3. 操作方法

	  ゲームは「ガチャフェーズ」と「バトルフェーズ」の2つの状態を行き来しながら進行します。

	  ■ ガチャフェーズ
		 ガチャを回し、キャラクターの装備を整えてステータスを強化するフェーズです。
		 マウス左クリック：画面上のボタン（ガチャを回す、ステータス画面切り替え、バトル開始、セーブデータ確認など）を選択・決定します。
		 R キー：ガチャを引く（ショートカットキー。1回あたり 3000 コイン消費）。
		 Delete キー：セーブデータを初期化します（装備データの削除と、ステータスの初期化）（デバッグ用機能）。

	  ■ バトルフェーズ
		 敵キャラクターと戦闘を行うフェーズです。
		 ・攻撃ターン（タイミングゲージ）
    		 画面上のゲージが左右に動きます。
    		 タイミングよくゲージ内でマウス左クリックまたはFキーを押しし、目標範囲内（赤部分）で止めることで、敵に対してダメージを与えることができます。
    		
		 ・防御ターン（サークル消し）
    		 画面上に円（サークル）が出現します。
    		 マウス左クリックすることで、防御フェーズの残り時間を進めることができ、敵の攻撃を食らう回数を減らすことができます。
			 B キー：ガチャ画面へ強制的に戻ります（デバッグ用機能）。

---

## 4. 制作意図・技術的な解説

	  1. ハクスラ的ゲームループの構築
		 本作は「ガチャで装備を引き、ランダムな追加ステータスによってキャラクターをどんどん強化し、より強力な敵に挑む」という、
		 ハックアンドスラッシュ（ハクスラ）の楽しさを凝縮したゲームループを目指して制作しました。バトルで獲得したコインを使ってさらにガチャを引き、
		 最強の装備を厳選する楽しさを提供します。

	  2. SQLite3 による徹底した「データ駆動型」設計
		 プログラムの再ビルドを行うことなくゲームバランスを調整できるよう、徹底したデータ駆動設計を導入しています。
		 確率やアセット設定の外部化：装備の出現確率、ステータス付与の確率、ミニゲームの難易度（ゲージの速さやサークルの位置）などがすべて SQLite のマスタテーブルに定義されています。
		 計算式のデータベース化：ダメージ計算式や被ダメージ計算式自体が、データベース内のテーブル（`charaFormulas`）に文字列データとして格納されています。

	  3. 自作の数式評価エンジン
		 データベースからロードした文字列形式の計算式（例：(攻撃×敵防御倍率×ダメージ減衰率)×会心倍率×ゲージ成功/失敗倍率）を、ゲーム実行時に解析・評価するパーサーを自作しています。
		 これにより、プログラミングコードを一切書き換えることなく、データベース上の数式を編集するだけでダメージ計算ロジックそのものをリアルタイムに変更できる高い拡張性を実現しています。
		 
	  4. 制作意図
	  	 １年次での共同制作の経験からキャラクターのステータスを一括でまとめて管理したいと感じたことがあり、SQLに触れることにしました。
	  	 いままでの課題点であった関数の肥大化、可読性の低いコードの解消を目標に、メンテナンスのしやすさや細かく処理を分けることを重視しました。
---


## 5. 各ソースコードの処理内容一覧（Gameフォルダ内）

	  Gameプロジェクトの `source` フォルダに含まれる各ファイルの役割は大まかに以下の通りです。

### 📊 ゲーム全体・シーン管理
		[ApplicationMain.h](https://github.com/ichi-yoshi/kojin202601/blob/20260611/Game/Game/source/ApplicationMain.h) / [ApplicationMain.cpp](https://github.com/ichi-yoshi/kojin202601/blob/20260611/Game/Game/source/ApplicationMain.cpp)  
    	アプリケーション全体の管理クラス。DxLib のウインドウ設定（1280x720）や、ゲームメインシーン（`ModeGame`）の登録を行います
		[ModeGame.h](https://github.com/ichi-yoshi/kojin202601/blob/20260611/Game/Game/source/ModeGame.h) / [ModeGame.cpp](https://github.com/ichi-yoshi/kojin202601/blob/20260611/Game/Game/source/ModeGame.cpp)  
    	ゲームのメイン進行シーン。ガチャフェーズとバトルフェーズの遷移、3D背景モデルの描画、3Dカメラ制御、各UI（ガチャ、ステータス、セーブデータ）の更新・描画を統括します
		[Resource.h](https://github.com/ichi-yoshi/kojin202601/blob/20260611/Game/Game/source/Resource.h)  
    	3Dアセット（空球、地面、ダンジョン、敵キャラクターのモデル）のファイルパスを定義するヘッダーファイル
    	[container.h](https://github.com/ichi-yoshi/kojin202601/blob/main/Game/Game/source/container.h)
    	共通インクルード用のヘッダーファイル


### ⚔ バトルシステム関連
		[ModeGameBattle.h](https://github.com/ichi-yoshi/kojin202601/blob/20260611/Game/Game/source/ModeGameBattle.h) / [ModeGameBattle.cpp](https://github.com/ichi-yoshi/kojin202601/blob/20260611/Game/Game/source/ModeGameBattle.cpp)  
    	バトル全体の進行管理。スタート・攻撃・防御・リザルトといったバトル内フェーズの制御、敵の描画、戦闘結果に応じたセーブデータの更新（経験値付与、コイン獲得、レベルアップ）を行います
		[BattleTimer.h](https://github.com/ichi-yoshi/kojin202601/blob/20260611/Game/Game/source/BattleTimer.h) / [BattleTimer.cpp](https://github.com/ichi-yoshi/kojin202601/blob/20260611/Game/Game/source/BattleTimer.cpp)  
    	バトルの制限時間や、フェーズ経過時間を管理するタイマークラス
		[BattleButtonUI.h](https://github.com/ichi-yoshi/kojin202601/blob/main/Game/Game/source/BattleButtonUI.h) / [BattleUI.cpp](https://github.com/ichi-yoshi/kojin202601/blob/main/Game/Game/source/BattleButtonUI.cpp)  
    	バトルフェーズ移行に関するUIの描画・クリック判定
    	[BattleUI.h](https://github.com/ichi-yoshi/kojin202601/blob/main/Game/Game/source/BattleUI.h) / [BattleUI](https://github.com/ichi-yoshi/kojin202601/blob/main/Game/Game/source/BattleUI.cpp)
    	バトル中に関するUIの描画
		[GaugeUI.h](https://github.com/ichi-yoshi/kojin202601/blob/20260611/Game/Game/source/GaugeUI.h) / [GaugeUI.cpp](https://github.com/ichi-yoshi/kojin202601/blob/20260611/Game/Game/source/GaugeUI.cpp)  
    	攻撃時にタイミングよく目印を止める、タイミングバー（ゲージ）の描画と成否の判定ロジック
		[CircleUI.h](https://github.com/ichi-yoshi/kojin202601/blob/20260611/Game/Game/source/CircleUI.h) / [CircleUI.cpp](https://github.com/ichi-yoshi/kojin202601/blob/20260611/Game/Game/source/CircleUI.cpp)  
    	防御時に出現する円を左クリックで消す、サークルの描画と成否の判定ロジック


### 🧙 キャラクタ・敵・計算処理
*   	[CharaBase.h](https://github.com/ichi-yoshi/kojin202601/blob/20260611/Game/Game/source/CharaBase.h) / [CharaBase.cpp](https://github.com/ichi-yoshi/kojin202601/blob/20260611/Game/Game/source/CharaBase.cpp)  
    	プレイヤーキャラクターの無装備状態における基本ステータスを保持
*   	[Chara.h](https://github.com/ichi-yoshi/kojin202601/blob/20260611/Game/Game/source/Chara.h) / [Chara.cpp](https://github.com/ichi-yoshi/kojin202601/blob/20260611/Game/Game/source/Chara.cpp)  
    	装備ステータスのパース（解析）と、基本ステータスに対する装備補正（％上昇など）の合算計算
*   	[CharaAfterStatus.h](https://github.com/ichi-yoshi/kojin202601/blob/20260611/Game/Game/source/CharaAfterStatus.h) / [CharaAfterStatus.cpp](https://github.com/ichi-yoshi/kojin202601/blob/20260611/Game/Game/source/CharaAfterStatus.cpp)  
    	装備適用後の最終ステータスの保持、および速度値に基づいたバトルのボーナスタイム換算
*   	[Enemy.h](https://github.com/ichi-yoshi/kojin202601/blob/20260611/Game/Game/source/Enemy.h) / [Enemy.cpp](https://github.com/ichi-yoshi/kojin202601/blob/20260611/Game/Game/source/Enemy.cpp)  
    	敵キャラクターの3Dモデル（タヌキ）のロード、描画、アニメーションステート（待機、攻撃、防御、勝利など）の切り替え
*   	[CharaFormula.h](https://github.com/ichi-yoshi/kojin202601/blob/20260611/Game/Game/source/CharaFormula.h) / [CharaFormula.cpp](https://github.com/ichi-yoshi/kojin202601/blob/20260611/Game/Game/source/CharaFormula.cpp)  
    	戦闘中の与ダメージ・被ダメージを、データベース上の数式をパースして算出するダメージ計算ブリッジ
*   	[EvaluateFormula.h](https://github.com/ichi-yoshi/kojin202601/blob/20260611/Game/Game/source/EvaluateFormula.h) / [EvaluateFormula.cpp](https://github.com/ichi-yoshi/kojin202601/blob/20260611/Game/Game/source/EvaluateFormula.cpp)  
    	独自の数式評価エンジン。文字列としての計算式をパースし、四則演算（カッコ対応）を行う電卓ロジック


### 🎁 ガチャシステム関連
		[GachaBase.h](https://github.com/ichi-yoshi/kojin202601/blob/20260611/Game/Game/source/GachaBase.h) / [GachaBase.cpp](https://github.com/ichi-yoshi/kojin202601/blob/20260611/Game/Game/source/GachaBase.cpp)  
    	重み付き確率抽選など、ガチャを回す際の共通確率計算処理
		[GachaArmor.h](https://github.com/ichi-yoshi/kojin202601/blob/20260611/Game/Game/source/GachaArmor.h) / [GachaArmor.cpp](https://github.com/ichi-yoshi/kojin202601/blob/20260611/Game/Game/source/GachaArmor.cpp)  
    	防具の部位の抽選処理
		[GachaBasicStatus.h](https://github.com/ichi-yoshi/kojin202601/blob/20260611/Game/Game/source/GachaBasicStatus.h) / [GachaBasicStatus.cpp](https://github.com/ichi-yoshi/kojin202601/blob/20260611/Game/Game/source/GachaBasicStatus.cpp)  
    	装備に付与されるメインステータス項目の抽選処理
		[Gacha.h](https://github.com/ichi-yoshi/kojin202601/blob/20260611/Game/Game/source/Gacha.h) / [Gacha.cpp](https://github.com/ichi-yoshi/kojin202601/blob/20260611/Game/Game/source/Gacha.cpp)  
    	装備に付与されるサブステータス項目の抽選処理
		[GachaSystem.h](https://github.com/ichi-yoshi/kojin202601/blob/20260611/Game/Game/source/GachaSystem.h) / [GachaSystem.cpp](https://github.com/ichi-yoshi/kojin202601/blob/20260611/Game/Game/source/GachaSystem.cpp)  
    	ガチャを引く（コイン消費・確率決定）、結果を装備する（セーブデータ同期）、結果を破棄するといった一連の流れを管理
		[GachaUI.h](https://github.com/ichi-yoshi/kojin202601/blob/20260611/Game/Game/source/GachaUI.h) / [GachaUI.cpp](https://github.com/ichi-yoshi/kojin202601/blob/20260611/Game/Game/source/GachaUI.cpp)  
    	ガチャ画面の背景、ガチャボタン、引いた装備のステータス表示、装備・破棄選択ボタンなどの描画


### 💾 セーブデータ・UI関連
		[SaveData.h](https://github.com/ichi-yoshi/kojin202601/blob/20260611/Game/Game/source/SaveData.h) / [SaveData.cpp](https://github.com/ichi-yoshi/kojin202601/blob/20260611/Game/Game/source/SaveData.cpp)  
    	アカウントデータ（レベル、経験値、コイン等）を SQLite 経由で保存・読込する
		[SaveEquipment.h](https://github.com/ichi-yoshi/kojin202601/blob/20260611/Game/Game/source/SaveEquipment.h) / [SaveEquipment.cpp](https://github.com/ichi-yoshi/kojin202601/blob/20260611/Game/Game/source/SaveEquipment.cpp)  
    	部位ごとの装備中の防具情報（メイン・サブステータス値）を SQLite 経由で保存・読込する
		[SaveCharaStatus.h](https://github.com/ichi-yoshi/kojin202601/blob/20260611/Game/Game/source/SaveCharaStatus.h) / [SaveCharaStatus.cpp](https://github.com/ichi-yoshi/kojin202601/blob/20260611/Game/Game/source/SaveCharaStatus.cpp)  
    	装備適用後の最終ステータスを SQLite に保存・同期する
		[StatusUI.h](https://github.com/ichi-yoshi/kojin202601/blob/20260611/Game/Game/source/StatusUI.h) / [StatusUI.cpp](https://github.com/ichi-yoshi/kojin202601/blob/20260611/Game/Game/source/StatusUI.cpp)  
    	キャラクターの最終ステータス一覧をウィンドウ表示するUI
		[SaveDataUI.h](https://github.com/ichi-yoshi/kojin202601/blob/20260611/Game/Game/source/SaveDataUI.h) / [SaveDataUI.cpp](https://github.com/ichi-yoshi/kojin202601/blob/20260611/Game/Game/source/SaveDataUI.cpp)  
    	アカウント情報（セーブデータ）をウィンドウ表示するUI
		[ButtonUI.h](https://github.com/ichi-yoshi/kojin202601/blob/20260611/Game/Game/source/ButtonUI.h) / [ButtonUI.cpp](https://github.com/ichi-yoshi/kojin202601/blob/20260611/Game/Game/source/ButtonUI.cpp)  
    	様々なUIで再利用される、汎用的な矩形ボタン
    	[DatabaseSelector.h](https://github.com/ichi-yoshi/kojin202601/blob/main/Game/Game/source/DatabaseSelector.h) / [DatabaseSelector.cpp](https://github.com/ichi-yoshi/kojin202601/blob/main/Game/Game/source/DatabaseSelector.cpp)
    	データベースを変更する。新規データベースの作成可
    	[DatabaseSelectorButtonUI.h](https://github.com/ichi-yoshi/kojin202601/blob/main/Game/Game/source/DatabaseSelectorButtonUI.h) / [DatabaseSelectorButtonUI.cpp](https://github.com/ichi-yoshi/kojin202601/blob/main/Game/Game/source/DatabaseSelectorButtonUI.cpp)
    	データベース変更フェーズに移行するためのボタン


### 🔌 SQLite連携ユーティリティ
		[SqliteConfig.h](https://github.com/ichi-yoshi/kojin202601/blob/20260611/Game/Game/source/SqliteConfig.h)  
    	使用するデータベースファイル名（`name.sqlite3` 等）を定義
		[SqliteUtill.h](https://github.com/ichi-yoshi/kojin202601/blob/20260611/Game/Game/source/SqliteUtill.h) / [SqliteUtill.cpp](https://github.com/ichi-yoshi/kojin202601/blob/20260611/Game/Game/source/SqliteUtill.cpp)  
    	SQLite 接続（Open / Close）を行う共通関数群
		[SqliteTextUtill.h](https://github.com/ichi-yoshi/kojin202601/blob/20260611/Game/Game/source/SqliteTextUtill.h) / [SqliteTextUtill.cpp](https://github.com/ichi-yoshi/kojin202601/blob/20260611/Game/Game/source/SqliteTextUtill.cpp)  
    	Shift_JIS と UTF-8 の文字エンコーディング相互変換、SQL インジェクション対策のエスケープ処理
		[SqliteTableCreator.h](https://github.com/ichi-yoshi/kojin202601/blob/20260611/Game/Game/source/SqliteTableCreator.h) / [SqliteTableCreator.cpp](https://github.com/ichi-yoshi/kojin202601/blob/20260611/Game/Game/source/SqliteTableCreator.cpp)  
    	ゲーム起動時に必要なすべてのデータベーステーブル（計12個）を生成する
		[SqliteSeeder.h](https://github.com/ichi-yoshi/kojin202601/blob/20260611/Game/Game/source/SqliteSeeder.h) / [SqliteSeeder.cpp](https://github.com/ichi-yoshi/kojin202601/blob/20260611/Game/Game/source/SqliteSeeder.cpp)  
    	初回起動時等に、確率マスタや計算式、敵パラメータ等の初期レコードをテーブルに流し込む（Seed）
		各データ取得クラス（SQLiteマスタロード用） 
    	データベースから各設定レコードを抽出し、ゲーム内インスタンスに割り振るブリッジ処理群
    	[SqliteBasicStatus.h](https://github.com/ichi-yoshi/kojin202601/blob/20260611/Game/Game/source/SqliteBasicStatus.h) / [SqliteBasicStatus.cpp](https://github.com/ichi-yoshi/kojin202601/blob/20260611/Game/Game/source/SqliteBasicStatus.cpp) (メインステータス確率マスタ)
    	[SqliteStatus.h](https://github.com/ichi-yoshi/kojin202601/blob/20260611/Game/Game/source/SqliteStatus.h) / [SqliteStatus.cpp](https://github.com/ichi-yoshi/kojin202601/blob/20260611/Game/Game/source/SqliteStatus.cpp) (サブステータス確率マスタ)
    	[SqliteArmor.h](https://github.com/ichi-yoshi/kojin202601/blob/20260611/Game/Game/source/SqliteArmor.h) / [SqliteArmor.cpp](https://github.com/ichi-yoshi/kojin202601/blob/20260611/Game/Game/source/SqliteArmor.cpp) (防具部位マスタ)
    	[SqliteCharaStatus.h](https://github.com/ichi-yoshi/kojin202601/blob/20260611/Game/Game/source/SqliteCharaStatus.h) / [SqliteCharaStatus.cpp](https://github.com/ichi-yoshi/kojin202601/blob/20260611/Game/Game/source/SqliteCharaStatus.cpp) (キャラ基礎ステータスマスタ)
    	[SqliteCharaFormula.h](https://github.com/ichi-yoshi/kojin202601/blob/20260611/Game/Game/source/SqliteCharaFormula.h) / [SqliteCharaFormula.cpp](https://github.com/ichi-yoshi/kojin202601/blob/20260611/Game/Game/source/SqliteCharaFormula.cpp) (計算式定義マスタ)
    	[SqliteEnemy.h](https://github.com/ichi-yoshi/kojin202601/blob/20260611/Game/Game/source/SqliteEnemy.h) / [SqliteEnemy.cpp](https://github.com/ichi-yoshi/kojin202601/blob/20260611/Game/Game/source/SqliteEnemy.cpp) (敵基礎ステータスマスタ)
    	[SqliteCircle.h](https://github.com/ichi-yoshi/kojin202601/blob/20260611/Game/Game/source/SqliteCircle.h) / [SqliteCircle.cpp](https://github.com/ichi-yoshi/kojin202601/blob/20260611/Game/Game/source/SqliteCircle.cpp) (サークルミニゲーム設定マスタ)
    	[SqliteGauge.h](https://github.com/ichi-yoshi/kojin202601/blob/20260611/Game/Game/source/SqliteGauge.h) / [SqliteGauge.cpp](https://github.com/ichi-yoshi/kojin202601/blob/20260611/Game/Game/source/SqliteGauge.cpp) (ゲージミニゲーム設定マスタ)
    	[SqliteSpeed.h](https://github.com/ichi-yoshi/kojin202601/blob/20260611/Game/Game/source/SqliteSpeed.h) / [SqliteSpeed.cpp](https://github.com/ichi-yoshi/kojin202601/blob/20260611/Game/Game/source/SqliteSpeed.cpp) (速度ボーナス対応表)


### 🖱 インプット・グラフィック周辺
		[MouseInput.h](https://github.com/ichi-yoshi/kojin202601/blob/20260611/Game/Game/source/MouseInput.h) / [MouseInput.cpp](https://github.com/ichi-yoshi/kojin202601/blob/20260611/Game/Game/source/MouseInput.cpp)  
    	マウスの座標、左・右クリック状態、矩形選択範囲の進入判定、ドラッグ、ホイールスクロール値を管理する入力仲介クラス
		[CameraBase.h](https://github.com/ichi-yoshi/kojin202601/blob/20260611/Game/Game/source/CameraBase.h) / [CameraBase.cpp](https://github.com/ichi-yoshi/kojin202601/blob/20260611/Game/Game/source/CameraBase.cpp)  
    	3D空間カメラの位置（Position）や注視点（Target）、視野角などを設定するカメラクラス
		[animationmanager.h](https://github.com/ichi-yoshi/kojin202601/blob/20260611/Game/Game/source/animationmanager.h) / [animationmanager.cpp](https://github.com/ichi-yoshi/kojin202601/blob/20260611/Game/Game/source/animationmanager.cpp)  
    	3Dモデルのアニメーションを滑らかに補間・ブレンド（フェードイン・アウト）して切り替えるためのアニメーション管理クラス
    
    
    
## 6. SQLiteのデータ確認方法

	  こちらでは実際に作成したデータベースの確認手順を明記します。
	  
	  フォルダ名:DB Browser for SQLiteの中にあるDB Browser for SQLite.exeを開きます。
	  開いたら「データベースを開く」から閲覧したいデータベース開きます。(.sqlite3というファイルが対象です。sqlite3.dllと同じ階層にあります。)
	  「データベース構造」「データ閲覧」からそれぞれのテーブルを閲覧できます。
	  
	  ※DB Browser for SQLite.exeのみでは動かないのでプロジェクトごとダウンロードしてください
	  
	  作成済みデータ
	  https://github.com/ichi-yoshi/kojin202601/blob/20260611/Game/Game/e.sqlite3
	  https://github.com/ichi-yoshi/kojin202601/blob/20260611/Game/Game/f.sqlite3
	  https://github.com/ichi-yoshi/kojin202601/blob/main/Game/Game/g.sqlite3
	  https://github.com/ichi-yoshi/kojin202601/blob/main/Game/Game/h.sqlite3
	  
	  リンク先で「view raw」を押すとダウンロードできます。
	  
	  

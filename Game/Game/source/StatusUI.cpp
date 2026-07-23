#include "StatusUI.h"
#include "MagicNumberConfig.h"
#include "container.h"

// 名前空間の使用の宣言
using namespace UIConfig;

void StatusUI::Update(MouseInput& mouse)
{
	const auto& charaBtn = GetCharaButtonRect();
	_isCharaClicked = mouse.IsLeftTrig() && mouse.IsInRect(_charaButton.x, _charaButton.y, _charaButton.w, _charaButton.h);
}

void StatusUI::Draw(const CharaAfterStatus& afterStatus, bool visible, const MouseInput& mouse) const
{
	// キャラクターボタンの描画
	DrawCharaButton();

	if(visible)
	{
		DrawStatusBox(afterStatus, mouse);
	}
}

void StatusUI::DrawCharaButton() const
{
	ButtonUI::DrawButton(_charaButton, "キャラ");
}

void StatusUI::DrawStatusBox(const CharaAfterStatus& afterStatus, const MouseInput& mouse) const
{
	const auto rows = afterStatus.ToRows();
	if(rows.empty()) { return; }

	// 解説メッセージの定義（ステータス名 -> 解説テキスト）
	static const std::unordered_map<std::string, std::string> Descriptions = 
	{
		{ "HP", "0になると負けになります。" },
		{ "攻撃", "与えるダメージに影響します。" },
		{ "防御", "受けるダメージを軽減します。" },
		{ "HP%", "HPに対する割合ボーナスです。" },
		{ "攻撃%", "攻撃力に対する割合ボーナスです。" },
		{ "防御%", "防御力に対する割合ボーナスです。" },
		{ "会心率", "会心が発生する確率です。" },
		{ "会心ダメージ", "会心発動時のダメージ倍率です。" },
		{ "速度", "防御フェーズ時の円を消した時の時間の減少量に影響します。" },
		{ "運値", "最終ダメージが２倍になる確率に影響します。" },
		{ "Poop", "残念なステータスです。ダメージ減衰率に影響します。" }
	};

	int fontSize = Font::Normal;			// フォントサイズを取得
	int rowSpace = Common::RowSpacingExtra;	// 行間の追加スペースを取得
	int rowHeight = fontSize + rowSpace;	// 行の高さを計算
	int padding = Common::DefaultPadding;	// ボックス内の余白を取得
	int boxW = Layout::StatusBoxWidth;		// ボックスの幅を取得
	int boxH = static_cast<int>(rows.size()) * rowHeight + padding * rowSpace;	// ボックスの高さを計算

	int x = Layout::StatusBoxPos.x;	// ボックスのX座標を取得
	int y = Layout::StatusBoxPos.y;	// ボックスのY座標を取得

	DrawBox(x, y, x + boxW, y + boxH, Color::BoxBg(), TRUE);

	SetFontSize(fontSize);

	// テキストの描画開始位置を計算
	int textY = y + padding;
	std::string hoveredDesc = ""; // マウスが乗っている項目の解説

	// 各行の文字列を描画
	for(const auto& row : rows)
	{
		DrawString(x + padding, textY, row.c_str(), Color::Black());

		// "HP:100.0" などの文字列から "HP" の部分（名前）を取り出す
		std::string name = row;
		size_t pos = row.find(':');
		if(pos != std::string::npos) {
			name = row.substr(0, pos);
		}

		// テキストの幅を取得して判定用の矩形を作成
		int textWidth = GetDrawStringWidth(row.c_str(), static_cast<int>(row.size()));

		// マウスポインタがこの行（テキストの上）にあるか判定
		if(mouse.IsInRect(x + padding, textY, textWidth, fontSize))
		{
			auto it = Descriptions.find(name);
			if(it != Descriptions.end())
			{
				hoveredDesc = it->second;
			}
		}

		textY += rowHeight;
	}

	// 該当する項目にマウスが乗っていれば最手前にツールチップを描画
	if(!hoveredDesc.empty())
	{
		DrawTooltip(mouse.GetX() + 12, mouse.GetY() + 12, hoveredDesc);
	}
}

// ツールチップの描画処理
void StatusUI::DrawTooltip(int x, int y, const std::string& text) const
{
	int padding = Common::DefaultPadding;
	int rowSpace = Common::RowSpacingExtra;
	int textWidth = GetDrawStringWidth(text.c_str(), static_cast<int>(text.size()));
	int fontHeight = Font::Normal;
	int boxW = textWidth + padding * rowSpace;
	int boxH = fontHeight + padding * rowSpace;

	// 画面外にはみ出さないように補正
	int screenW = Layout::Screen.w;
	int screenH = Layout::Screen.h;
	if(x + boxW > screenW) { x = screenW - boxW; }
	if(y + boxH > screenH) { y = screenH - boxH; }

	// 半透明の背景ボックスを描画
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, Alpha::Dim);
	DrawBox(x, y, x + boxW, y + boxH, Color::BoxBg(), TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	// 枠線とテキストを描画
	DrawBox(x, y, x + boxW, y + boxH, Color::Black(), FALSE);
	SetFontSize(fontHeight);
	DrawString(x + padding, y + padding, text.c_str(), Color::Blue());
}

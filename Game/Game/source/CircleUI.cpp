#include "CircleUI.h"
#include "ButtonUI.h"

const ButtonUI::ButtonRect& CircleUI::GetGaugeButtonRect() const
{
	return _circleButton;
}

bool CircleUI::Initialize(const std::string& dbPath, std::string* outError)
{
	// 円のマスターデータをロード
	if(!_sqliteCircle.Initialize(dbPath, outError))
	{
		return false;
	}
	// 最初のパターンをロードしてリセット
	Reset();
	return true;
}

void CircleUI::SetPattern(int minX, int maxX, int minY, int maxY, int radius, int count)
{
	_targetX = minX;
	_targetW = maxX - minX;
	_targetY = minY;
	_targetH = maxY - minY;
	_radius = radius;
	_spawnCount = count;
}

void CircleUI::SetPos(int x, int y, int w, int h)
{
	_circleButton.x = x;
	_circleButton.y = y;
	_circleButton.w = w;
	_circleButton.h = h;
}

void CircleUI::Reset()
{
	_isSuccess = false;
	_hasResult = false;
	_circles.clear();

	if(_sqliteCircle.RollrandomCircle())
	{
		CircleRow row;
		_sqliteCircle.GetCircle(row);
		SetPattern(row.minX, row.maxX, row.minY, row.maxY, row.radius, row.count);
	}

	for(int i = 0; i < _spawnCount; ++i)
	{
		CircleInstance c;

		// 指定された最小・最大範囲の中でランダム座標を計算
		c.x = _targetX + (rand() % (_targetW + 1));
		c.y = _targetY + (_targetH == 0 ? 0 : (rand() % (_targetH + 1)));

		c.radius = _radius;
		c.color = GetColor(255, 120, 120); // 弱点っぽい赤色（お好みで変えてください）
		c.isAlive = true;

		_circles.push_back(c);
	}
}

bool CircleUI::Update(MouseInput& mouse)
{
	// すでにゲームが終わっている（結果が出ている）なら何もしない
	if(_hasResult) return false;

	bool isHit = false;

	// 左クリックされた瞬間だけ当たり判定をチェック
	if(mouse.IsLeftTrig())
	{
		int mx = mouse.GetX();
		int my = mouse.GetY();

		// 後ろからループを回す（重なっていた場合、手前の円から消すため）
		for(auto it = _circles.rbegin(); it != _circles.rend(); ++it)
		{
			if(!it->isAlive) continue;

			// マウス座標と円の中心座標の距離の2乗を計算 (三平方の定理)
			int dx = mx - it->x;
			int dy = my - it->y;
			int distanceSq = (dx * dx) + (dy * dy);

			// 半径の2乗以下ならクリック成功！
			if(distanceSq <= (it->radius * it->radius))
			{
				it->isAlive = false; // 円を消す
				isHit = true;
				break; // 1クリックで1個だけ消す（貫通させない）
			}
		}

		// 画面上のすべての円が消されたかどうかをチェック
		bool allCleared = true;
		for(const auto& c : _circles)
		{
			if(c.isAlive)
			{
				allCleared = false;
				break;
			}
		}

		// 全て消えていたらフェーズ成功として終了
		if(allCleared)
		{
			_isSuccess = true;
			_hasResult = true;
		}
	}

	return isHit;
}

void CircleUI::Draw()
{
	// 生成範囲の枠を薄くデバッグ表示したい場合はここを有効にしてください
	DrawBox(_circleButton.x, _circleButton.y, _circleButton.x + _circleButton.w, _circleButton.y + _circleButton.h, GetColor(100,100,100), FALSE);

	// 生きている円だけを順番に描画する
	for(const auto& c : _circles)
	{
		if(c.isAlive)
		{
			// 塗りつぶしの円
			DrawCircle(c.x, c.y, c.radius, c.color, TRUE);
			// 円の外枠（黒）
			DrawCircle(c.x, c.y, c.radius, GetColor(0, 0, 0), FALSE);
		}
	}

	// 成功時の文字表示（確認用）
	if(_hasResult && _isSuccess)
	{
		SetFontSize(32);
		DrawString(_circleButton.x + 50, _circleButton.y + 150, "CLEAR!!", GetColor(0, 255, 0));
	}
}
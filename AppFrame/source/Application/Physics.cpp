#define _USE_MATH_DEFINES
#include "Physics.h"
#include <cmath>
#include "DxLib.h" 

Physics::Physics()
{
    Initialize();
}

Physics::~Physics()
{

}

void Physics::Initialize()
{
    // 物理パラメータの初期化
    _gravity = 0.3f;
    _airResistance = 0.01f;
    _frictionCoefficient = 0.0f;
}

// 衝突時の摩擦を適用（新規追加）
VECTOR Physics::ApplyCollisionFriction(VECTOR velocity, VECTOR normal, float frictionCoefficient)
{
    // 法線ベクトルを正規化
    normal = VNorm(normal);

    // 速度を法線方向と接線方向に分解
    float normalComponent = VDot(velocity, normal);
    VECTOR normalVelocity = VScale(normal, normalComponent);
    VECTOR tangentialVelocity = VSub(velocity, normalVelocity);

    // 接線方向（水平方向）に摩擦を適用
    float tangentialSpeed = VSize(tangentialVelocity);

	// 摩擦力の計算
    if (tangentialSpeed > 0.001f) // 十分な速度がある場合のみ摩擦適用
    {
        // 摩擦による速度減衰
        float frictionReduction = 1.0f - frictionCoefficient;
        if (frictionReduction < 0.0f) frictionReduction = 0.0f;

		// 接線速度を減衰させる
        tangentialVelocity = VScale(tangentialVelocity, frictionReduction);
    }

    // 法線方向と接線方向を再合成
    return VAdd(normalVelocity, tangentialVelocity);
}

// 卵形の反射計算
VECTOR Physics::CalculateEggReflection(VECTOR velocity, VECTOR normal, VECTOR eggOrientation,
    float eggMajorAxis, float eggMinorAxis, float eggAsymmetry, float cor)
{
    // 卵の長軸方向（縦方向）
    VECTOR eggUp = VNorm(eggOrientation);

    // 卵の短軸方向を計算（長軸に垂直な平面内）
    VECTOR temp = VGet(1, 0, 0);

    // 長軸とX軸とがほぼ平行な場合はY軸を使用
    if (fabs(VDot(eggUp, temp)) > 0.9f)
    {
        temp = VGet(0, 0, 1);
    }

	VECTOR eggRight = VNorm(VCross(eggUp, temp));       // 卵の横方向
	VECTOR eggForward = VNorm(VCross(eggRight, eggUp)); // 卵の前方向

    // 法線ベクトルを卵の局所座標系に変換
	float normalDotUp = VDot(normal, eggUp);            // 長軸方向
	float normalDotRight = VDot(normal, eggRight);      // 短軸横方向
	float normalDotForward = VDot(normal, eggForward);  // 短軸前方向

    // 衝突位置の判定（卵のどの部分に当たったか）
    float impactPosition = normalDotUp;

    // 反発係数の調整
	float longitudinalCor = eggMajorAxis;// 長軸方向の反発係数
	float lateralCor = eggMinorAxis;     // 短軸方向の反発係数

    // 非対称性の考慮
    float asymmetryCor = 1.0f;

	// 衝突位置に基づく非対称反発係数の調整
    if (impactPosition < 0)
    {
        // 尖った部分に当たった場合（強反発）
        asymmetryCor = 1.0f - eggAsymmetry;
    }
    else
    {
        // 丸い部分に当たった場合（弱反発）
        asymmetryCor = 1.0f + eggAsymmetry * 0.5f;
    }

    // 速度ベクトルを成分に分解
	float velocityAlongUp = VDot(velocity, eggUp);          // 長軸方向
	float velocityAlongRight = VDot(velocity, eggRight);    // 短軸横方向
	float velocityAlongForward = VDot(velocity, eggForward);// 短軸前方向

    // 法線方向の反射を反発係数で計算
	float reflectedUp = -velocityAlongUp * normalDotUp * longitudinalCor * asymmetryCor;// 長軸方向
	float reflectedRight = -velocityAlongRight * normalDotRight * lateralCor;           // 短軸横方向
	float reflectedForward = -velocityAlongForward * normalDotForward * lateralCor;     // 短軸前方向

    // 接触時摩擦（空気による減衰）
	float frictionFactor = 1.0f - _frictionCoefficient; // 摩擦係数に基づく減衰率
	float tangentialUp = velocityAlongUp * (1.0f - normalDotUp * normalDotUp) * frictionFactor;                     // 長軸方向
	float tangentialRight = velocityAlongRight * (1.0f - normalDotRight * normalDotRight) * frictionFactor;         // 短軸横方向
	float tangentialForward = velocityAlongForward * (1.0f - normalDotForward * normalDotForward) * frictionFactor; // 短軸前方向

    // 反射後の速度ベクトルを再構築
	VECTOR reflectedVelocity = VGet(0, 0, 0);  // 初期化                                     
	reflectedVelocity = VAdd(reflectedVelocity, VScale(eggUp, reflectedUp + tangentialUp));                 // 長軸方向
	reflectedVelocity = VAdd(reflectedVelocity, VScale(eggRight, reflectedRight + tangentialRight));        // 短軸横方向
	reflectedVelocity = VAdd(reflectedVelocity, VScale(eggForward, reflectedForward + tangentialForward));  // 短軸前方向

    // 全体的な減衰を適用
    reflectedVelocity = VScale(reflectedVelocity, cor);

    return reflectedVelocity;
}

// 卵形の反発係数を取得
float Physics::GetEggRestitutionCoefficient(VECTOR normal, VECTOR eggOrientation,
    float eggMajorAxis, float eggMinorAxis)
{
    // 法線と卵の向きの関係から反発係数を動的に計算
    float alignment = fabs(VDot(normal, eggOrientation));

    // 長軸方向に近い場合は衝撃が強く、短軸方向に近い場合は緩め
    return eggMinorAxis * alignment + eggMajorAxis * (1.0f - alignment);
}

// ラグビーボール専用の反射計算（改良版）
VECTOR Physics::CalculateRugbyBallReflection(VECTOR velocity, VECTOR normal, VECTOR ballOrientation,
    float ballLength, float ballWidth, float cor)
{
    // ラグビーボールの長軸方向（主軸）
    VECTOR ballAxis = VNorm(ballOrientation);

    // ラグビーボールの短軸方向を計算
    VECTOR temp = VGet(1, 0, 0);
    if (fabs(VDot(ballAxis, temp)) > 0.9f)
    {
        temp = VGet(0, 1, 0);
    }

	// 2つの直交する短軸方向ベクトルを取得
    VECTOR ballSide1 = VNorm(VCross(ballAxis, temp));
    VECTOR ballSide2 = VNorm(VCross(ballSide1, ballAxis));

    // 法線ベクトルをボールの局所座標系に変換
    float normalDotAxis = VDot(normal, ballAxis);
    float normalDotSide1 = VDot(normal, ballSide1);
    float normalDotSide2 = VDot(normal, ballSide2);

    // より詳細な衝突位置の判定
    float impactRatioLongitudinal = fabs(normalDotAxis);  // 長軸方向の衝突強度
    float impactRatioLateral = sqrt(normalDotSide1 * normalDotSide1 + normalDotSide2 * normalDotSide2);  // 側面の衝突強度

    // ボールの楕円形状における衝突位置を特定
    // 0.0 = 完全な側面, 1.0 = 完全な先端
    float collisionPosition = impactRatioLongitudinal;

    // ラグビーボールの形状特性を反映した反発係数
    float endTipCor = cor * 0.6f;       // 先端部：最も弱い反発
    float midSectionCor = cor * 1.15f;  // 中央部：最も強い反発
    float quarterCor = cor * 0.95f;     // 1/4地点：中程度の反発

    // 位置に応じた反発係数を段階的に計算
    float effectiveCor;
    if (collisionPosition > 0.85f)
    {
        // 先端部 (85%-100%)：最も弱い反発
        effectiveCor = endTipCor;
    }
    else if (collisionPosition > 0.6f)
    {
        // 先端寄り (60%-85%)：先端と1/4地点の中間
        float blend = (collisionPosition - 0.6f) / 0.25f;
        effectiveCor = quarterCor * (1.0f - blend) + endTipCor * blend;
    }
    else if (collisionPosition > 0.4f)
    {
        // 1/4地点 (40%-60%)：中程度の反発
        effectiveCor = quarterCor;
    }
    else if (collisionPosition > 0.15f)
    {
        // 中央寄り (15%-40%)：1/4地点と中央の中間
        float blend = (collisionPosition - 0.15f) / 0.25f;
        effectiveCor = midSectionCor * (1.0f - blend) + quarterCor * blend;
    }
    else
    {
        // 中央部 (0%-15%)：最も強い反発
        effectiveCor = midSectionCor;
    }

    // 楕円の長軸と短軸による異方性反射
    float longitudinalReflection = effectiveCor * 0.8f;  // 長軸方向は少し弱める
    float lateralReflection = effectiveCor * 1.0f;       // 短軸方向は標準

    // 速度ベクトルを成分に分解
    float velocityAlongAxis = VDot(velocity, ballAxis);
    float velocityAlongSide1 = VDot(velocity, ballSide1);
    float velocityAlongSide2 = VDot(velocity, ballSide2);

    // 反射計算（各軸方向）
    float reflectedAxis = -velocityAlongAxis * normalDotAxis * longitudinalReflection;
    float reflectedSide1 = -velocityAlongSide1 * normalDotSide1 * lateralReflection;
    float reflectedSide2 = -velocityAlongSide2 * normalDotSide2 * lateralReflection;

    // ラグビーボール特有のスピン効果を追加
    VECTOR spinEffect = CalculateRugbyBallSpinEffect(velocity, normal, ballOrientation, collisionPosition);

    //// 接線方向の速度成分（摩擦考慮）
    //float frictionFactor = 1.0f - _frictionCoefficient;
    //float tangentialAxis = velocityAlongAxis * (1.0f - normalDotAxis * normalDotAxis) * frictionFactor;
    //float tangentialSide1 = velocityAlongSide1 * (1.0f - normalDotSide1 * normalDotSide1) * frictionFactor;
    //float tangentialSide2 = velocityAlongSide2 * (1.0f - normalDotSide2 * normalDotSide2) * frictionFactor;

    // 反射後の速度ベクトルを再構築
    VECTOR reflectedVelocity = VGet(0, 0, 0);
    reflectedVelocity = VAdd(reflectedVelocity, VScale(ballAxis, reflectedAxis ));
    reflectedVelocity = VAdd(reflectedVelocity, VScale(ballSide1, reflectedSide1 ));
    reflectedVelocity = VAdd(reflectedVelocity, VScale(ballSide2, reflectedSide2 ));

    // スピン効果を適用
    reflectedVelocity = VAdd(reflectedVelocity, spinEffect);

    // 位置依存のエネルギー損失
    float energyLoss = 0.98f - (collisionPosition * 0.15f);     // 先端ほどエネルギー損失が大きい
    reflectedVelocity = VScale(reflectedVelocity, energyLoss);

    return reflectedVelocity;
}

// ラグビーボールのスピン効果を計算（改良版）
VECTOR Physics::CalculateRugbyBallSpinEffect(VECTOR velocity, VECTOR normal, VECTOR ballOrientation, float impactRatio)
{
    // ラグビーボールの長軸
    VECTOR ballAxis = VNorm(ballOrientation);

    // 入射速度と法線の関係から回転方向を決定
    VECTOR incidentDirection = VNorm(velocity);
    VECTOR rotationAxis = VNorm(VCross(incidentDirection, normal));

    // 衝突位置によるスピン強度の詳細計算
    float baseSpinIntensity = 0.05f;

    // 位置によるスピン強度調整
    float positionSpinFactor;
    if (impactRatio > 0.7f)
    {
        // 先端部：非常に強いスピン
        positionSpinFactor = 1.2f + (impactRatio - 0.7f) * 1.5f;
    }
    else if (impactRatio > 0.3f)
    {
        // 中間部：中程度のスピン
        positionSpinFactor = 0.8 + (impactRatio - 0.3f) * 1.0f;
    }
    else
    {
        // 中央部：最小のスピン
        positionSpinFactor = 0.3f + impactRatio * 1.0f;
    }

	// 最終的なスピン強度
    float spinIntensity = baseSpinIntensity * positionSpinFactor;

    // ボールの形状による非対称回転効果
	float asymmetryFactor = VDot(ballAxis, rotationAxis);               // -1.0f から 1.0f
	float asymmetryMultiplier = 1.0f + fabs(asymmetryFactor) * 0.3f;    // 最大30%増加

    spinIntensity *= asymmetryMultiplier;

    // 入射角度による調整（斜め衝突ほどスピンが強い）
	float incidenceAngle = fabs(VDot(incidentDirection, normal));   // 0 = 平行, 1 = 直角
	float angleSpinFactor = 1.0f + (1.0f - incidenceAngle) * 0.5f;  // 斜め衝突で最大1.5倍

    spinIntensity *= angleSpinFactor;

    // スピンによる追加速度（より現実的な計算）
    VECTOR spinVelocity = VScale(rotationAxis, spinIntensity * VSize(velocity) * 0.05f);

    // ラグビーボールの長軸方向のタンブリング効果
	VECTOR tumblingAxis = VNorm(VCross(ballAxis, normal));                                      // 長軸に垂直な回転軸
	float tumblingIntensity = spinIntensity * 0.15f * impactRatio;                              // 先端ほど強いタンブリング
	VECTOR tumblingEffect = VScale(tumblingAxis, tumblingIntensity * VSize(velocity) * 0.02f);  // 微小な速度変化

    return VAdd(spinVelocity, tumblingEffect);
}

// 重力を適用
VECTOR Physics::ApplyGravity(VECTOR velocity, float mass, float gravity)
{
    // 重力加速度を適用
	velocity.y -= mass * gravity;   
    return velocity;
}

// 通常の反射計算
VECTOR Physics::CalculateNormalReflection(VECTOR velocity, VECTOR normal, float cor)
{
    // 標準の反射ベクトル計算: R = V - 2(V・N)N
    float dot = VDot(velocity, normal);
    VECTOR reflection = VScale(normal, 2.0f * dot);
    VECTOR reflectedVelocity = VSub(velocity, reflection);

    // 反発係数を適用
    reflectedVelocity = VScale(reflectedVelocity, cor);

    // 衝突時の摩擦を適用
    reflectedVelocity = ApplyCollisionFriction(reflectedVelocity, normal, _frictionCoefficient);

    return reflectedVelocity;
}

// 発射角度を計算（カメラの向きベクトルから）
float Physics::CalculateLaunchAngle(VECTOR cameraDirection)
{
    // カメラ方向ベクトルを正規化
    cameraDirection = VNorm(cameraDirection);

    // 水平方向の成分を計算
    float horizontalDistance = sqrt(cameraDirection.x * cameraDirection.x + cameraDirection.z * cameraDirection.z);

    // 発射角度を計算（ラジアン）
    float launchAngle = atan2(cameraDirection.y, horizontalDistance);

    return launchAngle;
}

// 放物運動に適した初期速度ベクトルを計算
VECTOR Physics::CalculateProjectileVelocity(VECTOR direction, float launchAngle, float initialSpeed)
{
    // カメラ方向ベクトルを正規化
    direction = VNorm(direction);

    // 水平方向の単位ベクトルを計算
    VECTOR horizontalDir = VGet(direction.x, 0, direction.z);
    float horizontalLength = VSize(horizontalDir);

	// 水平方向ベクトルの正規化
    if (horizontalLength > 0.001f)
    {
        horizontalDir = VScale(horizontalDir, 1.0f / horizontalLength);
    }
    else
    {
        // 真上または真下の場合のデフォルト方向（カメラの向きに基づく）
        // Y軸周りの回転を考慮してデフォルト方向を設定
        horizontalDir = VGet(1, 0, 0);
    }

    // 物理学的に正確な初期速度成分の計算
    // v0 * cos(θ) = 水平方向初速度, v0 * sin(θ) = 垂直方向初速度
	float horizontalSpeed = initialSpeed * cos(launchAngle);// 水平方向速度
	float verticalSpeed = initialSpeed * sin(launchAngle);  // 垂直方向速度

    //// 空気抵抗を考慮した初期速度補正（オプション）
    //float airResistanceCompensation = 1.0f + (_airResistance * 0.1f);
    //horizontalSpeed *= airResistanceCompensation;
    //verticalSpeed *= airResistanceCompensation;

    // 3次元速度ベクトルを構築
    VECTOR velocity = VGet(
        horizontalDir.x * horizontalSpeed,
        verticalSpeed,
        horizontalDir.z * horizontalSpeed
    );

    return velocity;
}

// 最適発射角度を計算（最大飛距離を得るため）
float Physics::CalculateOptimalLaunchAngle(float targetDistance, float initialSpeed, float targetHeight)
{
    // 重力加速度
    float g = _gravity;

    // 目標高度差を考慮した最適角度の計算
    // θ = arctan((v²±√(v⁴-g(gx²+2yv²)))/(gx))
    float v2 = initialSpeed * initialSpeed;
    float v4 = v2 * v2;
    float gx2 = g * targetDistance * targetDistance;
    float discriminant = v4 - g * (gx2 + 2 * targetHeight * v2);

	// 判別式が負の場合、到達不可能
    if (discriminant < 0)
    {
        // 到達不可能な場合は45度を返す
        return M_PI / 4.0f;
    }

	// 最適角度の計算（低い軌道を選択）
    float optimalAngle = atan((v2 - sqrt(discriminant)) / (g * targetDistance));

    // 角度制限（-89度から89度まで）
    float maxAngle = (89.0f * M_PI) / 180.0f;
    if (optimalAngle > maxAngle) optimalAngle = maxAngle;
    if (optimalAngle < -maxAngle) optimalAngle = -maxAngle;

    return optimalAngle;
}

// 弾道軌跡の予測計算
VECTOR Physics::PredictTrajectoryPoint(VECTOR initialPos, VECTOR initialVel, float time)
{
    // x = x₀ + v₀ₓt
    // y = y₀ + v₀yt - (1/2)gt²
    // z = z₀ + v₀zt

	VECTOR predictedPos;    // 予測位置ベクトル
	predictedPos.x = initialPos.x + initialVel.x * time;                                // x座標
	predictedPos.y = initialPos.y + initialVel.y * time - 0.5f * _gravity * time * time;// y座標
	predictedPos.z = initialPos.z + initialVel.z * time;                                // z座標

    return predictedPos;
}

// 着弾時間を計算
float Physics::CalculateLandingTime(VECTOR initialVel, float initialHeight)
{
    // y = v₀yt - (1/2)gt² = 0 となる時間を求める
    // t = (v₀y + √(v₀y² + 2gh)) / g

	float vy = initialVel.y;    // 初速度のy成分
	float g = _gravity;         // 重力加速度
	float h = initialHeight;    // 初期高度

	float discriminant = vy * vy + 2 * g * h;   // 判別式
    if (discriminant < 0) return 0.0f;

    return (vy + sqrt(discriminant)) / g;
}

// 位置エネルギー計算
float Physics::CalculatePotentialEnergy(float mass, float gravity, float height)
{
	// 位置エネルギー：E = mgh
    return mass * gravity * height;
}

// 運動エネルギー計算
float Physics::CalculateKineticEnergy(float mass, VECTOR velocity)
{
	// 運動エネルギー：E = 1/2 * m * v^2
    float speed = VSize(velocity);
    return 0.5f * mass * speed * speed;
}

// 力から速度ベクトルを計算
VECTOR Physics::CalculateVelocityFromForce(VECTOR direction, float force)
{
	// 方向ベクトルを正規化して力をスケーリング
    direction = VNorm(direction);
    return VScale(direction, force);
}

// 物理パラメータ設定
void Physics::SetPhysicsParameters(float gravity, float airResistance)
{
	_gravity = gravity;             // 重力加速度設定
	_airResistance = airResistance; // 空気抵抗設定
}

// ばね力の計算（フックの法則 + 減衰）
VECTOR Physics::CalculateSpringForce(VECTOR currentPos, VECTOR anchorPos, float springConstant,
    float restLength, float dampingCoeff, VECTOR velocity)
{
    // ばねの変位ベクトルを計算
    VECTOR displacement = VSub(currentPos, anchorPos);
    float currentLength = VSize(displacement);

    // 長さが非常に小さい場合は力なし
    if (currentLength < 0.001f)
    {
        return VGet(0, 0, 0);
    }

    // ばねの伸び/縮み量
    float extension = currentLength - restLength;

    // フックの法則：F = -k * x（復元力）
    VECTOR direction = VNorm(displacement);
    VECTOR springForce = VScale(direction, -springConstant * extension);

    // 減衰力：F_damping = -c * v（速度に比例した抵抗）
    VECTOR dampingForce = VScale(velocity, -dampingCoeff);

    // 合成力を返す
    return VAdd(springForce, dampingForce);
}

// ばねの位置エネルギー計算
float Physics::CalculateSpringPotentialEnergy(VECTOR currentPos, VECTOR anchorPos,
    float springConstant, float restLength)
{
    VECTOR displacement = VSub(currentPos, anchorPos);
    float currentLength = VSize(displacement);
    float extension = currentLength - restLength;

    // ばねの位置エネルギー：E = 1/2 * k * x^2
    return 0.5f * springConstant * extension * extension;
}

// ばね効果を含む反射計算
VECTOR Physics::ApplySpringReflection(VECTOR velocity, VECTOR normal, float cor,
    float springEnergy, float springBounceFactor)
{
    // 通常の反射計算
    VECTOR reflectedVel = CalculateNormalReflection(velocity, normal, cor);

    // ばねエネルギーに基づく追加の反発力
    if (springEnergy > 0.1f)
    {
        VECTOR springBounce = VScale(normal, springEnergy * springBounceFactor);
        reflectedVel = VAdd(reflectedVel, springBounce);
    }

    return reflectedVel;
}
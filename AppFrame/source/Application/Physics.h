#pragma once
#include "DxLib.h"

// 物理計算とゲージ機能を管理するクラス
class Physics
{
public:
    Physics();
    virtual ~Physics();

    // 初期化
    virtual void Initialize();

    // 物理関連のメソッド
    VECTOR CalculateEggReflection(VECTOR velocity, VECTOR normal, VECTOR eggOrientation,
        float eggMajorAxis, float eggMinorAxis, float eggAsymmetry, float cor);
    float GetEggRestitutionCoefficient(VECTOR normal, VECTOR eggOrientation,
        float eggMajorAxis, float eggMinorAxis);
    VECTOR ApplyGravity(VECTOR velocity, float mass, float gravity);
    VECTOR CalculateNormalReflection(VECTOR velocity, VECTOR normal, float cor);

    // 摩擦関連のメソッド
    VECTOR ApplyCollisionFriction(VECTOR velocity, VECTOR normal, float frictionCoefficient);

    // ばね計算関連のメソッド
    VECTOR CalculateSpringForce(VECTOR currentPos, VECTOR anchorPos, float springConstant,
        float restLength, float dampingCoeff, VECTOR velocity);
    float CalculateSpringPotentialEnergy(VECTOR currentPos, VECTOR anchorPos,
        float springConstant, float restLength);
    VECTOR ApplySpringReflection(VECTOR velocity, VECTOR normal, float cor,
        float springEnergy, float springBounceFactor);

    // 力とエネルギー計算
	float CalculatePotentialEnergy(float mass, float gravity, float height);    // 位置エネルギー
	float CalculateKineticEnergy(float mass, VECTOR velocity);                  // 運動エネルギー
	VECTOR CalculateVelocityFromForce(VECTOR direction, float force);           // 力から速度ベクトルを計算

    // 放物運動用
    VECTOR CalculateProjectileVelocity(VECTOR direction, float launchAngle, float initialSpeed);
    float CalculateLaunchAngle(VECTOR cameraDirection);

    // 放物運動用の追加メソッド
    float CalculateOptimalLaunchAngle(float targetDistance, float initialSpeed, float targetHeight = 0.0f);
    VECTOR PredictTrajectoryPoint(VECTOR initialPos, VECTOR initialVel, float time);
    float CalculateLandingTime(VECTOR initialVel, float initialHeight);

    // 設定関連
    void SetPhysicsParameters(float gravity, float airResistance);
    void SetFrictionCoefficient(float friction) { _frictionCoefficient = friction; }

    // ラグビーボール用の反射計算
    VECTOR CalculateRugbyBallReflection(VECTOR velocity, VECTOR normal, VECTOR ballOrientation,
        float ballLength, float ballWidth, float cor);

    // ラグビーボールのスピン効果を計算
    VECTOR CalculateRugbyBallSpinEffect(VECTOR velocity, VECTOR normal, VECTOR ballOrientation, float impactRatio);

protected:
    // 物理パラメータ
    float _gravity;             // 重力加速度
    float _airResistance;       // 空気抵抗
    float _frictionCoefficient; // 摩擦係数
};
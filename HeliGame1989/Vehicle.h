#pragma once

#include "pch.h"
#include "Environment.h"
#include "DebugData.h"
#include "Utility.h"

struct ControlInput
{
    // input control data
    const float inputDeadZone = 0.000000001;  // small deadzone to ignore nominal control input

    float       collectiveInput;
    const float collectiveInputMax = 1.0f;
    const float collectiveInputMin = 0.0f;
    const float collectiveInputRate = 0.5f;
    const float collectiveInputRateGamePad = 0.2f;

    DirectX::SimpleMath::Vector3 cyclicStick;
    const float cyclicDecayRate = 0.3f;
    const float cyclicDecayRateGamePad = 0.3f;
    float       cyclicInputPitch;
    bool        cyclicInputPitchIsPressed;
    float       cyclicInputRoll;
    bool        cyclicInputRollIsPressed;
    const float cyclicInputMax = Utility::ToRadians(20.0f);
    const float cyclicInputMin = -Utility::ToRadians(20.0f);
    const float cyclicInputRate = 0.1f;
    const float cyclicInputRateGamePad = 0.6f;

    float       throttleInput;
    const float throttleInputMin = 0.0f;
    const float throttleInputMax = 1.0f;
    const float throttleInputRate = 0.2f;
    const float throttleInputRateGamePad = 0.6f; //0.4f;

    bool        yawPedalIsPressed;
    float       yawPedalInput;
    const float yawPedalDecayRate = 0.8f;
    const float yawPedalDecayRateGamePad = 0.5f;
    const float yawPedalInputMax = 1.0f;
    const float yawPedalInputMin = -1.0f;
    const float yawPedalInputRate = 0.15f;
    const float yawPedalInputRateGamePad = 0.1f;
};

struct Engine
{
    float currentTorque;
    float gearRatio;
    float powerCurveLow;
    float powerCurveMid;
    float powerCurveHigh;
    float revLimit;
    float rpm;
    float torquePointLow;
    float torquePointMid;
    float torquePointHigh;
};

struct LandingGear
{
    const float angleAtDown = Utility::ToRadians(-110.0f);
    const float angleAtUp = Utility::ToRadians(15.0f);  
    float angleCurrent = angleAtDown;
    const float angleDelta = Utility::ToRadians(30.0f);

    enum class LandingGearState
    {
        LANDINGGEARSTATE_ASCENDING,
        LANDINGGEARSTATE_DESCENDING,
        LANDINGGEARSTATE_DOWN,
        LANDINGGEARSTATE_UP,
    };
    LandingGearState currentState = LandingGearState::LANDINGGEARSTATE_DOWN;
};

struct Motion
{
    DirectX::SimpleMath::Vector3 airResistance;
    DirectX::SimpleMath::Vector3 bodyVelocity;
    DirectX::SimpleMath::Vector3 engineForce;

    DirectX::SimpleMath::Vector3 mainRotorForceNormal;
    float                        mainRotorForceMagnitude;
    DirectX::SimpleMath::Vector3 position;
  
    float                        tailRotorForceMagnitude;
    DirectX::SimpleMath::Vector3 tailRotorForceNormal;

    DirectX::SimpleMath::Vector3 totalVelocity;
    DirectX::SimpleMath::Vector3 velocity;

    Utility::Torque              bodyTorqueForce;
    float                        bodyTorqueMagnitude;
    DirectX::SimpleMath::Vector3 bodyTorqueVec;

    Utility::Torque              pendulumTorqueForceTest;

    DirectX::SimpleMath::Vector3 parabolicMomentum;

    DirectX::SimpleMath::Vector3 angularVelocity = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 angularMomentum = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Quaternion angularQuat = DirectX::SimpleMath::Quaternion::Identity;
};

struct Rotor
{
    int bladeCount;
    float angleBetweenBlades;
    float height;
    float inputPitchAngleMax;
    float inputPitchAngleMin;
    float length;
    float neutralAngle;
    float pitchAngleMax;
    float pitchAngleMin;
    float radius;
    float rpm;
    float rotorRotation;
    float width;

    struct RotorBlade
    {
        float pitchAngle;
        float cyclicAngle;   
        float liftForcePerSecond;
        float liftMag;
        DirectX::SimpleMath::Vector3 liftNorm;
        float rotationAngle;
    };
    std::vector<RotorBlade> bladeVec;

    enum class BladeType
    {
        BLADETYPE_MAINROTOR,
        BLADETYPE_TAILROTOR,
    };
    BladeType bladeType;
};

struct HeliData
{
    // rotor data
    const float mainRotorForceMagMax = 15.0f;
    const float mainRotorForceMagMin = 0.0f;
    const float tailRotorForceMagMax = 2000.0f; // vehicle mass value should work here
    DirectX::SimpleMath::Vector3 mainRotorPos;
    DirectX::SimpleMath::Vector3 localMainRotorPos;
    DirectX::SimpleMath::Vector3 tailRotorPos;
    DirectX::SimpleMath::Vector3 localTailRotorPos;
    DirectX::SimpleMath::Vector3 centerOfMass;
    DirectX::SimpleMath::Vector3 localCenterOfMass;
    DirectX::SimpleMath::Vector3 landingGearPos;
    DirectX::SimpleMath::Vector3 localLandingGearPos;
    DirectX::SimpleMath::Vector3 gravityTorqueArmPos;
    DirectX::SimpleMath::Vector3 localGravityTorqueArmPos;

    //
    int     numEqns;
    double  time;
    const float   areaFront = 14.67f;   
    const float   areaTailAndBoom = 9.0f;
    float   airResistance;
    float   airDensity;
    float   altitude;
    const float   dragCoefficient = 0.28f;
    const DirectX::SimpleMath::Vector3 gravity = DirectX::SimpleMath::Vector3(0.0f, -9.80665f, 0.0f);
    DirectX::SimpleMath::Plane groundPlane;
    const float   mainRotorMaxRPM = 500.0f;
    const float   mass = 2000.0f;

    Motion  q;
    float   totalResistance;
    //
    float   speed;                   // speed vehicle is traveling
    bool    isVehicleAirborne; 
    bool    isVehicleLanding;
    bool    isVelocityBackwards;
    DirectX::SimpleMath::Vector3 terrainNormal;
    DirectX::SimpleMath::Vector3 forward;
    DirectX::SimpleMath::Vector3 up;
    DirectX::SimpleMath::Vector3 right;
    DirectX::SimpleMath::Matrix alignment;
    DirectX::SimpleMath::Matrix inverseAlignment;
    DirectX::SimpleMath::Quaternion alignmentQuat;
    DirectX::SimpleMath::Quaternion inverseAlignmentQuat;
    DirectX::SimpleMath::Matrix cameraOrientation;
    DirectX::SimpleMath::Matrix cameraOrientationPrevious;
    float   terrainHightAtPos;

    float   testAccel = 0.0;
    DirectX::SimpleMath::Vector3 testAccelVec = DirectX::SimpleMath::Vector3::Zero;

    DirectX::SimpleMath::Vector3 centrifugalForce = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 centrifugalForcePrevFrame = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Quaternion centrifugalForcePrevQuatFrame = DirectX::SimpleMath::Quaternion::Identity;

    DirectX::SimpleMath::Vector3 angularDrag = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 angularDrag2 = DirectX::SimpleMath::Vector3::Zero;

    DirectX::SimpleMath::Matrix inertiaMatrix;
    DirectX::SimpleMath::Matrix inverseInertiaMatrix;
    DirectX::SimpleMath::Matrix localInertiaMatrix;
    DirectX::SimpleMath::Matrix localInverseInertiaMatrix;

    ControlInput  controlInput;
    Engine        engine;
    LandingGear   landingGear;
    Rotor         mainRotor;
    Rotor         tailRotor;
};

struct HeliModel
{
    // Colors for model parts
    DirectX::SimpleMath::Vector4 axelColor;
    DirectX::SimpleMath::Vector4 bodyColor;
    DirectX::SimpleMath::Vector4 rotorColor;
    DirectX::SimpleMath::Vector4 rotorStripeColor;
    DirectX::SimpleMath::Vector4 rotorStripeAltColor;
    DirectX::SimpleMath::Vector4 stripeColor0;
    DirectX::SimpleMath::Vector4 stripeColor1;
    DirectX::SimpleMath::Vector4 landingGearTireColor;
    DirectX::SimpleMath::Vector4 landingGearWheelColor;
    DirectX::SimpleMath::Vector4 landingGearArmColor;
    DirectX::SimpleMath::Vector4 windshieldColor;
    DirectX::SimpleMath::Vector4 exhaustColor;
    DirectX::SimpleMath::Vector4 shadowColor;
    DirectX::SimpleMath::Vector4 undersideColor;
    DirectX::SimpleMath::Vector4 toothColor;
    DirectX::SimpleMath::Vector4 testColor1;
    DirectX::SimpleMath::Vector4 testColor2;
    DirectX::SimpleMath::Vector4 testColor3;

    // tessellation values for rounded shapes
    const int circleTessellationVal1 = 6;
    const int circleTessellationVal2 = 32;

    // part shape and local positions
    std::unique_ptr<DirectX::GeometricPrimitive>    noseConeShape;
    DirectX::SimpleMath::Matrix noseConeMatrix;
    DirectX::SimpleMath::Matrix localNoseConeMatrix;
    DirectX::SimpleMath::Matrix noseConeBellyMatrix;
    DirectX::SimpleMath::Matrix localNoseConeBellyMatrix;

    std::unique_ptr<DirectX::GeometricPrimitive>    noseBodyShape;
    DirectX::SimpleMath::Matrix noseBodyMatrix;
    DirectX::SimpleMath::Matrix localNoseBodyMatrix;
    DirectX::SimpleMath::Matrix noseBodyBellyMatrix;
    DirectX::SimpleMath::Matrix localNoseBodyBellyMatrix;

    std::unique_ptr<DirectX::GeometricPrimitive>    bodyShape;
    DirectX::SimpleMath::Matrix bodyMatrix;
    DirectX::SimpleMath::Matrix localBodyMatrix;
    DirectX::SimpleMath::Matrix bodyBellyMatrix;
    DirectX::SimpleMath::Matrix localBodyBellyMatrix;

    std::unique_ptr<DirectX::GeometricPrimitive>    doorShape;
    DirectX::SimpleMath::Matrix doorMatrix;
    DirectX::SimpleMath::Matrix localDoorMatrix;

    std::unique_ptr<DirectX::GeometricPrimitive>    sideStripeShape0;
    DirectX::SimpleMath::Matrix sideStripeMatrix0;
    DirectX::SimpleMath::Matrix localSidestripeMatrix0;
    std::unique_ptr<DirectX::GeometricPrimitive>    sideStripeShape1;
    DirectX::SimpleMath::Matrix sideStripeMatrix1;
    DirectX::SimpleMath::Matrix localSidestripeMatrix1;
    std::unique_ptr<DirectX::GeometricPrimitive>    sideStripeShape2;
    DirectX::SimpleMath::Matrix sideStripeMatrix2;
    DirectX::SimpleMath::Matrix localSidestripeMatrix2;
    std::unique_ptr<DirectX::GeometricPrimitive>    sideStripeShape3;
    DirectX::SimpleMath::Matrix sideStripeMatrix3;
    DirectX::SimpleMath::Matrix localSidestripeMatrix3;
    std::unique_ptr<DirectX::GeometricPrimitive>    sideStripeShape4;
    DirectX::SimpleMath::Matrix sideStripeMatrix4;
    DirectX::SimpleMath::Matrix localSidestripeMatrix4;
    std::unique_ptr<DirectX::GeometricPrimitive>    sideStripeShape5;
    DirectX::SimpleMath::Matrix sideStripeMatrix5;
    DirectX::SimpleMath::Matrix localSidestripeMatrix5;

    std::unique_ptr<DirectX::GeometricPrimitive>    landingGearTireShape;
    std::unique_ptr<DirectX::GeometricPrimitive>    landingGearWheelShape;
    std::unique_ptr<DirectX::GeometricPrimitive>    landingGearLowerArmShape;
    std::unique_ptr<DirectX::GeometricPrimitive>    landingGearUpperArmShape;
    DirectX::SimpleMath::Matrix landingGearTireMatrix;
    DirectX::SimpleMath::Matrix localLandingGearTireMatrix;
    DirectX::SimpleMath::Matrix landingGearWheelMatrix;
    DirectX::SimpleMath::Matrix localLandingGearWheelMatrix;
    DirectX::SimpleMath::Matrix landingGearLowerArmMatrix;
    DirectX::SimpleMath::Matrix localLandingGearLowerArmMatrix;
    DirectX::SimpleMath::Matrix landingGearUpperArmMatrix;
    DirectX::SimpleMath::Matrix localLandingGearUperArmMatrix;
    DirectX::SimpleMath::Matrix landingGearFrontMatrix;
    DirectX::SimpleMath::Matrix localLandingGearFrontMatrix;
    DirectX::SimpleMath::Matrix landingGearRearLeftMatrix;
    DirectX::SimpleMath::Matrix localLandingGearRearLeftMatrix;
    DirectX::SimpleMath::Matrix landingGearRearRightMatrix;
    DirectX::SimpleMath::Matrix localLandingGearRearRightMatrix;

    std::unique_ptr<DirectX::GeometricPrimitive>    bodyCapShape;
    DirectX::SimpleMath::Matrix windShieldMatrix;
    DirectX::SimpleMath::Matrix localWindShieldMatrix;
    DirectX::SimpleMath::Matrix bodyRearMatrix;
    DirectX::SimpleMath::Matrix localBodyRearMatrix;

    DirectX::SimpleMath::Matrix bodyRearBellyMatrix;
    DirectX::SimpleMath::Matrix localBodyRearBellyMatrix;

    std::unique_ptr<DirectX::GeometricPrimitive>    noseCowlShape;
    DirectX::SimpleMath::Matrix noseCowlMatrix;
    DirectX::SimpleMath::Matrix localNoseCowlMatrix;

    std::unique_ptr<DirectX::GeometricPrimitive>    toothShape;
    std::unique_ptr<DirectX::GeometricPrimitive>    toothShape2;
    DirectX::SimpleMath::Matrix toothMatrix1;
    DirectX::SimpleMath::Matrix localToothMatrix1;
    DirectX::SimpleMath::Matrix toothMatrix2;
    DirectX::SimpleMath::Matrix localToothMatrix2;
    DirectX::SimpleMath::Matrix toothMatrix3;
    DirectX::SimpleMath::Matrix localToothMatrix3;
    DirectX::SimpleMath::Matrix toothMatrix4;
    DirectX::SimpleMath::Matrix localToothMatrix4;
    DirectX::SimpleMath::Matrix toothMatrix5;
    DirectX::SimpleMath::Matrix localToothMatrix5;
    DirectX::SimpleMath::Matrix toothMatrix6;
    DirectX::SimpleMath::Matrix localToothMatrix6;
    DirectX::SimpleMath::Matrix toothMatrix7;
    DirectX::SimpleMath::Matrix localToothMatrix7;
    DirectX::SimpleMath::Matrix toothMatrix8;
    DirectX::SimpleMath::Matrix localToothMatrix8;
    DirectX::SimpleMath::Matrix toothMatrix9;
    DirectX::SimpleMath::Matrix localToothMatrix9;
    DirectX::SimpleMath::Matrix toothMatrix10;
    DirectX::SimpleMath::Matrix localToothMatrix10;
    DirectX::SimpleMath::Matrix toothMatrix11;
    DirectX::SimpleMath::Matrix localToothMatrix11;   
    DirectX::SimpleMath::Matrix toothMatrix12;
    DirectX::SimpleMath::Matrix localToothMatrix12;

    std::unique_ptr<DirectX::GeometricPrimitive>    toothBackdropShape;
    DirectX::SimpleMath::Matrix toothBackdropLeftMatrix;
    DirectX::SimpleMath::Matrix localToothBackdropLeftMatrix;
    DirectX::SimpleMath::Matrix toothBackdropRightMatrix;
    DirectX::SimpleMath::Matrix localToothBackdropRightMatrix;
    std::unique_ptr<DirectX::GeometricPrimitive>    toothBackdropTestShape;

    std::unique_ptr<DirectX::GeometricPrimitive>    mainWingShape;
    DirectX::SimpleMath::Matrix mainWingMatrix;
    DirectX::SimpleMath::Matrix localMainWingMatrix;
    DirectX::SimpleMath::Matrix mainWingBellyMatrix;
    DirectX::SimpleMath::Matrix localMainWingBellyMatrix;

    std::unique_ptr<DirectX::GeometricPrimitive>    mainWingLeadingEdgeShape;
    DirectX::SimpleMath::Matrix mainWingLeadingEdgeMatrix;
    DirectX::SimpleMath::Matrix localMainWingLeadingEdgeMatrix;
    DirectX::SimpleMath::Matrix mainWingLeadingEdgeBellyMatrix;
    DirectX::SimpleMath::Matrix localMainWingLeadingBellyEdgeMatrix;

    std::unique_ptr<DirectX::GeometricPrimitive>    mainWingTailEdgeShape;
    DirectX::SimpleMath::Matrix mainWingTailEdgeMatrix;
    DirectX::SimpleMath::Matrix localMainWingTailEdgeMatrix;
    DirectX::SimpleMath::Matrix mainWingTailEdgeBellyMatrix;
    DirectX::SimpleMath::Matrix localMainWingTailEdgeBellyMatrix;

    std::unique_ptr<DirectX::GeometricPrimitive>    sideWindowUpperShape;
    DirectX::SimpleMath::Matrix sideWindowUpperMatrix;
    DirectX::SimpleMath::Matrix localSideWindowUpperMatrix;

    std::unique_ptr<DirectX::GeometricPrimitive>    sideWindowLowerShape;
    DirectX::SimpleMath::Matrix sideWindowLowerMatrix;
    DirectX::SimpleMath::Matrix localSideWindowLowerMatrix;
    DirectX::SimpleMath::Matrix sideWindowBottomMatrix;
    DirectX::SimpleMath::Matrix localSideWindowBottomMatrix;

    std::unique_ptr<DirectX::GeometricPrimitive>    sideWindowEyeShape;
    DirectX::SimpleMath::Matrix sideWindowEyeMatrix;
    DirectX::SimpleMath::Matrix localSideWindowEyeMatrix;

    std::unique_ptr<DirectX::GeometricPrimitive>    wingJetCenterShape;
    DirectX::SimpleMath::Matrix wingJetCenterMatrix;
    DirectX::SimpleMath::Matrix localWingJetCenterMatrix;
    std::unique_ptr<DirectX::GeometricPrimitive>    wingJetForwardHousingShape;
    DirectX::SimpleMath::Matrix wingJetForwardHousingLeftMatrix;
    DirectX::SimpleMath::Matrix localWingJetForwardHousingLeftMatrix;
    DirectX::SimpleMath::Matrix wingJetForwardHousingLeftBellyMatrix;
    DirectX::SimpleMath::Matrix localWingJetForwardHousingLeftBellyMatrix;
    DirectX::SimpleMath::Matrix wingJetForwardHousingRightMatrix;
    DirectX::SimpleMath::Matrix localWingJetForwardHousingRightMatrix;

    std::unique_ptr<DirectX::GeometricPrimitive>    wingJetForwardCowlShape;
    DirectX::SimpleMath::Matrix wingJetForwardCowlLeftMatrix;
    DirectX::SimpleMath::Matrix localWingJetForwardCowlLeftMatrix;
    DirectX::SimpleMath::Matrix wingJetForwardCowlRightMatrix;
    DirectX::SimpleMath::Matrix localWingJetForwardCowlRightMatrix;

    std::unique_ptr<DirectX::GeometricPrimitive>    wingJetForwardIntakeShape;
    std::unique_ptr<DirectX::GeometricPrimitive>    wingJetExhaustShape;

    std::unique_ptr<DirectX::GeometricPrimitive>    wingJetCouplingShape;
    DirectX::SimpleMath::Matrix wingJetCouplingLeftMatrix;
    DirectX::SimpleMath::Matrix localWingJetCouplingLeftMatrix;
    DirectX::SimpleMath::Matrix wingJetCouplingRightMatrix;
    DirectX::SimpleMath::Matrix localWingJetCouplingRightMatrix;

    std::unique_ptr<DirectX::GeometricPrimitive>    wingJetRearHousingShape;
    DirectX::SimpleMath::Matrix wingJetRearHousingLeftMatrix;
    DirectX::SimpleMath::Matrix localWingJetRearHousingLeftMatrix;
    DirectX::SimpleMath::Matrix wingJetRearHousingRightMatrix;
    DirectX::SimpleMath::Matrix localWingJetRearHousingRightMatrix;

    std::unique_ptr<DirectX::GeometricPrimitive>    wingJetExhaustTipShape;
    DirectX::SimpleMath::Matrix wingJetExhaustTipLeftMatrix;
    DirectX::SimpleMath::Matrix localWingJetExhaustTipLeftMatrix;
    DirectX::SimpleMath::Matrix wingJetExhaustTipRightMatrix;
    DirectX::SimpleMath::Matrix localWingJetExhaustTipRightMatrix;

    std::unique_ptr<DirectX::GeometricPrimitive>    sidePodShape;
    DirectX::SimpleMath::Matrix sidePodMatrix;
    DirectX::SimpleMath::Matrix localSidePodMatrix;

    std::unique_ptr<DirectX::GeometricPrimitive>    engineHousingShape;
    DirectX::SimpleMath::Matrix engineHousingMatrix;
    DirectX::SimpleMath::Matrix localEngineHousingMatrix;

    std::unique_ptr<DirectX::GeometricPrimitive>    engineHousingCornerShape;
    DirectX::SimpleMath::Matrix engineHousingFrontLeftMatrix;
    DirectX::SimpleMath::Matrix localEngineHousingFrontLeftMatrix;
    DirectX::SimpleMath::Matrix engineHousingFrontRightMatrix;
    DirectX::SimpleMath::Matrix localEngineHousingFrontRightMatrix;
    DirectX::SimpleMath::Matrix engineHousingRearLeftMatrix;
    DirectX::SimpleMath::Matrix localEngineHousingRearLeftMatrix;
    DirectX::SimpleMath::Matrix engineHousingRearRightMatrix;
    DirectX::SimpleMath::Matrix localEngineHousingRearRightMatrix;

    std::unique_ptr<DirectX::GeometricPrimitive>    engineHousingIntakeShape;
    DirectX::SimpleMath::Matrix engineHousingIntakeLeftMatrix;
    DirectX::SimpleMath::Matrix localEngineHousingIntakeLeftMatrix;
    DirectX::SimpleMath::Matrix engineHousingIntakeRightMatrix;
    DirectX::SimpleMath::Matrix localEngineHousingIntakeRightMatrix;

    std::unique_ptr<DirectX::GeometricPrimitive>    engineHousingIntakeCowlShape;
    DirectX::SimpleMath::Matrix engineHousingIntakeCowlLeftMatrix;
    DirectX::SimpleMath::Matrix localEngineHousingIntakeCowlLeftMatrix;
    DirectX::SimpleMath::Matrix engineHousingIntakeCowlRightMatrix;
    DirectX::SimpleMath::Matrix localEngineHousingIntakeCowlRightMatrix;

    std::unique_ptr<DirectX::GeometricPrimitive>    engineHousingFrontShape;
    DirectX::SimpleMath::Matrix engineHousingFrontMatrix;
    DirectX::SimpleMath::Matrix localEngineHousingFrontMatrix;
    DirectX::SimpleMath::Matrix engineHousingRearMatrix;
    DirectX::SimpleMath::Matrix localEngineHousingRearMatrix;

    std::unique_ptr<DirectX::GeometricPrimitive>    engineHousingSideShape;
    DirectX::SimpleMath::Matrix engineHousingSideLeftMatrix;
    DirectX::SimpleMath::Matrix localEngineHousingSideLeftMatrix;
    DirectX::SimpleMath::Matrix engineHousingSideRightMatrix;
    DirectX::SimpleMath::Matrix localEngineHousingSideRightMatrix;

    std::unique_ptr<DirectX::GeometricPrimitive>    engineExhaustExteriorShape;
    std::unique_ptr<DirectX::GeometricPrimitive>    engineExhaustInteriorShape;
    std::unique_ptr<DirectX::GeometricPrimitive>    engineExhaustTipShape;
    DirectX::SimpleMath::Matrix engineExhaustLeftMatrix;
    DirectX::SimpleMath::Matrix localEngineExhaustLeftMatrix;
    DirectX::SimpleMath::Matrix engineExhaustRightMatrix;
    DirectX::SimpleMath::Matrix localEngineExhaustRightMatrix;
    DirectX::SimpleMath::Matrix engineExhaustTipLeftMatrix;
    DirectX::SimpleMath::Matrix localEngineExhaustTipLeftMatrix;
    DirectX::SimpleMath::Matrix engineExhaustTipRightMatrix;
    DirectX::SimpleMath::Matrix localEngineExhaustTipRightMatrix;

    std::unique_ptr<DirectX::GeometricPrimitive>    tailBoomShape;
    DirectX::SimpleMath::Matrix tailBoomMatrix;
    DirectX::SimpleMath::Matrix localTailBoomMatrix;

    std::unique_ptr<DirectX::GeometricPrimitive>    tailBoomNewShape;
    DirectX::SimpleMath::Matrix tailBoomLowerMatrix;
    DirectX::SimpleMath::Matrix localTailBoomLowerMatrix;
    DirectX::SimpleMath::Matrix tailBoomUpperMatrix;
    DirectX::SimpleMath::Matrix localTailBoomUpperMatrix;

    std::unique_ptr<DirectX::GeometricPrimitive>    tailBoomMidEndCapShape;
    DirectX::SimpleMath::Matrix tailBoomMidEndCapMatrix;
    DirectX::SimpleMath::Matrix localTailBoomMidEndCapMatrix;

    std::unique_ptr<DirectX::GeometricPrimitive>    tailBoomLowerUpperEndCapShape;
    DirectX::SimpleMath::Matrix tailBoomLowerEndCapMatrix;
    DirectX::SimpleMath::Matrix localTailBoomLowerEndCapMatrix;
    DirectX::SimpleMath::Matrix tailBoomUpperEndCapMatrix;
    DirectX::SimpleMath::Matrix localTailBoomUpperEndCapMatrix;

    std::unique_ptr<DirectX::GeometricPrimitive>    tailFinUpperShape;
    DirectX::SimpleMath::Matrix tailFinUpperMatrix;
    DirectX::SimpleMath::Matrix localTailFinUpperMatrix;

    std::unique_ptr<DirectX::GeometricPrimitive>    tailFinUpperTipShape;
    DirectX::SimpleMath::Matrix tailFinUpperTipMatrix;
    DirectX::SimpleMath::Matrix localTailFinUpperTipMatrix;

    std::unique_ptr<DirectX::GeometricPrimitive>    tailFinLowerShape;
    DirectX::SimpleMath::Matrix tailFinLowerMatrix;
    DirectX::SimpleMath::Matrix localTailFinLowerMatrix;

    std::unique_ptr<DirectX::GeometricPrimitive>    tailFinLowerTipShape;
    DirectX::SimpleMath::Matrix tailFinLowerTipMatrix;
    DirectX::SimpleMath::Matrix localTailFinLowerTipMatrix;

    std::unique_ptr<DirectX::GeometricPrimitive>    tailFinLeadingEdgeShape;
    DirectX::SimpleMath::Matrix tailFinLeadingEdgeLowerMatrix;
    DirectX::SimpleMath::Matrix localTailFinLeadingEdgeLowerMatrix;
    DirectX::SimpleMath::Matrix tailFinLeadingEdgeUpperMatrix;
    DirectX::SimpleMath::Matrix localTailFinLeadingEdgeUpperMatrix;

    std::unique_ptr<DirectX::GeometricPrimitive>    tailWingShape;
    DirectX::SimpleMath::Matrix tailWingMatrix;
    DirectX::SimpleMath::Matrix localTailWingMatrix;
    DirectX::SimpleMath::Matrix tailWingBellyMatrix;
    DirectX::SimpleMath::Matrix localTailWingBellyMatrix;

    std::unique_ptr<DirectX::GeometricPrimitive>    tailWingEdgeShape;
    DirectX::SimpleMath::Matrix tailWingLeadingEdgeMatrix;
    DirectX::SimpleMath::Matrix localTailWingLeadingEdgeMatrix;
    DirectX::SimpleMath::Matrix tailWingLeadingEdgeBellyMatrix;
    DirectX::SimpleMath::Matrix localTailWingLeadingEdgeBellyMatrix;
    DirectX::SimpleMath::Matrix tailWingTrailingEdgeMatrix;
    DirectX::SimpleMath::Matrix localTailWingTrailingEdgeMatrix;
    DirectX::SimpleMath::Matrix tailWingTrailingEdgeBellyMatrix;
    DirectX::SimpleMath::Matrix localTailWingTrailingEdgeBellyMatrix;

    std::unique_ptr<DirectX::GeometricPrimitive>    tailWingFinShape;
    DirectX::SimpleMath::Matrix tailWingFinLeftMatrix;
    DirectX::SimpleMath::Matrix localTailWingFinLeftMatrix;
    DirectX::SimpleMath::Matrix tailWingFinRightMatrix;
    DirectX::SimpleMath::Matrix localTailWingFinRightMatrix;

    std::unique_ptr<DirectX::GeometricPrimitive>    tailWingFinEdgeShape;
    DirectX::SimpleMath::Matrix tailWingFinEdgeLeftMatrix;
    DirectX::SimpleMath::Matrix localTailWingFinEdgeLeftMatrix;
    DirectX::SimpleMath::Matrix tailWingFinEdgeRightMatrix;
    DirectX::SimpleMath::Matrix localTailWingFinEdgeRightMatrix;

    std::unique_ptr<DirectX::GeometricPrimitive>    mainRotorAxelShape;
    DirectX::SimpleMath::Matrix mainRotorAxelMatrix;
    DirectX::SimpleMath::Matrix localMainRotorAxelMatrix;

    std::unique_ptr<DirectX::GeometricPrimitive>    mainRotorHubShape;
    DirectX::SimpleMath::Matrix mainRotorHubMatrix;
    DirectX::SimpleMath::Matrix localMainRotorHubMatrix;

    std::unique_ptr<DirectX::GeometricPrimitive>    mainRotorArmShape;
    DirectX::SimpleMath::Matrix mainRotorArmMatrix;
    DirectX::SimpleMath::Matrix localMainRotorArmMatrix;

    std::unique_ptr<DirectX::GeometricPrimitive>    mainRotorBladeShape;
    DirectX::SimpleMath::Matrix mainRotorBladeMatrix1;
    DirectX::SimpleMath::Matrix mainRotorBladeTranslationMatrix1;
    DirectX::SimpleMath::Matrix localMainRotorBladeMatrix1;
    DirectX::SimpleMath::Matrix mainRotorBladeMatrix2;
    DirectX::SimpleMath::Matrix localMainRotorBladeMatrix2;
    DirectX::SimpleMath::Matrix mainRotorBladeTranslationMatrix2;

    std::unique_ptr<DirectX::GeometricPrimitive>    mainRotorBladeInteriorShape;
    DirectX::SimpleMath::Matrix mainRotorBladeInteriorMatrix1;
    DirectX::SimpleMath::Matrix mainRotorBladeInteriorTranslationMatrix1;
    DirectX::SimpleMath::Matrix localMainRotorBladeInteriorMatrix1;
    DirectX::SimpleMath::Matrix mainRotorBladeInteriorMatrix2;
    DirectX::SimpleMath::Matrix localMainRotorBladeInteriorMatrix2;
    DirectX::SimpleMath::Matrix mainRotorBladeInteriorTranslationMatrix2;

    std::unique_ptr<DirectX::GeometricPrimitive>    mainRotorBladeStripe1Shape;
    DirectX::SimpleMath::Matrix mainRotorBladeStripe1Matrix1;
    DirectX::SimpleMath::Matrix mainRotorBladeStripe1Matrix2;
    DirectX::SimpleMath::Matrix localMainRotorBladeStripe1Matrix1;

    std::unique_ptr<DirectX::GeometricPrimitive>    mainRotorArmStripe1Shape;
    DirectX::SimpleMath::Matrix mainRotorArmStripe1Matrix1;
    DirectX::SimpleMath::Matrix mainRotorArmStripe1Matrix2;
    DirectX::SimpleMath::Matrix localMainRotorArmStripe1Matrix1;

    DirectX::SimpleMath::Matrix mainRotorStripe1Matrix;
    DirectX::SimpleMath::Matrix localMainRotorStripe1Matrix;

    std::unique_ptr<DirectX::GeometricPrimitive>    mainRotorBladeEdgeStripe1Shape;
    DirectX::SimpleMath::Matrix mainRotorBladeEdgeStripe1Matrix1;
    DirectX::SimpleMath::Matrix mainRotorBladeEdgeStripe1Matrix2;
    DirectX::SimpleMath::Matrix localMainRotorEdgeBladeStripe1Matrix1;

    std::unique_ptr<DirectX::GeometricPrimitive>    mainRotorBladeEdgeShape;
    DirectX::SimpleMath::Matrix mainRotorBladeEdgeMatrix1;
    DirectX::SimpleMath::Matrix mainRotorBladeEdgeTranslationMatrix1;
    DirectX::SimpleMath::Matrix localMainRotorEdgeBladeMatrix1;
    DirectX::SimpleMath::Matrix mainRotorBladeEdgeMatrix2;
    DirectX::SimpleMath::Matrix mainRotorBladeEdgeTranslationMatrix2;
    DirectX::SimpleMath::Matrix localMainRotorEdgeBladeMatrix2;

    // main rotor stripe 2
    DirectX::SimpleMath::Matrix localMainRotorStripe2Matrix;
    std::unique_ptr<DirectX::GeometricPrimitive>    mainRotorBladeStripe2Shape;
    DirectX::SimpleMath::Matrix mainRotorBladeStripe2Matrix1;
    DirectX::SimpleMath::Matrix mainRotorBladeStripe2Matrix2;
    DirectX::SimpleMath::Matrix localMainRotorBladeStripe2Matrix1;
    DirectX::SimpleMath::Matrix localMainRotorBladeStripe2Matrix2;
    std::unique_ptr<DirectX::GeometricPrimitive>    mainRotorBladeEdgeStripe2Shape;
    DirectX::SimpleMath::Matrix localMainRotorEdgeStripe2Matrix1;
    DirectX::SimpleMath::Matrix localMainRotorEdgeStripe2Matrix2;
    DirectX::SimpleMath::Matrix mainRotorEdgeStripe2Matrix1;
    DirectX::SimpleMath::Matrix mainRotorEdgeStripe2Matrix2;
    std::unique_ptr<DirectX::GeometricPrimitive>    mainRotorArmStripe2Shape;
    DirectX::SimpleMath::Matrix mainRotorArmStripe2Matrix1;
    DirectX::SimpleMath::Matrix mainRotorArmStripe2Matrix2;
    DirectX::SimpleMath::Matrix localMainRotorArmStripe2Matrix1;
    DirectX::SimpleMath::Matrix localMainRotorArmStripe2Matrix2;
    //

    std::unique_ptr<DirectX::GeometricPrimitive>    mainRotorBladeEndCapShape;
    DirectX::SimpleMath::Matrix mainRotorBladeEndCapMatrix1;
    DirectX::SimpleMath::Matrix mainRotorBladeEndCapTranslationMatrix1;
    DirectX::SimpleMath::Matrix localMainRotorBladeEndCapMatrix1;
    DirectX::SimpleMath::Matrix mainRotorBladeEndCapMatrix2;
    DirectX::SimpleMath::Matrix mainRotorBladeEndCapTranslationMatrix2;
    DirectX::SimpleMath::Matrix localMainRotorBladeEndCapMatrix2;

    std::unique_ptr<DirectX::GeometricPrimitive>    mainRotorBladeEdgeEndCapShape;
    DirectX::SimpleMath::Matrix mainRotorBladeEdgeEndCapMatrix1;
    DirectX::SimpleMath::Matrix mainRotorBladeEdgeEndCapTranslationMatrix1;
    DirectX::SimpleMath::Matrix localMainRotorBladeEdgeEndCapMatrix1;
    DirectX::SimpleMath::Matrix mainRotorBladeEdgeEndCapMatrix2;
    DirectX::SimpleMath::Matrix mainRotorBladeEdgeEndCapTranslationMatrix2;
    DirectX::SimpleMath::Matrix localMainRotorBladeEdgeEndCapMatrix2;

    std::unique_ptr<DirectX::GeometricPrimitive>    mainRotorArmEndCapShape;
    DirectX::SimpleMath::Matrix mainRotorArmEndCapMatrix1;
    DirectX::SimpleMath::Matrix mainRotorArmEndCapTranslationMatrix1;
    DirectX::SimpleMath::Matrix localMainRotorArmEndCapMatrix1;
    DirectX::SimpleMath::Matrix mainRotorArmEndCapMatrix2;
    DirectX::SimpleMath::Matrix mainRotorArmEndCapTranslationMatrix2;
    DirectX::SimpleMath::Matrix localMainRotorArmEndCapMatrix2;

    std::unique_ptr<DirectX::GeometricPrimitive>    tailRotorAxelShape;
    DirectX::SimpleMath::Matrix tailRotorAxelMatrix;
    DirectX::SimpleMath::Matrix localTailRotorAxelMatrix;

    std::unique_ptr<DirectX::GeometricPrimitive>    tailRotorHubShape;
    DirectX::SimpleMath::Matrix tailRotorHubMatrix;
    DirectX::SimpleMath::Matrix localTailRotorHubMatrix;

    std::unique_ptr<DirectX::GeometricPrimitive>    tailRotorArmShape;
    DirectX::SimpleMath::Matrix tailRotorArmMatrix;
    DirectX::SimpleMath::Matrix localTailRotorArmMatrix;
    std::unique_ptr<DirectX::GeometricPrimitive>    tailRotorArmEndCapShape;
    DirectX::SimpleMath::Matrix tailRotorArmEndCapMatrix1;
    DirectX::SimpleMath::Matrix localTailRotorArmEndCapMatrix1;
    DirectX::SimpleMath::Matrix tailRotorArmEndCapTranslationMatrix1;
    DirectX::SimpleMath::Matrix tailRotorArmEndCapMatrix2;
    DirectX::SimpleMath::Matrix localTailRotorArmEndCapMatrix2;
    DirectX::SimpleMath::Matrix tailRotorArmEndCapTranslationMatrix2;

    std::unique_ptr<DirectX::GeometricPrimitive>    tailRotorBladeShape;
    DirectX::SimpleMath::Matrix tailRotorBladeMatrix1;
    DirectX::SimpleMath::Matrix tailRotorBladeTranslationMatrix1;
    DirectX::SimpleMath::Matrix localTailRotorBladeMatrix1;
    DirectX::SimpleMath::Matrix tailRotorBladeMatrix2;
    DirectX::SimpleMath::Matrix localTailRotorBladeMatrix2;
    DirectX::SimpleMath::Matrix tailRotorBladeTranslationMatrix2;
    std::unique_ptr<DirectX::GeometricPrimitive>    tailRotorBladeEndCapShape;
    DirectX::SimpleMath::Matrix tailRotorBladeEndCapMatrix1;
    DirectX::SimpleMath::Matrix tailRotorBladeEndCapTranslationMatrix1;
    DirectX::SimpleMath::Matrix localTailRotorBladeEndCapMatrix1;
    DirectX::SimpleMath::Matrix tailRotorBladeEndCapMatrix2;
    DirectX::SimpleMath::Matrix localTailRotorBladeEndCapMatrix2;
    DirectX::SimpleMath::Matrix tailRotorBladeEndCapTranslationMatrix2;

    std::unique_ptr<DirectX::GeometricPrimitive>    tailRotorBladeEdgeShape;
    DirectX::SimpleMath::Matrix tailRotorBladeEdgeMatrix1;
    DirectX::SimpleMath::Matrix tailRotorBladeEdgeTranslationMatrix1;
    DirectX::SimpleMath::Matrix localTailRotorBladeEdgeMatrix1;
    DirectX::SimpleMath::Matrix tailRotorBladeEdgeMatrix2;
    DirectX::SimpleMath::Matrix tailRotorBladeEdgeTranslationMatrix2;
    DirectX::SimpleMath::Matrix localTailRotorBladeEdgeMatrix2;

    std::unique_ptr<DirectX::GeometricPrimitive>    tailRotorBladeEdgeEndCapShape;
    DirectX::SimpleMath::Matrix tailRotorBladeEdgeEndCapMatrix1;
    DirectX::SimpleMath::Matrix tailRotorBladeEdgeEndCapTranslationMatrix1;
    DirectX::SimpleMath::Matrix localTailRotorBladeEdgeEndCapMatrix1;
    DirectX::SimpleMath::Matrix tailRotorBladeEdgeEndCapMatrix2;
    DirectX::SimpleMath::Matrix tailRotorBladeEdgeEndCapTranslationMatrix2;
    DirectX::SimpleMath::Matrix localTailRotorBladeEdgeEndCapMatrix2;

    std::unique_ptr<DirectX::GeometricPrimitive>    tailRotorBladeInteriorShape;
    DirectX::SimpleMath::Matrix tailRotorBladeInteriorMatrix1;
    DirectX::SimpleMath::Matrix tailRotorBladeInteriorTranslationMatrix1;
    DirectX::SimpleMath::Matrix localTailRotorBladeInteriorMatrix1;
    DirectX::SimpleMath::Matrix tailRotorBladeInteriorMatrix2;
    DirectX::SimpleMath::Matrix localTailRotorBladeInteriorMatrix2;
    DirectX::SimpleMath::Matrix tailRotorBladeInteriorTranslationMatrix2;

    std::unique_ptr<DirectX::GeometricPrimitive>    tailRotorArmStripe1Shape;
    DirectX::SimpleMath::Matrix tailRotorArmStripe1Matrix1;
    DirectX::SimpleMath::Matrix tailRotorArmStripe1TranslationMatrix1;
    DirectX::SimpleMath::Matrix localTailRotorArmStripe1Matrix1;
    DirectX::SimpleMath::Matrix tailRotorArmStripe1Matrix2;
    DirectX::SimpleMath::Matrix localTailRotorArmStripe1Matrix2;
    std::unique_ptr<DirectX::GeometricPrimitive>    tailRotorBladeStripe1Shape;
    DirectX::SimpleMath::Matrix tailRotorBladeStripe1Matrix1;
    DirectX::SimpleMath::Matrix localTailRotorBladeStripe1Matrix1;
    DirectX::SimpleMath::Matrix tailRotorBladeStripe1Matrix2;
    DirectX::SimpleMath::Matrix localTailRotorBladeStripe1Matrix2;

    std::unique_ptr<DirectX::GeometricPrimitive>    tailRotorEdgeStripe1Shape;
    DirectX::SimpleMath::Matrix tailRotorEdgeStripe1Matrix1;
    DirectX::SimpleMath::Matrix localTailRotorEdgeStripe1Matrix1;
    DirectX::SimpleMath::Matrix tailRotorEdgeStripe1Matrix2;
    DirectX::SimpleMath::Matrix localTailRotorEdgeStripe1Matrix2;

    std::unique_ptr<DirectX::GeometricPrimitive>    tailRotorArmStripe2Shape;
    DirectX::SimpleMath::Matrix tailRotorArmStripe2Matrix1;
    DirectX::SimpleMath::Matrix localTailRotorArmStripe2Matrix1;
    DirectX::SimpleMath::Matrix tailRotorArmStripe2Matrix2;
    DirectX::SimpleMath::Matrix localTailRotorArmStripe2Matrix2;

    std::unique_ptr<DirectX::GeometricPrimitive>    tailRotorBladeStripe2Shape;
    DirectX::SimpleMath::Matrix tailRotorBladeStripe2Matrix1;
    DirectX::SimpleMath::Matrix localTailRotorBladeStripe2Matrix1;
    DirectX::SimpleMath::Matrix tailRotorBladeStripe2Matrix2;
    DirectX::SimpleMath::Matrix localTailRotorBladeStripe2Matrix2;

    std::unique_ptr<DirectX::GeometricPrimitive>    tailRotorEdgeStripe2Shape;
    DirectX::SimpleMath::Matrix tailRotorEdgeStripe2Matrix1;
    DirectX::SimpleMath::Matrix localTailRotorEdgeStripe2Matrix1;
    DirectX::SimpleMath::Matrix tailRotorEdgeStripe2Matrix2;
    DirectX::SimpleMath::Matrix localTailRotorEdgeStripe2Matrix2;

    DirectX::SimpleMath::Matrix tailRotorTranslationMatrix1;
    DirectX::SimpleMath::Matrix tailRotorTranslationMatrix2;

    // shadows
    std::unique_ptr<DirectX::GeometricPrimitive>    shadowBaseShape;
    DirectX::SimpleMath::Matrix localShadowBaseMat = DirectX::SimpleMath::Matrix::Identity;
    DirectX::SimpleMath::Matrix shadowBaseMat = DirectX::SimpleMath::Matrix::Identity;
    DirectX::SimpleMath::Matrix localShadowFrontNoseMat = DirectX::SimpleMath::Matrix::Identity;
    DirectX::SimpleMath::Matrix shadowFrontNoseTranslationMat = DirectX::SimpleMath::Matrix::Identity;
    DirectX::SimpleMath::Matrix shadowFrontNoseMat = DirectX::SimpleMath::Matrix::Identity;
    DirectX::SimpleMath::Matrix shadowRearNoseMat = DirectX::SimpleMath::Matrix::Identity;
    DirectX::SimpleMath::Matrix shadowMainRotorMat1 = DirectX::SimpleMath::Matrix::Identity;
    DirectX::SimpleMath::Matrix shadowMainRotorMat2 = DirectX::SimpleMath::Matrix::Identity;

    DirectX::SimpleMath::Matrix shadowBoomMat = DirectX::SimpleMath::Matrix::Identity;
    DirectX::SimpleMath::Matrix shadowBoomBottomMat = DirectX::SimpleMath::Matrix::Identity;
    DirectX::SimpleMath::Matrix shadowBoomTopMat = DirectX::SimpleMath::Matrix::Identity;
    DirectX::SimpleMath::Matrix shadowBoomEndCapMat = DirectX::SimpleMath::Matrix::Identity;
    DirectX::SimpleMath::Matrix shadowMainWingMat = DirectX::SimpleMath::Matrix::Identity;

    DirectX::SimpleMath::Matrix shadowMainWingFrontMat = DirectX::SimpleMath::Matrix::Identity;
    DirectX::SimpleMath::Matrix shadowMainWingRearMat = DirectX::SimpleMath::Matrix::Identity;

    DirectX::SimpleMath::Matrix shadowTailWingMat = DirectX::SimpleMath::Matrix::Identity;
    DirectX::SimpleMath::Matrix shadowTailFinUpperMat = DirectX::SimpleMath::Matrix::Identity;
    DirectX::SimpleMath::Matrix shadowTailFinLowerMat = DirectX::SimpleMath::Matrix::Identity;

    DirectX::SimpleMath::Matrix shadowTailRotorMat1 = DirectX::SimpleMath::Matrix::Identity;
    DirectX::SimpleMath::Matrix shadowTailRotorMat2 = DirectX::SimpleMath::Matrix::Identity;

    DirectX::SimpleMath::Matrix shadowTailRotorAxelMat = DirectX::SimpleMath::Matrix::Identity;
    DirectX::SimpleMath::Matrix shadowTailRotorHubMat = DirectX::SimpleMath::Matrix::Identity;
    DirectX::SimpleMath::Matrix shadowMainRotorAxelMat = DirectX::SimpleMath::Matrix::Identity;
    DirectX::SimpleMath::Matrix shadowMainRotorHubMat = DirectX::SimpleMath::Matrix::Identity;

    DirectX::SimpleMath::Matrix shadowHouseingFrontMat = DirectX::SimpleMath::Matrix::Identity;
    DirectX::SimpleMath::Matrix shadowHouseingLeftMat = DirectX::SimpleMath::Matrix::Identity;
    DirectX::SimpleMath::Matrix shadowHouseingMainMat = DirectX::SimpleMath::Matrix::Identity;
    DirectX::SimpleMath::Matrix shadowHouseingRearMat = DirectX::SimpleMath::Matrix::Identity;
    DirectX::SimpleMath::Matrix shadowHouseingRightMat = DirectX::SimpleMath::Matrix::Identity;
    
    DirectX::SimpleMath::Matrix shadowRearWingFinLeftMat = DirectX::SimpleMath::Matrix::Identity;
    DirectX::SimpleMath::Matrix shadowRearWingFinRightMat = DirectX::SimpleMath::Matrix::Identity;

    DirectX::SimpleMath::Matrix shadowMainRotorArmMat = DirectX::SimpleMath::Matrix::Identity;
    DirectX::SimpleMath::Matrix shadowTailRotorArmMat = DirectX::SimpleMath::Matrix::Identity;

    DirectX::SimpleMath::Matrix shadowNoseMat = DirectX::SimpleMath::Matrix::Identity;
    DirectX::SimpleMath::Matrix shadowNoseBodyMat = DirectX::SimpleMath::Matrix::Identity;

    std::unique_ptr<DirectX::GeometricPrimitive>    swashplateFrameShape;
    std::unique_ptr<DirectX::GeometricPrimitive>    swashplateHubShape;
    DirectX::SimpleMath::Matrix swashplateHubMatrix;
    DirectX::SimpleMath::Matrix swashplateHubTranslationMatrix;
    DirectX::SimpleMath::Matrix localSwashplateHubMatrix;
    DirectX::SimpleMath::Matrix localSwashplateFrameMatrix;
    DirectX::SimpleMath::Matrix swashplateFrameMatrix;

    std::unique_ptr<DirectX::GeometricPrimitive>    pitchArmShape;
    DirectX::SimpleMath::Matrix pitchArmMatrix1;
    DirectX::SimpleMath::Matrix pitchArmTranslationMatrix1;
    DirectX::SimpleMath::Matrix localPitchArmMatrix1;

    DirectX::SimpleMath::Matrix pitchArmMatrix2;
    DirectX::SimpleMath::Matrix pitchArmTranslationMatrix2;
    DirectX::SimpleMath::Matrix localPitchArmMatrix2;

    std::unique_ptr<DirectX::GeometricPrimitive>    pitchArmCouplingShape;
    DirectX::SimpleMath::Matrix pitchArmCouplingMatrix1;
    DirectX::SimpleMath::Matrix pitchArmTranslationCouplingMatrix1;
    DirectX::SimpleMath::Matrix localPitchArmCouplingMatrix1;

    DirectX::SimpleMath::Matrix pitchArmCouplingMatrix2;
    DirectX::SimpleMath::Matrix pitchArmTranslationCouplingMatrix2;
    DirectX::SimpleMath::Matrix localPitchArmCouplingMatrix2 = DirectX::SimpleMath::Matrix::Identity;

    std::unique_ptr<DirectX::GeometricPrimitive>    pitchLinkShape;
    DirectX::SimpleMath::Matrix pitchLinkMatrix1 = DirectX::SimpleMath::Matrix::Identity;
    DirectX::SimpleMath::Matrix pitchLinkTranslationMatrix1 = DirectX::SimpleMath::Matrix::Identity;
    DirectX::SimpleMath::Matrix localPitchLinkMatrix1 = DirectX::SimpleMath::Matrix::Identity;
    DirectX::SimpleMath::Matrix pitchLinkBodyTransMatrix1 = DirectX::SimpleMath::Matrix::Identity;
    DirectX::SimpleMath::Matrix pitchLinkBaseMatrix1 = DirectX::SimpleMath::Matrix::Identity;

    std::unique_ptr<DirectX::GeometricPrimitive>    pitchJointShape;
    DirectX::SimpleMath::Matrix pitchJointMatrix1 = DirectX::SimpleMath::Matrix::Identity;
    DirectX::SimpleMath::Matrix pitchJointTranslationMatrix1 = DirectX::SimpleMath::Matrix::Identity;
    DirectX::SimpleMath::Matrix localPitchJointMatrix1 = DirectX::SimpleMath::Matrix::Identity;

    DirectX::SimpleMath::Matrix pitchJointMatrix2 = DirectX::SimpleMath::Matrix::Identity;
    DirectX::SimpleMath::Matrix pitchJointTranslationMatrix2 = DirectX::SimpleMath::Matrix::Identity;
    DirectX::SimpleMath::Matrix localPitchJointMatrix2 = DirectX::SimpleMath::Matrix::Identity;

    DirectX::SimpleMath::Matrix pitchLinkMatrix2;
    DirectX::SimpleMath::Matrix pitchLinkTranslationMatrix2;
    DirectX::SimpleMath::Matrix localPitchLinkMatrix2;
    DirectX::SimpleMath::Matrix pitchLinkBodyTransMatrix2 = DirectX::SimpleMath::Matrix::Identity;
    DirectX::SimpleMath::Matrix pitchLinkBaseMatrix2 = DirectX::SimpleMath::Matrix::Identity;
};

class Vehicle
{
public: 
    void DebugToggle();
    void DebugToggle2();

    void DrawModelEffectsOff(const DirectX::SimpleMath::Matrix aView, const DirectX::SimpleMath::Matrix aProj);
    void DrawModelEffectsOn(std::shared_ptr<DirectX::NormalMapEffect> aEffect, Microsoft::WRL::ComPtr<ID3D11InputLayout> aInputLayout);

    float GetAccel() const { return m_heli.testAccel; };
    DirectX::SimpleMath::Vector3 GetAccelVec() const { return m_heli.testAccelVec; };
    float GetAirSpeed() { return m_heli.q.velocity.Length(); };
    float GetAltitude() const { return m_heli.q.position.y - m_heli.terrainHightAtPos - 3.0f; };
    float GetCollective() const { return m_heli.controlInput.collectiveInput; };
    DirectX::SimpleMath::Vector3 GetFollowPos() const;
    DirectX::SimpleMath::Vector3 GetForward() const { return m_heli.forward; };
    DirectX::SimpleMath::Vector3 GetPos() const { return m_heli.q.position; };
    float GetRPM() const { return m_heli.mainRotor.rpm; };
    float GetGroundSpeed() { return m_heli.speed; };
    float GetThrottle() { return m_heli.controlInput.throttleInput; };
    double GetTime() { return m_heli.time; };
    DirectX::SimpleMath::Vector3 GetVehicleUp() const { return m_heli.up; };
    DirectX::SimpleMath::Matrix GetVehicleOrientation() const { return m_heli.cameraOrientation; };
 
    DirectX::SimpleMath::Vector3 GetVelocity() const { return m_heli.q.velocity; };
  
    void InitializeVehicle(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext);

    // helicopter functions
    void InputCollective(const float aCollectiveInput);
    void InputCollectiveGamePad(const float aCollectiveInput);
    void InputCyclicPitch(const float aPitchInput);
    void InputCyclicRoll(const float aRollInput);
    void InputCyclicPitchGamePad(const float aPitchInput, const float aTimeStep);
    void InputCyclicRollGamePad(const float aRollInput, const float aTimeStep);
    
    void InputThrottle(const float aThrottleInput);
    void InputThrottleGamePad(const float aThrottleInput);
    void InputYawPedal(const float aYawInput);
    void InputYawPedalGamePad(const float aYawInput, const float aTimeStep);

    void Jump();

    void ResetVehicle();
    void SetDebugData(std::shared_ptr<DebugData> aDebugPtr);
    void SetEnvironment(Environment* aEnviron);
    void SetGamePadConnectionState(const bool aIsGamePadConnected);
    void ToggleLandingGearState();

    void UpdateVehicle(const double aTimeDelta);

private:
    float CalculateLiftCoefficient(const float aAngle);
    DirectX::SimpleMath::Vector3 CalculateDragAngularLocal(const DirectX::SimpleMath::Vector3 aAngVelocity);
    DirectX::SimpleMath::Vector3 CalculateStabilityTorqueLocal(const HeliData& aHeliData);

    void InitializeEngine(Engine& aEngine);
    void InitializeFlightControls(ControlInput& aInput);
    void InitializeModel(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext, HeliData& aHeliData);
    void InitializeRotorBlades(HeliData& aHeliData);

    void InputDecay(const double aTimeDelta);
    void InputDecayGamePad(const double aTimeDelta);
    DirectX::SimpleMath::Vector3 HorizontalStabilizerVec(const HeliData& aHeliData);

    void LandVehicle();

    void RepositionModelCordinates(const DirectX::SimpleMath::Vector3 aPos, struct HeliModel& aModel);

    void RightHandSide(struct HeliData* aHeli, Motion* aQ, Motion* aDeltaQ, double aTimeDelta, float aQScale, Motion* aDQ);
    void RungeKutta4(struct HeliData* aHeli, double aTimeDelta);
    
    void UpdateAlignmentFromTorque(const float aTimeDelta);

    void UpdateBladeLiftForce(const float aTimeStep);

    Utility::Torque UpdateBodyTorqueRunge(Utility::Torque aPendulumTorque);
    void UpdateAlignmentCamera();
    void UpdateCentrifugalForce(const float aTimeStep);
    void UpdateCyclicStick(ControlInput& aInput);
    float UpdateGroundEffectForce(const float aLiftForce);
    void UpdateLandingGear(struct LandingGear& aLandingGear, const double aTimeDelta);
    void UpdateModel();
    void UpdatePendulumMotion(Utility::Torque& aTorque, DirectX::SimpleMath::Vector3& aVelocity, const float aTimeStep);
    void UpdatePhysicsPoints(struct HeliData& aHeli);
    void UpdateResistance();
    DirectX::SimpleMath::Vector3 UpdateRotorForceRunge();

    void UpdateRotorData(HeliData& aHeliData, const double aTimer);
    void UpdateRotorPitch(HeliData& aHeliData);
    void UpdateRotorSpin(HeliData& aHeliData, const double aTimer);

    void UpdateTerrainData();
    void UpdateTerrainNorm();

    float WindVaningVal(const HeliData& aHeliData);
    DirectX::SimpleMath::Vector3 WindVaningVec(const HeliData& aHeliData);

    Environment const*              m_environment;
    std::shared_ptr<DebugData>      m_debugData;
    HeliData                        m_heli;
    HeliModel                       m_heliModel;

    DirectX::SimpleMath::Vector4    m_defaultForward = DirectX::XMVectorSet(1.0, 0.0, 0.0, 0.0);
    DirectX::SimpleMath::Vector4    m_forward = DirectX::XMVectorSet(1.0, 0.0, 0.0, 0.0);
    DirectX::SimpleMath::Vector4    m_defaultRight = DirectX::XMVectorSet(0.0, 0.0, 1.0, 0.0);
    DirectX::SimpleMath::Vector4    m_right = DirectX::XMVectorSet(0.0, 0.0, 1.0, 0.0);

    float                           m_moveBackForward = 0.0;
    float                           m_moveLeftRight = 0.0;
    float                           m_moveUpDown = 0.0;

    float m_rotorTimerTest = 0.0f;
    float m_rotorTimerTest2 = 0.0f;
    float m_testFPS = 0.0f;
    float m_testTimer = 0.0f;
    float m_testTimer2 = 0.0f;

    float testYawInput = 0.0f;
    float testPitchInput = 0.0f;
    float testRollInput = 0.0f;
    float testTurnSpeed = 1.0f;
    float testInputMod = 1.0f;

    DirectX::SimpleMath::Vector3 m_prevPos = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 m_prevRight = -DirectX::SimpleMath::Vector3::UnitZ;
    DirectX::SimpleMath::Vector3 m_prevUp = DirectX::SimpleMath::Vector3::UnitY;

    bool m_debugToggle = false;
    bool m_debugToggle2 = false;
    bool m_debugToggle3 = false;
    DirectX::SimpleMath::Vector3 m_testPos = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 m_testPos2 = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 m_testPos3 = DirectX::SimpleMath::Vector3::Zero;
    
    const float m_inertiaModelX = 5.0f;
    const float m_inertiaModelY = 2.0f;
    const float m_inertiaModelZ = 4.0f;
    
    const float m_inertiaMass = 1900.0f;
    const float m_inertiaMass2 = 100.0f;
    const float m_inertiaModelX2 = 3.0f;
    const float m_inertiaModelY2 = 0.8f;
    const float m_inertiaModelZ2 = 0.6f;    
    const DirectX::SimpleMath::Vector3 m_tailInertiaOffset = DirectX::SimpleMath::Vector3(-5.0f, 0.0f, 0.0f);

    const float m_testMass = 2000.0f;
    const float m_angDragCoefficient = 0.6f;
    const float m_angDragCoefficient2 = 0.1f;
    const float m_angDragLength = 8.0f;
    const float m_angDragWidth = 4.0f;
    const float m_angDragHeight = 2.0f;

    const float m_gravTorqueModTest = 150.0f;

    const bool m_isUseNewPhysicsTrue = true;
    const float m_angularDamping = 0.9f;

    bool m_isGamePadConnected = false;

    bool m_isUseSimpleFlight = false;

    float m_swashplate0 = 0.0f;
    float m_swashplate1 = 0.0f;
    float m_swashplateOffset0 = 0.0f;
    float m_swashplateOffset1 = 0.0f;

    float m_pitchArmLength = 0.0f;
};


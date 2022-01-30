#pragma once
//#include "Camera.h"
#include "pch.h"
#include "Utility.h"
#include "Environment.h"



struct ControlInput
{
    // input control data
    const float inputDeadZone = 0.001;  // small deadzone to ignore nominal control input

    float       collectiveInput;
    const float collectiveInputMax = 1.0f;
    const float collectiveInputMin = 0.0f;
    const float collectiveInputRate = 1.0f;

    DirectX::SimpleMath::Vector3 cyclicStick;
    const float cyclicDecayRate = 0.8f;
    float       cyclicInputPitch;
    bool        cyclicInputPitchIsPressed;
    float       cyclicInputRoll;
    bool        cyclicInputRollIsPressed;
    const float cyclicInputMax = Utility::ToRadians(20.0f);
    const float cyclicInputMin = -Utility::ToRadians(20.0f);
    const float cyclicInputRate = 1.0f;

    float       throttleInput;
    const float throttleInputMin = 0.0f;
    const float throttleInputMax = 1.0f;
    const float throttleInputRate = 1.0f;

    bool        yawPedalIsPressed;
    float       yawPedalInput;
    const float yawPedalDecayRate = 0.5f;
    const float yawPedalInputMax = 1.0f;
    const float yawPedalInputMin = -1.0f;
    const float yawPedalInputRate = 1.0f;
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
    const float angleAtUp = Utility::ToRadians(5.0f);  
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
    DirectX::SimpleMath::Vector3 mainRotorPos;
    DirectX::SimpleMath::Vector3 localMainRotorPos;
    float       mainRotorRPM;
    const float mainRotorRPMmin = 0.0f;
    const float mainRotorRPMmax = 500.0f;
    DirectX::SimpleMath::Vector3 tailRotorPos;
    DirectX::SimpleMath::Vector3 localTailRotorPos;
    DirectX::SimpleMath::Vector3 centerOfMass;
    DirectX::SimpleMath::Vector3 localCenterOfMass;
    DirectX::SimpleMath::Vector3 landingGearPos;
    DirectX::SimpleMath::Vector3 localLandingGearPos;
    //
    int     numEqns;
    double  time;
    float   area;   
    float   airResistance;
    float   airDensity;
    float   dragCoefficient;
    const DirectX::SimpleMath::Vector3 gravity = DirectX::SimpleMath::Vector3(0.0f, -9.80665f, 0.0f);
    float   mass;
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
    DirectX::SimpleMath::Matrix cameraOrientation;
    DirectX::SimpleMath::Matrix cameraOrientationPrevious;
    float   terrainHightAtPos;

    float   testAccel = 0.0;
    DirectX::SimpleMath::Vector3 testAccelVec = DirectX::SimpleMath::Vector3::Zero;

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
    DirectX::SimpleMath::Vector4 stripeColor0;
    DirectX::SimpleMath::Vector4 stripeColor1;
    DirectX::SimpleMath::Vector4 landingGearTireColor;
    DirectX::SimpleMath::Vector4 landingGearWheelColor;
    DirectX::SimpleMath::Vector4 landingGearArmColor;
    DirectX::SimpleMath::Vector4 windshieldColor;
    DirectX::SimpleMath::Vector4 exhaustColor;
    DirectX::SimpleMath::Vector4 shadowColor;

    DirectX::SimpleMath::Vector4 testColor1;
    DirectX::SimpleMath::Vector4 testColor2;
    DirectX::SimpleMath::Vector4 testColor3;

    // tessellation values for rounded shapes
    const int circleTessellationVal1 = 8;
    const int circleTessellationVal2 = 32;

    // part shape and local positions
    std::unique_ptr<DirectX::GeometricPrimitive>    noseConeShape;
    DirectX::SimpleMath::Matrix noseConeMatrix;
    DirectX::SimpleMath::Matrix localNoseConeMatrix;

    std::unique_ptr<DirectX::GeometricPrimitive>    noseBodyShape;
    DirectX::SimpleMath::Matrix noseBodyMatrix;
    DirectX::SimpleMath::Matrix localNoseBodyMatrix;

    std::unique_ptr<DirectX::GeometricPrimitive>    bodyShape;
    DirectX::SimpleMath::Matrix bodyMatrix;
    DirectX::SimpleMath::Matrix localBodyMatrix;

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

    std::unique_ptr<DirectX::GeometricPrimitive>    mainWingLeadingEdgeShape;
    DirectX::SimpleMath::Matrix mainWingLeadingEdgeMatrix;
    DirectX::SimpleMath::Matrix localMainWingLeadingEdgeMatrix;

    std::unique_ptr<DirectX::GeometricPrimitive>    mainWingTailEdgeShape;
    DirectX::SimpleMath::Matrix mainWingTailEdgeMatrix;
    DirectX::SimpleMath::Matrix localMainWingTailEdgeMatrix;

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

    std::unique_ptr<DirectX::GeometricPrimitive>    tailWingEdgeShape;
    DirectX::SimpleMath::Matrix tailWingLeadingEdgeMatrix;
    DirectX::SimpleMath::Matrix localTailWingLeadingEdgeMatrix;
    DirectX::SimpleMath::Matrix tailWingTrailingEdgeMatrix;
    DirectX::SimpleMath::Matrix localTailWingTrailingEdgeMatrix;

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
    //DirectX::SimpleMath::Matrix mainRotorBladeTranslationStripe1Matrix1;
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
};

class Vehicle
{
public: 
    void DebugToggle();
    std::vector<std::pair<std::string, float>> DebugGetUI() { return m_debugUI; };
    std::vector<std::string> DebugGetUIVector() { return m_debugUIVector; };
    std::vector<std::tuple<DirectX::SimpleMath::Vector3, DirectX::SimpleMath::Vector3, DirectX::SimpleMath::Vector4>> DebugGetTestLines() const { return m_debugLinesVec; };

    void DrawModel(const DirectX::SimpleMath::Matrix aView, const DirectX::SimpleMath::Matrix aProj);

    float GetAccel() const { return m_heli.testAccel; };
    DirectX::SimpleMath::Vector3 GetAccelVec() const { return m_heli.testAccelVec; };
    DirectX::SimpleMath::Vector3 GetFollowPos() const;
    DirectX::SimpleMath::Vector3 GetPos() const { return m_heli.q.position; };
    //DirectX::SimpleMath::Vector3 GetPos() const { return m_prevPos; };
    float GetSpeed() { return m_heli.speed; };
    double GetTime() { return m_heli.time; };
    DirectX::SimpleMath::Vector3 GetVehicleUp() const { return m_heli.up; };
    DirectX::SimpleMath::Matrix GetVehicleOrientation() const { return m_heli.cameraOrientation; };
    //DirectX::SimpleMath::Matrix GetVehicleOrientation() const { return m_heli.cameraOrientationPrevious; };

    DirectX::SimpleMath::Vector3 GetVelocity() const { return m_heli.q.velocity; };
  
    void InitializeVehicle(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext);

    // helicopter functions
    void InputCollective(const float aCollectiveInput);
    void InputCyclicPitch(const float aPitchInput);
    void InputCyclicRoll(const float aRollInput);
    void InputDecay(const double aTimeDelta);
    void InputThrottle(const float aThrottleInput);
    void InputYawPedal(const float aYawInput);

    void Jump();

    void ResetVehicle();
    void SetEnvironment(Environment* aEnviron);

    void ToggleLandingGearState();

    void UpdateVehicle(const double aTimeDelta);

private:
    float CalculateLiftCoefficient(const float aAngle);

    void DebugClearUI() { 
        m_debugUI.clear();
        m_debugUIVector.clear();
        m_debugLinesVec.clear();};
    void DebugPushUILine(std::string aString, float aVal);
    void DebugPushUILineDecimalNumber(std::string aString1, float aVal, std::string aString2);
    void DebugPushUILineWholeNumber(std::string aString1, int aVal, std::string aString2);
    void DebugPushTestLine(DirectX::SimpleMath::Vector3 aLineBase, DirectX::SimpleMath::Vector3 aLineEnd, float aLength, float aYOffset, DirectX::SimpleMath::Vector4 aColor);
    void DebugPushTestLineBetweenPoints(DirectX::SimpleMath::Vector3 aPoint1, DirectX::SimpleMath::Vector3 aPoint2, DirectX::SimpleMath::Vector4 aColor);
    void DebugPushTestLinePositionIndicator(const DirectX::SimpleMath::Vector3 aPoint, const float aLineLength, const float aOffset, const DirectX::SimpleMath::Vector4 aColor);
    void InitializeEngine(Engine& aEngine);
    void InitializeFlightControls(ControlInput& aInput);
    void InitializeModel(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext, HeliData& aHeliData);
    void InitializeRotorBlades(HeliData& aHeliData);

    void LandVehicle();

    void RepositionModelCordinates(const DirectX::SimpleMath::Vector3 aPos, struct HeliModel& aModel);

    void RightHandSide(struct HeliData* aHeli, Motion* aQ, Motion* aDeltaQ, double aTimeDelta, float aQScale, Motion* aDQ);
    void RungeKutta4(struct HeliData* aHeli, double aTimeDelta);
    
    void UpdateAlignmentTorqueTest();

    void UpdateBladeLiftForce(const float aTimeStep);

    Utility::Torque UpdateBodyTorqueTest(const float aTimeStep);
    Utility::Torque UpdateBodyTorqueTestRunge(Utility::Torque aPendulumTorque, const float aTimeStep);
    void UpdateAlignmentCamera();
    void UpdateCyclicStick(ControlInput& aInput);
    void UpdateLandingGear(struct LandingGear& aLandingGear, const double aTimeDelta);
    void UpdateModel();
    void UpdatePendulumMotion(Utility::Torque& aTorque, DirectX::SimpleMath::Vector3& aVelocity, const float aTimeStep);
    void UpdatePendulumMotion2(const float aTimeStep);
    void UpdatePhysicsPoints(struct HeliData& aHeli);
    void UpdateResistance();
    void UpdateRotorForce();
    DirectX::SimpleMath::Vector3 UpdateRotorForceRunge();

    void UpdateRotorData(HeliData& aHeliData, const double aTimer);
    void UpdateRotorPitch(HeliData& aHeliData, const double aTimer);
    void UpdateRotorSpin(HeliData& aHeliData, const double aTimer);

    void UpdateTerrainNorm();

    Environment const*              m_environment;
    HeliData                        m_heli;
    HeliModel                       m_heliModel;

    DirectX::SimpleMath::Vector4    m_defaultForward = DirectX::XMVectorSet(1.0, 0.0, 0.0, 0.0);
    DirectX::SimpleMath::Vector4    m_forward = DirectX::XMVectorSet(1.0, 0.0, 0.0, 0.0);
    DirectX::SimpleMath::Vector4    m_defaultRight = DirectX::XMVectorSet(0.0, 0.0, 1.0, 0.0);
    DirectX::SimpleMath::Vector4    m_right = DirectX::XMVectorSet(0.0, 0.0, 1.0, 0.0);

    float                           m_moveBackForward = 0.0;
    float                           m_moveLeftRight = 0.0;
    float                           m_moveUpDown = 0.0;

    std::vector<std::tuple<DirectX::SimpleMath::Vector3, DirectX::SimpleMath::Vector3, DirectX::SimpleMath::Vector4>> m_debugLinesVec;
    std::vector<std::pair<std::string, float>> m_debugUI;
    std::vector<std::string> m_debugUIVector;

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
    DirectX::SimpleMath::Vector3 m_testPos = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 m_testPos2 = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 m_testPos3 = DirectX::SimpleMath::Vector3::Zero;
};


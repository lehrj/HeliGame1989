#pragma once
//#include "Camera.h"
#include "pch.h"
#include "Utility.h"
#include "Environment.h"


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
    //DirectX::SimpleMath::Matrix torqueForce;
    float   terrainHightAtPos;

    float   testAccel = 0.0;

    Rotor         mainRotor;
    Rotor         tailRotor;
    Engine        engine;
    ControlInput  controlInput;
};

struct HeliModel
{
    // Colors for model parts
    DirectX::SimpleMath::Vector4 bodyColor;
    DirectX::SimpleMath::Vector4 rotorColor;
    DirectX::SimpleMath::Vector4 axelColor;
    DirectX::SimpleMath::Vector4 windshieldColor;
    DirectX::SimpleMath::Vector4 testColor1;
    DirectX::SimpleMath::Vector4 testColor2;

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

    std::unique_ptr<DirectX::GeometricPrimitive>    bodyCapShape;
    DirectX::SimpleMath::Matrix windShieldMatrix;
    DirectX::SimpleMath::Matrix localWindShieldMatrix;
    DirectX::SimpleMath::Matrix bodyRearMatrix;
    DirectX::SimpleMath::Matrix localBodyRearMatrix;

    std::unique_ptr<DirectX::GeometricPrimitive>    engineHousingShape;
    DirectX::SimpleMath::Matrix engineHousingMatrix;
    DirectX::SimpleMath::Matrix localEngineHousingMatrix;

    std::unique_ptr<DirectX::GeometricPrimitive>    engineHousingFrontShape;
    DirectX::SimpleMath::Matrix engineHousingFrontMatrix;
    DirectX::SimpleMath::Matrix localEngineHousingFrontMatrix;

    std::unique_ptr<DirectX::GeometricPrimitive>    tailBoomShape;
    DirectX::SimpleMath::Matrix tailBoomMatrix;
    DirectX::SimpleMath::Matrix localTailBoomMatrix;

    std::unique_ptr<DirectX::GeometricPrimitive>    tailFinShape;
    DirectX::SimpleMath::Matrix tailFinMatrix;
    DirectX::SimpleMath::Matrix localTailFinMatrix;

    std::unique_ptr<DirectX::GeometricPrimitive>    tailWingShape;
    DirectX::SimpleMath::Matrix tailWingMatrix;
    DirectX::SimpleMath::Matrix localTailWingMatrix;

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

    std::unique_ptr<DirectX::GeometricPrimitive>    tailRotorAxelShape;
    DirectX::SimpleMath::Matrix tailRotorAxelMatrix;
    DirectX::SimpleMath::Matrix localTailRotorAxelMatrix;

    std::unique_ptr<DirectX::GeometricPrimitive>    tailRotorHubShape;
    DirectX::SimpleMath::Matrix tailRotorHubMatrix;
    DirectX::SimpleMath::Matrix localTailRotorHubMatrix;

    std::unique_ptr<DirectX::GeometricPrimitive>    tailRotorArmShape;
    DirectX::SimpleMath::Matrix tailRotorArmMatrix;
    DirectX::SimpleMath::Matrix localTailRotorArmMatrix;

    std::unique_ptr<DirectX::GeometricPrimitive>    tailRotorBladeShape;
    DirectX::SimpleMath::Matrix tailRotorBladeMatrix1;
    DirectX::SimpleMath::Matrix tailRotorBladeTranslationMatrix1;
    DirectX::SimpleMath::Matrix localTailRotorBladeMatrix1;
    DirectX::SimpleMath::Matrix tailRotorBladeMatrix2;
    DirectX::SimpleMath::Matrix localTailRotorBladeMatrix2;
    DirectX::SimpleMath::Matrix tailRotorBladeTranslationMatrix2;
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
    DirectX::SimpleMath::Vector3 GetPos() const { return m_heli.q.position; };
    float GetSpeed() { return m_heli.speed; };
    double GetTime() { return m_heli.time; };
    DirectX::SimpleMath::Vector3 GetVehicleUp() const { return m_heli.up; };
    DirectX::SimpleMath::Matrix GetVehicleOrientation() const { return m_heli.cameraOrientation; };

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

    void SetEnvironment(Environment* aEnviron);

    void ResetVehicle();

    void UpdateVehicle(const double aTimeDelta);

private:
    void DebugClearUI() { 
        m_debugUI.clear();
        m_debugUIVector.clear();
        m_debugLinesVec.clear();};
    void DebugPushUILine(std::string aString, float aVal);
    void DebugPushUILineDecimalNumber(std::string aString1, float aVal, std::string aString2);
    void DebugPushUILineWholeNumber(std::string aString1, int aVal, std::string aString2);
    void DebugPushTestLine(DirectX::SimpleMath::Vector3 aLineBase, DirectX::SimpleMath::Vector3 aLineEnd, float aLength, float aYOffset, DirectX::SimpleMath::Vector4 aColor);
    void DebugPushTestLineBetweenPoints(DirectX::SimpleMath::Vector3 aPoint1, DirectX::SimpleMath::Vector3 aPoint2, DirectX::SimpleMath::Vector4 aColor);
   
    void InitializeEngine(Engine& aEngine);
    void InitializeFlightControls(ControlInput& aInput);
    void InitializeModel(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext, HeliData& aHeliData);
    void InitializeRotorBlades(HeliData& aHeliData);

    void LandVehicle();

    void RepositionModelCordinates(const DirectX::SimpleMath::Vector3 aPos, struct HeliModel& aModel);

    void RightHandSide(struct HeliData* aHeli, Motion* aQ, Motion* aDeltaQ, double aTimeDelta, float aQScale, Motion* aDQ);
    void RungeKutta4(struct HeliData* aHeli, double aTimeDelta);
    
    void UpdateAlignmentTorqueTest();

    Utility::Torque UpdateBodyTorqueTest(const float aTimeStep);

    void UpdateCyclicStick(ControlInput& aInput);

    void UpdateModel();
    void UpdateParabolicMomentum();
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

    float testYawInput = 0.0f;
    float testPitchInput = 0.0f;
    float testRollInput = 0.0f;
    float testTurnSpeed = 1.0f;
    float testInputMod = 1.0f;

    DirectX::SimpleMath::Vector3 m_prevPos = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 m_prevRight = -DirectX::SimpleMath::Vector3::UnitZ;
    DirectX::SimpleMath::Vector3 m_prevUp = DirectX::SimpleMath::Vector3::UnitY;

    bool m_debugToggle = false;
};


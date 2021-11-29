#pragma once
//#include "Camera.h"
#include "pch.h"
#include "Utility.h"
#include "Environment.h"


struct Motion
{
    DirectX::SimpleMath::Vector3 position;
    DirectX::SimpleMath::Vector3 velocity;
    DirectX::SimpleMath::Vector3 bodyVelocity;
    DirectX::SimpleMath::Vector3 engineForce;
    DirectX::SimpleMath::Vector3 airResistance;
    DirectX::SimpleMath::Vector3 gravityForce;
    DirectX::SimpleMath::Vector3 totalVelocity;

    // helicopter data
    DirectX::SimpleMath::Vector3 mainRotorForceNormal;
    float mainRotorForceMagnitude{};
};

struct HeliData
{
    DirectX::SimpleMath::Vector3 mainRotorPos = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 tailRotorPos = DirectX::SimpleMath::Vector3::Zero;
    float collectiveInput;
    const float collectiveInputMax = 1.0f;
    const float collectiveInputMin = 0.0f;
    const float collectiveInputRate = 1.0f;

    float cyclicInputPitch;
    bool cyclicInputPitchIsPressed;
    float cyclicInputRoll;
    bool cyclicInputRollIsPressed;
    const float cyclicDecayRate = 0.8f;
    const float cyclicInputMax = 1.0f;
    const float cyclicInputMin = -1.0f;
    const float cyclicInputRate = 1.0f;

    float throttleInput;
    const float throttleInputMin = 0.0f;
    const float throttleInputMax = 1.0f;
    const float throttleInputRate = 1.0f;

    float yawPedalInput;
    bool yawPedalIsPressed;
    const float yawPedalInputMax = 1.0f;
    const float yawPedalInputMin = -1.0f;
    const float yawPedalInputRate = 1.0f;
    const float yawPedalDecayRate = 0.5f;

    const float mainRotorForceMagMax = 15.0f;
    const float mainRotorForceMagMin = 0.0f;

    float mainRotorRPM;
    const float mainRotorRPMmin = 0.0f;
    const float mainRotorRPMmax = 500.0f;

    int numEqns;
    double time;
    Motion q;
    float mass;
    float area;   
    float Cd;
    float airResistance;
    float airDensity;
    float totalResistance;

    DirectX::SimpleMath::Vector3 gravity;

    //////////////////////
    const float inputDeadZone = 0.001;  // small deadzone to ignore gas and brake peddle input

    float vehicleRotation;
    float speed;                   // speed vehicle is traveling
    bool isVehicleAirborne; 
    bool isVehicleLanding;
    bool isVelocityBackwards;
    DirectX::SimpleMath::Vector3 terrainNormal;
    DirectX::SimpleMath::Vector3 forward;
    DirectX::SimpleMath::Vector3 up;
    DirectX::SimpleMath::Vector3 right;

    float terrainHightAtPos;

    float testAccel = 0.0;
};

struct HeliModel
{
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
    // helicopter functions
    void InputCollective(const float aCollectiveInput);
    void InputCyclicPitch(const float aPitchInput);
    void InputCyclicRoll(const float aRollInput);
    void InputDecay(const double aTimeDelta);
    void InputHThrottle(const float aThrottleInput);
    void InputYawPedal(const float aYawInput);
    
    std::vector<std::pair<std::string, float>> DebugGetUI() { return m_debugUI; };
    std::vector<std::string> DebugGetUIVector() { return m_debugUIVector; };
    std::vector<std::tuple<DirectX::SimpleMath::Vector3, DirectX::SimpleMath::Vector3, DirectX::XMVECTORF32>> DebugGetTestLines() const { return m_debugLinesVec; };

    void DrawModel(DirectX::SimpleMath::Matrix aView, DirectX::SimpleMath::Matrix aProj);

    float GetAccel() const { return m_heli.testAccel; };

    DirectX::SimpleMath::Vector3 GetDebugPoint() { return  m_debugPoint; };
    
    //DirectX::SimpleMath::Vector3 GetModelTestPos() const { return m_heli.testModelPos; };
    DirectX::SimpleMath::Vector3 GetPos() const { return m_heli.q.position; };
    float GetVehicleRotation() const { return m_heli.vehicleRotation; };
    
    float GetSpeed() { return m_heli.speed; };
    double GetTime() { return m_heli.time; };
    DirectX::SimpleMath::Vector3 GetVehicleUp() const { return m_heli.up; };
    
    DirectX::SimpleMath::Vector3 GetVelocity() const { return m_heli.q.velocity; };
  
    void InitializeVehicle(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext);

    void Jump();

    void SetEnvironment(Environment* aEnviron);

    void ResetVehicle();

    void UpdateVehicle(const double aTimeDelta);

private:
    void DebugClearUI() { 
        m_debugUI.clear();
        m_debugUIVector.clear();
        m_debugLinesVec.clear();
    };

    void DebugPushUILine(std::string aString, float aVal);
    void DebugPushUILineDecimalNumber(std::string aString1, float aVal, std::string aString2);
    void DebugPushUILineWholeNumber(std::string aString1, int aVal, std::string aString2);
    void DebugPushTestLine(DirectX::SimpleMath::Vector3 aLineBase, DirectX::SimpleMath::Vector3 aLineEnd, float aLength, float aYOffset, DirectX::XMVECTORF32 aColor);

    void InitializeModel(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext);

    float GetYawRate(double aTimeDelta);

    void LandVehicle();

    void RightHandSide(struct HeliData* aHeli, Motion* aQ, Motion* aDeltaQ, double aTimeDelta, float aQScale, Motion* aDQ);
    void RungeKutta4(struct HeliData* aHeli, double aTimeDelta);

    void TurnVehicle(double aTimeDelta);
    void UpdateVehicleAlignment();

    //void UpdateHeadingVec();
    
    void UpdateModel(const double aTimer);
    void UpdateResistance();
    void UpdateRotorForce();
    void UpdateTailYawForce();
    void UpdateTerrainNorm();
    void UpdateVelocity(double aTimeDelta);

    HeliData                        m_heli;
    HeliModel                       m_heliModel;
    Environment const*              m_environment;
    
    DirectX::SimpleMath::Vector3    m_heading;   // direction the vehicle is facing
    float                           m_speed;        // speed vehicle is traveling

    DirectX::SimpleMath::Vector4    m_defaultForward = DirectX::XMVectorSet(1.0, 0.0, 0.0, 0.0);
    DirectX::SimpleMath::Vector4    m_forward = DirectX::XMVectorSet(1.0, 0.0, 0.0, 0.0);
    DirectX::SimpleMath::Vector4    m_defaultRight = DirectX::XMVectorSet(0.0, 0.0, 1.0, 0.0);
    DirectX::SimpleMath::Vector4    m_right = DirectX::XMVectorSet(0.0, 0.0, 1.0, 0.0);

    float                           m_moveBackForward = 0.0;
    float                           m_moveLeftRight = 0.0;
    float                           m_moveUpDown = 0.0;

    DirectX::SimpleMath::Vector3 m_debugPoint = DirectX::SimpleMath::Vector3::Zero;

    std::vector<std::tuple<DirectX::SimpleMath::Vector3, DirectX::SimpleMath::Vector3, DirectX::XMVECTORF32>> m_debugLinesVec;
    std::vector<std::pair<std::string, float>> m_debugUI;
    std::vector<std::string> m_debugUIVector;

    float m_rotorTimer = 0.0f;


};


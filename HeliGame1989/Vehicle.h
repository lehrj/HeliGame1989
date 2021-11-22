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
    DirectX::SimpleMath::Vector3 brakeForce;
    DirectX::SimpleMath::Vector3 slopeForce;
    DirectX::SimpleMath::Vector3 airResistance;
    DirectX::SimpleMath::Vector3 gravityForce;
    DirectX::SimpleMath::Vector3 totalVelocity;
};

struct HeliData
{
    int numEqns;
    double time;
    Motion q;
    float mass;
    float area;
    float density;
    float Cd;
    float muR;  // rolling friction
    float airResistance;
    //float airDensity;
    float totalResistance;
    float omegaE;
    float redline;
    float revlimit;
    float finalDriveRatio;
    float wheelMass;
    float wheelRadius;
    float wheelWidth;
    int gearNumber;     //  gear the car is in
    int numberOfGears;  //  total number of gears
    float gearRatio[8];  //  gear ratios
    DirectX::SimpleMath::Vector3 gravity;

    //////////////////////
    float inputDeadZone;  // small deadzone to ignore gas and brake peddle input
    float brakeInput;
    float throttleInput;    
    float brakeDecayRate;
    float throttleDecayRate;
    float maxThrottleInput; /////////////////////////////
    float maxBrakeInput; /////////////////////////////////
    float maxThrottleRate;
    float maxBrakeRate;
    bool isClutchPressed;
    bool isThrottlePressed;
    bool isBrakePressed;
    bool isTurningPressed;

    float steeringAngle;
    float steeringAngleDecay;
    float steeringAngleMax;
    float steeringSpeed; 

    float carRotation;
    DirectX::SimpleMath::Vector3 headingVec;          // direction the vehicle is facing
    float shiftCooldown;           // cooldown timer after gear is changed, before engine power is direct to driver train
    float shiftDelay;              // time it takes to change gears in which the clutch is pressed and engine does not deliver power
    float speed;                   // speed vehicle is traveling
    float wheelBase;
    bool isAccelerating;
    bool isBraking;
    bool isRevlimitHit;
    bool isTransmissionManual;
    bool isCarAirborne; 
    bool isCarLanding;
    bool isVelocityBackwards;
    DirectX::SimpleMath::Vector3 terrainNormal;
    DirectX::SimpleMath::Vector3 forward;
    DirectX::SimpleMath::Vector3 up;
    DirectX::SimpleMath::Vector3 right;

    float terrainHightAtPos;
    DirectX::SimpleMath::Vector3 testModelPos;
    float testModelRotation;
    DirectX::SimpleMath::Vector3 testTerrainNormal;
    DirectX::SimpleMath::Vector3 testHeadingVec;
    DirectX::SimpleMath::Vector3 testHeading1;
    DirectX::SimpleMath::Vector3 testHeading2;
    DirectX::SimpleMath::Vector3 testHeading3;

    DirectX::SimpleMath::Vector3 testAcceleration = DirectX::SimpleMath::Vector3::Zero;
    float testAccel = 0.0;
    // test values for wheel slip
    float testRearCylinderMass;
    float testTorque;
    float testRearAnglularVelocity;
    float testRearAngularVelocityAngle;
};

struct HeliModel
{
    std::unique_ptr<DirectX::GeometricPrimitive>    bodyShape;
    DirectX::SimpleMath::Matrix bodyMatrix;
    DirectX::SimpleMath::Matrix localBodyMatrix;

    std::unique_ptr<DirectX::GeometricPrimitive>    tailBoomShape;
    DirectX::SimpleMath::Matrix tailBoomMatrix;
    DirectX::SimpleMath::Matrix localTailBoomMatrix;

    std::unique_ptr<DirectX::GeometricPrimitive>    tailFinShape;
    DirectX::SimpleMath::Matrix tailFinMatrix;
    DirectX::SimpleMath::Matrix localTailFinMatrix;

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
    //Vehicle();

    void DebugEBrake();
    std::vector<std::pair<std::string, float>> DebugGetUI() { return m_debugUI; };
    std::vector<std::string> DebugGetUIVector() { return m_debugUIVector; };
    std::vector<std::tuple<DirectX::SimpleMath::Vector3, DirectX::SimpleMath::Vector3, DirectX::XMVECTORF32>> DebugGetTestLines() const { return m_debugLinesVec; };

    void DrawModel(DirectX::SimpleMath::Matrix aWorld, DirectX::SimpleMath::Matrix aView, DirectX::SimpleMath::Matrix aProj);
    void GearDown();
    void GearUp();

    float GetAccel() const { return m_heli.testAccel; };
    float GetDebugValue() { return m_heli.carRotation; };
    DirectX::SimpleMath::Vector3 GetDebugPoint() { return  m_debugPoint; };
    int GetGear() { return m_heli.gearNumber; };
    DirectX::SimpleMath::Vector3 GetHeading() const { return m_heli.headingVec; };
    
    DirectX::SimpleMath::Vector3 GetModelTestPos() const { return m_heli.testModelPos; };
    DirectX::SimpleMath::Vector3 GetPos() const { return m_heli.q.position; };
    float GetCarRotation() const { return m_heli.carRotation; };
    
    float GetInputBrake() const { return m_heli.brakeInput; };
    float GetInputThrottle() const { return m_heli.throttleInput; };
    
    float GetRedLine() const { return m_heli.redline; };
    float GetRPM() const { return m_heli.omegaE; };
    float GetRotation() const { return m_heli.carRotation; };
    float GetSpeed() { return m_heli.speed; };
    float GetSteering() const { return m_heli.steeringAngle; };
    double GetTime() { return m_heli.time; };
    DirectX::SimpleMath::Vector3 GetVehicleUp() const { return m_heli.up; };
    DirectX::SimpleMath::Vector3 GetVelocity() { return m_heli.q.velocity; };
    DirectX::SimpleMath::Vector3 GetVelocity() const { return m_heli.q.velocity; };
  
    void InitializeVehicle(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext);

    void Jump(double aTimer);

    void PressBrake(const float aBrakeInput);
    void PressClutch(const bool aClutchInput);
    void PressThrottle(const float aThrottleInput);
    void ThrottleBrakeDecay(const double aTimeDelta);
    void SetEnvironment(Environment* aEnviron);
    void SteeringInputDecay(const double aTimeDelta);

    void ResetVehicle();
    void RevLimiter();
    void ToggleBrake();
    void ToggleFuel();
    void ToggleGas();

    void TurnInput(float aTurnInput);
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

    float GetCarRotation();
    float GetTurnRadius();
    float GetYawRate(double aTimeDelta);
    DirectX::SimpleMath::Vector3 GetVehicleDirection();
    float GetWheelRotationRadians(const double aTimeDelta);
    float GetWheelRotationRadiansRear(const double aTimeDelta);

    void LandVehicle();

    void RightHandSide(struct HeliData* aHeli, Motion* aQ, Motion* aDeltaQ, double aTimeDelta, float aQScale, Motion* aDQ);
    void RungeKutta4(struct HeliData* aHeli, double aTimeDelta);
    
    void TestGetForceLateral();

    void TurnVehicle(double aTimeDelta);
    void UpdateCarAlignment();

    void UpdateHeadingVec();
    
    void UpdateModel(const double aTimer);
    void UpdateResistance();
    void UpdateTerrainNorm();
    void UpdateTransmission(const double aTimeDelta);
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

    DirectX::SimpleMath::Matrix     m_testBody;
    DirectX::SimpleMath::Matrix     m_testBodyTop;
    DirectX::SimpleMath::Matrix     m_testFrontAxle;
    DirectX::SimpleMath::Matrix     m_testRearAxle;

    DirectX::SimpleMath::Vector3 m_debugPoint = DirectX::SimpleMath::Vector3::Zero;

    float m_testRotation = 0.0;
    float m_testRotationRear = 0.0;

    float m_testMax = 0.0;
    float m_testMin = 1.0;

    std::vector<std::tuple<DirectX::SimpleMath::Vector3, DirectX::SimpleMath::Vector3, DirectX::XMVECTORF32>> m_debugLinesVec;
    std::vector<std::pair<std::string, float>> m_debugUI;
    std::vector<std::string> m_debugUIVector;

    float m_debugWheelDistance = 0.0;
    float m_debugVehicleDistanceTraveled = 0.0;

    bool m_testIsBreakLightOn = false;

    bool m_isFuelOn = true;

    float m_testVelocity = 0.0;

    float m_testEnginePower = 0.0;

    float m_rotorTimer = 0.0f;
};


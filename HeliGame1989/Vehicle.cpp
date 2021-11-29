#include "pch.h"
#include "Vehicle.h"


/*
Vehicle::Vehicle()
{
    //InitializeVehicle();
}
*/

void Vehicle::DebugEBrake()
{
    m_heli.q.velocity = DirectX::SimpleMath::Vector3::Zero;
}

void Vehicle::DrawModel(DirectX::SimpleMath::Matrix aWorld, DirectX::SimpleMath::Matrix aView, DirectX::SimpleMath::Matrix aProj)
{

    DirectX::SimpleMath::Matrix view = aView;
    DirectX::SimpleMath::Matrix proj = aProj;
    DirectX::SimpleMath::Vector4 bodyColor(0.501960814f, 0.501960814f, 0.501960814f, 1.000000000f);
    DirectX::SimpleMath::Vector4 rotorColor(0.827451050f, 0.827451050f, 0.827451050f, 1.000000000f);
    DirectX::SimpleMath::Vector4 axelColor(0.411764741f, 0.411764741f, 0.411764741f, 1.000000000f);
    DirectX::SimpleMath::Vector4 windshieldColor(0.662745118f, 0.662745118f, 0.662745118f, 1.000000000f);
    DirectX::SimpleMath::Vector4 testColor(1.0, 0.0f, 0.0f, 1.000000000f);
    DirectX::SimpleMath::Vector4 testColor2(1.0, 1.0f, 1.0f, 1.000000000f);
    //DirectX::Colors::DarkGray XMGLOBALCONST XMVECTORF32 DarkGray = { { { 0.662745118f, 0.662745118f, 0.662745118f, 1.000000000f } } };
    //DirectX::Colors::LightGray XMGLOBALCONST XMVECTORF32 LightGray = { { { 0.827451050f, 0.827451050f, 0.827451050f, 1.000000000f } } };
    //DirectX::Colors::DimGray XMGLOBALCONST XMVECTORF32 DimGray = { { { 0.411764741f, 0.411764741f, 0.411764741f, 1.000000000f } } };
    //DirectX::Colors::DarkSlateGray XMGLOBALCONST XMVECTORF32 DarkSlateGray = { { { 0.184313729f, 0.309803933f, 0.309803933f, 1.000000000f } } };

    m_heliModel.bodyShape->Draw(m_heliModel.bodyMatrix, view, proj, bodyColor);
    m_heliModel.bodyCapShape->Draw(m_heliModel.windShieldMatrix, view, proj, windshieldColor);
    m_heliModel.bodyCapShape->Draw(m_heliModel.bodyRearMatrix, view, proj, bodyColor);

    m_heliModel.noseConeShape->Draw(m_heliModel.noseConeMatrix, view, proj, bodyColor);
    m_heliModel.noseBodyShape->Draw(m_heliModel.noseBodyMatrix, view, proj, bodyColor);

    m_heliModel.engineHousingShape->Draw(m_heliModel.engineHousingMatrix, view, proj, bodyColor);
    m_heliModel.engineHousingFrontShape->Draw(m_heliModel.engineHousingFrontMatrix, view, proj, bodyColor);

    m_heliModel.tailBoomShape->Draw(m_heliModel.tailBoomMatrix, view, proj, bodyColor);
    m_heliModel.tailFinShape->Draw(m_heliModel.tailFinMatrix, view, proj, bodyColor);

    m_heliModel.tailWingShape->Draw(m_heliModel.tailWingMatrix, view, proj, bodyColor);

    m_heliModel.mainRotorAxelShape->Draw(m_heliModel.mainRotorAxelMatrix, view, proj, axelColor);
    m_heliModel.mainRotorHubShape->Draw(m_heliModel.mainRotorHubMatrix, view, proj, axelColor);

    m_heliModel.mainRotorArmShape->Draw(m_heliModel.mainRotorArmMatrix, view, proj, rotorColor);
    m_heliModel.mainRotorBladeShape->Draw(m_heliModel.mainRotorBladeMatrix1, view, proj, rotorColor);
    m_heliModel.mainRotorBladeShape->Draw(m_heliModel.mainRotorBladeMatrix2, view, proj, rotorColor);

    m_heliModel.tailRotorAxelShape->Draw(m_heliModel.tailRotorAxelMatrix, view, proj, axelColor);
    m_heliModel.tailRotorHubShape->Draw(m_heliModel.tailRotorHubMatrix, view, proj, axelColor);
    m_heliModel.tailRotorArmShape->Draw(m_heliModel.tailRotorArmMatrix, view, proj, rotorColor);
    m_heliModel.tailRotorBladeShape->Draw(m_heliModel.tailRotorBladeMatrix1, view, proj, rotorColor);
    m_heliModel.tailRotorBladeShape->Draw(m_heliModel.tailRotorBladeMatrix2, view, proj, rotorColor);
}

void Vehicle::GearDown()
{
    if (m_heli.gearNumber > 0 && m_heli.gearNumber <= m_heli.numberOfGears)
    {
        m_heli.shiftCooldown = m_heli.shiftDelay;
        --m_heli.gearNumber;
    }
}

void Vehicle::GearUp()
{
    if (m_heli.gearNumber >= 0 && m_heli.gearNumber < m_heli.numberOfGears)
    {
        m_heli.shiftCooldown = m_heli.shiftDelay;
        ++m_heli.gearNumber;
    }
}

float Vehicle::GetCarRotation()
{
    float turnRadius = GetTurnRadius();
    float steeringAngle = m_heli.steeringAngle;

    DirectX::SimpleMath::Vector3 testRadVec(0.0, 0.0, static_cast<float>(-turnRadius));
    DirectX::SimpleMath::Matrix testTurnMat = DirectX::SimpleMath::Matrix::CreateRotationY(steeringAngle);

    testRadVec = DirectX::SimpleMath::Vector3::Transform(testRadVec, testTurnMat);
    DirectX::SimpleMath::Vector3 testRadVecNorm = testRadVec;
    testRadVecNorm.Normalize();

    DirectX::SimpleMath::Vector3 ballVec = testRadVecNorm;
    DirectX::SimpleMath::Vector3 zeroDirection = DirectX::SimpleMath::Vector3::UnitZ;

    DirectX::SimpleMath::Vector3 a = - DirectX::SimpleMath::Vector3::UnitZ;
    DirectX::SimpleMath::Vector3 b = DirectX::SimpleMath::Vector3::UnitX;
    b = testRadVecNorm;

    float testAngle = acos(a.Dot(b));

    if (testAngle > m_testMax)
    {
        m_testMax = testAngle;
    }
    if (testAngle < m_testMin)
    {
        m_testMin = testAngle;
    }

    float carRotation = testAngle + 0.1f; 

    return carRotation;
}

float Vehicle::GetTurnRadius()
{
    float wheelBase = m_heli.wheelBase;
    float sinDelta = sin(m_heli.steeringAngle);
    float sinDelta2 = sin((10.0f * Utility::GetPi()) / 180.0f);
    float turnRadius = 1.0f;
    float turnRadius2 = 1.0f;
    if (sinDelta == 0.0f)
    {
        turnRadius = 0.0f;
    }
    else
    {
        turnRadius = wheelBase / sinDelta;
        turnRadius2 = wheelBase / sinDelta2;
    }

    return turnRadius;
}

float Vehicle::GetYawRate(double aTimeDelta)
{
    float wheelBase = m_heli.wheelBase;
    float velocity = m_heli.q.velocity.Length();
    float steeringAngle = m_heli.steeringAngle;

    float sinDelta = sin(steeringAngle);

    float omega = (velocity * sinDelta) / wheelBase;
    float omegaT = omega * static_cast<float>(aTimeDelta);

    // testing tail blade yaw turn
    DebugPushUILineDecimalNumber("m_heli.yawPedalInput", m_heli.yawPedalInput, "m_heli.yawPedalInput");
    omegaT = m_heli.yawPedalInput * static_cast<float>(aTimeDelta);
    return omegaT;
}

DirectX::SimpleMath::Vector3 Vehicle::GetVehicleDirection()
{
    /*
    DirectX::SimpleMath::Vector3 frontAxelPos;
    DirectX::SimpleMath::Vector3 tempScale;
    DirectX::SimpleMath::Quaternion tempQuat;
    m_heliModel.frontAxelMatrix.Decompose(tempScale, tempQuat, frontAxelPos);
    DirectX::SimpleMath::Vector3 rearAxelPos;
    m_heliModel.rearAxelMatrix.Decompose(tempScale, tempQuat, rearAxelPos);
    DirectX::SimpleMath::Vector3 direction = rearAxelPos - frontAxelPos;
    direction.Normalize();
   
    DirectX::SimpleMath::Vector3 testPos;
    m_heliModel.bodyTopMatrix.Decompose(tempScale, tempQuat, testPos);
    m_debugPoint = testPos;
    */
    DirectX::SimpleMath::Vector3 direction = m_heli.forward;
    return direction;
}

float Vehicle::GetWheelRotationRadians(const double aTimeDelta)
{
    DirectX::SimpleMath::Vector3 velocity = m_heli.q.velocity;
    float distance = DirectX::SimpleMath::Vector3::Distance(velocity, DirectX::SimpleMath::Vector3::Zero);
    float stepDistance;
    if (aTimeDelta != 0.0)
    {
        stepDistance = distance * static_cast<float>(aTimeDelta);
    }
    else
    {
        stepDistance = 0.0;
    }

    float circumference = 2.0f * Utility::GetPi() * m_heli.wheelRadius;
    float turnRatio = stepDistance / circumference;
    float rotations = turnRatio * (2.0f * Utility::GetPi());
    float wheelMove = rotations * circumference;
    m_debugWheelDistance += wheelMove;

    // flip wheel spin direction if velocity is backwards
    if (m_heli.isVelocityBackwards == true)
    {
        rotations *= -1;
    }

    return rotations;
}

float Vehicle::GetWheelRotationRadiansRear(const double aTimeDelta)
{
    float rotations = 0.0f;
    if (aTimeDelta != 0.0)
    {
        rotations = m_heli.testRearAnglularVelocity * static_cast<float>(aTimeDelta);
    }
    else
    {
        rotations = 0.0f;
    }

    return rotations;
}

void Vehicle::InitializeModel(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext)
{
    // main body
    //const DirectX::SimpleMath::Vector3 bodySize(7.0f, 3.28f, 4.09f);
    const DirectX::SimpleMath::Vector3 bodySize(5.0f, 3.0f, 3.0f);
    const DirectX::SimpleMath::Vector3 bodyTranslation(0.0f, bodySize.y * 0.5f, 0.0f);
    m_heliModel.bodyShape = DirectX::GeometricPrimitive::CreateBox(aContext.Get(), bodySize);
    m_heliModel.bodyMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.bodyMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(bodyTranslation);
    m_heliModel.localBodyMatrix = m_heliModel.bodyMatrix;
    
    /// windshield
    const float windshieldSize = (bodySize.z * 0.5) * sqrt(2);
    const DirectX::SimpleMath::Vector3 windshieldTranslation(bodySize.x * 0.5f, -bodySize.y * 0.0f, 0.0f);
    m_heliModel.bodyCapShape = DirectX::GeometricPrimitive::CreateOctahedron(aContext.Get(), windshieldSize);
    m_heliModel.windShieldMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.windShieldMatrix = DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(45.0f));
    m_heliModel.windShieldMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(bodyTranslation);
    m_heliModel.windShieldMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(windshieldTranslation);
    m_heliModel.localWindShieldMatrix = m_heliModel.windShieldMatrix;

    // nose cone
    const float noseConeLength = bodySize.z * cos(Utility::ToRadians(45.0));
    const float noseConeXoffset = 3.2f;
    const DirectX::SimpleMath::Vector3 noseConeSize(noseConeLength, bodySize.y * 0.5f, noseConeLength);
    const DirectX::SimpleMath::Vector3 noseConeTranslation(noseConeXoffset, bodySize.y * 0.25f, 0.0f);
    m_heliModel.noseConeShape = DirectX::GeometricPrimitive::CreateBox(aContext.Get(), noseConeSize);
    m_heliModel.noseConeMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.noseConeMatrix *= DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(45.0));
    m_heliModel.noseConeMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(noseConeTranslation);
    m_heliModel.localNoseConeMatrix = m_heliModel.noseConeMatrix;

    /// nose body
    float noseBodyXSize = 0.7;
    DirectX::SimpleMath::Vector3 noseBodySize(noseBodyXSize, bodySize.y * 0.5f, bodySize.z);
    const DirectX::SimpleMath::Vector3 noseBodyTranslation(bodySize.x * 0.5f + (noseBodySize.x * 0.5), bodySize.y * 0.25f, 0.0f);
    m_heliModel.noseBodyShape = DirectX::GeometricPrimitive::CreateBox(aContext.Get(), noseBodySize);
    m_heliModel.noseBodyMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.noseBodyMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(noseBodyTranslation);
    m_heliModel.localNoseBodyMatrix = m_heliModel.noseBodyMatrix;

    /// Main body rear
    const DirectX::SimpleMath::Vector3 bodyRearTranslation(-bodySize.x * 0.5f, -bodySize.y * 0.0f, 0.0f);
    m_heliModel.bodyRearMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.bodyRearMatrix = DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(45.0f));
    m_heliModel.bodyRearMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(bodyTranslation);
    m_heliModel.bodyRearMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(bodyRearTranslation);
    m_heliModel.localBodyRearMatrix = m_heliModel.bodyRearMatrix;

    // upper engine housing body
    const DirectX::SimpleMath::Vector3 engineHousingSize(3.0f, 1.0f, 1.0f);
    const DirectX::SimpleMath::Vector3 engineHousingTranslation(-engineHousingSize.x * 0.4f, (bodySize.y * 1.0f) + (engineHousingSize.y * 0.0f), 0.0f);
    m_heliModel.engineHousingShape = DirectX::GeometricPrimitive::CreateBox(aContext.Get(), engineHousingSize);
    m_heliModel.engineHousingMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.engineHousingMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(engineHousingTranslation);
    m_heliModel.localEngineHousingMatrix = m_heliModel.engineHousingMatrix;

    // upper front engine housing body
    const float engineHousingFrontSize = (engineHousingSize.z * 0.5) * sqrt(2);
    const DirectX::SimpleMath::Vector3 engineHousingFrontTranslation(engineHousingSize.x * 0.5f, -engineHousingSize.y * 0.0f, 0.0f);
    m_heliModel.engineHousingFrontShape = DirectX::GeometricPrimitive::CreateOctahedron(aContext.Get(), engineHousingFrontSize);
    m_heliModel.engineHousingFrontMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.engineHousingFrontMatrix = DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(45.0f));
    m_heliModel.engineHousingFrontMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(engineHousingTranslation);
    m_heliModel.engineHousingFrontMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(engineHousingFrontTranslation);
    m_heliModel.localEngineHousingFrontMatrix = m_heliModel.engineHousingFrontMatrix;

    // tail boom
    const DirectX::SimpleMath::Vector3 tailBoomSize(7.5f, 1.0f, 0.5f);
    const DirectX::SimpleMath::Vector3 tailBoomTranslation(-bodySize.x, bodySize.y - tailBoomSize.y, 0.0f);
    m_heliModel.tailBoomShape = DirectX::GeometricPrimitive::CreateBox(aContext.Get(), tailBoomSize);
    m_heliModel.tailBoomMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.tailBoomMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(-4.0));
    m_heliModel.tailBoomMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(tailBoomTranslation);
    m_heliModel.localTailBoomMatrix = m_heliModel.tailBoomMatrix;

    // tail fin
    const DirectX::SimpleMath::Vector3 tailFinSize(1.0f, 1.8f, tailBoomSize.z * 0.6f);
    const DirectX::SimpleMath::Vector3 tailFinTranslation(tailBoomTranslation.x - (tailBoomSize.x * 0.546f) + (tailFinSize.x * 0.5f), tailBoomTranslation.y + (tailBoomSize.y * 1.0f), 0.0f);
    const DirectX::SimpleMath::Matrix tailFinRotation = DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(20.0f));
    m_heliModel.tailFinShape = DirectX::GeometricPrimitive::CreateBox(aContext.Get(), tailFinSize);
    m_heliModel.tailFinMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.tailFinMatrix *= tailFinRotation;
    m_heliModel.tailFinMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(tailFinTranslation);
    m_heliModel.localTailFinMatrix = m_heliModel.tailFinMatrix;

    // tail wing
    const DirectX::SimpleMath::Vector3 tailWingSize(0.7f, 0.2f, tailBoomSize.z * 4.4f);
    const DirectX::SimpleMath::Vector3 tailWingTranslation(tailBoomTranslation.x - (tailBoomSize.x * 0.18f) + (tailWingSize.x * 0.5f), tailBoomTranslation.y + (tailBoomSize.y * .1f), 0.0f);
    const DirectX::SimpleMath::Matrix tailWingRotation = DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(0.0f));
    m_heliModel.tailWingShape = DirectX::GeometricPrimitive::CreateBox(aContext.Get(), tailWingSize);
    m_heliModel.tailFinMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.tailWingMatrix *= tailWingRotation;
    m_heliModel.tailWingMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(tailWingTranslation);
    m_heliModel.localTailWingMatrix = m_heliModel.tailWingMatrix;


    // main rotor axel
    const float mainAxelLength = 0.5f;
    const float mainAxelDiameter = 0.2f;
    const DirectX::SimpleMath::Vector3 mainAxelTranslation(0.0f, bodySize.y + (engineHousingSize.y * 0.5f) + (mainAxelLength * 0.5f), 0.0f);
    m_heliModel.mainRotorAxelShape = DirectX::GeometricPrimitive::CreateCylinder(aContext.Get(), mainAxelLength, mainAxelDiameter, 16);
    m_heliModel.mainRotorAxelMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.mainRotorAxelMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(mainAxelTranslation);
    m_heliModel.localMainRotorAxelMatrix = m_heliModel.mainRotorAxelMatrix;

    // main rotor hub
    const float mainHubLength = 0.5f;
    const float mainHubDiameter = 0.35f;
    const DirectX::SimpleMath::Vector3 mainHubTranslation(0.0f, mainAxelTranslation.y + mainHubLength, 0.0f);
    m_heliModel.mainRotorHubShape = DirectX::GeometricPrimitive::CreateCylinder(aContext.Get(), mainHubLength, mainHubDiameter, 16);
    m_heliModel.mainRotorHubMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.mainRotorHubMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(mainHubTranslation);
    m_heliModel.localMainRotorHubMatrix = m_heliModel.mainRotorHubMatrix;
    //
    DirectX::SimpleMath::Vector3 mainRotorPos = DirectX::SimpleMath::Vector3::Zero;
    mainRotorPos = mainRotorPos.Transform(mainRotorPos, m_heliModel.mainRotorHubMatrix);
    m_heli.mainRotorPos = mainRotorPos;

    // tail rotor axel
    const float tailAxelLength = 0.35f;
    const float tailAxelDiameter = 0.1f;
    const float tailAxelOffsetY = 0.25f;
    //const DirectX::SimpleMath::Vector3 tailAxelTranslation(tailBoomTranslation.x * 1.0 - (tailBoomSize.x * 0.425f), tailBoomTranslation.y + tailAxelOffsetY, -tailBoomSize.z * 0.5f);
    const DirectX::SimpleMath::Vector3 tailAxelTranslation(tailFinTranslation.x * 1.02f, tailFinTranslation.y * 1.15f, (-tailFinSize.z * 0.5f) - (tailAxelLength * 0.5f));
    m_heliModel.tailRotorAxelShape = DirectX::GeometricPrimitive::CreateCylinder(aContext.Get(), tailAxelLength, tailAxelDiameter, 16);
    m_heliModel.tailRotorAxelMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.tailRotorAxelMatrix *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(-90.0f));
    m_heliModel.tailRotorAxelMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(tailAxelTranslation);
    m_heliModel.localTailRotorAxelMatrix = m_heliModel.tailRotorAxelMatrix;

    // tail rotor hub
    const float tailHubLength = 0.25f;
    const float tailHubDiameter = 0.15f;
    const DirectX::SimpleMath::Vector3 tailHubTranslation(tailAxelTranslation.x, tailAxelTranslation.y, tailAxelTranslation.z - tailHubLength);
    m_heliModel.tailRotorHubShape = DirectX::GeometricPrimitive::CreateCylinder(aContext.Get(), tailHubLength, tailHubDiameter, 16);
    m_heliModel.tailRotorHubMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.tailRotorHubMatrix *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(-90.0f));
    m_heliModel.tailRotorHubMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(tailHubTranslation);
    m_heliModel.localTailRotorHubMatrix = m_heliModel.tailRotorHubMatrix;
    //
    DirectX::SimpleMath::Vector3 tailRotorPos = DirectX::SimpleMath::Vector3::Zero;
    tailRotorPos = tailRotorPos.Transform(tailRotorPos, m_heliModel.mainRotorHubMatrix);
    m_heli.tailRotorPos = tailRotorPos;

    // tail rotor arm
    const float tailArmLength = 2.65f;
    const float tailArmDiameter = 0.1f;
    const DirectX::SimpleMath::Vector3 tailArmTranslation(tailHubTranslation.x, tailHubTranslation.y, tailHubTranslation.z);
    m_heliModel.tailRotorArmShape = DirectX::GeometricPrimitive::CreateCylinder(aContext.Get(), tailArmLength, tailArmDiameter);
    m_heliModel.tailRotorArmMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.tailRotorArmMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(0.0f));
    m_heliModel.tailRotorArmMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(tailArmTranslation);
    m_heliModel.localTailRotorArmMatrix = m_heliModel.tailRotorArmMatrix;

    // main rotor arm
    const float mainArmLength = 14.0f;
    const float mainArmDiameter = 0.15f;    
    m_heliModel.mainRotorArmShape = DirectX::GeometricPrimitive::CreateCylinder(aContext.Get(), mainArmLength, mainArmDiameter);
    const DirectX::SimpleMath::Vector3 rotorArmTranslation(0.0f, mainHubTranslation.y, 0.0f);
    m_heliModel.mainRotorArmMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.mainRotorArmMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(90.0f));
    m_heliModel.mainRotorArmMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(rotorArmTranslation);
    m_heliModel.localMainRotorArmMatrix = m_heliModel.mainRotorArmMatrix;

    // main rotor blade 1
    const float mainBladeWidth = 0.8f;
    DirectX::SimpleMath::Vector3 rotorBladeSize(mainArmLength * 0.47f, mainArmDiameter, mainBladeWidth);
    const DirectX::SimpleMath::Vector3 rotorBladeTranslation1(((mainArmLength * 0.5f) - (rotorBladeSize.x * 0.5f)), mainHubTranslation.y, 0.0f);
    m_heliModel.mainRotorBladeShape = DirectX::GeometricPrimitive::CreateBox(aContext.Get(), rotorBladeSize);
    m_heliModel.mainRotorBladeMatrix1 = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.mainRotorBladeMatrix1 *= DirectX::SimpleMath::Matrix::CreateTranslation(rotorBladeTranslation1);
    m_heliModel.localMainRotorBladeMatrix1 = m_heliModel.mainRotorBladeMatrix1;
    m_heliModel.mainRotorBladeTranslationMatrix1 = DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(0.0f, 0.0f, rotorBladeSize.z * 0.5f));

    // main rotor blade 2
    const DirectX::SimpleMath::Vector3 rotorBladeTranslation2(((-mainArmLength * 0.5f) + (rotorBladeSize.x * 0.5f)), mainHubTranslation.y, 0.0f);
    m_heliModel.mainRotorBladeMatrix2 = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.mainRotorBladeMatrix2 *= DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(180.0f));
    m_heliModel.mainRotorBladeMatrix2 *= DirectX::SimpleMath::Matrix::CreateTranslation(rotorBladeTranslation2);
    m_heliModel.localMainRotorBladeMatrix2 = m_heliModel.mainRotorBladeMatrix2;
    m_heliModel.mainRotorBladeTranslationMatrix2 = DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(0.0f, 0.0f, rotorBladeSize.z * 0.5f));

    // tail rotor blade 1
    const float tailBladeWidth = 0.3f;
    DirectX::SimpleMath::Vector3 tailBladeSize(tailBladeWidth, tailArmLength * 0.42f, tailArmDiameter);
    const DirectX::SimpleMath::Vector3 tailBladeTranslation1(tailBladeWidth * 0.5, (tailArmLength * 0.5) - (tailBladeSize.y * 0.5), 0.0);
    m_heliModel.tailRotorBladeShape = DirectX::GeometricPrimitive::CreateBox(aContext.Get(), tailBladeSize);
    m_heliModel.tailRotorBladeMatrix1 = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.tailRotorBladeMatrix1 *= DirectX::SimpleMath::Matrix::CreateTranslation(tailBladeTranslation1);
    m_heliModel.localTailRotorBladeMatrix1 = m_heliModel.tailRotorBladeMatrix1;
    m_heliModel.tailRotorBladeTranslationMatrix1 = DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(tailAxelLength, 0.0, 0.0));
    m_heliModel.tailRotorBladeTranslationMatrix1 = DirectX::SimpleMath::Matrix::CreateTranslation(tailArmTranslation);

    // tail rotor blade 2
    const DirectX::SimpleMath::Vector3 tailBladeTranslation2(-tailBladeWidth * 0.5, -(tailArmLength * 0.5) + (tailBladeSize.y * 0.5), 0.0);
    m_heliModel.tailRotorBladeMatrix2 = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.tailRotorBladeMatrix2 *= DirectX::SimpleMath::Matrix::CreateTranslation(tailBladeTranslation2);
    m_heliModel.localTailRotorBladeMatrix2 = m_heliModel.tailRotorBladeMatrix2;
    m_heliModel.tailRotorBladeTranslationMatrix2 = DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(tailAxelLength, 0.0, 0.0));
    m_heliModel.tailRotorBladeTranslationMatrix2 = DirectX::SimpleMath::Matrix::CreateTranslation(tailArmTranslation);

}

void Vehicle::InitializeVehicle(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext)
{
    // helicopter data
    m_heli.collectiveInput = 0.0f;
    m_heli.cyclicInputPitch = 0.0f;
    m_heli.cyclicInputPitchIsPressed = false;
    m_heli.cyclicInputRoll = 0.0f;
    m_heli.cyclicInputRollIsPressed = false;
    m_heli.hThrottleInput = 0.0f;
    m_heli.yawPedalInput = 0.0f;
    m_heli.yawPedalIsPressed = false;
    m_heli.mainRotorRPM = 0.0f;
    // 
    m_heli.q.mainRotorForceNormal = DirectX::SimpleMath::Vector3::UnitY;
    m_heli.q.mainRotorForceMagnitude = 15.0f;

    // roughly based on porsche boxster
    m_heli.mass = 1393.0f;
    m_heli.area = 1.94f;
    m_heli.density = 1.2f;
    m_heli.Cd = 0.31f;
    m_heli.redline = 7200.f;
    m_heli.revlimit = 7800.f;
    m_heli.finalDriveRatio = 3.44f;
    m_heli.wheelRadius = 0.3186f;
    m_heli.wheelWidth = 0.235f;
    m_heli.numberOfGears = 7;
    m_heli.muR = 0.015f;             //  coefficient of rolling friction
    m_heli.airResistance = 0.0f;
    //m_heli.airDensity = 1.225; // ToDo : pull air density from environment data
    m_heli.totalResistance = m_heli.muR + m_heli.airResistance;
    m_heli.omegaE = 1000.0;         //  engine rpm
    m_heli.gearNumber = 2;          //  gear the car is in
    m_heli.gearRatio[0] = -3.82f;
    m_heli.gearRatio[1] = 0.0f;
    m_heli.gearRatio[2] = 3.82f;
    m_heli.gearRatio[3] = 2.20f;
    m_heli.gearRatio[4] = 1.52f;
    m_heli.gearRatio[5] = 1.22f;
    m_heli.gearRatio[6] = 1.02f;
    m_heli.gearRatio[7] = 0.84f;
    m_heli.wheelMass = 22.68f;    // Total guess, not confirmed with porsche or volvo stats

    m_heli.gravity = DirectX::SimpleMath::Vector3(0.0, -9.81, 0.0);
    m_heli.numEqns = 6;
    m_heli.time = 0.0;  
    m_heli.q.position = DirectX::SimpleMath::Vector3::Zero;
    m_heli.q.velocity = DirectX::SimpleMath::Vector3::Zero;
    m_heli.q.bodyVelocity = DirectX::SimpleMath::Vector3::Zero;
    m_heli.q.brakeForce = DirectX::SimpleMath::Vector3::Zero;
    m_heli.q.slopeForce = DirectX::SimpleMath::Vector3::Zero;
    m_heli.q.airResistance = DirectX::SimpleMath::Vector3::Zero;
    m_heli.q.gravityForce = DirectX::SimpleMath::Vector3::Zero;
    m_heli.q.engineForce = DirectX::SimpleMath::Vector3::Zero;
    m_heli.q.totalVelocity = DirectX::SimpleMath::Vector3::Zero;

    m_heli.inputDeadZone = 0.05;
    m_heli.throttleInput = 0.0;
    m_heli.brakeInput = 0.0;
    m_heli.maxThrottleInput = 1.0;
    m_heli.maxThrottleRate = 1.0;
    m_heli.maxBrakeInput = 1.0;
    m_heli.maxBrakeRate = 15.0;

    m_heli.brakeDecayRate = 1.2;
    m_heli.throttleDecayRate = 1.2;
    m_heli.steeringAngleDecay = -0.3;
    m_heli.steeringSpeed = 0.1;

    m_heli.carRotation = 0.0;
    m_heli.steeringAngle = Utility::ToRadians(0.0);
    m_heli.steeringAngleMax = Utility::ToRadians(26.0);
    m_heli.headingVec = -DirectX::SimpleMath::Vector3::UnitZ;
    m_heli.forward = DirectX::SimpleMath::Vector3::UnitX;
    m_heli.up = DirectX::SimpleMath::Vector3::UnitY;
    m_heli.right = m_heli.forward.Cross(m_heli.up);
    m_heli.shiftCooldown = 0.0;
    m_heli.shiftDelay = 0.5;
    m_heli.speed = 0.0;

    m_heli.isClutchPressed = false;
    m_heli.isThrottlePressed = false;
    m_heli.isBrakePressed = false;
    m_heli.isTurningPressed = false;
    m_heli.isAccelerating = false;
    m_heli.isBraking = false;
    m_heli.isRevlimitHit = false;
    m_heli.isTransmissionManual = false;
    m_heli.isCarAirborne = false;
    m_heli.isCarLanding = false;
    m_heli.isVelocityBackwards = false;
    m_heli.wheelBase = 2.41;

    m_heli.terrainHightAtPos = 0.0;

    m_heli.testRearAnglularVelocity = 0.0;

    m_heli.terrainNormal = DirectX::SimpleMath::Vector3::UnitY;
    m_heli.testModelPos = m_heli.q.position;
    m_heli.testModelRotation = m_heli.carRotation;
    m_heli.testTerrainNormal = m_heli.terrainNormal;
    m_heli.testHeadingVec = m_heli.headingVec;



    InitializeModel(aContext);
}

void Vehicle::InputCollective(const float aCollectiveInput)
{
    const float updatedCollective = (aCollectiveInput * m_heli.collectiveInputRate) + m_heli.collectiveInput;
    if (updatedCollective > m_heli.collectiveInputMax)
    {
        m_heli.collectiveInput = m_heli.collectiveInputMax;
    }
    else if (updatedCollective < m_heli.collectiveInputMin)
    {
        m_heli.collectiveInput = m_heli.collectiveInputMin;
    }
    else
    {
        m_heli.collectiveInput = updatedCollective;
    }
}

void Vehicle::InputCyclicPitch(const float aPitchInput)
{
    m_heli.cyclicInputPitchIsPressed = true;
    const float updatedPitch = (aPitchInput * m_heli.cyclicInputRate) + m_heli.cyclicInputPitch;
    if (updatedPitch > m_heli.cyclicInputMax)
    {
        m_heli.cyclicInputPitch = m_heli.cyclicInputMax;
    }
    else if (updatedPitch < m_heli.cyclicInputMin)
    {
        m_heli.cyclicInputPitch = m_heli.cyclicInputMin;
    }
    else
    {
        m_heli.cyclicInputPitch = updatedPitch;
    }
}

void Vehicle::InputCyclicRoll(const float aRollInput)
{
    m_heli.cyclicInputRollIsPressed = true;
    const float updatedRoll = (aRollInput * m_heli.cyclicInputRate) + m_heli.cyclicInputRoll;
    if (updatedRoll > m_heli.cyclicInputMax)
    {
        m_heli.cyclicInputRoll = m_heli.cyclicInputMax;
    }
    else if (updatedRoll < m_heli.cyclicInputMin)
    {
        m_heli.cyclicInputRoll = m_heli.cyclicInputMin;
    }
    else
    {
        m_heli.cyclicInputRoll = updatedRoll;
    }
}

void Vehicle::InputDecay(const double aTimeDelta)
{
    const float timeDelta = static_cast<float>(aTimeDelta);

    if (m_heli.cyclicInputPitchIsPressed == false)
    {
        // Cyclic Pitch Decay
        if (m_heli.cyclicInputPitch > 0.0f)
        {
            if (m_heli.cyclicInputPitch - (m_heli.cyclicDecayRate * timeDelta) < 0.0f)
            {
                m_heli.cyclicInputPitch = 0.0f;
            }
            else
            {
                m_heli.cyclicInputPitch -= m_heli.cyclicDecayRate * timeDelta;
            }
        }
        else if (m_heli.cyclicInputPitch < 0.0f)
        {
            if (m_heli.cyclicInputPitch + (m_heli.cyclicDecayRate * timeDelta) > 0.0f)
            {
                m_heli.cyclicInputPitch = 0.0f;
            }
            else
            {
                m_heli.cyclicInputPitch += m_heli.cyclicDecayRate * timeDelta;
            }
        }
        else
        {
            m_heli.cyclicInputPitch = 0.0f;
        }
    }
    // Cyclic Roll Decay
    if (m_heli.cyclicInputRollIsPressed == false)
    {
        if (m_heli.cyclicInputRoll > 0.0f)
        {
            if (m_heli.cyclicInputRoll - (m_heli.cyclicDecayRate * timeDelta) < 0.0f)
            {
                m_heli.cyclicInputRoll = 0.0f;
            }
            else
            {
                m_heli.cyclicInputRoll -= m_heli.cyclicDecayRate * timeDelta;
            }
        }
        else if (m_heli.cyclicInputRoll < 0.0f)
        {
            if (m_heli.cyclicInputRoll + (m_heli.cyclicDecayRate * timeDelta) > 0.0f)
            {
                m_heli.cyclicInputRoll = 0.0f;
            }
            else
            {
                m_heli.cyclicInputRoll += m_heli.cyclicDecayRate * timeDelta;
            }
        }
        else
        {
            m_heli.cyclicInputRoll = 0.0f;
        }
    }
    // Yaw Pedal Decay
    if (m_heli.yawPedalIsPressed == false)
    {
        if (m_heli.yawPedalInput - (m_heli.yawPedalDecayRate * timeDelta) < 0.0f)
        {
            if (m_heli.yawPedalInput - (m_heli.yawPedalDecayRate * timeDelta) > m_heli.inputDeadZone)
            {
                m_heli.yawPedalInput = 0.0f;
            }
            else
            {
                m_heli.yawPedalInput += m_heli.yawPedalDecayRate * timeDelta;
            }
        }
        else if (m_heli.yawPedalInput - (m_heli.yawPedalDecayRate * timeDelta) > 0.0f)
        {
            if (m_heli.yawPedalInput + (m_heli.yawPedalDecayRate * timeDelta) < m_heli.inputDeadZone)
            {
                m_heli.yawPedalInput = 0.0f;
            }
            else
            {
                m_heli.yawPedalInput -= m_heli.yawPedalDecayRate * timeDelta;
            }
        }
    }

    m_heli.cyclicInputPitchIsPressed = false;
    m_heli.cyclicInputRollIsPressed = false;
    m_heli.yawPedalIsPressed = false;
}

void Vehicle::InputHThrottle(const float aThrottleInput)
{
    const float updatedThrottle = (aThrottleInput * m_heli.hThrottleInputRate) + m_heli.hThrottleInput;
    if (updatedThrottle > m_heli.hThrottleInputMax)
    {
        m_heli.hThrottleInput = m_heli.hThrottleInputMax;
    }
    else if (updatedThrottle < m_heli.hThrottleInputMin)
    {
        m_heli.hThrottleInput = m_heli.hThrottleInputMin;
    }
    else
    {
        m_heli.hThrottleInput = updatedThrottle;
    }
}

void Vehicle::InputYawPedal(const float aYawInput)
{
    m_heli.yawPedalIsPressed = true;
    const float updatedYaw = (aYawInput * m_heli.yawPedalInputRate) + m_heli.yawPedalInput;
    if (updatedYaw > m_heli.yawPedalInputMax)
    {
        m_heli.yawPedalInput = m_heli.yawPedalInputMax;
    }
    else if (updatedYaw < m_heli.yawPedalInputMin)
    {
        m_heli.yawPedalInput = m_heli.yawPedalInputMin;
    }
    else
    {
        m_heli.yawPedalInput = updatedYaw;
    }
}

void Vehicle::Jump(double aTimer)
{
    float jumpHeight = 10.0;
    m_heli.q.velocity.y += jumpHeight;
}

void Vehicle::LandVehicle()
{   
    DirectX::SimpleMath::Vector3 terrainNorm = m_heli.terrainNormal;
    DirectX::SimpleMath::Vector3 landingVelocity = m_heli.q.velocity;
    DirectX::SimpleMath::Vector3 landingVelocityNorm = landingVelocity;
    landingVelocityNorm.Normalize();

    float impact = landingVelocityNorm.Dot(m_heli.terrainNormal);

    DirectX::SimpleMath::Vector3 updateVelocity;
    updateVelocity.x = m_heli.q.velocity.x * abs(impact);
    updateVelocity.y = m_heli.q.velocity.y * abs(impact);
    updateVelocity.z = m_heli.q.velocity.z * abs(impact);

    updateVelocity = m_heli.q.velocity * -impact;
    updateVelocity = m_heli.q.velocity;
    updateVelocity.y = 0.0;
    
    ///////////////////////////////
    // Testing new landing equation, from golf ball
    //  ::: Vnew = b * (-2 * (V dot N) * N + V)
    // const float b = .3f;
    // DirectX::SimpleMath::Vector3 terrainNorm = pBallEnvironment->GetTerrainNormal(GetBallPosInEnviron(m_ball.q.position));
    // m_ball.q.velocity = b * (-2 * (preVelocity.Dot(terrainNorm)) * terrainNorm + preVelocity);
    DirectX::SimpleMath::Vector3 v = m_heli.q.velocity;
    DirectX::SimpleMath::Vector3 n = m_heli.terrainNormal;
    const float b = .9f;

    DirectX::SimpleMath::Vector3 testUpdateVel = b * (-2 * (v.Dot(n)) * n + v);
    testUpdateVel.y = 0.0;

    m_heli.q.velocity = testUpdateVel;
}

void Vehicle::PressBrake(const float aBrakeInput)
{
    m_testIsBreakLightOn = true;
    //if(m_heli.brakeInput > 0.0)
    if (aBrakeInput > 0.0)
    {
        m_heli.isBrakePressed = true;
    }
    float updatedBrake = aBrakeInput + m_heli.brakeInput;
    if (updatedBrake > m_heli.maxBrakeInput)
    {
        m_heli.brakeInput = m_heli.maxBrakeInput;
    }
    else
    {
        m_heli.brakeInput += aBrakeInput;
    }
}

void Vehicle::PressClutch(const bool aClutchInput)
{
    m_heli.isClutchPressed = aClutchInput;
}

void Vehicle::PressThrottle(const float aThrottleInput)
{
    if (aThrottleInput > 0.0)
    {
        m_heli.isThrottlePressed = true;
    }
    float updatedThrottle = aThrottleInput + m_heli.throttleInput;
    if (updatedThrottle > m_heli.maxThrottleInput)
    {
        m_heli.throttleInput = m_heli.maxThrottleInput;
    }
    else
    {
        m_heli.throttleInput += aThrottleInput;
    }
}

void Vehicle::ResetVehicle()
{
    m_heli.gearNumber = 1;         

    m_heli.steeringAngle = 0.0;
    m_heli.q.position = DirectX::SimpleMath::Vector3::Zero;
    m_heli.headingVec = DirectX::SimpleMath::Vector3::UnitX;
    m_heli.forward = DirectX::SimpleMath::Vector3::UnitX;
    m_heli.up = DirectX::SimpleMath::Vector3::UnitY;
    m_heli.right = DirectX::SimpleMath::Vector3::UnitZ;
    m_heli.speed = 0.0;
    m_heli.q.velocity = DirectX::SimpleMath::Vector3::Zero;
}

void Vehicle::RevLimiter()
{
    if (m_heli.omegaE < m_heli.redline)
    {
        m_heli.isRevlimitHit = false;
    }
    if (m_heli.omegaE > m_heli.revlimit)
    {
        m_heli.isRevlimitHit = true;
    }
    if (m_heli.isRevlimitHit == true)
    {
        m_heli.isThrottlePressed = false;
        m_heli.throttleInput = 0.0;
    }
}

//*************************************************************
//  This method loads the right-hand sides for the car ODEs
//*************************************************************
void Vehicle::RightHandSide(struct HeliData* aCar, Motion* aQ, Motion* aDeltaQ, double aTimeDelta, float aQScale, Motion* aDQ)
{
    //  Compute the intermediate values of the 
    //  dependent variables.
    Motion newQ;
    newQ.velocity = aQ->velocity + static_cast<float>(aQScale) * aDeltaQ->velocity;
    newQ.position = aQ->position + static_cast<float>(aQScale) * aDeltaQ->position;

    DirectX::SimpleMath::Vector3 rotorForce = aQ->mainRotorForceNormal * aQ->mainRotorForceMagnitude * m_heli.collectiveInput;
    DebugPushUILineDecimalNumber("rotorForce ", rotorForce.y, "");

    newQ.velocity += rotorForce + static_cast<float>(aQScale) * aDeltaQ->velocity;

    //  Compute the constants that define the
    //  torque curve line.
    float powerCurve;
    float torque; // in newton meters
    float omegaE = aCar->omegaE;

    if (omegaE <= 1000.0)
    {
        powerCurve = 0.0;
        torque = 220.0;
    }
    else if (omegaE < 4600.0)
    {
        powerCurve = 0.025;
        torque = 195.0;
    }
    else
    {
        powerCurve = -0.032;
        torque = 457.2;
    }

    if (m_isFuelOn == false)
    {
        torque = 0.0;
    }

    //  Compute the velocity magnitude. The 1.0e-8 term
    //  ensures there won't be a divide by zero later on
    //  if all of the velocity components are zero.
    float v = sqrt(newQ.velocity.Length() * newQ.velocity.Length()) + 1.0e-8f;

    //  Compute the total drag force.
    float airDensity = aCar->density;
    float dragCoefficient = aCar->Cd;
    float frontSurfaceArea = aCar->area;
    float frontDragResistance = 0.5f * airDensity * frontSurfaceArea * dragCoefficient * v * v;

    //  Compute the force of rolling friction. Because
    //  the G constant has a negative sign, the value 
    //  computed here will be negative
    float gravity = aCar->gravity.y;
    float rollingFrictionCoefficient = aCar->muR;
    float mass = aCar->mass;
    float rollingFriction = rollingFrictionCoefficient * mass * gravity;
    rollingFriction = rollingFrictionCoefficient * mass * (m_heli.terrainNormal * m_heli.gravity).y;
    //  Compute the right-hand sides of the six ODEs
    //  newQ[0] is the intermediate value of velocity.
    //  The acceleration of the car is determined by 
    //  whether the car is accelerating, cruising, or
    //  braking.   
    // Accelerating
    int gearNumber = aCar->gearNumber;
    float gearRatio = aCar->gearRatio[gearNumber];
    float finalDriveRatio = aCar->finalDriveRatio;
    float wheelRadius = aCar->wheelRadius;
    float pi = acos(-1.0f);

    DirectX::SimpleMath::Vector3 headingVec = m_heli.forward;

    float c1 = -frontDragResistance / mass;
    float tmp = gearRatio * finalDriveRatio / wheelRadius;
    float testTmp = ((gearRatio * aCar->throttleInput) * finalDriveRatio) / wheelRadius;
    tmp = testTmp;
    float c2 = 60.0f * tmp * tmp * powerCurve * v / (2.0f * pi * mass);
    float c3 = (tmp * torque + rollingFriction) / mass;
    float c4 = headingVec.Dot(m_heli.terrainNormal * m_heli.gravity);

    DirectX::SimpleMath::Vector3 velocityUpdate = (static_cast<float>(aTimeDelta) * (c1 + c2 + c3 + c4)) * headingVec;

    DirectX::SimpleMath::Vector3 slopeForce = m_heli.forward * m_heli.forward.Dot(m_heli.terrainNormal * m_heli.gravity);
    slopeForce = slopeForce * static_cast<float>(aTimeDelta);

    DirectX::SimpleMath::Vector3 velocityNorm = m_heli.q.velocity;
    velocityNorm.Normalize();

    DirectX::SimpleMath::Vector3 airResistance = velocityNorm * (static_cast<float>(aTimeDelta) * (-frontDragResistance / mass));
    DirectX::SimpleMath::Vector3 brakeForce = (static_cast<float>(aTimeDelta) * ((-aCar->brakeInput * aCar->maxBrakeRate))) * velocityNorm;
    if (m_heli.isVelocityBackwards == true)
    {
        rollingFriction *= -1.0;
    }

    DirectX::SimpleMath::Vector3 engineForce = (static_cast<float>(aTimeDelta) * (c2 + ((tmp * torque + rollingFriction) / mass))) * headingVec;
    DirectX::SimpleMath::Vector3 rollingResistance = (static_cast<float>(aTimeDelta) * (rollingFriction) / mass) * velocityNorm;

    float testEngineForce = (static_cast<float>(aTimeDelta) * (c2 + ((tmp * torque + rollingFriction) / mass)));
    testEngineForce = (static_cast<float>(aTimeDelta) * (c2 + ((tmp * torque) / mass)));
    testEngineForce = (static_cast<float>(aTimeDelta) * (c2 + ((tmp * torque))));
    testEngineForce = (static_cast<float>(aTimeDelta) * (((tmp * torque))));

    if (testEngineForce > m_testEnginePower)
    {
        m_testEnginePower = testEngineForce;
    }

    if (aCar->isClutchPressed == true)
    {
        engineForce = (static_cast<float>(aTimeDelta) * (c2 + ((rollingFriction) / mass))) * headingVec;
    }

    velocityUpdate = (static_cast<float>(aTimeDelta) * (c1 + c2 + c3 + c4)) * headingVec;
    if (m_heli.throttleInput > 0.0 || m_isFuelOn == false)
    {
        aDQ->velocity = velocityUpdate;
    }
    else if (m_heli.brakeInput > 0.0)  // braking 
    {
        //  Only brake if the velocity is positive.
        if (newQ.velocity.Length() > 0.1)
        {
            velocityUpdate = ((static_cast<float>(aTimeDelta) * (c1 + c2 + c3 + c4)) * headingVec) + brakeForce;
            aDQ->velocity = ((static_cast<float>(aTimeDelta) * (c1 + c2 + c3 + c4)) * headingVec) + brakeForce;

        }
        else
        {
            velocityUpdate = DirectX::SimpleMath::Vector3::Zero;
            aDQ->velocity = DirectX::SimpleMath::Vector3::Zero;
        }
    }
    else  // cruise
    {     
        if (newQ.velocity.Length() < 0.000001 && m_heli.throttleInput < 0.01)
        {
            velocityUpdate = DirectX::SimpleMath::Vector3::Zero;
            aDQ->velocity = DirectX::SimpleMath::Vector3::Zero;
            velocityUpdate = (static_cast<float>(aTimeDelta) * (c1 + c4)) * headingVec;
        }
        else
        {
            velocityUpdate = (static_cast<float>(aTimeDelta) * (c1 + c2 + c3 + c4)) * headingVec;
            velocityUpdate = (static_cast<float>(aTimeDelta) * (c1  + c3 + c4)) * headingVec;
            aDQ->velocity = (static_cast<float>(aTimeDelta) * (c1 + c2 + c3 + c4)) * headingVec;
            velocityUpdate = (static_cast<float>(aTimeDelta) * (c1 + c4)) * headingVec;
        }        
    }
    
    DirectX::SimpleMath::Vector3 gravForce = m_heli.gravity * static_cast<float>(aTimeDelta);
    DirectX::SimpleMath::Vector3 terrainNormalForce = (m_heli.terrainNormal * -m_heli.gravity.y) * static_cast<float>(aTimeDelta);

    if (m_heli.isCarAirborne == true)
    {
        terrainNormalForce = DirectX::SimpleMath::Vector3::Zero;
    }

    float gravDown = gravForce.y + terrainNormalForce.y;
    
    velocityUpdate = engineForce + brakeForce + slopeForce + airResistance;
    velocityUpdate.y += gravDown;
    
    
    if (m_heli.shiftCooldown > 0.0)
    {
        velocityUpdate =  brakeForce + slopeForce + airResistance + rollingResistance;
    }

    if (m_heli.isCarAirborne == true)
    {
        velocityUpdate = m_heli.gravity;
        velocityUpdate.x = 0.0;
        velocityUpdate.z = 0.0;
        velocityUpdate.y = gravity * static_cast<float>(aTimeDelta);
        velocityUpdate += rotorForce * static_cast<float>(aTimeDelta);
        gravForce = m_heli.gravity * static_cast<float>(aTimeDelta);

    }

    if (m_heli.isCarLanding == true)
    {
        //velocityUpdate.y = -m_heli.q.velocity.y;
        //velocityUpdate = DirectX::SimpleMath::Vector3::Zero;
    }

    //  Compute right-hand side values.
    //aDQ->engineForce = engineForce;
    aDQ->engineForce = testEngineForce * headingVec;
   
    aDQ->brakeForce = brakeForce;
    aDQ->slopeForce = slopeForce;
    aDQ->airResistance = airResistance;
    aDQ->gravityForce = gravForce;
    aDQ->velocity = velocityUpdate;
    aDQ->totalVelocity = velocityUpdate;
  
    aDQ->position = static_cast<float>(aTimeDelta) * newQ.velocity;

    return;
}

//************************************************************
//  This method solves for the car motion using a
//  4th-order Runge-Kutta solver
//************************************************************
void Vehicle::RungeKutta4(struct HeliData* aHeli, double aTimeDelta)
{
    //  Define a convenience variable to make the
    //  code more readable
    // int numEqns = aCar->numEqns;
    const float numEqns = static_cast<float>(aHeli->numEqns);
    //  Retrieve the current values of the dependent
    //  and independent variables.
    Motion q = aHeli->q;

    Motion dq1;
    Motion dq2;
    Motion dq3;
    Motion dq4;

    if (m_heli.isCarLanding == true)
    {

    }
    if (m_heli.isCarAirborne == false)
    {
        m_heli.q.gravityForce = DirectX::SimpleMath::Vector3::Zero;
    }

    // Compute the four Runge-Kutta steps, The return 
    // value of carRightHandSide method is an array
    // of delta-q values for each of the four steps.
    RightHandSide(aHeli, &q, &q, aTimeDelta, 0.0, &dq1);
    RightHandSide(aHeli, &q, &dq1, aTimeDelta, 0.5, &dq2);
    RightHandSide(aHeli, &q, &dq2, aTimeDelta, 0.5, &dq3);
    RightHandSide(aHeli, &q, &dq3, aTimeDelta, 1.0, &dq4);

    //  Update the dependent and independent variable values
    //  at the new dependent variable location and store the
    //  values in the ODE object arrays.
    aHeli->time = aHeli->time + aTimeDelta;

    DirectX::SimpleMath::Vector3 posUpdate = (dq1.position + 2.0 * dq2.position + 2.0 * dq3.position + dq4.position) / numEqns;
    q.position += posUpdate;

    DirectX::SimpleMath::Vector3 velocityUpdate = (dq1.velocity + 2.0 * dq2.velocity + 2.0 * dq3.velocity + dq4.velocity) / numEqns;
    DirectX::SimpleMath::Vector3 bodyVelocityyUpdate = (dq1.bodyVelocity + 2.0 * dq2.bodyVelocity + 2.0 * dq3.bodyVelocity + dq4.bodyVelocity) / numEqns;
    DirectX::SimpleMath::Vector3 engineVelocityUpdate = (dq1.engineForce + 2.0 * dq2.engineForce + 2.0 * dq3.engineForce + dq4.engineForce) / numEqns;
    DirectX::SimpleMath::Vector3 brakeVelocityUpdate = (dq1.brakeForce + 2.0 * dq2.brakeForce + 2.0 * dq3.brakeForce + dq4.brakeForce) / numEqns;
    DirectX::SimpleMath::Vector3 slopeVelocityUpdate = (dq1.slopeForce + 2.0 * dq2.slopeForce + 2.0 * dq3.slopeForce + dq4.slopeForce) / numEqns;
    DirectX::SimpleMath::Vector3 airResistnaceVelocityUpdate = (dq1.airResistance + 2.0 * dq2.airResistance + 2.0 * dq3.airResistance + dq4.airResistance) / numEqns;
    DirectX::SimpleMath::Vector3 gravityVelocityUpdate = (dq1.gravityForce + 2.0 * dq2.gravityForce + 2.0 * dq3.gravityForce + dq4.gravityForce) / numEqns;
    DirectX::SimpleMath::Vector3 totalVelocityUpdate = (dq1.totalVelocity + 2.0 * dq2.totalVelocity + 2.0 * dq3.totalVelocity + dq4.totalVelocity) / numEqns;

    const float stopTolerance = 0.1;
    // To prevent the car from continuing to roll forward if car velocity is less thatn the tollerance value and update velocity is zero
    if (q.velocity.Length() < stopTolerance && velocityUpdate == DirectX::SimpleMath::Vector3::Zero)
    {
        //q.velocity = DirectX::SimpleMath::Vector3::Zero;
    }
    else
    {
        //q.velocity += velocityUpdate;
    }
    q.velocity += velocityUpdate;

    q.engineForce += engineVelocityUpdate;
    q.brakeForce += brakeVelocityUpdate;
    q.airResistance += airResistnaceVelocityUpdate;
    q.gravityForce += gravityVelocityUpdate;
    q.slopeForce += slopeVelocityUpdate;
    q.totalVelocity += velocityUpdate;

    aHeli->q.engineForce = q.engineForce;
    aHeli->q.position = q.position;
    aHeli->q.velocity = q.velocity;

    aHeli->q = q;

    return;
}

void Vehicle::SetEnvironment(Environment* aEnviron)
{
    m_environment = aEnviron;
}

void Vehicle::SteeringInputDecay(const double aTimeDelta)
{
    if (m_heli.isTurningPressed == false)
    {
        const float timeDelta = static_cast<float>(aTimeDelta);
        if (m_heli.steeringAngle != 0.0)
        {
            if (m_heli.steeringAngle > 0.0)
            {
                if (m_heli.steeringAngle + (m_heli.steeringAngleDecay * timeDelta) < 0.0)
                {
                    m_heli.steeringAngle = 0.0;
                }
                else
                {
                    m_heli.steeringAngle += m_heli.steeringAngleDecay * timeDelta;
                }
            }
            else
            {
                if (m_heli.steeringAngle - (m_heli.steeringAngleDecay * timeDelta) > 0.0)
                {
                    m_heli.steeringAngle = 0.0;
                }
                else
                {
                    m_heli.steeringAngle -= m_heli.steeringAngleDecay * timeDelta;
                }
            }
        }
    }
}

void Vehicle::ToggleBrake()
{
    if (m_heli.isBraking == true)
    {
        m_heli.isBraking = false;
    }
    else
    {
        m_heli.isBraking = true;
    }
}

void Vehicle::ToggleFuel()
{
    if (m_isFuelOn == true)
    {
        m_isFuelOn = false;
    }
    else
    {
        m_isFuelOn = true;
    }
}

void Vehicle::ToggleGas()
{
    if (m_heli.isAccelerating == true)
    {
        m_heli.isAccelerating = false;
    }
    else
    {
        m_heli.isAccelerating = true;
    }
}

void Vehicle::TurnInput(float aTurnInput)
{
    m_heli.isTurningPressed = true;

    m_heli.steeringAngle += aTurnInput * m_heli.steeringSpeed;

    if (m_heli.steeringAngle > m_heli.steeringAngleMax)
    {
        m_heli.steeringAngle = m_heli.steeringAngleMax;
    }
    else if (m_heli.steeringAngle < -m_heli.steeringAngleMax)
    {
        m_heli.steeringAngle = - m_heli.steeringAngleMax;
    }
}

void Vehicle::TurnVehicle(double aTimeDelta)
{  
    if (m_heli.isCarAirborne == false)
    {
        if (m_heli.isVelocityBackwards == false)
        {
            m_heli.carRotation -= GetYawRate(aTimeDelta);
        }
        else
        {
            m_heli.carRotation += GetYawRate(aTimeDelta);
        }
    }
    else
    {
        m_heli.carRotation += GetYawRate(aTimeDelta);
    }
    m_heli.carRotation = Utility::WrapAngle(m_heli.carRotation);
}

void Vehicle::ThrottleBrakeDecay(const double aTimeDelta)
{
    const float timeDelta = static_cast<float>(aTimeDelta);
    if (m_heli.isThrottlePressed == false)
    {
        if (m_heli.throttleInput - (m_heli.throttleDecayRate * timeDelta) < 0.0)
        {
            m_heli.throttleInput = 0.0;
        }
        else
        {
            m_heli.throttleInput -= m_heli.throttleDecayRate * timeDelta;
        }
    }

    if (m_heli.isBrakePressed == false)
    {
        if (m_heli.brakeInput - (m_heli.brakeDecayRate * timeDelta) < 0.0)
        {
            m_heli.brakeInput = 0.0;
        }
        else
        {
            m_heli.brakeInput -= m_heli.brakeDecayRate * timeDelta;
        }
    }
}

void Vehicle::UpdateCarAlignment()
{
    // Set up to match terrain norm until car jump is implemented
    if (m_heli.isCarAirborne == false)
    {
        DirectX::SimpleMath::Vector3 newUp = m_heli.terrainNormal;
        DirectX::SimpleMath::Vector3 oldUp = m_heli.up;
        DirectX::SimpleMath::Vector3 updateUp = DirectX::SimpleMath::Vector3::SmoothStep(oldUp, newUp, 0.2);
        updateUp.Normalize();
        m_heli.up = updateUp;
    }
    m_heli.forward = DirectX::SimpleMath::Vector3::TransformNormal(DirectX::SimpleMath::Vector3::UnitX, DirectX::SimpleMath::Matrix::CreateFromAxisAngle(m_heli.up, m_heli.carRotation));
    //m_heli.forward = DirectX::SimpleMath::Vector3::TransformNormal(m_heli.forward, DirectX::SimpleMath::Matrix::CreateFromAxisAngle(m_heli.up, m_heli.carRotation));
    m_heli.right = m_heli.forward.Cross(m_heli.up);
    m_heli.right.Normalize();
    m_heli.forward = m_heli.up.Cross(m_heli.right);
}

void Vehicle::UpdateHeadingVec()
{
    m_heli.headingVec = m_heli.forward;
}

void Vehicle::UpdateModel(const double aTimer)
{
    DirectX::SimpleMath::Matrix updateMat = DirectX::SimpleMath::Matrix::Identity;
    updateMat *= DirectX::SimpleMath::Matrix::CreateWorld(m_heli.q.position, -m_heli.right, m_heli.up);

    m_heliModel.noseConeMatrix = m_heliModel.localNoseConeMatrix;
    m_heliModel.noseConeMatrix *= updateMat;

    m_heliModel.noseBodyMatrix = m_heliModel.localNoseBodyMatrix;
    m_heliModel.noseBodyMatrix *= updateMat;

    m_heliModel.bodyMatrix = m_heliModel.localBodyMatrix;
    m_heliModel.bodyMatrix *= updateMat;

    m_heliModel.windShieldMatrix = m_heliModel.localWindShieldMatrix;
    m_heliModel.windShieldMatrix *= updateMat;

    m_heliModel.bodyRearMatrix = m_heliModel.localBodyRearMatrix;
    m_heliModel.bodyRearMatrix *= updateMat;

    m_heliModel.engineHousingMatrix = m_heliModel.localEngineHousingMatrix;
    m_heliModel.engineHousingMatrix *= updateMat;

    m_heliModel.engineHousingFrontMatrix = m_heliModel.localEngineHousingFrontMatrix;
    m_heliModel.engineHousingFrontMatrix *= updateMat;

    m_heliModel.tailBoomMatrix = m_heliModel.localTailBoomMatrix;
    m_heliModel.tailBoomMatrix *= updateMat;

    m_heliModel.tailFinMatrix = m_heliModel.localTailFinMatrix;
    m_heliModel.tailFinMatrix *= updateMat;

    m_heliModel.tailWingMatrix = m_heliModel.localTailWingMatrix;
    m_heliModel.tailWingMatrix *= updateMat;

    m_heliModel.mainRotorAxelMatrix = m_heliModel.localMainRotorAxelMatrix;
    m_heliModel.mainRotorAxelMatrix *= updateMat;

    m_heliModel.mainRotorHubMatrix = m_heliModel.localMainRotorHubMatrix;
    m_heliModel.mainRotorHubMatrix *= updateMat;

    DirectX::SimpleMath::Vector3 mainRotorPos = DirectX::SimpleMath::Vector3::Zero;
    mainRotorPos = mainRotorPos.Transform(mainRotorPos, m_heliModel.mainRotorHubMatrix);
    m_heli.mainRotorPos = mainRotorPos;

    m_rotorTimer += static_cast<float>(aTimer);
    const float rotorAngle = static_cast<float>(aTimer) * 0.2f;
    float testSpin = (1.0f * Utility::GetPi());
    testSpin = m_rotorTimer;
    float testPitch = Utility::ToRadians(30.0f);
    testPitch = (1.5f * Utility::GetPi());
    testPitch = (1.5f * Utility::GetPi());
    testPitch = m_rotorTimer;
    DirectX::SimpleMath::Matrix mainRotorSpin = DirectX::SimpleMath::Matrix::CreateRotationY(testSpin);
    DirectX::SimpleMath::Matrix rotorPitch = DirectX::SimpleMath::Matrix::CreateRotationX(cos(testPitch));

    m_heliModel.mainRotorBladeMatrix1 = m_heliModel.mainRotorBladeTranslationMatrix1;
    m_heliModel.mainRotorBladeMatrix1 *= rotorPitch ;    
    m_heliModel.mainRotorBladeMatrix1 *= m_heliModel.localMainRotorBladeMatrix1;  
    m_heliModel.mainRotorBladeMatrix1 *= mainRotorSpin;   
    m_heliModel.mainRotorBladeMatrix1 *= updateMat;
    
    m_heliModel.mainRotorBladeMatrix2 = m_heliModel.mainRotorBladeTranslationMatrix2;
    m_heliModel.mainRotorBladeMatrix2 *= rotorPitch;
    m_heliModel.mainRotorBladeMatrix2 *= m_heliModel.localMainRotorBladeMatrix2;
    m_heliModel.mainRotorBladeMatrix2 *= mainRotorSpin;
    m_heliModel.mainRotorBladeMatrix2 *= updateMat;

    
    m_heliModel.mainRotorArmMatrix = m_heliModel.localMainRotorArmMatrix;   
    m_heliModel.mainRotorArmMatrix *= mainRotorSpin;
    m_heliModel.mainRotorArmMatrix *= updateMat;
    
    // tail rotor axel
    m_heliModel.tailRotorAxelMatrix = m_heliModel.localTailRotorAxelMatrix;
    m_heliModel.tailRotorAxelMatrix *= updateMat;
    // tail rotor hub
    m_heliModel.tailRotorHubMatrix = m_heliModel.localTailRotorHubMatrix;
    m_heliModel.tailRotorHubMatrix *= updateMat;
    //
    DirectX::SimpleMath::Vector3 tailRotorPos = DirectX::SimpleMath::Vector3::Zero;
    tailRotorPos = tailRotorPos.Transform(tailRotorPos, m_heliModel.tailRotorHubMatrix);
    m_heli.tailRotorPos = tailRotorPos;

    // tail arm 
    testSpin += Utility::ToRadians(90.0f);
    DirectX::SimpleMath::Matrix tailBladeSpin = DirectX::SimpleMath::Matrix::CreateRotationZ(testSpin);
    DirectX::SimpleMath::Matrix tailArmSpin = DirectX::SimpleMath::Matrix::CreateRotationZ(testSpin);
    m_heliModel.tailRotorArmMatrix = tailArmSpin;
    m_heliModel.tailRotorArmMatrix *= m_heliModel.localTailRotorArmMatrix;       
    m_heliModel.tailRotorArmMatrix *= updateMat;

    const float tailPitch = cos(testSpin);
    DirectX::SimpleMath::Matrix tailRotorPitch = DirectX::SimpleMath::Matrix::CreateRotationY(tailPitch);
    DirectX::SimpleMath::Matrix tailRotorPitch2 = DirectX::SimpleMath::Matrix::CreateRotationY(-tailPitch);
    // tail rotor blade 1
    m_heliModel.tailRotorBladeMatrix1 = m_heliModel.localTailRotorBladeMatrix1;
    m_heliModel.tailRotorBladeMatrix1 *= tailRotorPitch;
    m_heliModel.tailRotorBladeMatrix1 *= tailBladeSpin;
    m_heliModel.tailRotorBladeMatrix1 *= m_heliModel.tailRotorBladeTranslationMatrix1;
    m_heliModel.tailRotorBladeMatrix1 *= updateMat;

    // tail rotor blade 2
    m_heliModel.tailRotorBladeMatrix2 = m_heliModel.localTailRotorBladeMatrix2;
    m_heliModel.tailRotorBladeMatrix2 *= tailRotorPitch2;
    m_heliModel.tailRotorBladeMatrix2 *= tailBladeSpin;
    m_heliModel.tailRotorBladeMatrix2 *= m_heliModel.tailRotorBladeTranslationMatrix2;
    m_heliModel.tailRotorBladeMatrix2 *= updateMat;

}

void Vehicle::UpdateResistance()
{
    /*
    Rair = (1/2) rho[mass ensity of air] V^2 Sp  Cd
        Sp = projected frontal area of car normalto the direction V
        Cd = drag coeffient == 0.4?ish
        */
    float velocity = m_heli.q.velocity.Length();
    float drag = .5f * m_heli.Cd * m_heli.density * m_heli.area * (velocity * velocity);

    m_heli.airResistance = drag;
}

void Vehicle::UpdateRotorForce()
{
    DirectX::SimpleMath::Matrix xRot = DirectX::SimpleMath::Matrix::CreateRotationX(m_heli.cyclicInputRoll * 1.0f);
    DirectX::SimpleMath::Matrix zRot = DirectX::SimpleMath::Matrix::CreateRotationZ(m_heli.cyclicInputPitch * 1.0f);
    DirectX::SimpleMath::Matrix localizeRot = DirectX::SimpleMath::Matrix::CreateFromAxisAngle(m_heli.up, m_heli.carRotation);
    DirectX::SimpleMath::Matrix tRot = xRot + zRot;
    tRot *= localizeRot;
    DirectX::SimpleMath::Vector3 updateForce = m_heli.up;
    updateForce = DirectX::SimpleMath::Vector3::Transform(updateForce, tRot);
    updateForce.Normalize();
    m_heli.q.mainRotorForceNormal = updateForce;

    // testing body tilt
    DirectX::SimpleMath::Vector3 testRotorForce = m_heli.q.mainRotorForceNormal * m_heli.q.mainRotorForceMagnitude;
    DirectX::SimpleMath::Vector3 newUp = DirectX::SimpleMath::Vector3::SmoothStep(m_heli.up, testRotorForce, 0.15);
    newUp = DirectX::SimpleMath::Vector3::SmoothStep(newUp, -m_heli.q.gravityForce * 1.0f, 0.14);
    newUp.Normalize();
    m_heli.up = newUp;
}

void Vehicle::UpdateTailYawForce()
{
    DirectX::SimpleMath::Matrix yawRot = DirectX::SimpleMath::Matrix::CreateFromAxisAngle(m_heli.up, m_heli.yawPedalInput * 1.0f);
    DirectX::SimpleMath::Vector3 newForward = DirectX::SimpleMath::Vector3::Transform(m_heli.forward, yawRot);
    DirectX::XMVECTORF32 debugLineColor;
    debugLineColor.f[0] = 1.0f;
    debugLineColor.f[1] = 0.0f;
    debugLineColor.f[2] = 0.0f;
    debugLineColor.f[3] = 1.0f;
    DebugPushTestLine(m_heli.mainRotorPos, newForward, 2.0, 1.0, debugLineColor);
    newForward = DirectX::SimpleMath::Vector3::Lerp(m_heli.forward, newForward, 0.1);
    newForward.Normalize();
    m_heli.forward = newForward;

    DebugPushUILineDecimalNumber("m_heli.yawPedalInput : ", m_heli.yawPedalInput, "");
    DebugPushUILineDecimalNumber("newForward.x : ", newForward.x, "");
    DebugPushUILineDecimalNumber("newForward.y : ", newForward.y, "");
    DebugPushUILineDecimalNumber("newForward.z : ", newForward.z, "");
}

void Vehicle::UpdateTerrainNorm()
{
    DirectX::SimpleMath::Vector3 newTerrainNorm = m_environment->GetTerrainNormal(m_heli.q.position);
    DirectX::SimpleMath::Vector3 oldTerrainNorm = m_heli.terrainNormal;
    DirectX::SimpleMath::Vector3 updateTerrainNorm = DirectX::SimpleMath::Vector3::SmoothStep(newTerrainNorm, oldTerrainNorm, 0.8);
    updateTerrainNorm.Normalize();
    m_heli.terrainNormal = updateTerrainNorm;

    // test switch smoothing to m_heli.up
    m_heli.terrainNormal = m_environment->GetTerrainNormal(m_heli.q.position);
}

void Vehicle::UpdateTransmission(const double aTimeDelta)
{
    // update shift delay cooldown
    m_heli.shiftCooldown -= static_cast<float>(aTimeDelta);
    if (m_heli.shiftCooldown < 0.0)
    {
        m_heli.shiftCooldown = 0.0;
    }

    float velocity = m_heli.q.velocity.Length();
    //  Compute the new engine rpm value
    // test rpm with clutch depressed
    if (m_heli.isClutchPressed == true)
    {
        m_heli.omegaE = velocity * 60.0f * m_heli.gearRatio[m_heli.gearNumber] * m_heli.finalDriveRatio / (2.0f * Utility::GetPi() * (m_heli.wheelRadius * 0.1f));
    }
    else
    {
        m_heli.omegaE = velocity * 60.0f * m_heli.gearRatio[m_heli.gearNumber] * m_heli.finalDriveRatio / (2.0f * Utility::GetPi() * m_heli.wheelRadius);
    }

    if (m_heli.omegaE < 800.0)
    {
        m_heli.omegaE = 800.0;
    }
    //  If the engine is at the redline rpm value,
    //  shift gears upward.
    if (m_heli.isTransmissionManual == false)
    {
        if (m_heli.omegaE > m_heli.redline)
        {
            float oldGearRatio = m_heli.gearRatio[m_heli.gearNumber];
            GearUp();
            float newGearRatio = m_heli.gearRatio[m_heli.gearNumber];
            m_heli.omegaE = m_heli.omegaE * newGearRatio / oldGearRatio;
        }
        /*
        if (m_heli.omegaE < downShiftLimit && m_heli.gearNumber > 1)
        {
            float oldGearRatio = m_heli.gearRatio[m_heli.gearNumber];
            --m_heli.gearNumber;
            float newGearRatio = m_heli.gearRatio[m_heli.gearNumber];
            m_heli.omegaE = m_heli.omegaE * newGearRatio / oldGearRatio;
        }
        */
    }
}

void Vehicle::UpdateVehicle(const double aTimeDelta)
{
    DebugClearUI();
    DirectX::SimpleMath::Vector3 prevVelocity = m_heli.q.velocity;
    DirectX::SimpleMath::Vector3 prevPos = m_heli.q.position;


    UpdateRotorForce();
    
    DirectX::XMVECTORF32 debugLineColor;
    debugLineColor.f[0] = 1.0f;
    debugLineColor.f[1] = 0.0f;
    debugLineColor.f[2] = 0.0f;
    debugLineColor.f[3] = 1.0f;
    DebugPushTestLine(m_heli.mainRotorPos, m_heli.q.mainRotorForceNormal, 5.0, 0.0, debugLineColor);
    //void DebugPushTestLine(DirectX::SimpleMath::Vector3 aLineBase, DirectX::SimpleMath::Vector3 aLineEnd, float aLength, float aYOffset, DirectX::XMVECTORF32 aColor);

    m_heli.testModelRotation = m_heli.carRotation;
    m_heli.testTerrainNormal = m_heli.terrainNormal;
    m_heli.testHeadingVec = m_heli.headingVec;

    m_heli.isCarLanding = false;
    m_heli.terrainHightAtPos = m_environment->GetTerrainHeightAtPos(m_heli.q.position);
    
    DebugPushUILineDecimalNumber("Altitude : ", m_heli.q.position.y - m_heli.terrainHightAtPos, "");
    if (m_heli.q.position.y - m_heli.terrainHightAtPos > 0.1)
    {
        m_heli.isCarAirborne = true;
    }
    else
    {
        if (m_heli.isCarAirborne == true)
        {
            LandVehicle();
            m_heli.isCarLanding = true;
        }
        m_heli.isCarAirborne = false;
        m_heli.q.position.y = m_heli.terrainHightAtPos;
    }
    
    //RevLimiter();
    ThrottleBrakeDecay(aTimeDelta);
    SteeringInputDecay(aTimeDelta);
    TurnVehicle(aTimeDelta);
    UpdateTerrainNorm();   
    m_heli.testModelPos = m_heli.q.position;
    UpdateCarAlignment();
    RungeKutta4(&m_heli, aTimeDelta);

    if (m_heli.forward.Dot(m_heli.q.velocity) < 0.0)
    {
        m_heli.isVelocityBackwards = true;
    }
    else
    {
        m_heli.isVelocityBackwards = false;
    }
  
    UpdateVelocity(aTimeDelta);
    UpdateHeadingVec();
    UpdateTransmission(aTimeDelta);

    m_heli.speed = m_heli.q.velocity.Length();
    UpdateModel(aTimeDelta);

    m_heli.isThrottlePressed = false;
    m_heli.isTurningPressed = false;
    m_heli.isBrakePressed = false;
    m_testIsBreakLightOn = false;
    InputDecay(aTimeDelta);
    DebugPushUILineDecimalNumber("test rotor mag : ", m_heli.collectiveInput, "");


    UpdateTailYawForce();

    
    m_heli.testAccel = (m_heli.q.velocity.Length() - prevVelocity.Length()) / static_cast<float>(aTimeDelta);
    m_heli.testAcceleration = (m_heli.q.velocity - prevVelocity) / static_cast<float>(aTimeDelta);
    m_heli.testAcceleration = m_heli.testAcceleration / m_heli.q.velocity;
    
    UpdateResistance();

    DirectX::SimpleMath::Vector3 postPos = m_heli.q.position;
    DirectX::SimpleMath::Vector3 deltaPos = prevPos - postPos;
    float deltaLength = deltaPos.Length();
    m_testVelocity = deltaLength / static_cast<float>(aTimeDelta);
}

void Vehicle::UpdateVelocity(double aTimeDelta)
{
    float lateralFrictionFactor = 0.2;
    float backwardsFrictionFactor = 0.0;
    DirectX::SimpleMath::Vector3 testVelocity = m_heli.q.velocity;
    DirectX::SimpleMath::Vector3 lateralVelocity;
    lateralVelocity = m_heli.right * testVelocity.Dot(m_heli.right);
    DirectX::SimpleMath::Vector3 lateralFriction = -lateralVelocity * lateralFrictionFactor;
    DirectX::SimpleMath::Vector3 backwardsFriction = -testVelocity * backwardsFrictionFactor;
    testVelocity += (backwardsFriction + lateralFriction) * static_cast<float>(aTimeDelta);

    const float lerpSize = 0.5;
    if (m_heli.isCarAirborne == true)
    {

    }
    else
    {
        if (m_heli.forward.Dot(m_heli.q.velocity) < 0.0)
        {
            m_heli.q.totalVelocity = DirectX::SimpleMath::Vector3::Lerp(m_heli.q.totalVelocity, -m_heli.forward * m_heli.q.totalVelocity.Length(), lerpSize);
            m_heli.q.velocity = DirectX::SimpleMath::Vector3::Lerp(m_heli.q.velocity, -m_heli.forward * m_heli.q.velocity.Length(), lerpSize);
        }
        else
        {
            m_heli.q.totalVelocity = DirectX::SimpleMath::Vector3::Lerp(m_heli.q.totalVelocity, m_heli.forward * m_heli.q.totalVelocity.Length(), lerpSize);
            m_heli.q.velocity = DirectX::SimpleMath::Vector3::Lerp(m_heli.q.velocity, m_heli.forward * m_heli.q.velocity.Length(), lerpSize);
        }
    }
}

void Vehicle::DebugPushTestLine(DirectX::SimpleMath::Vector3 aLineBase, DirectX::SimpleMath::Vector3 aLineEnd, float aLength, float aYOffset, DirectX::XMVECTORF32 aColor)
{
    DirectX::SimpleMath::Vector3 scaledLineBase = aLineBase;
    scaledLineBase.y += aYOffset;
    DirectX::SimpleMath::Vector3 scaledLineEnd = aLineEnd;
    scaledLineEnd.Normalize();
    scaledLineEnd *= aLength;
    scaledLineEnd += scaledLineBase;
    std::tuple<DirectX::SimpleMath::Vector3, DirectX::SimpleMath::Vector3, DirectX::XMVECTORF32> lineTup(scaledLineBase, scaledLineEnd, aColor);
    m_debugLinesVec.push_back(lineTup);
}

void Vehicle::DebugPushUILine(std::string aString, float aVal)
{
    std::pair<std::string, float> newPair = std::make_pair(aString, aVal);
    m_debugUI.push_back(newPair);
}

void Vehicle::DebugPushUILineDecimalNumber(std::string aString1, float aVal, std::string aString2)
{
    std::string textLine = aString1 + " " + std::to_string(aVal) + " " +aString2;
    m_debugUIVector.push_back(textLine);
}

void Vehicle::DebugPushUILineWholeNumber(std::string aString1, int aVal, std::string aString2)
{
    std::string textLine = aString1 + " " + std::to_string(aVal) + " " + aString2;
    m_debugUIVector.push_back(textLine);
}

void Vehicle::TestGetForceLateral()
{
    float radius = GetTurnRadius();
    float mass = m_heli.mass;
    float velocity = m_heli.q.velocity.Length();
    velocity *= velocity;
    float muK = 0.7f; // guess at this point

    float forceLat;
    forceLat = ((mass * velocity) / radius) - (muK * mass * m_heli.gravity.y * cos(0.0f));  
    DebugPushUILine("Force Lateral", forceLat);

    DirectX::SimpleMath::Vector3 testForceLat;
    DirectX::SimpleMath::Vector3 gravVec(0.0f, -9.8f, 0.0f);

    testForceLat = ((mass * (m_heli.q.velocity * m_heli.q.velocity)) / radius) - (muK * mass * gravVec * cos(0.0f));

    DebugPushUILine("Force Lateral X ", testForceLat.x);
    DebugPushUILine("Force Lateral Y ", testForceLat.y);
    DebugPushUILine("Force Lateral Z ", testForceLat.z);

    DirectX::SimpleMath::Vector3 testHeading = m_heli.q.velocity;
    DirectX::SimpleMath::Matrix rotMat = DirectX::SimpleMath::Matrix::CreateRotationY(-m_heli.carRotation);
    testHeading = DirectX::SimpleMath::Vector3::Transform(testHeading, rotMat);

    float slipAngle;
    slipAngle = -atan(testHeading.x / abs(testHeading.z));
    DebugPushUILine("slipAngle", Utility::ToDegrees(slipAngle)  + 90.0f);
    DebugPushUILine("slipAngle 2", Utility::ToDegrees(slipAngle) - Utility::ToDegrees(m_heli.carRotation));
    DebugPushUILine("slipAngle 3", Utility::ToDegrees(slipAngle) + 90.0f - Utility::ToDegrees(m_heli.carRotation));
    DebugPushUILine("m_heli.carRotation ", Utility::ToDegrees(m_heli.carRotation));
}


#include "pch.h"
#include "Vehicle.h"



void Vehicle::DrawModel(const DirectX::SimpleMath::Matrix aView, const DirectX::SimpleMath::Matrix aProj)
{
    m_heliModel.bodyShape->Draw(m_heliModel.bodyMatrix, aView, aProj, m_heliModel.bodyColor);
    m_heliModel.bodyCapShape->Draw(m_heliModel.windShieldMatrix, aView, aProj, m_heliModel.windshieldColor);
    m_heliModel.bodyCapShape->Draw(m_heliModel.bodyRearMatrix, aView, aProj, m_heliModel.bodyColor);

    m_heliModel.noseConeShape->Draw(m_heliModel.noseConeMatrix, aView, aProj, m_heliModel.bodyColor);
    m_heliModel.noseBodyShape->Draw(m_heliModel.noseBodyMatrix, aView, aProj, m_heliModel.bodyColor);

    m_heliModel.engineHousingShape->Draw(m_heliModel.engineHousingMatrix, aView, aProj, m_heliModel.bodyColor);
    m_heliModel.engineHousingFrontShape->Draw(m_heliModel.engineHousingFrontMatrix, aView, aProj, m_heliModel.bodyColor);

    m_heliModel.tailBoomShape->Draw(m_heliModel.tailBoomMatrix, aView, aProj, m_heliModel.bodyColor);
    m_heliModel.tailFinShape->Draw(m_heliModel.tailFinMatrix, aView, aProj, m_heliModel.bodyColor);
    m_heliModel.tailWingShape->Draw(m_heliModel.tailWingMatrix, aView, aProj, m_heliModel.bodyColor);

    m_heliModel.mainRotorAxelShape->Draw(m_heliModel.mainRotorAxelMatrix, aView, aProj, m_heliModel.axelColor);
    m_heliModel.mainRotorHubShape->Draw(m_heliModel.mainRotorHubMatrix, aView, aProj, m_heliModel.axelColor);

    m_heliModel.mainRotorArmShape->Draw(m_heliModel.mainRotorArmMatrix, aView, aProj, m_heliModel.rotorColor);
    m_heliModel.mainRotorBladeShape->Draw(m_heliModel.mainRotorBladeMatrix1, aView, aProj, m_heliModel.rotorColor);
    m_heliModel.mainRotorBladeShape->Draw(m_heliModel.mainRotorBladeMatrix2, aView, aProj, m_heliModel.rotorColor);

    m_heliModel.tailRotorAxelShape->Draw(m_heliModel.tailRotorAxelMatrix, aView, aProj, m_heliModel.axelColor);
    m_heliModel.tailRotorHubShape->Draw(m_heliModel.tailRotorHubMatrix, aView, aProj, m_heliModel.axelColor);
    m_heliModel.tailRotorArmShape->Draw(m_heliModel.tailRotorArmMatrix, aView, aProj, m_heliModel.rotorColor);
    m_heliModel.tailRotorBladeShape->Draw(m_heliModel.tailRotorBladeMatrix1, aView, aProj, m_heliModel.rotorColor);
    m_heliModel.tailRotorBladeShape->Draw(m_heliModel.tailRotorBladeMatrix2, aView, aProj, m_heliModel.rotorColor);
}

float Vehicle::GetYawRate(double aTimeDelta)
{
    /*
    float wheelBase = m_heli.wheelBase;
    float velocity = m_heli.q.velocity.Length();
    float steeringAngle = m_heli.steeringAngle;
    float sinDelta = sin(steeringAngle);
    float omega = (velocity * sinDelta) / wheelBase;
    float omegaT = omega * static_cast<float>(aTimeDelta);
    */
    // testing tail blade yaw turn
    DebugPushUILineDecimalNumber("m_heli.yawPedalInput", m_heli.yawPedalInput, "m_heli.yawPedalInput");
    const float omegaT = m_heli.yawPedalInput * static_cast<float>(aTimeDelta);
    return omegaT;
}

void Vehicle::InitializeModel(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext)
{
    // set model part colors
    m_heliModel.bodyColor = DirectX::SimpleMath::Vector4(0.501960814f, 0.501960814f, 0.501960814f, 1.0f);
    m_heliModel.rotorColor = DirectX::SimpleMath::Vector4(0.827451050f, 0.827451050f, 0.827451050f, 1.0f);
    m_heliModel.axelColor = DirectX::SimpleMath::Vector4(0.411764741f, 0.411764741f, 0.411764741f, 1.0f);
    m_heliModel.windshieldColor = DirectX::SimpleMath::Vector4(0.662745118f, 0.662745118f, 0.662745118f, 1.20f);
    m_heliModel.testColor1 = DirectX::SimpleMath::Vector4(1.0, 0.0f, 0.0f, 1.0f);
    m_heliModel.testColor2 = DirectX::SimpleMath::Vector4(1.0, 1.0f, 1.0f, 1.0f);

    // set model shapes and local positions   
    // main body
    const DirectX::SimpleMath::Vector3 bodySize(5.0f, 3.0f, 3.0f);
    const DirectX::SimpleMath::Vector3 bodyTranslation(0.0f, bodySize.y * 0.5f, 0.0f);
    m_heliModel.bodyShape = DirectX::GeometricPrimitive::CreateBox(aContext.Get(), bodySize);
    m_heliModel.bodyMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.bodyMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(bodyTranslation);
    m_heliModel.localBodyMatrix = m_heliModel.bodyMatrix;
    
    /// windshield
    const float windshieldSize = (bodySize.z * 0.5f) * sqrt(2.0f);
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
    const DirectX::SimpleMath::Vector3 noseBodyTranslation(bodySize.x * 0.5f + (noseBodySize.x * 0.5f), bodySize.y * 0.25f, 0.0f);
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
    const float engineHousingFrontSize = (engineHousingSize.z * 0.5f) * sqrt(2.0f);
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

    // tail rotor axel
    const float tailAxelLength = 0.35f;
    const float tailAxelDiameter = 0.1f;
    const float tailAxelOffsetY = 0.25f;
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
    const DirectX::SimpleMath::Vector3 tailBladeTranslation1(tailBladeWidth * 0.5f, (tailArmLength * 0.5f) - (tailBladeSize.y * 0.5f), 0.0f);
    m_heliModel.tailRotorBladeShape = DirectX::GeometricPrimitive::CreateBox(aContext.Get(), tailBladeSize);
    m_heliModel.tailRotorBladeMatrix1 = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.tailRotorBladeMatrix1 *= DirectX::SimpleMath::Matrix::CreateTranslation(tailBladeTranslation1);
    m_heliModel.localTailRotorBladeMatrix1 = m_heliModel.tailRotorBladeMatrix1;
    m_heliModel.tailRotorBladeTranslationMatrix1 = DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(tailAxelLength, 0.0, 0.0));
    m_heliModel.tailRotorBladeTranslationMatrix1 = DirectX::SimpleMath::Matrix::CreateTranslation(tailArmTranslation);

    // tail rotor blade 2
    const DirectX::SimpleMath::Vector3 tailBladeTranslation2(-tailBladeWidth * 0.5f, -(tailArmLength * 0.5f) + (tailBladeSize.y * 0.5f), 0.0f);
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
    m_heli.throttleInput = 0.0f;
    m_heli.yawPedalInput = 0.0f;
    m_heli.yawPedalIsPressed = false;
    m_heli.mainRotorRPM = 0.0f;

    m_heli.numEqns = 6;
    m_heli.mass = 1760.0f;
    m_heli.area = 1.94f;
    m_heli.airDensity = 1.2f; // ToDo : pull air density from environment data
    m_heli.dragCoefficient = 0.31f;
    m_heli.airResistance = 0.0f;
    m_heli.totalResistance = m_heli.airResistance;
    m_heli.time = 0.0;  

    m_heli.q.airResistance = DirectX::SimpleMath::Vector3::Zero;
    m_heli.q.bodyVelocity = DirectX::SimpleMath::Vector3::Zero;
    m_heli.q.engineForce = DirectX::SimpleMath::Vector3::Zero;
    m_heli.q.gravityForce = DirectX::SimpleMath::Vector3::Zero;
    m_heli.q.mainRotorForceNormal = DirectX::SimpleMath::Vector3::UnitY;
    m_heli.q.mainRotorForceMagnitude = 15.0f;
    m_heli.q.position = DirectX::SimpleMath::Vector3::Zero;
    m_heli.q.tailRotorYawForce = 0.0f;
    m_heli.q.totalVelocity = DirectX::SimpleMath::Vector3::Zero;
    m_heli.q.velocity = DirectX::SimpleMath::Vector3::Zero;

    m_heli.vehicleRotation = 0.0f;
    m_heli.forward = DirectX::SimpleMath::Vector3::UnitX;
    m_heli.up = DirectX::SimpleMath::Vector3::UnitY;
    m_heli.right = m_heli.forward.Cross(m_heli.up);
    m_heli.speed = 0.0;

    m_heli.isVehicleAirborne = false;
    m_heli.isVehicleLanding = false;
    m_heli.isVelocityBackwards = false;

    m_heli.terrainHightAtPos = 0.0;
    m_heli.terrainNormal = DirectX::SimpleMath::Vector3::UnitY;

    InitializeModel(aContext);

    // Intialize key physics hardpoints based on model 
    m_heli.localMainRotorPos = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::Zero, m_heliModel.localMainRotorHubMatrix);
    m_heli.mainRotorPos = m_heli.localMainRotorPos;
    m_heli.localCenterOfMass = m_heli.localMainRotorPos;
    m_heli.localCenterOfMass.y *= 0.5f;
    m_heli.centerOfMass = m_heli.localCenterOfMass;
    m_heli.localTailRotorPos = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::Zero, m_heliModel.localTailRotorHubMatrix);
    m_heli.tailRotorPos = m_heli.localTailRotorPos;

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
        if (m_heli.yawPedalInput + (m_heli.yawPedalDecayRate * timeDelta) < 0.0f)
        {
            if (m_heli.yawPedalInput - (m_heli.yawPedalDecayRate * timeDelta) > -m_heli.inputDeadZone)
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
        else
        {
            m_heli.yawPedalInput = 0.0f;
        }
    }

    m_heli.cyclicInputPitchIsPressed = false;
    m_heli.cyclicInputRollIsPressed = false;
    m_heli.yawPedalIsPressed = false;
}

void Vehicle::InputHThrottle(const float aThrottleInput)
{
    const float updatedThrottle = (aThrottleInput * m_heli.throttleInputRate) + m_heli.throttleInput;
    if (updatedThrottle > m_heli.throttleInputMax)
    {
        m_heli.throttleInput = m_heli.throttleInputMax;
    }
    else if (updatedThrottle < m_heli.throttleInputMin)
    {
        m_heli.throttleInput = m_heli.throttleInputMin;
    }
    else
    {
        m_heli.throttleInput = updatedThrottle;
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
    else if (updatedYaw < m_heli.inputDeadZone && updatedYaw > -m_heli.inputDeadZone)
    {
        m_heli.yawPedalInput = 0.0f;
    }
    else
    {
        m_heli.yawPedalInput = updatedYaw;
    }
}

void Vehicle::Jump()
{
    const float jumpHeight = 10.0f;
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


void Vehicle::ResetVehicle()
{
    m_heli.q.position = DirectX::SimpleMath::Vector3::Zero;
    m_heli.forward = DirectX::SimpleMath::Vector3::UnitX;
    m_heli.up = DirectX::SimpleMath::Vector3::UnitY;
    m_heli.right = DirectX::SimpleMath::Vector3::UnitZ;
    m_heli.speed = 0.0;
    m_heli.q.velocity = DirectX::SimpleMath::Vector3::Zero;
}

//  This method loads the right-hand sides for the vehicle ODEs
void Vehicle::RightHandSide(struct HeliData* aHeli, Motion* aQ, Motion* aDeltaQ, double aTimeDelta, float aQScale, Motion* aDQ)
{
    //  Compute the intermediate values of the 
    //  dependent variables.
    Motion newQ;
    newQ.velocity = aQ->velocity + static_cast<float>(aQScale) * aDeltaQ->velocity;
    newQ.position = aQ->position + static_cast<float>(aQScale) * aDeltaQ->position;

    DirectX::SimpleMath::Vector3 rotorForce = aQ->mainRotorForceNormal * aQ->mainRotorForceMagnitude * m_heli.collectiveInput;
    DebugPushUILineDecimalNumber("rotorForce ", rotorForce.y, "");

    newQ.velocity += rotorForce + static_cast<float>(aQScale) * aDeltaQ->velocity;

    // tail yaw testing
    DirectX::SimpleMath::Matrix yawRot = DirectX::SimpleMath::Matrix::CreateFromAxisAngle(m_heli.up, m_heli.yawPedalInput * 1.0f);
    newQ.velocity = DirectX::SimpleMath::Vector3::Transform(newQ.velocity,yawRot);

    //DirectX::SimpleMath::Vector3 tailRotorForce = aQ->mainRotorForceNormal * aQ->mainRotorForceMagnitude * m_heli.collectiveInput;
    
    /*
    DirectX::SimpleMath::Vector3 testTorque;
    //DirectX::SimpleMath::Matrix testTorque;
    testTorque = rotorForce.Cross(m_heli.localMainRotorPos - m_heli.centerOfMass);
    DebugPushUILineDecimalNumber("testTorque.x ", testTorque.x, "");
    DebugPushUILineDecimalNumber("testTorque.y ", testTorque.y, "");
    DebugPushUILineDecimalNumber("testTorque.z ", testTorque.z, "");

    DirectX::SimpleMath::Matrix testMat;
    //testMat = DirectX::SimpleMath::Matrix::
    */


    //  Compute the constants that define the
    //  torque curve line.
    // ToDo once physics equations are in place after testing model is finished

    //  Compute the velocity magnitude. The 1.0e-8 term
    //  ensures there won't be a divide by zero later on
    //  if all of the velocity components are zero.
    float v = sqrt(newQ.velocity.Length() * newQ.velocity.Length()) + 1.0e-8f;

    //  Compute the total drag force.
    float airDensity = aHeli->airDensity;
    float dragCoefficient = aHeli->dragCoefficient;
    float frontSurfaceArea = aHeli->area;
    float frontDragResistance = 0.5f * airDensity * frontSurfaceArea * dragCoefficient * v * v;

    //  Compute the force of rolling friction. Because
    //  the G constant has a negative sign, the value 
    //  computed here will be negative
    float gravity = aHeli->gravity.y;
    float mass = aHeli->mass;

    //  Compute the right-hand sides of the six ODEs
    //  newQ[0] is the intermediate value of velocity.
    // ToDo once physics equations are in place after testing model is finished
    DirectX::SimpleMath::Vector3 headingVec = m_heli.forward;

    DirectX::SimpleMath::Vector3 velocityNorm = m_heli.q.velocity;
    velocityNorm.Normalize();

    DirectX::SimpleMath::Vector3 airResistance = velocityNorm * (static_cast<float>(aTimeDelta) * (-frontDragResistance / mass));

    if (m_heli.isVelocityBackwards == true)
    {
        airResistance *= -1.0;
    }

    DirectX::SimpleMath::Vector3 gravForce = m_heli.gravity * static_cast<float>(aTimeDelta);
    DirectX::SimpleMath::Vector3 terrainNormalForce = (m_heli.terrainNormal * -m_heli.gravity.y) * static_cast<float>(aTimeDelta);

    DirectX::SimpleMath::Vector3 velocityUpdate;
    if (m_heli.isVehicleAirborne == true)
    {
        terrainNormalForce = DirectX::SimpleMath::Vector3::Zero;
        velocityUpdate = m_heli.gravity;
        velocityUpdate.x = 0.0;
        velocityUpdate.z = 0.0;
        velocityUpdate.y = gravity * static_cast<float>(aTimeDelta);
        velocityUpdate += rotorForce * static_cast<float>(aTimeDelta);
    }
    else
    {
        velocityUpdate = DirectX::SimpleMath::Vector3::Zero;
    }

    //  Assign right-hand side values.
    aDQ->airResistance = airResistance;
    aDQ->gravityForce = gravForce;
    aDQ->velocity = velocityUpdate;
    aDQ->totalVelocity = velocityUpdate; 
    aDQ->position = static_cast<float>(aTimeDelta) * newQ.velocity;
    
    return;
}

//  This method solves for the vehicle motion using a 4th-order Runge-Kutta solver
void Vehicle::RungeKutta4(struct HeliData* aHeli, double aTimeDelta)
{
    //  Define a convenience variables
    const float numEqns = static_cast<float>(aHeli->numEqns);
    //  Retrieve the current values of the dependent and independent variables.
    Motion q = aHeli->q;
    Motion dq1;
    Motion dq2;
    Motion dq3;
    Motion dq4;

    if (m_heli.isVehicleLanding == true)
    {
        m_heli.q.velocity = DirectX::SimpleMath::Vector3::Zero;
    }
    if (m_heli.isVehicleAirborne == false)
    {
        m_heli.q.gravityForce = DirectX::SimpleMath::Vector3::Zero;
    }

    // Compute the four Runge-Kutta steps, The return 
    // value of RightHandSide method is an array
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
    DirectX::SimpleMath::Vector3 airResistnaceVelocityUpdate = (dq1.airResistance + 2.0 * dq2.airResistance + 2.0 * dq3.airResistance + dq4.airResistance) / numEqns;
    DirectX::SimpleMath::Vector3 gravityVelocityUpdate = (dq1.gravityForce + 2.0 * dq2.gravityForce + 2.0 * dq3.gravityForce + dq4.gravityForce) / numEqns;
    DirectX::SimpleMath::Vector3 totalVelocityUpdate = (dq1.totalVelocity + 2.0 * dq2.totalVelocity + 2.0 * dq3.totalVelocity + dq4.totalVelocity) / numEqns;

    
    // To prevent the vehicle from continuing to roll forward if vehicle velocity is less than the tolerance value and update velocity is zero
    const float stopTolerance = 0.1;
    if (q.velocity.Length() < stopTolerance && velocityUpdate == DirectX::SimpleMath::Vector3::Zero)
    {
        q.velocity = DirectX::SimpleMath::Vector3::Zero;
    }
    else
    {
        q.velocity += velocityUpdate;
    }

    q.velocity += velocityUpdate;
    q.engineForce += engineVelocityUpdate;
    q.airResistance += airResistnaceVelocityUpdate;
    q.gravityForce += gravityVelocityUpdate;
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

void Vehicle::TurnVehicle(double aTimeDelta)
{  
    if (m_heli.isVehicleAirborne == true)
    {
        m_heli.vehicleRotation += GetYawRate(aTimeDelta);
    }
    else
    {
        // do not turn while vehicle is grounded to, could change if helicopter is switched to wheeled landing gear from tracked base
    }
    
    m_heli.vehicleRotation = Utility::WrapAngle(m_heli.vehicleRotation);
}


void Vehicle::UpdateVehicleAlignment()
{
    // Set up to match terrain norm until vehicle jump is implemented
    if (m_heli.isVehicleAirborne == false)
    {
        DirectX::SimpleMath::Vector3 newUp = m_heli.terrainNormal;
        DirectX::SimpleMath::Vector3 oldUp = m_heli.up;
        DirectX::SimpleMath::Vector3 updateUp = DirectX::SimpleMath::Vector3::SmoothStep(oldUp, newUp, 0.2);
        updateUp.Normalize();
        m_heli.up = updateUp;
    }
    m_heli.forward = DirectX::SimpleMath::Vector3::TransformNormal(DirectX::SimpleMath::Vector3::UnitX, DirectX::SimpleMath::Matrix::CreateFromAxisAngle(m_heli.up, m_heli.vehicleRotation));
    m_heli.right = m_heli.forward.Cross(m_heli.up);
    m_heli.right.Normalize();
    m_heli.forward = m_heli.up.Cross(m_heli.right);
}

void Vehicle::UpdateModel(const double aTimer)
{
    DirectX::SimpleMath::Matrix updateMat = DirectX::SimpleMath::Matrix::Identity;
    updateMat *= DirectX::SimpleMath::Matrix::CreateWorld(m_heli.q.position, -m_heli.right, m_heli.up);

    DirectX::SimpleMath::Matrix updateMat2 = DirectX::SimpleMath::Matrix::Identity;
    updateMat2 *= DirectX::SimpleMath::Matrix::CreateWorld(DirectX::SimpleMath::Vector3::Zero, -m_heli.right, m_heli.up);
    m_heli.mainRotorPos = m_heli.localMainRotorPos;
    m_heli.mainRotorPos = DirectX::SimpleMath::Vector3::Transform(m_heli.mainRotorPos, updateMat);
    m_heli.centerOfMass = m_heli.localCenterOfMass;
    m_heli.centerOfMass = DirectX::SimpleMath::Vector3::Transform(m_heli.centerOfMass, updateMat);

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
        Sp = projected frontal area of vehicle normal to the direction V
        Cd = drag coeffient == 0.4?ish
        */
    float velocity = m_heli.q.velocity.Length();
    float drag = .5f * m_heli.dragCoefficient * m_heli.airDensity * m_heli.area * (velocity * velocity);

    m_heli.airResistance = drag;
}

void Vehicle::UpdateRotorForce()
{
    DirectX::SimpleMath::Matrix xRot = DirectX::SimpleMath::Matrix::CreateRotationX(m_heli.cyclicInputRoll * 1.0f);
    DirectX::SimpleMath::Matrix zRot = DirectX::SimpleMath::Matrix::CreateRotationZ(m_heli.cyclicInputPitch * 1.0f);
    DirectX::SimpleMath::Matrix localizeRot = DirectX::SimpleMath::Matrix::CreateFromAxisAngle(m_heli.up, m_heli.vehicleRotation);
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
    //m_heli.up = newUp;

    /////////
    DirectX::SimpleMath::Vector3 rotorForce = m_heli.q.mainRotorForceNormal * m_heli.q.mainRotorForceMagnitude * m_heli.collectiveInput;
    DirectX::SimpleMath::Vector3 testTorque;
    //DirectX::SimpleMath::Matrix testTorque;
    testTorque = rotorForce.Cross(m_heli.mainRotorPos - m_heli.centerOfMass);
    DirectX::SimpleMath::Vector3 testTorque2 = Utility::GetTorqueVec(rotorForce, (m_heli.mainRotorPos - m_heli.centerOfMass));
    DebugPushUILineDecimalNumber("testTorque.x  ", testTorque.x, "");
    DebugPushUILineDecimalNumber("testTorque2.x ", testTorque2.x, "");
    DebugPushUILineDecimalNumber("testTorque.y  ", testTorque.y, "");
    DebugPushUILineDecimalNumber("testTorque2.y ", testTorque2.y, "");
    DebugPushUILineDecimalNumber("testTorque.z  ", testTorque.z, "");
    DebugPushUILineDecimalNumber("testTorque2.z ", testTorque2.z, "");

    DebugPushUILineDecimalNumber("Gravity ", m_heli.gravity.y, "");

    DirectX::XMVECTORF32 debugLineColor;
    debugLineColor.f[0] = 1.0f;
    debugLineColor.f[1] = 0.0f;
    debugLineColor.f[2] = 0.0f;
    debugLineColor.f[3] = 1.0f;
    //DebugPushTestLine(m_heli.mainRotorPos, testTorque, 5.0, 0.0, debugLineColor);

    /// Test magnatude
    DirectX::SimpleMath::Vector3 vec1 = m_heli.q.mainRotorForceNormal;
    vec1.Normalize();
    DirectX::SimpleMath::Vector3 vec2 = m_heli.mainRotorPos - m_heli.centerOfMass;
    vec2.Normalize();
    float angle = acos(vec1.Dot(vec2));
    angle = Utility::ToDegrees(angle);
    DebugPushUILineDecimalNumber("angle  ", angle, "");
    float angle2 = Utility::GetAngleBetweenVectors(vec1, vec2);
    DebugPushUILineDecimalNumber("angle2 ", Utility::ToDegrees(angle2), "");
    //float mag = (6.0f * 10.0f) * sin(Utility::ToRadians(50.0f));
    DirectX::SimpleMath::Vector3 vec3 = testTorque * sin(Utility::ToRadians(angle));
    float mag = vec3.Length();
    DebugPushUILineDecimalNumber("mag  ", mag, "");
    /// 
    float mag2 = Utility::GetTorqueVec(rotorForce, (m_heli.mainRotorPos - m_heli.centerOfMass)).Length();

    DebugPushUILineDecimalNumber("mag2 ", mag2, "");

    DirectX::SimpleMath::Vector3 axisLine = m_heli.centerOfMass - m_heli.mainRotorPos ;
    DebugPushTestLine(m_heli.mainRotorPos, axisLine, 15.0, 0.0, debugLineColor);

    float mod = 0.1f;
    if (testTorque.Length() > 0.1)
    {       
        DirectX::SimpleMath::Matrix testMat3 = Utility::GetTorqueMat(axisLine, m_heli.gravity, mod);
        //DirectX::SimpleMath::Matrix testMat3 = Utility::GetTorqueMat(m_heli.gravity, ( m_heli.centerOfMass - m_heli.localMainRotorPos), mod);
        DirectX::SimpleMath::Vector3 testUp3 = axisLine;
        testUp3 = DirectX::SimpleMath::Vector3::Transform(testUp3, testMat3);
        debugLineColor.f[1] = 1.0f;
        DebugPushTestLine(m_heli.mainRotorPos, testUp3, 5.0, 0.0, debugLineColor);
        //DebugPushTestLine(m_heli.mainRotorPos, m_heli.gravity, 5.0, 0.0, debugLineColor);
        
        //DirectX::SimpleMath::Matrix testMat = DirectX::SimpleMath::Matrix::CreateFromAxisAngle(testTorque, - testTorque.Length() * 0.0001);
        DirectX::SimpleMath::Matrix testMat = DirectX::SimpleMath::Matrix::CreateFromAxisAngle(testTorque, -mag * mod);
        DirectX::SimpleMath::Vector3 testUp = m_heli.up;
        DirectX::SimpleMath::Matrix testMat4 = testMat + testMat3;
        testUp = DirectX::SimpleMath::Vector3::Transform(testUp, testMat4);
        testUp.Normalize();
        //DebugPushTestLine(m_heli.mainRotorPos, testUp, 5.0, 0.0, debugLineColor);


        //DirectX::SimpleMath::Matrix testMat2 = Utility::GetTorqueMat(rotorForce, (m_heli.localMainRotorPos - m_heli.centerOfMass), mod);
        DirectX::SimpleMath::Matrix testMat2 = Utility::GetTorqueMat((m_heli.mainRotorPos - m_heli.centerOfMass), rotorForce, mod);
        DirectX::SimpleMath::Vector3 testUp2 = m_heli.up;

        
        testUp2 = DirectX::SimpleMath::Vector3::Transform(testUp2, testMat2);
        debugLineColor.f[2] = 1.0f;
        //DebugPushTestLine(m_heli.mainRotorPos, testUp2, 5.0, 0.0, debugLineColor);
  
        //m_heli.up = testUp;
    }

    /*
    DirectX::SimpleMath::Matrix gravTorqueMat = Utility::GetTorqueMat(axisLine, m_heli.gravity, mod);
    DirectX::SimpleMath::Vector3 gravVec = axisLine;
    gravVec = DirectX::SimpleMath::Vector3::Transform(gravVec, gravTorqueMat);
    debugLineColor.f[1] = 1.0f;
    DebugPushTestLine(m_heli.mainRotorPos, gravVec, 5.0, 0.0, debugLineColor);

    //DirectX::SimpleMath::Matrix rotorTorqueMat = Utility::GetTorqueMat((m_heli.mainRotorPos - m_heli.centerOfMass), rotorForce, mod);
    DirectX::SimpleMath::Matrix rotorTorqueMat = Utility::GetTorqueMat((-axisLine), rotorForce, mod);
    DirectX::SimpleMath::Vector3 rotorVec = m_heli.up;
    rotorVec = DirectX::SimpleMath::Vector3::Transform(rotorVec, rotorTorqueMat);
    debugLineColor.f[2] = 1.0f;
    //DebugPushTestLine(m_heli.mainRotorPos, testUp2, 5.0, 0.0, debugLineColor);
    DirectX::SimpleMath::Vector3 newUp2 = m_heli.up;
    if (testTorque.Length() > 0.1)
    {
        newUp2 = DirectX::SimpleMath::Vector3::Transform(newUp2, gravTorqueMat * rotorTorqueMat);
    }
    else
    {
        newUp2 = DirectX::SimpleMath::Vector3::Transform(newUp2, gravTorqueMat);
    }
    newUp2.Normalize();
    m_heli.up = newUp2;
    */

    DirectX::SimpleMath::Quaternion gravTorqueQuat = Utility::GetTorqueQuat(axisLine, m_heli.gravity, mod);
    DirectX::SimpleMath::Vector3 gravVec = axisLine;
    gravVec = DirectX::SimpleMath::Vector3::Transform(gravVec, gravTorqueQuat);
    debugLineColor.f[1] = 1.0f;
    DebugPushTestLine(m_heli.mainRotorPos, gravVec, 5.0, 0.0, debugLineColor);

    //DirectX::SimpleMath::Matrix rotorTorqueMat = Utility::GetTorqueMat((m_heli.mainRotorPos - m_heli.centerOfMass), rotorForce, mod);
    DirectX::SimpleMath::Quaternion rotorTorqueQuat = Utility::GetTorqueQuat((-axisLine), rotorForce, mod);
    DirectX::SimpleMath::Vector3 rotorVec = m_heli.up;
    rotorVec = DirectX::SimpleMath::Vector3::Transform(rotorVec, rotorTorqueQuat);
    debugLineColor.f[2] = 1.0f;
    //DebugPushTestLine(m_heli.mainRotorPos, testUp2, 5.0, 0.0, debugLineColor);
    DirectX::SimpleMath::Vector3 newUp2 = m_heli.up;
    if (testTorque.Length() > 0.1)
    {
        newUp2 = DirectX::SimpleMath::Vector3::Transform(newUp2, gravTorqueQuat * rotorTorqueQuat);
    }
    else
    {
        newUp2 = DirectX::SimpleMath::Vector3::Transform(newUp2, gravTorqueQuat);
    }
    newUp2.Normalize();
    m_heli.up = newUp2;

    DebugPushUILineDecimalNumber("AxisLineLength ", axisLine.Length(), "");
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
    //DebugPushTestLine(m_heli.mainRotorPos, m_heli.q.mainRotorForceNormal, 5.0, 0.0, debugLineColor);

    m_heli.isVehicleLanding = false;
    m_heli.terrainHightAtPos = m_environment->GetTerrainHeightAtPos(m_heli.q.position);
    
    DebugPushUILineDecimalNumber("Altitude : ", m_heli.q.position.y - m_heli.terrainHightAtPos, "");
    if (m_heli.q.position.y - m_heli.terrainHightAtPos > 0.1)
    {
        m_heli.isVehicleAirborne = true;
    }
    else
    {
        if (m_heli.isVehicleAirborne == true)
        {
            LandVehicle();
            m_heli.isVehicleLanding = true;
        }
        m_heli.isVehicleAirborne = false;
        m_heli.q.position.y = m_heli.terrainHightAtPos;
    }
    
    //TurnVehicle(aTimeDelta);
    UpdateTerrainNorm();   

    UpdateVehicleAlignment();
    RungeKutta4(&m_heli, aTimeDelta);

    if (m_heli.forward.Dot(m_heli.q.velocity) < 0.0)
    {
        m_heli.isVelocityBackwards = true;
    }
    else
    {
        m_heli.isVelocityBackwards = false;
    }
  
    //UpdateVelocity(aTimeDelta);

    m_heli.speed = m_heli.q.velocity.Length();
    UpdateModel(aTimeDelta);

    InputDecay(aTimeDelta);
    DebugPushUILineDecimalNumber("test rotor mag : ", m_heli.collectiveInput, "");

    //UpdateTailYawForce();
  
    m_heli.testAccel = (m_heli.q.velocity.Length() - prevVelocity.Length()) / static_cast<float>(aTimeDelta);
    
    UpdateResistance();

}

void Vehicle::UpdateVelocity(double aTimeDelta)
{
    const float lerpSize = 0.5;
    if (m_heli.isVehicleAirborne == false)
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

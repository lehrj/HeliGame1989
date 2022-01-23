#include "pch.h"
#include "Vehicle.h"



float Vehicle::CalculateLiftCoefficient(const float aAngle)
{
    //const float angleMax = m_heli.mainRotor.pitchAngleMax;
    const float angleMax = Utility::ToRadians(24.0f);
    const float angleMin = m_heli.mainRotor.pitchAngleMin;
    float inputAngle = aAngle;
    if (inputAngle > angleMax)
    {
        inputAngle = angleMax;
    }
    if (inputAngle < angleMin)
    {
        inputAngle = angleMin;
    }

    const float currentCurvePos = (inputAngle / angleMax);

    float Cl;
    float curveDeltaRate;
    float ClTarget;
    if (currentCurvePos < 0.666f)
    {
        curveDeltaRate = 1.0f;
        Cl = curveDeltaRate * currentCurvePos;
    }
    else if (currentCurvePos < 0.80f)
    {
        curveDeltaRate = 1.0f;
        Cl = curveDeltaRate * currentCurvePos;
    }
    else
    {
        curveDeltaRate = -0.85f;
        Cl = curveDeltaRate * currentCurvePos;
    }
    // ////////////////////////////
    const float ClMax = 1.7;
    const float inflectionPoint = 0.75f;
    const float curvePos2 = currentCurvePos / inflectionPoint;
    if (currentCurvePos < inflectionPoint)
    {
        curveDeltaRate = 1.0f;
        Cl = curveDeltaRate * currentCurvePos;
        ClTarget = 1.5f;
        ClTarget = curvePos2 * ClMax;
    }
    else
    {
        float leftBound = ClMax;
        float rightBound = 1.3f;
        float downCurve = rightBound - ClMax;

        const float curvePos4 = (currentCurvePos - inflectionPoint) / (1.0f - inflectionPoint);
        float ClRemove = curvePos4 * downCurve;
        ClTarget = ClMax;
        ClTarget += ClRemove;
    }


    return ClTarget;
}

void Vehicle::DebugToggle()
{
    if (m_debugToggle == false)
    {
        m_debugToggle = true;
    }
    else
    {
        m_debugToggle = false;
    }
}

void Vehicle::DrawModel(const DirectX::SimpleMath::Matrix aView, const DirectX::SimpleMath::Matrix aProj)
{
    m_heliModel.bodyShape->Draw(m_heliModel.bodyMatrix, aView, aProj, m_heliModel.bodyColor);
    m_heliModel.doorShape->Draw(m_heliModel.doorMatrix, aView, aProj, m_heliModel.bodyColor);
    m_heliModel.mainWingShape->Draw(m_heliModel.mainWingMatrix, aView, aProj, m_heliModel.bodyColor);
    m_heliModel.mainWingLeadingEdgeShape->Draw(m_heliModel.mainWingLeadingEdgeMatrix, aView, aProj, m_heliModel.bodyColor);
    m_heliModel.mainWingTailEdgeShape->Draw(m_heliModel.mainWingTailEdgeMatrix, aView, aProj, m_heliModel.bodyColor);
    m_heliModel.wingJetCenterShape->Draw(m_heliModel.wingJetCenterMatrix, aView, aProj, m_heliModel.bodyColor);
    m_heliModel.wingJetForwardHousingShape->Draw(m_heliModel.wingJetForwardHousingLeftMatrix, aView, aProj, m_heliModel.bodyColor);
    m_heliModel.wingJetForwardHousingShape->Draw(m_heliModel.wingJetForwardHousingRightMatrix, aView, aProj, m_heliModel.bodyColor);
    m_heliModel.wingJetForwardCowlShape->Draw(m_heliModel.wingJetForwardCowlLeftMatrix, aView, aProj, m_heliModel.bodyColor);
    m_heliModel.wingJetForwardCowlShape->Draw(m_heliModel.wingJetForwardCowlRightMatrix, aView, aProj, m_heliModel.bodyColor);
    m_heliModel.wingJetCouplingShape->Draw(m_heliModel.wingJetCouplingLeftMatrix, aView, aProj, m_heliModel.bodyColor);
    m_heliModel.wingJetCouplingShape->Draw(m_heliModel.wingJetCouplingRightMatrix, aView, aProj, m_heliModel.bodyColor);

    m_heliModel.sideStripeShape0->Draw(m_heliModel.sideStripeMatrix0, aView, aProj, m_heliModel.stripeColor1);
    m_heliModel.sideStripeShape1->Draw(m_heliModel.sideStripeMatrix1, aView, aProj, m_heliModel.stripeColor1);
    m_heliModel.sideStripeShape2->Draw(m_heliModel.sideStripeMatrix2, aView, aProj, m_heliModel.stripeColor1);
    m_heliModel.sideStripeShape3->Draw(m_heliModel.sideStripeMatrix3, aView, aProj, m_heliModel.stripeColor1);
    m_heliModel.sideStripeShape4->Draw(m_heliModel.sideStripeMatrix4, aView, aProj, m_heliModel.stripeColor1);
    m_heliModel.sideStripeShape5->Draw(m_heliModel.sideStripeMatrix5, aView, aProj, m_heliModel.stripeColor1);
    m_heliModel.bodyCapShape->Draw(m_heliModel.windShieldMatrix, aView, aProj, m_heliModel.windshieldColor);
    m_heliModel.noseCowlShape->Draw(m_heliModel.noseCowlMatrix, aView, aProj, m_heliModel.bodyColor);
    m_heliModel.bodyCapShape->Draw(m_heliModel.bodyRearMatrix, aView, aProj, m_heliModel.bodyColor);
    m_heliModel.sideWindowUpperShape->Draw(m_heliModel.sideWindowUpperMatrix, aView, aProj, m_heliModel.windshieldColor);
    m_heliModel.sideWindowLowerShape->Draw(m_heliModel.sideWindowLowerMatrix, aView, aProj, m_heliModel.windshieldColor);

    m_heliModel.toothShape->Draw(m_heliModel.toothMatrix1, aView, aProj, m_heliModel.windshieldColor);
    m_heliModel.toothShape->Draw(m_heliModel.toothMatrix2, aView, aProj, m_heliModel.windshieldColor);
    m_heliModel.toothShape->Draw(m_heliModel.toothMatrix3, aView, aProj, m_heliModel.windshieldColor);
    m_heliModel.toothShape->Draw(m_heliModel.toothMatrix4, aView, aProj, m_heliModel.windshieldColor);
    m_heliModel.toothShape->Draw(m_heliModel.toothMatrix5, aView, aProj, m_heliModel.windshieldColor);
    m_heliModel.toothShape->Draw(m_heliModel.toothMatrix6, aView, aProj, m_heliModel.windshieldColor);
    m_heliModel.toothShape2->Draw(m_heliModel.toothMatrix7, aView, aProj, m_heliModel.windshieldColor);
    m_heliModel.toothShape2->Draw(m_heliModel.toothMatrix8, aView, aProj, m_heliModel.windshieldColor);
    m_heliModel.toothShape2->Draw(m_heliModel.toothMatrix9, aView, aProj, m_heliModel.windshieldColor);
    m_heliModel.toothShape2->Draw(m_heliModel.toothMatrix10, aView, aProj, m_heliModel.windshieldColor);
    m_heliModel.toothShape2->Draw(m_heliModel.toothMatrix11, aView, aProj, m_heliModel.windshieldColor);
    m_heliModel.toothShape2->Draw(m_heliModel.toothMatrix12, aView, aProj, m_heliModel.windshieldColor);

    //m_heliModel.toothBackdropShape->Draw(m_heliModel.toothBackdropLeftMatrix, aView, aProj, m_heliModel.testColor3);
    //m_heliModel.toothBackdropShape->Draw(m_heliModel.toothBackdropRightMatrix, aView, aProj, m_heliModel.testColor3);
    m_heliModel.toothBackdropTestShape->Draw(m_heliModel.toothBackdropLeftMatrix, aView, aProj, m_heliModel.testColor3);


    m_heliModel.landingGearLowerArmShape->Draw(m_heliModel.landingGearLowerArmMatrix , aView, aProj, m_heliModel.landingGearArmColor);
    m_heliModel.landingGearUpperArmShape->Draw(m_heliModel.landingGearUpperArmMatrix , aView, aProj, m_heliModel.landingGearArmColor);
    m_heliModel.landingGearTireShape->Draw(m_heliModel.landingGearWheelMatrix , aView, aProj, m_heliModel.landingGearTireColor);
    m_heliModel.landingGearWheelShape->Draw(m_heliModel.landingGearWheelMatrix , aView, aProj, m_heliModel.landingGearWheelColor);


    m_heliModel.landingGearLowerArmShape->Draw(m_heliModel.landingGearLowerArmMatrix * m_heliModel.landingGearFrontMatrix, aView, aProj, m_heliModel.landingGearArmColor);
    m_heliModel.landingGearUpperArmShape->Draw(m_heliModel.landingGearUpperArmMatrix * m_heliModel.landingGearFrontMatrix, aView, aProj, m_heliModel.landingGearArmColor);
    m_heliModel.landingGearTireShape->Draw(m_heliModel.landingGearWheelMatrix * m_heliModel.landingGearFrontMatrix, aView, aProj, m_heliModel.landingGearTireColor);
    m_heliModel.landingGearWheelShape->Draw(m_heliModel.landingGearWheelMatrix * m_heliModel.landingGearFrontMatrix, aView, aProj, m_heliModel.landingGearWheelColor);

    m_heliModel.landingGearLowerArmShape->Draw(m_heliModel.landingGearLowerArmMatrix * m_heliModel.landingGearRearLeftMatrix, aView, aProj, m_heliModel.landingGearArmColor);
    m_heliModel.landingGearUpperArmShape->Draw(m_heliModel.landingGearUpperArmMatrix * m_heliModel.landingGearRearLeftMatrix, aView, aProj, m_heliModel.landingGearArmColor);
    m_heliModel.landingGearTireShape->Draw(m_heliModel.landingGearWheelMatrix * m_heliModel.landingGearRearLeftMatrix, aView, aProj, m_heliModel.landingGearTireColor);
    m_heliModel.landingGearWheelShape->Draw(m_heliModel.landingGearWheelMatrix * m_heliModel.landingGearRearLeftMatrix, aView, aProj, m_heliModel.landingGearWheelColor);

    m_heliModel.landingGearLowerArmShape->Draw(m_heliModel.landingGearLowerArmMatrix * m_heliModel.landingGearRearRightMatrix, aView, aProj, m_heliModel.landingGearArmColor);
    m_heliModel.landingGearUpperArmShape->Draw(m_heliModel.landingGearUpperArmMatrix * m_heliModel.landingGearRearRightMatrix, aView, aProj, m_heliModel.landingGearArmColor);
    m_heliModel.landingGearTireShape->Draw(m_heliModel.landingGearWheelMatrix * m_heliModel.landingGearRearRightMatrix, aView, aProj, m_heliModel.landingGearTireColor);
    m_heliModel.landingGearWheelShape->Draw(m_heliModel.landingGearWheelMatrix * m_heliModel.landingGearRearRightMatrix, aView, aProj, m_heliModel.landingGearWheelColor);

    m_heliModel.noseConeShape->Draw(m_heliModel.noseConeMatrix, aView, aProj, m_heliModel.bodyColor);
    m_heliModel.noseBodyShape->Draw(m_heliModel.noseBodyMatrix, aView, aProj, m_heliModel.bodyColor);

    m_heliModel.engineHousingShape->Draw(m_heliModel.engineHousingMatrix, aView, aProj, m_heliModel.bodyColor);
    m_heliModel.engineHousingCornerShape->Draw(m_heliModel.engineHousingFrontRightMatrix, aView, aProj, m_heliModel.bodyColor);
    m_heliModel.engineHousingCornerShape->Draw(m_heliModel.engineHousingFrontLeftMatrix, aView, aProj, m_heliModel.bodyColor);
    m_heliModel.engineHousingCornerShape->Draw(m_heliModel.engineHousingRearLeftMatrix, aView, aProj, m_heliModel.bodyColor);
    m_heliModel.engineHousingCornerShape->Draw(m_heliModel.engineHousingRearRightMatrix, aView, aProj, m_heliModel.bodyColor);

    m_heliModel.engineHousingIntakeShape->Draw(m_heliModel.engineHousingIntakeLeftMatrix, aView, aProj, m_heliModel.stripeColor1);
    m_heliModel.engineHousingIntakeShape->Draw(m_heliModel.engineHousingIntakeRightMatrix, aView, aProj, m_heliModel.stripeColor1);
    m_heliModel.engineHousingFrontShape->Draw(m_heliModel.engineHousingFrontMatrix, aView, aProj, m_heliModel.bodyColor);
    m_heliModel.engineHousingFrontShape->Draw(m_heliModel.engineHousingRearMatrix, aView, aProj, m_heliModel.bodyColor);
    m_heliModel.engineHousingSideShape->Draw(m_heliModel.engineHousingSideLeftMatrix, aView, aProj, m_heliModel.bodyColor);
    m_heliModel.engineHousingSideShape->Draw(m_heliModel.engineHousingSideRightMatrix, aView, aProj, m_heliModel.bodyColor);

    m_heliModel.engineExhaustExteriorShape->Draw(m_heliModel.engineExhaustLeftMatrix, aView, aProj, m_heliModel.stripeColor0);
    m_heliModel.engineExhaustExteriorShape->Draw(m_heliModel.engineExhaustRightMatrix, aView, aProj, m_heliModel.stripeColor0);
    m_heliModel.engineExhaustInteriorShape->Draw(m_heliModel.engineExhaustLeftMatrix, aView, aProj, m_heliModel.stripeColor1);
    m_heliModel.engineExhaustInteriorShape->Draw(m_heliModel.engineExhaustRightMatrix, aView, aProj, m_heliModel.stripeColor1);

    m_heliModel.tailBoomShape->Draw(m_heliModel.tailBoomMatrix, aView, aProj, m_heliModel.bodyColor);
    m_heliModel.tailBoomNewShape->Draw(m_heliModel.tailBoomLowerMatrix, aView, aProj, m_heliModel.bodyColor);
    m_heliModel.tailBoomNewShape->Draw(m_heliModel.tailBoomUpperMatrix, aView, aProj, m_heliModel.bodyColor);
    m_heliModel.tailBoomEndCapShape->Draw(m_heliModel.tailBoomEndCapMatrix, aView, aProj, m_heliModel.bodyColor);

    m_heliModel.tailFinUpperShape->Draw(m_heliModel.tailFinUpperMatrix, aView, aProj, m_heliModel.bodyColor);
    m_heliModel.tailFinUpperTipShape->Draw(m_heliModel.tailFinUpperTipMatrix, aView, aProj, m_heliModel.bodyColor);
    m_heliModel.tailFinLowerShape->Draw(m_heliModel.tailFinLowerMatrix, aView, aProj, m_heliModel.bodyColor);
    m_heliModel.tailFinLowerTipShape->Draw(m_heliModel.tailFinLowerTipMatrix, aView, aProj, m_heliModel.bodyColor);
    m_heliModel.tailFinLeadingEdgeShape->Draw(m_heliModel.tailFinLeadingEdgeLowerMatrix, aView, aProj, m_heliModel.bodyColor);
    m_heliModel.tailFinLeadingEdgeShape->Draw(m_heliModel.tailFinLeadingEdgeUpperMatrix, aView, aProj, m_heliModel.bodyColor);

    m_heliModel.tailWingShape->Draw(m_heliModel.tailWingMatrix, aView, aProj, m_heliModel.bodyColor);
    m_heliModel.tailWingLeadingEdgeShape->Draw(m_heliModel.tailWingLeadingEdgeMatrix, aView, aProj, m_heliModel.bodyColor);
    m_heliModel.tailWingFinShape->Draw(m_heliModel.tailWingFinLeftMatrix, aView, aProj, m_heliModel.bodyColor);
    m_heliModel.tailWingFinShape->Draw(m_heliModel.tailWingFinRightMatrix, aView, aProj, m_heliModel.bodyColor);

    m_heliModel.mainRotorAxelShape->Draw(m_heliModel.mainRotorAxelMatrix, aView, aProj, m_heliModel.axelColor);
    m_heliModel.mainRotorHubShape->Draw(m_heliModel.mainRotorHubMatrix, aView, aProj, m_heliModel.axelColor);

    m_heliModel.mainRotorArmShape->Draw(m_heliModel.mainRotorArmMatrix, aView, aProj, m_heliModel.rotorColor);
    m_heliModel.mainRotorBladeShape->Draw(m_heliModel.mainRotorBladeMatrix1, aView, aProj, m_heliModel.rotorColor);
    m_heliModel.mainRotorBladeShape->Draw(m_heliModel.mainRotorBladeMatrix2, aView, aProj, m_heliModel.rotorColor);

    m_heliModel.mainRotorBladeEdgeShape->Draw(m_heliModel.mainRotorBladeEdgeMatrix1, aView, aProj, m_heliModel.rotorColor);
    m_heliModel.mainRotorBladeEdgeShape->Draw(m_heliModel.mainRotorBladeEdgeMatrix2, aView, aProj, m_heliModel.rotorColor);

    m_heliModel.tailRotorAxelShape->Draw(m_heliModel.tailRotorAxelMatrix, aView, aProj, m_heliModel.axelColor);
    m_heliModel.tailRotorHubShape->Draw(m_heliModel.tailRotorHubMatrix, aView, aProj, m_heliModel.axelColor);
    m_heliModel.tailRotorArmShape->Draw(m_heliModel.tailRotorArmMatrix, aView, aProj, m_heliModel.rotorColor);
    m_heliModel.tailRotorBladeShape->Draw(m_heliModel.tailRotorBladeMatrix1, aView, aProj, m_heliModel.rotorColor);
    m_heliModel.tailRotorBladeShape->Draw(m_heliModel.tailRotorBladeMatrix2, aView, aProj, m_heliModel.rotorColor);
    m_heliModel.tailRotorBladeEdgeShape->Draw(m_heliModel.tailRotorBladeEdgeMatrix1, aView, aProj, m_heliModel.rotorColor);
    m_heliModel.tailRotorBladeEdgeShape->Draw(m_heliModel.tailRotorBladeEdgeMatrix2, aView, aProj, m_heliModel.rotorColor);
}

void Vehicle::InitializeEngine(Engine& aEngine)
{
    // all data is temp and just guessed at. update once good data from a turboshaft engine found, currently using an internal combustion engine model
    aEngine.currentTorque = 0.0f;
    aEngine.gearRatio = 3.44f;
    aEngine.powerCurveLow = 0.0f;
    aEngine.powerCurveMid = 0.025f;
    aEngine.powerCurveHigh = -0.032f;
    aEngine.revLimit = 7400.0f;
    aEngine.rpm = 0.0f;
    aEngine.torquePointLow = 220.0f;
    aEngine.torquePointMid = 195.0f;
    aEngine.torquePointHigh = 457.2f;
}

void Vehicle::InitializeFlightControls(ControlInput& aInput)
{
    aInput.collectiveInput = 0.0f;
    aInput.cyclicInputPitch = 0.0f;
    aInput.cyclicInputPitchIsPressed = false;
    aInput.cyclicInputRoll = 0.0f;
    aInput.cyclicInputRollIsPressed = false;
    aInput.cyclicStick = DirectX::SimpleMath::Vector3::UnitY;
    aInput.throttleInput = 0.0f;
    aInput.yawPedalInput = 0.0f;
    aInput.yawPedalIsPressed = false;
}

void Vehicle::InitializeModel(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext, HeliData& aHeliData)
{
    // set model part colors
    m_heliModel.axelColor = DirectX::SimpleMath::Vector4(0.411764741f, 0.411764741f, 0.411764741f, 1.0f);
    //m_heliModel.bodyColor = DirectX::SimpleMath::Vector4(0.501960814f, 0.501960814f, 0.501960814f, 1.0f);
    m_heliModel.bodyColor = DirectX::SimpleMath::Vector4(0.2f, 0.2f, 0.2f, 1.0f);
    m_heliModel.landingGearArmColor = DirectX::SimpleMath::Vector4(0.9f, 0.9f, 0.9f, 1.0f);
    m_heliModel.landingGearTireColor = DirectX::SimpleMath::Vector4(0.411764741f, 0.411764741f, 0.411764741f, 1.0f);
    //m_heliModel.landingGearWheelColor = DirectX::SimpleMath::Vector4(0.501960814f, 0.501960814f, 0.501960814f, 1.0f);
    m_heliModel.landingGearWheelColor = DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f);
    m_heliModel.rotorColor = DirectX::SimpleMath::Vector4(0.827451050f, 0.827451050f, 0.827451050f, 1.0f);
    m_heliModel.stripeColor0 = DirectX::SimpleMath::Vector4(0.8, 0.8f, 0.8f, 1.0f);
    m_heliModel.stripeColor1 = DirectX::SimpleMath::Vector4(0.0, 0.0f, 0.0f, 1.0f);
    m_heliModel.windshieldColor = DirectX::SimpleMath::Vector4(0.662745118f, 0.662745118f, 0.662745118f, 1.20f);
    m_heliModel.testColor1 = DirectX::SimpleMath::Vector4(1.0, 0.0f, 0.0f, 1.0f);
    m_heliModel.testColor2 = DirectX::SimpleMath::Vector4(1.0, 1.0f, 1.0f, 1.0f);
    m_heliModel.testColor3 = DirectX::SimpleMath::Vector4(1.0, 0.0f, 0.0f, 1.0f);

    // set model shapes and local positions   
    // main body
    const DirectX::SimpleMath::Vector3 bodySize(5.0f, 3.0f, 3.0f);
    const DirectX::SimpleMath::Vector3 bodyTranslation(0.0f, bodySize.y * 0.5f, 0.0f);
    m_heliModel.bodyShape = DirectX::GeometricPrimitive::CreateBox(aContext.Get(), bodySize);
    m_heliModel.bodyMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.bodyMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(bodyTranslation);
    m_heliModel.localBodyMatrix = m_heliModel.bodyMatrix;

    // main wing
    const DirectX::SimpleMath::Vector3 mainWingSize(1.5f, 0.9f, 5.8f);
    const DirectX::SimpleMath::Vector3 mainWingTranslation(-mainWingSize.x * 0.65f, mainWingSize.y * 0.6f, 0.0f);
    m_heliModel.mainWingShape = DirectX::GeometricPrimitive::CreateBox(aContext.Get(), mainWingSize);
    m_heliModel.mainWingMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.mainWingMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(mainWingTranslation);
    m_heliModel.localMainWingMatrix = m_heliModel.mainWingMatrix;

    // main wing leading edge   
    DirectX::SimpleMath::Vector3 mainWingLeadingEdgeSize((mainWingSize.y * 0.5f) * sqrt(2.0f), mainWingSize.z, (mainWingSize.y * 0.5f) * sqrt(2.0f));
    m_heliModel.mainWingLeadingEdgeShape = DirectX::GeometricPrimitive::CreateBox(aContext.Get(), mainWingLeadingEdgeSize);
    DirectX::SimpleMath::Vector3 mainWingLeadingEdgeTranslation(mainWingTranslation.x + mainWingSize.x * 0.5f, mainWingTranslation.y, mainWingTranslation.z);
    m_heliModel.mainWingLeadingEdgeMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.mainWingLeadingEdgeMatrix = DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(45.0f));
    m_heliModel.mainWingLeadingEdgeMatrix *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(90.0f));
    m_heliModel.mainWingLeadingEdgeMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(mainWingLeadingEdgeTranslation);
    m_heliModel.localMainWingLeadingEdgeMatrix = m_heliModel.mainWingLeadingEdgeMatrix;
    
    // main wing leading edge   
    /*
    const float mainWingLeadingEdgeDiameter = mainWingSize.y / sin(Utility::ToRadians(60.0f));
    const float mainWingLeadingEdgeHeight = mainWingSize.z;
    DirectX::SimpleMath::Vector3 mainWingLeadingEdgeTranslation = mainWingTranslation;
    const float median = (mainWingLeadingEdgeDiameter * 0.5f) + ((mainWingLeadingEdgeDiameter * 0.5f) / 2.0f);
    mainWingLeadingEdgeTranslation.x -= -mainWingSize.x * 0.5f - mainWingLeadingEdgeDiameter * 0.5f + (mainWingLeadingEdgeDiameter - median);
    m_heliModel.mainWingLeadingEdgeShape = DirectX::GeometricPrimitive::CreateCylinder(aContext.Get(), mainWingLeadingEdgeHeight, mainWingLeadingEdgeDiameter, 3);
    m_heliModel.mainWingLeadingEdgeMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.mainWingLeadingEdgeMatrix = DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(-30.0f));
    m_heliModel.mainWingLeadingEdgeMatrix *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(90.0f));
    m_heliModel.mainWingLeadingEdgeMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(mainWingLeadingEdgeTranslation);
    m_heliModel.localMainWingLeadingEdgeMatrix = m_heliModel.mainWingLeadingEdgeMatrix;
    */

    // main wing tail edge
    const float mainWingTailEdgeDiameter = mainWingSize.y / sin(Utility::ToRadians(60.0f));
    const float mainWingTailEdgeHeight = mainWingSize.z;
    DirectX::SimpleMath::Vector3 mainWingTailEdgeTranslation = mainWingTranslation;
    const float median = (mainWingTailEdgeDiameter * 0.5f) + ((mainWingTailEdgeDiameter * 0.5f) / 2.0f);
    mainWingTailEdgeTranslation.x += -mainWingSize.x * 0.5f - mainWingTailEdgeDiameter * 0.5f + (mainWingTailEdgeDiameter - median);
    m_heliModel.mainWingTailEdgeShape = DirectX::GeometricPrimitive::CreateCylinder(aContext.Get(), mainWingTailEdgeHeight, mainWingTailEdgeDiameter, 3);
    m_heliModel.mainWingTailEdgeMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.mainWingTailEdgeMatrix = DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(30.0f));
    m_heliModel.mainWingTailEdgeMatrix *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(90.0f));  
    m_heliModel.mainWingTailEdgeMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(mainWingTailEdgeTranslation);
    m_heliModel.localMainWingTailEdgeMatrix = m_heliModel.mainWingTailEdgeMatrix;


    // wing jet center
    const DirectX::SimpleMath::Vector3 wingJetCenterSize(mainWingSize.x * 1.0, mainWingSize.y * 1.2f, mainWingSize.z * 0.6f);
    m_heliModel.wingJetCenterShape = DirectX::GeometricPrimitive::CreateBox(aContext.Get(), wingJetCenterSize);
    const DirectX::SimpleMath::Vector3 wingJetCenterTranslation(mainWingTranslation.x + (mainWingLeadingEdgeSize.x * 0.75f),  wingJetCenterSize.y  * 0.5f, mainWingTranslation.z);
    m_heliModel.wingJetCenterMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.wingJetCenterMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(wingJetCenterTranslation);
    m_heliModel.localWingJetCenterMatrix = m_heliModel.wingJetCenterMatrix;

    // wing jet forward housing left
    const float wingJetForwardHousingDiameter = wingJetCenterSize.y;
    const float wingJetForwardHousingHeight = wingJetCenterSize.x;
    m_heliModel.wingJetForwardHousingShape = DirectX::GeometricPrimitive::CreateCylinder(aContext.Get(), wingJetForwardHousingHeight, wingJetForwardHousingDiameter, 6);
    const DirectX::SimpleMath::Vector3 wingJetForwardHousingLeftTranslation(wingJetCenterTranslation.x, wingJetCenterTranslation.y, -wingJetCenterSize.z * 0.5f);
    m_heliModel.wingJetForwardHousingLeftMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.wingJetForwardHousingLeftMatrix = DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(90.0f));
    m_heliModel.wingJetForwardHousingLeftMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(wingJetForwardHousingLeftTranslation);
    m_heliModel.localWingJetForwardHousingLeftMatrix = m_heliModel.wingJetForwardHousingLeftMatrix;

    // wing jet forward housing right
    const DirectX::SimpleMath::Vector3 wingJetForwardHousingRightTranslation(wingJetCenterTranslation.x, wingJetCenterTranslation.y, wingJetCenterSize.z * 0.5f);
    m_heliModel.wingJetForwardHousingRightMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.wingJetForwardHousingRightMatrix = DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(90.0f));
    m_heliModel.wingJetForwardHousingRightMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(wingJetForwardHousingRightTranslation);
    m_heliModel.localWingJetForwardHousingRightMatrix = m_heliModel.wingJetForwardHousingRightMatrix;

    // wing jet forward cowl left  
    const float wingJetForwardCowlThickness = 0.3f;
    const float wingJetForwardCowlDiameter = wingJetForwardHousingDiameter - wingJetForwardCowlThickness;
    m_heliModel.wingJetForwardCowlShape = DirectX::GeometricPrimitive::CreateTorus(aContext.Get(), wingJetForwardCowlDiameter, wingJetForwardCowlThickness, 6);
    const DirectX::SimpleMath::Vector3 wingJetForwardCowlLeftTranslation(wingJetForwardHousingLeftTranslation.x + wingJetForwardHousingHeight * 0.5f, wingJetForwardHousingLeftTranslation.y, -wingJetForwardHousingLeftTranslation.z);
    m_heliModel.wingJetForwardCowlLeftMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.wingJetForwardCowlLeftMatrix = DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(90.0f));
    m_heliModel.wingJetForwardCowlLeftMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(wingJetForwardCowlLeftTranslation);
    m_heliModel.localWingJetForwardCowlLeftMatrix = m_heliModel.wingJetForwardCowlLeftMatrix;

    // wing jet forward cowl right
    const DirectX::SimpleMath::Vector3 wingJetForwardCowlRightTranslation(wingJetForwardHousingRightTranslation.x + wingJetForwardHousingHeight * 0.5f, wingJetForwardHousingRightTranslation.y, -wingJetForwardHousingRightTranslation.z);
    m_heliModel.wingJetForwardCowlRightMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.wingJetForwardCowlRightMatrix = DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(90.0f));
    m_heliModel.wingJetForwardCowlRightMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(wingJetForwardCowlRightTranslation);
    m_heliModel.localWingJetForwardCowlRightMatrix = m_heliModel.wingJetForwardCowlRightMatrix;

    // wing jet coupling left  
    const float wingJetCouplingHeight = wingJetForwardHousingHeight;
    const float wingJetCouplingDiameter = wingJetForwardHousingDiameter;
    m_heliModel.wingJetCouplingShape = DirectX::GeometricPrimitive::CreateCone(aContext.Get(), wingJetCouplingDiameter, wingJetCouplingHeight, 6);
    const DirectX::SimpleMath::Vector3 wingJetCouplingLeftTranslation(wingJetForwardHousingLeftTranslation.x - wingJetForwardHousingHeight, wingJetForwardHousingLeftTranslation.y, -wingJetForwardHousingLeftTranslation.z);
    m_heliModel.wingJetCouplingLeftMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.wingJetCouplingLeftMatrix = DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(90.0f));
    m_heliModel.wingJetCouplingLeftMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(wingJetCouplingLeftTranslation);
    m_heliModel.localWingJetCouplingLeftMatrix = m_heliModel.wingJetCouplingLeftMatrix;

    // wing jet coupling right
    const DirectX::SimpleMath::Vector3 wingJetCouplingRightTranslation(wingJetForwardHousingRightTranslation.x - wingJetForwardHousingHeight, wingJetForwardHousingRightTranslation.y, -wingJetForwardHousingRightTranslation.z);
    m_heliModel.wingJetCouplingRightMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.wingJetCouplingRightMatrix = DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(90.0f));
    m_heliModel.wingJetCouplingRightMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(wingJetCouplingRightTranslation);
    m_heliModel.localWingJetCouplingRightMatrix = m_heliModel.wingJetCouplingRightMatrix;



    // side stripe 0 base
    const DirectX::SimpleMath::Vector3 sideStripeSize0(4.1f, 0.06f, bodySize.z + 0.001);
    const DirectX::SimpleMath::Vector3 sideStripeTranslation0(-0.155f, 2.85, 0.0f);
    m_heliModel.sideStripeShape0 = DirectX::GeometricPrimitive::CreateBox(aContext.Get(), sideStripeSize0);
    m_heliModel.sideStripeMatrix0 = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.sideStripeMatrix0 *= DirectX::SimpleMath::Matrix::CreateTranslation(sideStripeTranslation0);
    m_heliModel.localSidestripeMatrix0 = m_heliModel.sideStripeMatrix0;

    // side stripe 1 
    const float stripeSpacing = -0.15f;
    const float stripeIndent = -0.3f;
    DirectX::SimpleMath::Vector3 sideStripeSize1 = sideStripeSize0;
    sideStripeSize1.x += stripeIndent;
    sideStripeSize1.x = 3.0f;
    DirectX::SimpleMath::Vector3 sideStripeTranslation1 = sideStripeTranslation0;
    sideStripeTranslation1.x -= (sideStripeSize0.x - sideStripeSize1.x) * 0.5f;
    sideStripeTranslation1.y += stripeSpacing;
    m_heliModel.sideStripeShape1 = DirectX::GeometricPrimitive::CreateBox(aContext.Get(), sideStripeSize1);
    m_heliModel.sideStripeMatrix1 = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.sideStripeMatrix1 *= DirectX::SimpleMath::Matrix::CreateTranslation(sideStripeTranslation1);
    m_heliModel.localSidestripeMatrix1 = m_heliModel.sideStripeMatrix1;

    // side stripe 2  
    DirectX::SimpleMath::Vector3 sideStripeSize2 = sideStripeSize1;
    sideStripeSize2.x += stripeIndent;
    DirectX::SimpleMath::Vector3 sideStripeTranslation2 = sideStripeTranslation1;
    sideStripeTranslation2.x -= (sideStripeSize1.x - sideStripeSize2.x) * 0.5f;
    sideStripeTranslation2.y += stripeSpacing;
    m_heliModel.sideStripeShape2 = DirectX::GeometricPrimitive::CreateBox(aContext.Get(), sideStripeSize2);
    m_heliModel.sideStripeMatrix2 = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.sideStripeMatrix2 *= DirectX::SimpleMath::Matrix::CreateTranslation(sideStripeTranslation2);
    m_heliModel.localSidestripeMatrix2 = m_heliModel.sideStripeMatrix2;

    // side stripe 3
    DirectX::SimpleMath::Vector3 sideStripeSize3 = sideStripeSize2;
    sideStripeSize3.x += stripeIndent;
    DirectX::SimpleMath::Vector3 sideStripeTranslation3 = sideStripeTranslation2;
    sideStripeTranslation3.x -= (sideStripeSize2.x - sideStripeSize3.x) * 0.5f;
    sideStripeTranslation3.y += stripeSpacing;
    m_heliModel.sideStripeShape3 = DirectX::GeometricPrimitive::CreateBox(aContext.Get(), sideStripeSize3);
    m_heliModel.sideStripeMatrix3 = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.sideStripeMatrix3 *= DirectX::SimpleMath::Matrix::CreateTranslation(sideStripeTranslation3);
    m_heliModel.localSidestripeMatrix3 = m_heliModel.sideStripeMatrix3;

    // side stripe 4
    DirectX::SimpleMath::Vector3 sideStripeSize4 = sideStripeSize3;
    sideStripeSize4.x += stripeIndent;
    DirectX::SimpleMath::Vector3 sideStripeTranslation4 = sideStripeTranslation3;
    sideStripeTranslation4.x -= (sideStripeSize3.x - sideStripeSize4.x) * 0.5f;
    sideStripeTranslation4.y += stripeSpacing;
    m_heliModel.sideStripeShape4 = DirectX::GeometricPrimitive::CreateBox(aContext.Get(), sideStripeSize4);
    m_heliModel.sideStripeMatrix4 = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.sideStripeMatrix4 *= DirectX::SimpleMath::Matrix::CreateTranslation(sideStripeTranslation4);
    m_heliModel.localSidestripeMatrix4 = m_heliModel.sideStripeMatrix4;

    // side stripe 5
    DirectX::SimpleMath::Vector3 sideStripeSize5 = sideStripeSize4;
    sideStripeSize5.x += stripeIndent;
    DirectX::SimpleMath::Vector3 sideStripeTranslation5 = sideStripeTranslation4;
    sideStripeTranslation5.x -= (sideStripeSize4.x - sideStripeSize5.x) * 0.5f;
    sideStripeTranslation5.y += stripeSpacing;
    m_heliModel.sideStripeShape5 = DirectX::GeometricPrimitive::CreateBox(aContext.Get(), sideStripeSize5);
    m_heliModel.sideStripeMatrix5 = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.sideStripeMatrix5 *= DirectX::SimpleMath::Matrix::CreateTranslation(sideStripeTranslation5);
    m_heliModel.localSidestripeMatrix5 = m_heliModel.sideStripeMatrix5;
  

    // landing gear parts
    const float landingGearTireDiameter = 0.50f;
    const float landingGearTireThickness = 0.25f;
    const float landingGearHeight = 0.4f;
    const float landingGearWheelDiameter = landingGearTireDiameter * 0.94f;
    const float landingGearWheelHeight = landingGearTireThickness * 1.05f;
    m_heliModel.landingGearWheelShape = DirectX::GeometricPrimitive::CreateCylinder(aContext.Get(), landingGearWheelHeight, landingGearWheelDiameter);
    m_heliModel.landingGearTireShape = DirectX::GeometricPrimitive::CreateTorus(aContext.Get(), landingGearTireDiameter, landingGearTireThickness);

    const float gearArmThickness = 0.1f;
    const float gearArmDiameter = landingGearTireDiameter * 1.25f;
    m_heliModel.landingGearLowerArmShape = DirectX::GeometricPrimitive::CreateTorus(aContext.Get(), gearArmDiameter, gearArmThickness, 4);

    m_heliModel.landingGearUpperArmShape = DirectX::GeometricPrimitive::CreateTorus(aContext.Get(), gearArmDiameter, gearArmThickness, 4);
  
    const float landingGearUpperArmAngle = Utility::ToRadians(35.0f);
    DirectX::SimpleMath::Vector3 landingGearUpperArmTranslation(gearArmDiameter * 0.5f, 0.0f, 0.0f);
    m_heliModel.landingGearUpperArmMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.landingGearUpperArmMatrix *= DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(45.0f));
    m_heliModel.landingGearUpperArmMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(landingGearUpperArmTranslation);   
    m_heliModel.landingGearUpperArmMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(landingGearUpperArmAngle);
    m_heliModel.localLandingGearUperArmMatrix = m_heliModel.landingGearUpperArmMatrix;

    DirectX::SimpleMath::Vector3 landingGearLowerArmTranslation(gearArmDiameter * 0.5f, 0.0f, 0.0f);
    const float landingGearLowerArmAngle = Utility::ToRadians(-35.0f);
    m_heliModel.landingGearLowerArmMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.landingGearLowerArmMatrix *= DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(45.0f));
    m_heliModel.landingGearLowerArmMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(-landingGearLowerArmTranslation);
    m_heliModel.landingGearLowerArmMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(landingGearLowerArmAngle);
    DirectX::SimpleMath::Vector3 landingGearLowerArmTranslation2 = landingGearUpperArmTranslation;
    landingGearLowerArmTranslation2.x *= 3.0f;
    landingGearLowerArmTranslation2.x -= gearArmThickness * 1.0f;
    m_heliModel.landingGearLowerArmMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(landingGearUpperArmTranslation + landingGearUpperArmTranslation + landingGearUpperArmTranslation);
    m_heliModel.localLandingGearLowerArmMatrix = m_heliModel.landingGearLowerArmMatrix;
    DirectX::SimpleMath::Vector3 landingGearWheelTranslation(landingGearTireDiameter * 0.5f, gearArmDiameter * 0.1f, 0.0f);
    DirectX::SimpleMath::Vector3 landingGearWheelTranslationTest = landingGearWheelTranslation;
    landingGearWheelTranslation.x += gearArmDiameter * cos(landingGearUpperArmAngle);
    landingGearWheelTranslation.x += gearArmDiameter * cos(landingGearLowerArmAngle);
    landingGearWheelTranslation.x -= (gearArmDiameter * 0.85f) * cos(Utility::ToRadians(35.0f));
    m_heliModel.landingGearWheelMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.landingGearWheelMatrix = DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(90.0f));
    m_heliModel.landingGearWheelMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(landingGearWheelTranslation);
    m_heliModel.localLandingGearWheelMatrix = m_heliModel.landingGearWheelMatrix;

    // landing gear front
    const float landingGearVerticalOffset = 0.3f;
    const DirectX::SimpleMath::Vector3 landingGearFrontTranslation(bodySize.x * 0.6f, landingGearVerticalOffset, 0.0f);
    m_heliModel.landingGearFrontMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.landingGearFrontMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(landingGearFrontTranslation);
    m_heliModel.localLandingGearFrontMatrix = m_heliModel.landingGearFrontMatrix;

    // landing gear back left
    const DirectX::SimpleMath::Vector3 landingGearRearLeftTranslation(bodySize.x * -0.34f, landingGearVerticalOffset, bodySize.z * -0.75f);
    m_heliModel.landingGearRearLeftMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.landingGearRearLeftMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(landingGearRearLeftTranslation);
    m_heliModel.localLandingGearRearLeftMatrix = m_heliModel.landingGearRearLeftMatrix;

    // landing gear back right
    const DirectX::SimpleMath::Vector3 landingGearRearRightTranslation(bodySize.x * -0.34f, landingGearVerticalOffset, bodySize.z * 0.75f);
    m_heliModel.landingGearRearRightMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.landingGearRearRightMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(landingGearRearRightTranslation);
    m_heliModel.localLandingGearRearRightMatrix = m_heliModel.landingGearRearRightMatrix;

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
    DirectX::SimpleMath::Vector3 noseConeSize(noseConeLength, bodySize.y * 0.5f, noseConeLength);
    float noseConeOffSetY = 0.00f;
    noseConeSize.y += noseConeOffSetY;
    DirectX::SimpleMath::Vector3 noseConeTranslation(noseConeXoffset, bodySize.y * 0.25f, 0.0f);
    noseConeTranslation.y += noseConeOffSetY * 0.5f;
    m_heliModel.noseConeShape = DirectX::GeometricPrimitive::CreateBox(aContext.Get(), noseConeSize);
    m_heliModel.noseConeMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.noseConeMatrix *= DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(45.0));
    m_heliModel.noseConeMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(noseConeTranslation);
    m_heliModel.localNoseConeMatrix = m_heliModel.noseConeMatrix;

    //////////////////////////////////////////////////////////////////////////////////////////////////////////

    // Nose teeth
    const float toothHeight = 0.1f;
    const float toothDiameter = 0.75f;
    const float toothDiameter2 = 0.75f;
    m_heliModel.toothShape = DirectX::GeometricPrimitive::CreateCylinder(aContext.Get(), toothHeight, toothDiameter, 3);
    m_heliModel.toothShape2 = DirectX::GeometricPrimitive::CreateCylinder(aContext.Get(), toothHeight, toothDiameter2, 3);
    
    const float noseLength = (noseConeSize.z * 0.5f) * sqrt(2.0f);
    DirectX::SimpleMath::Vector3 toothUpperTranslation(noseConeTranslation.x, noseConeTranslation.y + toothDiameter * 0.65, 0.0f);
    DirectX::SimpleMath::Vector3 toothLowerTranslation(noseConeTranslation.x, noseConeTranslation.y - toothDiameter * 0.65, 0.0f);
    DirectX::SimpleMath::Vector3 toothHorizontalTranslation;

    DirectX::SimpleMath::Matrix toothRotX1 = DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(90.0));
    DirectX::SimpleMath::Matrix toothRotY1 = DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(45.0f));
    DirectX::SimpleMath::Matrix toothRotX2 = DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(-90.0));
    DirectX::SimpleMath::Matrix toothRotY2 = DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(-45.0f));
    float toothRot = Utility::ToRadians(80.0);
    toothHorizontalTranslation.x = noseLength * cos(toothRot);
    toothHorizontalTranslation.y = 0.0f;
    toothHorizontalTranslation.z = noseLength - (noseLength * cos(toothRot));
    m_heliModel.toothMatrix1 = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.toothMatrix1 *= toothRotX1;
    m_heliModel.toothMatrix1 *= toothRotY1;
    m_heliModel.toothMatrix1 *= DirectX::SimpleMath::Matrix::CreateTranslation(toothUpperTranslation);
    m_heliModel.toothMatrix1 *= DirectX::SimpleMath::Matrix::CreateTranslation(toothHorizontalTranslation);
    m_heliModel.localToothMatrix1 = m_heliModel.toothMatrix1;

    toothRot = Utility::ToRadians(60.0);
    toothHorizontalTranslation.x = noseLength * cos(toothRot);
    toothHorizontalTranslation.y = 0.0f;
    toothHorizontalTranslation.z = noseLength - (noseLength * cos(toothRot));
    m_heliModel.toothMatrix2 = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.toothMatrix2 *= toothRotX1;
    m_heliModel.toothMatrix2 *= toothRotY1;
    m_heliModel.toothMatrix2 *= DirectX::SimpleMath::Matrix::CreateTranslation(toothUpperTranslation);
    m_heliModel.toothMatrix2 *= DirectX::SimpleMath::Matrix::CreateTranslation(toothHorizontalTranslation);
    m_heliModel.localToothMatrix2 = m_heliModel.toothMatrix2;

    toothRot = Utility::ToRadians(35.0);
    toothHorizontalTranslation.x = noseLength * cos(toothRot);
    toothHorizontalTranslation.y = 0.0f;
    toothHorizontalTranslation.z = noseLength - (noseLength * cos(toothRot));
    m_heliModel.toothMatrix3 = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.toothMatrix3 *= toothRotX1;
    m_heliModel.toothMatrix3 *= toothRotY1;
    m_heliModel.toothMatrix3 *= DirectX::SimpleMath::Matrix::CreateTranslation(toothUpperTranslation);
    m_heliModel.toothMatrix3 *= DirectX::SimpleMath::Matrix::CreateTranslation(toothHorizontalTranslation);
    m_heliModel.localToothMatrix3 = m_heliModel.toothMatrix3;

    toothRot = Utility::ToRadians(-80.0);
    toothHorizontalTranslation.x = noseLength * cos(toothRot);
    toothHorizontalTranslation.y = 0.0f;
    toothHorizontalTranslation.z = -noseLength + (noseLength * cos(toothRot));
    m_heliModel.toothMatrix4 = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.toothMatrix4 *= toothRotX1;
    m_heliModel.toothMatrix4 *= toothRotY2;
    m_heliModel.toothMatrix4 *= DirectX::SimpleMath::Matrix::CreateTranslation(toothUpperTranslation);
    m_heliModel.toothMatrix4 *= DirectX::SimpleMath::Matrix::CreateTranslation(toothHorizontalTranslation);
    m_heliModel.localToothMatrix4 = m_heliModel.toothMatrix4;

    toothRot = Utility::ToRadians(-60.0);
    toothHorizontalTranslation.x = noseLength * cos(toothRot);
    toothHorizontalTranslation.y = 0.0f;
    toothHorizontalTranslation.z = -noseLength + (noseLength * cos(toothRot));
    m_heliModel.toothMatrix5 = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.toothMatrix5 *= toothRotX1;
    m_heliModel.toothMatrix5 *= toothRotY2;
    m_heliModel.toothMatrix5 *= DirectX::SimpleMath::Matrix::CreateTranslation(toothUpperTranslation);
    m_heliModel.toothMatrix5 *= DirectX::SimpleMath::Matrix::CreateTranslation(toothHorizontalTranslation);
    m_heliModel.localToothMatrix5 = m_heliModel.toothMatrix5;

    toothRot = Utility::ToRadians(-35.0);
    toothHorizontalTranslation.x = noseLength * cos(toothRot);
    toothHorizontalTranslation.y = 0.0f;
    toothHorizontalTranslation.z = -noseLength + (noseLength * cos(toothRot));
    m_heliModel.toothMatrix6 = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.toothMatrix6 *= toothRotX1;
    m_heliModel.toothMatrix6 *= toothRotY2;
    m_heliModel.toothMatrix6 *= DirectX::SimpleMath::Matrix::CreateTranslation(toothUpperTranslation);
    m_heliModel.toothMatrix6*= DirectX::SimpleMath::Matrix::CreateTranslation(toothHorizontalTranslation);
    m_heliModel.localToothMatrix6 = m_heliModel.toothMatrix6;

    toothRot = Utility::ToRadians(80.0);
    toothHorizontalTranslation.x = noseLength * cos(toothRot);
    toothHorizontalTranslation.y = 0.0f;
    toothHorizontalTranslation.z = noseLength - (noseLength * cos(toothRot));
    m_heliModel.toothMatrix7 = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.toothMatrix7 *= toothRotX2;
    m_heliModel.toothMatrix7 *= toothRotY1;
    m_heliModel.toothMatrix7 *= DirectX::SimpleMath::Matrix::CreateTranslation(toothLowerTranslation);
    m_heliModel.toothMatrix7 *= DirectX::SimpleMath::Matrix::CreateTranslation(toothHorizontalTranslation);
    m_heliModel.localToothMatrix7 = m_heliModel.toothMatrix7;

    toothRot = Utility::ToRadians(60.0);
    toothHorizontalTranslation.x = noseLength * cos(toothRot);
    toothHorizontalTranslation.y = 0.0f;
    toothHorizontalTranslation.z = noseLength - (noseLength * cos(toothRot));
    m_heliModel.toothMatrix8 = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.toothMatrix8 *= toothRotX2;
    m_heliModel.toothMatrix8 *= toothRotY1;
    m_heliModel.toothMatrix8 *= DirectX::SimpleMath::Matrix::CreateTranslation(toothLowerTranslation);
    m_heliModel.toothMatrix8 *= DirectX::SimpleMath::Matrix::CreateTranslation(toothHorizontalTranslation);
    m_heliModel.localToothMatrix8 = m_heliModel.toothMatrix8;

    toothRot = Utility::ToRadians(35.0);
    toothHorizontalTranslation.x = noseLength * cos(toothRot);
    toothHorizontalTranslation.y = 0.0f;
    toothHorizontalTranslation.z = noseLength - (noseLength * cos(toothRot));
    m_heliModel.toothMatrix9 = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.toothMatrix9 *= toothRotX2;
    m_heliModel.toothMatrix9 *= toothRotY1;
    m_heliModel.toothMatrix9 *= DirectX::SimpleMath::Matrix::CreateTranslation(toothLowerTranslation);
    m_heliModel.toothMatrix9 *= DirectX::SimpleMath::Matrix::CreateTranslation(toothHorizontalTranslation);
    m_heliModel.localToothMatrix9 = m_heliModel.toothMatrix9;

    toothRot = Utility::ToRadians(-35.0);
    toothHorizontalTranslation.x = noseLength * cos(toothRot);
    toothHorizontalTranslation.y = 0.0f;
    toothHorizontalTranslation.z = -noseLength + (noseLength * cos(toothRot));
    m_heliModel.toothMatrix10 = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.toothMatrix10 *= toothRotX2;
    m_heliModel.toothMatrix10 *= toothRotY2;
    m_heliModel.toothMatrix10 *= DirectX::SimpleMath::Matrix::CreateTranslation(toothLowerTranslation);
    m_heliModel.toothMatrix10 *= DirectX::SimpleMath::Matrix::CreateTranslation(toothHorizontalTranslation);
    m_heliModel.localToothMatrix10 = m_heliModel.toothMatrix10;

    toothRot = Utility::ToRadians(-60.0);
    toothHorizontalTranslation.x = noseLength * cos(toothRot);
    toothHorizontalTranslation.y = 0.0f;
    toothHorizontalTranslation.z = -noseLength + (noseLength * cos(toothRot));
    m_heliModel.toothMatrix11 = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.toothMatrix11 *= toothRotX2;
    m_heliModel.toothMatrix11 *= toothRotY2;
    m_heliModel.toothMatrix11 *= DirectX::SimpleMath::Matrix::CreateTranslation(toothLowerTranslation);
    m_heliModel.toothMatrix11 *= DirectX::SimpleMath::Matrix::CreateTranslation(toothHorizontalTranslation);
    m_heliModel.localToothMatrix11 = m_heliModel.toothMatrix11;

    toothRot = Utility::ToRadians(-80.0);
    toothHorizontalTranslation.x = noseLength * cos(toothRot);
    toothHorizontalTranslation.y = 0.0f;
    toothHorizontalTranslation.z = -noseLength + (noseLength * cos(toothRot));
    m_heliModel.toothMatrix12 = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.toothMatrix12 *= toothRotX2;
    m_heliModel.toothMatrix12 *= toothRotY2;
    m_heliModel.toothMatrix12 *= DirectX::SimpleMath::Matrix::CreateTranslation(toothLowerTranslation);
    m_heliModel.toothMatrix12 *= DirectX::SimpleMath::Matrix::CreateTranslation(toothHorizontalTranslation);
    m_heliModel.localToothMatrix12 = m_heliModel.toothMatrix12;

    // nose tooth mouth backdrop left
    const float toothBackdropHeight = noseConeSize.x * 0.99f;
    const float toothBackdropDiameter = -noseConeSize.y * 0.95f;
    m_heliModel.toothBackdropShape = DirectX::GeometricPrimitive::CreateCylinder(aContext.Get(), toothBackdropHeight, toothBackdropDiameter, 3);
    DirectX::SimpleMath::Vector3 toothBackDropTranslation(noseConeTranslation.x + toothBackdropDiameter * 0.32f, noseConeTranslation.y + toothBackdropDiameter * 0.0f, 0.0f);
    DirectX::SimpleMath::Vector3 toothBackdropHorizontalTranslation;
    toothRot = Utility::ToRadians(47.8f);
    toothBackdropHorizontalTranslation.x = noseLength * cos(toothRot);
    toothBackdropHorizontalTranslation.y = 0.0f;
    toothBackdropHorizontalTranslation.z = - noseLength + (noseLength * cos(toothRot));
    m_heliModel.toothBackdropLeftMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.toothBackdropLeftMatrix *= DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(-30.0));
    m_heliModel.toothBackdropLeftMatrix *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(90.0));
    m_heliModel.toothBackdropLeftMatrix *= DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(45.0));
    m_heliModel.toothBackdropLeftMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(toothBackDropTranslation);
    m_heliModel.toothBackdropLeftMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(toothBackdropHorizontalTranslation);
    m_heliModel.localToothBackdropLeftMatrix = m_heliModel.toothBackdropLeftMatrix;

    // nose tooth mouth backdrop left
    toothRot = Utility::ToRadians(-47.8f);
    toothBackdropHorizontalTranslation.x = noseLength * cos(toothRot);
    toothBackdropHorizontalTranslation.y = 0.0f;
    toothBackdropHorizontalTranslation.z = noseLength - (noseLength * cos(toothRot));
    m_heliModel.toothBackdropLeftMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.toothBackdropRightMatrix *= DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(-30.0));
    m_heliModel.toothBackdropRightMatrix *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(-90.0));
    m_heliModel.toothBackdropRightMatrix *= DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(-45.0));
    m_heliModel.toothBackdropRightMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(toothBackDropTranslation);
    m_heliModel.toothBackdropRightMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(toothBackdropHorizontalTranslation);
    m_heliModel.localToothBackdropRightMatrix = m_heliModel.toothBackdropRightMatrix;

    // test mouth design
    DirectX::SimpleMath::Vector3 testMouthSize = noseConeSize;
    testMouthSize.x *= 0.845f;
    testMouthSize.y *= 0.25f;
    testMouthSize.z *= 0.845f;
    m_heliModel.toothBackdropTestShape = DirectX::GeometricPrimitive::CreateBox(aContext.Get(), testMouthSize);
    DirectX::SimpleMath::Vector3 testMouthTrans = noseConeTranslation;
    testMouthTrans.x += 0.26f;
    testMouthTrans.y -= 0.41f;
    m_heliModel.toothBackdropLeftMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.toothBackdropLeftMatrix *= DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(45.0));
    m_heliModel.toothBackdropLeftMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(0.0));
    m_heliModel.toothBackdropLeftMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(testMouthTrans);
    m_heliModel.localToothBackdropLeftMatrix = m_heliModel.toothBackdropLeftMatrix;

    /// nose body
    float noseBodyXSize = 0.7;
    DirectX::SimpleMath::Vector3 noseBodySize(noseBodyXSize, bodySize.y * 0.5f, bodySize.z);
    const DirectX::SimpleMath::Vector3 noseBodyTranslation(bodySize.x * 0.5f + (noseBodySize.x * 0.5f), bodySize.y * 0.25f, 0.0f);
    m_heliModel.noseBodyShape = DirectX::GeometricPrimitive::CreateBox(aContext.Get(), noseBodySize);
    m_heliModel.noseBodyMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.noseBodyMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(noseBodyTranslation);
    m_heliModel.localNoseBodyMatrix = m_heliModel.noseBodyMatrix;

    // nose cowel
    const float noseCowlSize = windshieldSize * 0.4f;
    DirectX::SimpleMath::Vector3 noseCowlTranslation = windshieldTranslation;
    const float cowlPosOffset = 0.01f;
    noseCowlTranslation.x += windshieldSize;
    noseCowlTranslation.x -= noseCowlSize;
    noseCowlTranslation.x += cowlPosOffset;
    noseCowlTranslation.y += cowlPosOffset;
    noseCowlTranslation.z += 0.0f;
    m_heliModel.noseCowlShape = DirectX::GeometricPrimitive::CreateOctahedron(aContext.Get(), noseCowlSize);
    m_heliModel.noseCowlMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.noseCowlMatrix = DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(45.0f));
    m_heliModel.noseCowlMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(bodyTranslation);
    m_heliModel.noseCowlMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(noseCowlTranslation);
    m_heliModel.localNoseCowlMatrix = m_heliModel.noseCowlMatrix;

    /// Main body rear
    const DirectX::SimpleMath::Vector3 bodyRearTranslation(-bodySize.x * 0.5f, -bodySize.y * 0.0f, 0.0f);
    m_heliModel.bodyRearMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.bodyRearMatrix = DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(45.0f));
    m_heliModel.bodyRearMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(bodyTranslation);
    m_heliModel.bodyRearMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(bodyRearTranslation);
    m_heliModel.localBodyRearMatrix = m_heliModel.bodyRearMatrix;

    // door
    DirectX::SimpleMath::Vector3 doorSize = bodySize;
    const float indentOffset = 0.09f;
    doorSize.x += 0.0f;
    doorSize.y *= .75f;
    doorSize.z += indentOffset;
    DirectX::SimpleMath::Vector3 doorTranslation = bodyTranslation;
    doorTranslation.x += 1.85f;
    doorTranslation.y += 0.45f;
    m_heliModel.doorShape = DirectX::GeometricPrimitive::CreateCylinder(aContext.Get(), doorSize.z, doorSize.y, 3);
    m_heliModel.doorMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.doorMatrix *= DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(30.0f));
    m_heliModel.doorMatrix *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(90.0f));
    m_heliModel.doorMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(doorTranslation);
    m_heliModel.localDoorMatrix = m_heliModel.doorMatrix;

    // Side window upper
    const DirectX::SimpleMath::Vector3 sideWindowUpperSize(bodySize.x * 0.25f, bodySize.y * 0.52f, doorSize.z + indentOffset);
    const DirectX::SimpleMath::Vector3 sideWindowUpperTranslation(sideWindowUpperSize.z * 0.65f - indentOffset, sideWindowUpperSize.y * 1.386f, 0.0f);
    m_heliModel.sideWindowUpperShape = DirectX::GeometricPrimitive::CreateCylinder(aContext.Get(), sideWindowUpperSize.z, sideWindowUpperSize.y, 3);
    m_heliModel.sideWindowUpperMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.sideWindowUpperMatrix = DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(30.0f));
    m_heliModel.sideWindowUpperMatrix *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(90.0f));
    m_heliModel.sideWindowUpperMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(sideWindowUpperTranslation);
    m_heliModel.localSideWindowUpperMatrix = m_heliModel.sideWindowUpperMatrix;

    // Side window lower
    const DirectX::SimpleMath::Vector3 sideWindowLowerSize(noseBodySize.x * 0.85f, noseBodySize.y * 0.75f, bodySize.z + indentOffset);
    const DirectX::SimpleMath::Vector3 sideWindowLowerTranslation(noseBodyTranslation.x * 1.0f , noseBodyTranslation.y * 1.6f, noseBodyTranslation.z);
    m_heliModel.sideWindowLowerShape = DirectX::GeometricPrimitive::CreateCylinder(aContext.Get(), sideWindowLowerSize.z, sideWindowLowerSize.x, 3);
    m_heliModel.sideWindowLowerMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.sideWindowLowerMatrix *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(90.0f));
    m_heliModel.sideWindowLowerMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(sideWindowLowerTranslation);
    m_heliModel.localSideWindowLowerMatrix = m_heliModel.sideWindowLowerMatrix;

    // upper engine housing body
    const DirectX::SimpleMath::Vector3 engineHousingSize(2.35f, 1.5f, 1.5f);
    const DirectX::SimpleMath::Vector3 engineHousingTranslation(-engineHousingSize.x * 0.2455f, (bodySize.y * 1.0f) + (engineHousingSize.y * 0.0f), 0.0f);
    m_heliModel.engineHousingShape = DirectX::GeometricPrimitive::CreateBox(aContext.Get(), engineHousingSize);
    m_heliModel.engineHousingMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.engineHousingMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(engineHousingTranslation);
    m_heliModel.localEngineHousingMatrix = m_heliModel.engineHousingMatrix;
   
    // upper front engine housing right
    float engineHousingFrontSize = (engineHousingSize.z * 0.5f) * sqrt(2.0f);
    engineHousingFrontSize = (engineHousingSize.z * 0.5f);
    DirectX::SimpleMath::Vector3 engineHousingFrontTranslation(engineHousingSize.x * 0.5f, -engineHousingSize.y * 0.0f, 0.0f);
    engineHousingFrontTranslation.z += engineHousingSize.z * 0.5f;
    m_heliModel.engineHousingCornerShape = DirectX::GeometricPrimitive::CreateOctahedron(aContext.Get(), engineHousingFrontSize);
    m_heliModel.engineHousingFrontRightMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.engineHousingFrontRightMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(engineHousingTranslation);
    m_heliModel.engineHousingFrontRightMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(engineHousingFrontTranslation);
    m_heliModel.localEngineHousingFrontRightMatrix = m_heliModel.engineHousingFrontRightMatrix;

    //  upper front engine housing left
    DirectX::SimpleMath::Vector3 engineHousingFrontLeftTranslation(engineHousingSize.x * 0.5f, -engineHousingSize.y * 0.0f, 0.0f);
    engineHousingFrontLeftTranslation.z -= engineHousingSize.z * 0.5f;
    m_heliModel.engineHousingFrontLeftMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.engineHousingFrontLeftMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(engineHousingTranslation);
    m_heliModel.engineHousingFrontLeftMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(engineHousingFrontLeftTranslation);
    m_heliModel.localEngineHousingFrontLeftMatrix = m_heliModel.engineHousingFrontLeftMatrix;

    //  upper rear engine housing left
    DirectX::SimpleMath::Vector3 engineHousingRearLeftTranslation(-engineHousingSize.x * 0.5f, -engineHousingSize.y * 0.0f, 0.0f);
    engineHousingRearLeftTranslation.z -= engineHousingSize.z * 0.5f;
    m_heliModel.engineHousingRearLeftMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.engineHousingRearLeftMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(engineHousingTranslation);
    m_heliModel.engineHousingRearLeftMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(engineHousingRearLeftTranslation);
    m_heliModel.localEngineHousingRearLeftMatrix = m_heliModel.engineHousingRearLeftMatrix;

    //  upper rear engine housing right
    DirectX::SimpleMath::Vector3 engineHousingRearRearTranslation(-engineHousingSize.x * 0.5f, -engineHousingSize.y * 0.0f, 0.0f);
    engineHousingRearRearTranslation.z += engineHousingSize.z * 0.5f;
    m_heliModel.engineHousingRearRightMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.engineHousingRearRightMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(engineHousingTranslation);
    m_heliModel.engineHousingRearRightMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(engineHousingRearRearTranslation);
    m_heliModel.localEngineHousingRearRightMatrix = m_heliModel.engineHousingRearRightMatrix;

    // engine housing intake left
    const float intakeVerticalOffset = 0.0f;
    const float intakeHorizontalOffset = 0.0766f;
    const float intakeShapeSize = engineHousingFrontSize * 0.8f;
    m_heliModel.engineHousingIntakeShape = DirectX::GeometricPrimitive::CreateOctahedron(aContext.Get(), intakeShapeSize);
    m_heliModel.engineHousingIntakeLeftMatrix = m_heliModel.engineHousingFrontLeftMatrix;
    const DirectX::SimpleMath::Vector3 engineIntakeLeftTranslation(intakeHorizontalOffset, intakeVerticalOffset, -intakeHorizontalOffset);
    m_heliModel.engineHousingIntakeLeftMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(engineIntakeLeftTranslation);
    m_heliModel.localEngineHousingIntakeLeftMatrix = m_heliModel.engineHousingIntakeLeftMatrix;

    // engine housing intake right
    m_heliModel.engineHousingIntakeRightMatrix = m_heliModel.engineHousingFrontRightMatrix;
    const DirectX::SimpleMath::Vector3 engineIntakeRightTranslation(intakeHorizontalOffset, intakeVerticalOffset, intakeHorizontalOffset);
    m_heliModel.engineHousingIntakeRightMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(engineIntakeRightTranslation);
    m_heliModel.localEngineHousingIntakeRightMatrix = m_heliModel.engineHousingIntakeRightMatrix;

    // upper front engine housing body 
    DirectX::SimpleMath::Vector3 engineHousingFrontCoverSize((engineHousingSize.z * 0.5f)* sqrt(2.0f), (engineHousingSize.z * 0.5f)* sqrt(2.0f), engineHousingSize.z);
    const DirectX::SimpleMath::Vector3 engineHousingFrontCoverTranslation(engineHousingSize.x * 0.5f, -engineHousingSize.y * 0.0f, 0.0f);
    m_heliModel.engineHousingFrontShape = DirectX::GeometricPrimitive::CreateBox(aContext.Get(), engineHousingFrontCoverSize);
    m_heliModel.engineHousingFrontMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.engineHousingFrontMatrix = DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(45.0f));
    m_heliModel.engineHousingFrontMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(engineHousingTranslation);
    m_heliModel.engineHousingFrontMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(engineHousingFrontCoverTranslation);
    m_heliModel.localEngineHousingFrontMatrix = m_heliModel.engineHousingFrontMatrix;

    // upper rear engine housing body 
    DirectX::SimpleMath::Vector3 engineHousingRearCoverSize((engineHousingSize.z * 0.5f)* sqrt(2.0f), (engineHousingSize.z * 0.5f)* sqrt(2.0f), engineHousingSize.z);
    const DirectX::SimpleMath::Vector3 engineHousingRearCoverTranslation(-engineHousingSize.x * 0.5f, -engineHousingSize.y * 0.0f, 0.0f);
    m_heliModel.engineHousingRearMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.engineHousingRearMatrix = DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(45.0f));

    m_heliModel.engineHousingRearMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(engineHousingTranslation);
    m_heliModel.engineHousingRearMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(engineHousingRearCoverTranslation);
    m_heliModel.localEngineHousingRearMatrix = m_heliModel.engineHousingRearMatrix;

    // upper engine side housing left
    DirectX::SimpleMath::Vector3 engineHousingSideSize(engineHousingSize.x, (engineHousingSize.z * 0.5f)* sqrt(2.0f), (engineHousingSize.z * 0.5f)* sqrt(2.0f));
    DirectX::SimpleMath::Vector3 engineHousingSideRightTranslation(0.0f, 0.0f, engineHousingSize.z * 0.5f);
    m_heliModel.engineHousingSideShape = DirectX::GeometricPrimitive::CreateBox(aContext.Get(), engineHousingSideSize);
    m_heliModel.engineHousingSideRightMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.engineHousingSideRightMatrix = DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(45.0f));
    m_heliModel.engineHousingSideRightMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(engineHousingTranslation);
    m_heliModel.engineHousingSideRightMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(engineHousingSideRightTranslation);
    m_heliModel.localEngineHousingSideRightMatrix = m_heliModel.engineHousingSideRightMatrix;

    // upper engine side housing right
    DirectX::SimpleMath::Vector3 engineHousingSideLeftTranslation(0.0f, 0.0f, -engineHousingSize.z * 0.5f);
    m_heliModel.engineHousingSideLeftMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.engineHousingSideLeftMatrix = DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(-45.0f));
    m_heliModel.engineHousingSideLeftMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(engineHousingTranslation);
    m_heliModel.engineHousingSideLeftMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(engineHousingSideLeftTranslation);
    m_heliModel.localEngineHousingSideLeftMatrix = m_heliModel.engineHousingSideLeftMatrix;

    // engine exhaust left
    const DirectX::SimpleMath::Vector3 exhaustSize(1.6f, engineHousingSize.y * 0.48f, engineHousingSize.z * 0.25f);
    const DirectX::SimpleMath::Vector3 exhaustLeftTranslation(-engineHousingSize.x * 0.5f, engineHousingSize.y * 0.18f, engineHousingSize.z * 0.35f);
    m_heliModel.engineExhaustExteriorShape = DirectX::GeometricPrimitive::CreateCylinder(aContext.Get(), exhaustSize.x, exhaustSize.y, 6);
    m_heliModel.engineExhaustLeftMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.engineExhaustLeftMatrix = DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(75.0f));
    m_heliModel.engineExhaustLeftMatrix *= DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(25.0f));
    m_heliModel.engineExhaustLeftMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(engineHousingTranslation);
    m_heliModel.engineExhaustLeftMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(exhaustLeftTranslation);
    m_heliModel.localEngineExhaustLeftMatrix = m_heliModel.engineExhaustLeftMatrix;

    // engine exhaust right
    const DirectX::SimpleMath::Vector3 exhaustRightTranslation(-engineHousingSize.x * 0.5f, engineHousingSize.y * 0.18f, -engineHousingSize.z * 0.35f);
    m_heliModel.engineExhaustRightMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.engineExhaustRightMatrix = DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(75.0f));
    m_heliModel.engineExhaustRightMatrix *= DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(-25.0f));
    m_heliModel.engineExhaustRightMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(engineHousingTranslation);
    m_heliModel.engineExhaustRightMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(exhaustRightTranslation);
    m_heliModel.localEngineExhaustRightMatrix = m_heliModel.engineExhaustRightMatrix;

    DirectX::SimpleMath::Vector3 exhaustSizeInterior = exhaustSize;
    exhaustSizeInterior.x *= 1.001f;
    exhaustSizeInterior.y *= 0.96f;
    m_heliModel.engineExhaustInteriorShape = DirectX::GeometricPrimitive::CreateCylinder(aContext.Get(), exhaustSizeInterior.x, exhaustSizeInterior.y, 6);


    // tail boom
    const DirectX::SimpleMath::Vector3 tailBoomSize(8.9f, 0.65f, 0.7f);
    float testSize = (tailBoomSize.z * tailBoomSize.z) + (tailBoomSize.z * tailBoomSize.z);
    testSize = sqrt(testSize);
    testSize *= 0.5f;
    DirectX::SimpleMath::Vector3 tailBoomTranslation(-bodySize.x, bodySize.y - (tailBoomSize.y * 1.0f), 0.0f);
    tailBoomTranslation.y = bodySize.y - testSize - testSize;
    tailBoomTranslation.y += 0.165f;
    //const DirectX::SimpleMath::Vector3 tailBoomTranslation(-bodySize.x, bodySize.y - (tailBoomSize.y * 1.5f), 0.0f);    
    m_heliModel.tailBoomShape = DirectX::GeometricPrimitive::CreateBox(aContext.Get(), tailBoomSize);
    m_heliModel.tailBoomMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.tailBoomMatrix *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(0.0));
    //m_heliModel.tailBoomMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(0.0));
    m_heliModel.tailBoomMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(tailBoomTranslation);
    m_heliModel.localTailBoomMatrix = m_heliModel.tailBoomMatrix;

    // tail boom lower
    float tailBoomDiameter = 0.8f;
    //tailBoomDiameter = tailBoomSize.z / sin(Utility::ToRadians(60.0f));
    tailBoomDiameter = tailBoomSize.z;
    tailBoomDiameter = 0.2f;

    const float tailBoomHeight = 8.9f;
    //DirectX::SimpleMath::Vector3 tailBoomLowerTranslation(-bodySize.x, bodySize.y - (tailBoomDiameter * 1.0f), 0.0f);
    DirectX::SimpleMath::Vector3 tailBoomLowerTranslation(-bodySize.x, tailBoomTranslation.y, 0.0f);
    tailBoomLowerTranslation.y +=  ((tailBoomDiameter * 0.5f) / 2.0f);
    //tailBoomLowerTranslation.y -= tailBoomDiameter * 0.5f + tailBoomSize.y * 0.5f;
    tailBoomLowerTranslation.y -= tailBoomSize.y * 0.5f + tailBoomDiameter * 0.25f;
    DirectX::SimpleMath::Vector3 tailBoomLowerSize(tailBoomHeight, testSize, testSize);
    //m_heliModel.tailBoomNewShape = DirectX::GeometricPrimitive::CreateCylinder(aContext.Get(), tailBoomHeight, tailBoomDiameter, 3);
    m_heliModel.tailBoomNewShape = DirectX::GeometricPrimitive::CreateBox(aContext.Get(), tailBoomLowerSize);
    m_heliModel.tailBoomLowerMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.tailBoomLowerMatrix *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(45.0));
    //m_heliModel.tailBoomLowerMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(90.0));
    m_heliModel.tailBoomLowerMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(tailBoomLowerTranslation);
    m_heliModel.localTailBoomLowerMatrix = m_heliModel.tailBoomLowerMatrix;

    // tail boom upper
    //DirectX::SimpleMath::Vector3 tailBoomUpperTranslation(-bodySize.x, bodySize.y - (tailBoomSize.y * 0.5f), 0.0f);
    DirectX::SimpleMath::Vector3 tailBoomUpperTranslation(-bodySize.x, tailBoomTranslation.y, 0.0f);
    tailBoomUpperTranslation.y -= ((tailBoomDiameter * 0.5f) / 2.0f);
    tailBoomUpperTranslation.y += tailBoomDiameter * 0.5f + tailBoomSize.y * 0.5f;

    tailBoomUpperTranslation = tailBoomLowerTranslation;
    tailBoomUpperTranslation.y += tailBoomSize.y;
    //tailBoomUpperTranslation.y -= .5f;
    m_heliModel.tailBoomUpperMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.tailBoomUpperMatrix *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(-45.0));
    //m_heliModel.tailBoomUpperMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(90.0));
    m_heliModel.tailBoomUpperMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(tailBoomUpperTranslation);
    m_heliModel.localTailBoomUpperMatrix = m_heliModel.tailBoomUpperMatrix;

    // tail boom endcap
    const DirectX::SimpleMath::Vector3 tailBoomEndCapSize((tailBoomSize.z * 0.5f) * sqrt(2.0f), tailBoomSize.y, (tailBoomSize.z * 0.5f) * sqrt(2.0f));
    DirectX::SimpleMath::Vector3 tailBoomEndCapTranslation(tailBoomTranslation.x - tailBoomSize.x * 0.5f, tailBoomTranslation.y, tailBoomTranslation.z);
    m_heliModel.tailBoomEndCapShape = DirectX::GeometricPrimitive::CreateBox(aContext.Get(), tailBoomEndCapSize);
    m_heliModel.tailBoomEndCapMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.tailBoomEndCapMatrix *= DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(45.0));
    m_heliModel.tailBoomEndCapMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(tailBoomEndCapTranslation);
    m_heliModel.localTailBoomEndCapMatrix = m_heliModel.tailBoomEndCapMatrix;

    // tail fin upper
    const float tailfinWidth = 0.2f;
    const DirectX::SimpleMath::Vector3 tailFinOffset(-0.34f, 0.0f, 0.0f);
    const DirectX::SimpleMath::Vector3 tailFinUpperSize(1.0f, 2.2f, tailfinWidth);
    //const DirectX::SimpleMath::Vector3 tailFinUpperTranslation(tailBoomTranslation.x - (tailBoomSize.x * 0.546f) + (tailFinUpperSize.x * 0.5f), tailBoomTranslation.y + (tailBoomSize.y * 1.0f), 0.0f);
    DirectX::SimpleMath::Vector3 tailFinUpperTranslation(tailBoomUpperTranslation.x - (tailBoomHeight * 0.546f) + (tailFinUpperSize.x * 0.5f), tailBoomTranslation.y + (tailBoomSize.y * 1.85f), 0.0f);
    const float tailFinUpperRotation = Utility::ToRadians(20.0f);
    const DirectX::SimpleMath::Matrix tailFinUpperRotationMat = DirectX::SimpleMath::Matrix::CreateRotationZ(tailFinUpperRotation);
    m_heliModel.tailFinUpperShape = DirectX::GeometricPrimitive::CreateBox(aContext.Get(), tailFinUpperSize);
    m_heliModel.tailFinUpperMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.tailFinUpperMatrix *= tailFinUpperRotationMat;
    m_heliModel.tailFinUpperMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(tailFinUpperTranslation + tailFinOffset);
    m_heliModel.localTailFinUpperMatrix = m_heliModel.tailFinUpperMatrix;

    // tail fin upper tip
    DirectX::SimpleMath::Vector3 tailFinUpperTipSize = tailFinUpperSize;
    tailFinUpperTipSize.x *= (tailFinUpperSize.x * cos(tailFinUpperRotation));
    tailFinUpperTipSize.y = (tailFinUpperSize.x * sin(tailFinUpperRotation));
    tailFinUpperTipSize.z += 0.001f;
    m_heliModel.tailFinUpperTipShape = DirectX::GeometricPrimitive::CreateBox(aContext.Get(), tailFinUpperTipSize);
    DirectX::SimpleMath::Vector3 tailFinUpperTipTranslation(0.0f, tailFinUpperSize.y * 0.5f, 0.0);
    tailFinUpperTipTranslation = DirectX::SimpleMath::Vector3::Transform(tailFinUpperTipTranslation, DirectX::SimpleMath::Matrix::CreateRotationZ(tailFinUpperRotation));
    DirectX::SimpleMath::Vector3 tailFinUpperTipTranslation3 = tailFinUpperTranslation;
    m_heliModel.tailFinUpperTipMatrix = DirectX::SimpleMath::Matrix::CreateTranslation(tailFinUpperTranslation + tailFinUpperTipTranslation + tailFinOffset);
    m_heliModel.localTailFinUpperTipMatrix = m_heliModel.tailFinUpperTipMatrix;

    // tail fin lower
    const DirectX::SimpleMath::Vector3 tailFinLowerSize(1.0f, 1.7f, tailfinWidth);
    const DirectX::SimpleMath::Vector3 tailFinLowerTranslation(tailBoomLowerTranslation.x - (tailBoomHeight * 0.546f) + (tailFinLowerSize.x * 0.5f), tailBoomTranslation.y + (tailBoomSize.y * -1.0f), 0.0f);
    const float tailFinLowerRotation = Utility::ToRadians(-40.0f);
    const DirectX::SimpleMath::Matrix tailFinLowerRotationMat = DirectX::SimpleMath::Matrix::CreateRotationZ(tailFinLowerRotation);
    m_heliModel.tailFinLowerShape = DirectX::GeometricPrimitive::CreateBox(aContext.Get(), tailFinLowerSize);
    m_heliModel.tailFinLowerMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.tailFinLowerMatrix *= tailFinLowerRotationMat;
    m_heliModel.tailFinLowerMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(tailFinLowerTranslation + tailFinOffset);
    m_heliModel.localTailFinLowerMatrix = m_heliModel.tailFinLowerMatrix;
 
    // tail fin lower tip
    DirectX::SimpleMath::Vector3 tailFinLowerTipSize = tailFinLowerSize;
    tailFinLowerTipSize.x *= (tailFinLowerSize.x * cos(tailFinLowerRotation));
    tailFinLowerTipSize.y = (tailFinLowerSize.x * sin(-tailFinLowerRotation));
    tailFinLowerTipSize.z -= 0.001f;
    m_heliModel.tailFinLowerTipShape = DirectX::GeometricPrimitive::CreateBox(aContext.Get(), tailFinLowerTipSize);
    m_heliModel.tailFinLowerTipMatrix = DirectX::SimpleMath::Matrix::Identity;
    DirectX::SimpleMath::Vector3 tailFinLowerTipTranslation(0.0f, -tailFinLowerSize.y * 0.5f, 0.0);
    tailFinLowerTipTranslation = DirectX::SimpleMath::Vector3::Transform(tailFinLowerTipTranslation, DirectX::SimpleMath::Matrix::CreateRotationZ(tailFinLowerRotation));
    m_heliModel.tailFinLowerTipMatrix = DirectX::SimpleMath::Matrix::CreateTranslation(tailFinLowerTranslation + tailFinLowerTipTranslation + tailFinOffset);
    m_heliModel.localTailFinLowerTipMatrix = m_heliModel.tailFinLowerTipMatrix;

    // tail fin leading edge lower
    const DirectX::SimpleMath::Vector3 tailFinLeadingEdgeSize((tailFinUpperSize.z * 0.5f)* sqrt(2.0f), tailFinUpperSize.y, (tailFinUpperSize.z * 0.5f)* sqrt(2.0f));
    m_heliModel.tailFinLeadingEdgeShape = DirectX::GeometricPrimitive::CreateBox(aContext.Get(), tailFinLeadingEdgeSize);
    const DirectX::SimpleMath::Vector3 tailFinLeadingEdgeLowerTranslation1(tailFinLowerSize.x * 0.5f, (tailFinUpperSize.y - tailFinLowerSize.y) * 0.5f, 0.0f);
    const DirectX::SimpleMath::Vector3 tailFinLeadingEdgeLowerTranslation2(tailFinLowerTranslation.x, tailFinLowerTranslation.y, tailFinLowerTranslation.z);
    m_heliModel.tailFinLeadingEdgeLowerMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.tailFinLeadingEdgeLowerMatrix *= DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(45.0f));
    m_heliModel.tailFinLeadingEdgeLowerMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(tailFinLeadingEdgeLowerTranslation1);
    m_heliModel.tailFinLeadingEdgeLowerMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(tailFinLowerRotation);
    m_heliModel.tailFinLeadingEdgeLowerMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(tailFinLeadingEdgeLowerTranslation2 + tailFinOffset);
    m_heliModel.localTailFinLeadingEdgeLowerMatrix = m_heliModel.tailFinLeadingEdgeLowerMatrix;
    // tail fin leading edge upper
    const DirectX::SimpleMath::Vector3 tailFinLeadingEdgeUpperTranslation1(tailFinUpperSize.x * 0.5f, 0.0f, 0.0f);
    const DirectX::SimpleMath::Vector3 tailFinLeadingEdgeUpperTranslation2(tailFinUpperTranslation.x, tailFinUpperTranslation.y, tailFinUpperTranslation.z);
    m_heliModel.tailFinLeadingEdgeUpperMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.tailFinLeadingEdgeUpperMatrix *= DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(45.0f));
    m_heliModel.tailFinLeadingEdgeUpperMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(tailFinLeadingEdgeUpperTranslation1);
    m_heliModel.tailFinLeadingEdgeUpperMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(tailFinUpperRotation);
    m_heliModel.tailFinLeadingEdgeUpperMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(tailFinLeadingEdgeUpperTranslation2 + tailFinOffset);
    m_heliModel.localTailFinLeadingEdgeUpperMatrix = m_heliModel.tailFinLeadingEdgeUpperMatrix;

    // tail wing
    const float tailWingFinWidth = 0.15f;
    const DirectX::SimpleMath::Vector3 tailWingSize(0.7f, 0.2f, mainWingSize.z - (4.0f * tailWingFinWidth));
    const DirectX::SimpleMath::Vector3 tailWingTranslation(tailBoomTranslation.x - (tailBoomSize.x * 0.27f) + (tailWingSize.x * 0.5f), tailBoomTranslation.y + (tailBoomSize.y * .1f), 0.0f);
    const DirectX::SimpleMath::Matrix tailWingRotation = DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(0.0f));
    m_heliModel.tailWingShape = DirectX::GeometricPrimitive::CreateBox(aContext.Get(), tailWingSize);
    m_heliModel.tailWingMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.tailWingMatrix *= tailWingRotation;
    m_heliModel.tailWingMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(tailWingTranslation);
    m_heliModel.localTailWingMatrix = m_heliModel.tailWingMatrix;

    // tail wing leading edge
    const DirectX::SimpleMath::Vector3 tailWingLeadingEdgeSize((tailWingSize.y * 0.5f) * sqrt(2.0f), (tailWingSize.y * 0.5f) * sqrt(2.0f), tailWingSize.z);
    m_heliModel.tailWingLeadingEdgeShape = DirectX::GeometricPrimitive::CreateBox(aContext.Get(), tailWingLeadingEdgeSize);
    const DirectX::SimpleMath::Vector3 tailWingLeadingEdgeTranslation(tailWingTranslation.x + tailWingSize.x * 0.5f, tailWingTranslation.y, tailWingTranslation.z);
    m_heliModel.tailWingLeadingEdgeMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.tailWingLeadingEdgeMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(45.0f));
    m_heliModel.tailWingLeadingEdgeMatrix *= tailWingRotation;
    m_heliModel.tailWingLeadingEdgeMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(tailWingLeadingEdgeTranslation);
    m_heliModel.localTailWingLeadingEdgeMatrix = m_heliModel.tailWingLeadingEdgeMatrix;

    // tail wing fin left
    const float tailWingFinDiameter = 1.25f;
    const float tailWingFinHeight = tailWingFinWidth;
    m_heliModel.tailWingFinShape = DirectX::GeometricPrimitive::CreateCylinder(aContext.Get(), tailWingFinHeight, tailWingFinDiameter, 3);
    DirectX::SimpleMath::Vector3 tailWingFinLeftTranslation = tailWingTranslation;
    tailWingFinLeftTranslation.x -= 0.075f;
    tailWingFinLeftTranslation.z -= tailWingSize.z * 0.5f + tailWingFinHeight * 0.5f;
    m_heliModel.tailWingFinLeftMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.tailWingFinLeftMatrix = DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(-30.0f));
    m_heliModel.tailWingFinLeftMatrix *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(91.0f));
    m_heliModel.tailWingFinLeftMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(tailWingFinLeftTranslation);
    m_heliModel.localTailWingFinLeftMatrix = m_heliModel.tailWingFinLeftMatrix;

    // tail wing fin right
    DirectX::SimpleMath::Vector3 tailWingFinRightTranslation = tailWingTranslation;
    tailWingFinRightTranslation.x -= 0.075f;
    tailWingFinRightTranslation.z += tailWingSize.z * 0.5f + tailWingFinHeight * 0.5f;
    m_heliModel.tailWingFinRightMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.tailWingFinRightMatrix = DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(-30.0f));
    m_heliModel.tailWingFinRightMatrix *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(89.0f));
    m_heliModel.tailWingFinRightMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(tailWingFinRightTranslation);
    m_heliModel.localTailWingFinRightMatrix = m_heliModel.tailWingFinRightMatrix;

    // main rotor axel
    const float mainAxelLength = 0.75f;
    const float mainAxelDiameter = 0.2f;
    const DirectX::SimpleMath::Vector3 mainAxelTranslation(0.0f, bodySize.y + (engineHousingSize.y * 0.5f) + (mainAxelLength * 0.5f), 0.0f);
    m_heliModel.mainRotorAxelShape = DirectX::GeometricPrimitive::CreateCylinder(aContext.Get(), mainAxelLength, mainAxelDiameter, 16);
    m_heliModel.mainRotorAxelMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.mainRotorAxelMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(mainAxelTranslation);
    m_heliModel.localMainRotorAxelMatrix = m_heliModel.mainRotorAxelMatrix;

    // main rotor hub
    const float mainHubLength = 0.4f;
    const float mainHubDiameter = 0.4f;
    const DirectX::SimpleMath::Vector3 mainHubTranslation(0.0f, mainAxelTranslation.y + mainHubLength, 0.0f);
    m_heliModel.mainRotorHubShape = DirectX::GeometricPrimitive::CreateCylinder(aContext.Get(), mainHubLength, mainHubDiameter, 16);
    m_heliModel.mainRotorHubMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.mainRotorHubMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(mainHubTranslation);
    m_heliModel.localMainRotorHubMatrix = m_heliModel.mainRotorHubMatrix;

    // tail rotor axel
    const float tailAxelLength = 0.45f;
    const float tailAxelDiameter = 0.1f;
    const float tailAxelOffsetY = 0.25f;
    const DirectX::SimpleMath::Vector3 tailAxelTranslation(tailBoomTranslation.x * 1.85f, tailBoomTranslation.y * 1.0f, -tailAxelLength);
    m_heliModel.tailRotorAxelShape = DirectX::GeometricPrimitive::CreateCylinder(aContext.Get(), tailAxelLength, tailAxelDiameter, 16);
    m_heliModel.tailRotorAxelMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.tailRotorAxelMatrix *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(-90.0f));
    m_heliModel.tailRotorAxelMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(tailAxelTranslation);
    m_heliModel.localTailRotorAxelMatrix = m_heliModel.tailRotorAxelMatrix;

    // tail position for torque forces
    DirectX::SimpleMath::Vector3 tailAxelPos = tailAxelTranslation;
    tailAxelPos.z = 0.0f;
    aHeliData.localTailRotorPos = tailAxelPos;
    aHeliData.tailRotorPos = aHeliData.localTailRotorPos;

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
    const float tailArmDiameter = aHeliData.tailRotor.radius * 2.0f;
    const float tailArmHeight = aHeliData.tailRotor.height;;
    const DirectX::SimpleMath::Vector3 tailArmTranslation(tailHubTranslation.x, tailHubTranslation.y, tailHubTranslation.z);
    m_heliModel.tailRotorArmShape = DirectX::GeometricPrimitive::CreateCylinder(aContext.Get(), tailArmDiameter, tailArmHeight);
    m_heliModel.tailRotorArmMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.tailRotorArmMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(0.0f));
    m_heliModel.tailRotorArmMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(tailArmTranslation);
    m_heliModel.localTailRotorArmMatrix = m_heliModel.tailRotorArmMatrix;

    // main rotor arm
    const float mainArmDiameter = aHeliData.mainRotor.radius * 2.0f;
    const float mainArmHeight = aHeliData.mainRotor.height;
    m_heliModel.mainRotorArmShape = DirectX::GeometricPrimitive::CreateCylinder(aContext.Get(), mainArmDiameter, mainArmHeight);
    const DirectX::SimpleMath::Vector3 rotorArmTranslation(0.0f, mainHubTranslation.y, 0.0f);
    m_heliModel.mainRotorArmMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.mainRotorArmMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(90.0f));
    m_heliModel.mainRotorArmMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(rotorArmTranslation);
    m_heliModel.localMainRotorArmMatrix = m_heliModel.mainRotorArmMatrix;

    // main rotor blade 1
    const float mainBladeWidth = aHeliData.mainRotor.width;
    const float mainBladeLength = aHeliData.mainRotor.length;
    const DirectX::SimpleMath::Vector3 rotorBladeSize(mainBladeLength, mainArmHeight, mainBladeWidth);
    const DirectX::SimpleMath::Vector3 rotorBladeTranslation1(((mainArmDiameter * 0.5f) - (rotorBladeSize.x * 0.5f)), mainHubTranslation.y, 0.0f);
    m_heliModel.mainRotorBladeShape = DirectX::GeometricPrimitive::CreateBox(aContext.Get(), rotorBladeSize);

    m_heliModel.mainRotorBladeMatrix1 = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.mainRotorBladeMatrix1 *= DirectX::SimpleMath::Matrix::CreateTranslation(rotorBladeTranslation1);
    m_heliModel.localMainRotorBladeMatrix1 = m_heliModel.mainRotorBladeMatrix1;
    m_heliModel.mainRotorBladeTranslationMatrix1 = DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(0.0f, 0.0f, rotorBladeSize.z * 0.5f));

    // main rotor edge  
    float mainRotorLeadingEdgeDiameter = rotorBladeSize.y / sin(Utility::ToRadians(60.0f));
    float mainRotorLeadingEdgeHeight = mainBladeLength;
    m_heliModel.mainRotorBladeEdgeShape = DirectX::GeometricPrimitive::CreateCylinder(aContext.Get(), mainRotorLeadingEdgeHeight, mainRotorLeadingEdgeDiameter, 3);
    m_heliModel.mainRotorBladeEdgeMatrix1 = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.mainRotorBladeEdgeMatrix1 *= DirectX::SimpleMath::Matrix::CreateTranslation(rotorBladeTranslation1);
    m_heliModel.localMainRotorEdgeBladeMatrix1 = m_heliModel.mainRotorBladeEdgeMatrix1;
    m_heliModel.mainRotorBladeEdgeTranslationMatrix1 = DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(0.0f));
    m_heliModel.mainRotorBladeEdgeTranslationMatrix1 *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(90.0f));
    m_heliModel.mainRotorBladeEdgeTranslationMatrix1 *= DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(0.0f, 0.0f, rotorBladeSize.z + (((sqrt(3) / 3) * rotorBladeSize.y) * 0.5f)));

    // main rotor blade 2
    const DirectX::SimpleMath::Vector3 rotorBladeTranslation2(((-mainArmDiameter * 0.5f) + (rotorBladeSize.x * 0.5f)), mainHubTranslation.y, 0.0f);
    m_heliModel.mainRotorBladeMatrix2 = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.mainRotorBladeMatrix2 *= DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(180.0f));
    m_heliModel.mainRotorBladeMatrix2 *= DirectX::SimpleMath::Matrix::CreateTranslation(rotorBladeTranslation2);
    m_heliModel.localMainRotorBladeMatrix2 = m_heliModel.mainRotorBladeMatrix2;
    m_heliModel.mainRotorBladeTranslationMatrix2 = DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(0.0f, 0.0f, rotorBladeSize.z * 0.5f));

    // main rotor edge 2
    m_heliModel.mainRotorBladeEdgeMatrix2 = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.mainRotorBladeEdgeMatrix2 *= DirectX::SimpleMath::Matrix::CreateTranslation(rotorBladeTranslation2);
    m_heliModel.localMainRotorEdgeBladeMatrix2 = m_heliModel.mainRotorBladeEdgeMatrix2;
    m_heliModel.mainRotorBladeEdgeTranslationMatrix2 = DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(60.0f));
    m_heliModel.mainRotorBladeEdgeTranslationMatrix2 *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(-90.0f));
    m_heliModel.mainRotorBladeEdgeTranslationMatrix2 *= DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(0.0f, 0.0f, -rotorBladeSize.z + (((sqrt(3) / 3) * -rotorBladeSize.y) * 0.5f)));

    // tail rotor blade 1
    const float tailBladeWidth = aHeliData.tailRotor.width;
    const float tailBladeLength = aHeliData.tailRotor.length;
    const DirectX::SimpleMath::Vector3 tailBladeSize(tailBladeWidth, tailBladeLength, tailArmHeight);
    m_heliModel.tailRotorBladeShape = DirectX::GeometricPrimitive::CreateBox(aContext.Get(), tailBladeSize);
    const DirectX::SimpleMath::Vector3 tailBladeTranslation(-tailBladeWidth * 0.5f, -(tailArmDiameter * 0.5f) + (tailBladeSize.y * 0.5f), 0.0f);  
    m_heliModel.tailRotorBladeMatrix1 = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.tailRotorBladeMatrix1 *= DirectX::SimpleMath::Matrix::CreateTranslation(tailBladeTranslation);
    m_heliModel.localTailRotorBladeMatrix1 = m_heliModel.tailRotorBladeMatrix1;
    m_heliModel.tailRotorBladeTranslationMatrix1 *= DirectX::SimpleMath::Matrix::CreateTranslation(tailArmTranslation);


    // tail rotor blade 2
    m_heliModel.tailRotorBladeMatrix2 = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.tailRotorBladeMatrix2 *= DirectX::SimpleMath::Matrix::CreateTranslation(tailBladeTranslation);
    m_heliModel.localTailRotorBladeMatrix2 = m_heliModel.tailRotorBladeMatrix2;
    m_heliModel.tailRotorBladeTranslationMatrix2 *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(180.0f));
    m_heliModel.tailRotorBladeTranslationMatrix2 *= DirectX::SimpleMath::Matrix::CreateTranslation(tailArmTranslation);


    /*
    // main wing tail edge
    const float mainWingTailEdgeDiameter = mainWingSize.y / sin(Utility::ToRadians(60.0f));
    const float mainWingTailEdgeHeight = mainWingSize.z;
    DirectX::SimpleMath::Vector3 mainWingTailEdgeTranslation = mainWingTranslation;
    const float median = (mainWingTailEdgeDiameter * 0.5f) + ((mainWingTailEdgeDiameter * 0.5f) / 2.0f);
    mainWingTailEdgeTranslation.x += -mainWingSize.x * 0.5f - mainWingTailEdgeDiameter * 0.5f + (mainWingTailEdgeDiameter - median);
    m_heliModel.mainWingTailEdgeShape = DirectX::GeometricPrimitive::CreateCylinder(aContext.Get(), mainWingTailEdgeHeight, mainWingTailEdgeDiameter, 3);
    m_heliModel.mainWingTailEdgeMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.mainWingTailEdgeMatrix = DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(30.0f));
    m_heliModel.mainWingTailEdgeMatrix *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(90.0f));
    m_heliModel.mainWingTailEdgeMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(mainWingTailEdgeTranslation);
    m_heliModel.localMainWingTailEdgeMatrix = m_heliModel.mainWingTailEdgeMatrix;
    */
    // tail rotor blade edge 1   
//float tailRotorLeadingEdgeDiameter = tailBladeSize.z / sin(Utility::ToRadians(60.0f));
//float tailRotorLeadingEdgeHeight = tailBladeLength + 3.0f;

    float tailRotorLeadingEdgeDiameter = tailArmHeight / sin(Utility::ToRadians(60.0f));
    float tailRotorLeadingEdgeHeight = tailBladeLength;
    m_heliModel.tailRotorBladeEdgeShape = DirectX::GeometricPrimitive::CreateCylinder(aContext.Get(), tailRotorLeadingEdgeHeight, tailRotorLeadingEdgeDiameter, 3);
    DirectX::SimpleMath::Vector3 tailBladeEdgeTranslation = tailBladeTranslation;
    const float medianTest = (tailRotorLeadingEdgeDiameter * 0.5f) + ((tailRotorLeadingEdgeDiameter * 0.5f) / 2.0f);
    //DirectX::SimpleMath::Vector3 tailBladeEdgeTranslation1(-tailBladeSize.x * 0.5f - tailRotorLeadingEdgeDiameter * 0.5f + (tailRotorLeadingEdgeDiameter - medianTest), tailBladeTranslation.y, 0.0f);
    DirectX::SimpleMath::Vector3 tailBladeEdgeTranslation1(-tailBladeSize.x * 1.0f - tailRotorLeadingEdgeDiameter * 0.5f + (tailRotorLeadingEdgeDiameter - medianTest), tailBladeTranslation.y, 0.0f);
    //tailBladeEdgeTranslation.x += tailBladeSize.x;
    m_heliModel.tailRotorBladeEdgeMatrix1 = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.tailRotorBladeEdgeMatrix1 = DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(-90.0f));
    m_heliModel.tailRotorBladeEdgeMatrix1 *= DirectX::SimpleMath::Matrix::CreateTranslation(tailBladeEdgeTranslation1);
    m_heliModel.localTailRotorBladeEdgeMatrix1 = m_heliModel.tailRotorBladeEdgeMatrix1;
    m_heliModel.tailRotorBladeEdgeTranslationMatrix1 = DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(0.0f));
    m_heliModel.tailRotorBladeEdgeTranslationMatrix1 *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(0.0f));
    //m_heliModel.tailRotorBladeEdgeTranslationMatrix1 *= DirectX::SimpleMath::Matrix::CreateTranslation(tailArmTranslation);
    //m_heliModel.tailRotorBladeEdgeTranslationMatrix1 *= DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(0.0f, 0.0f, tailBladeSize.z + (((sqrt(3) / 3) * tailBladeSize.y) * 0.5f)));
    m_heliModel.tailRotorBladeEdgeTranslationMatrix1 *= DirectX::SimpleMath::Matrix::CreateTranslation(tailArmTranslation);


    float tHeight = tailBladeSize.z;
    float tWidth = tailBladeSize.y;
    float tLength = tailBladeSize.x;
    // {x = 0.300000012 y = 1.11300004 z = 0.100000001 }
    DirectX::SimpleMath::Vector3 testEdgeTrans((tailBladeSize.x + ((sqrt(3) / 3) * tailBladeSize.z) * 0.5f), 0.0f, 0.0f);
    testEdgeTrans.x = -tWidth - ((sqrt(3) / 3) * tHeight) * 0.5f;
    testEdgeTrans.x = - tailBladeSize.x * 0.5f;
    //testEdgeTrans.x += 0.100000001 * (sqrt(3) / 3);

    testEdgeTrans.x = -tailBladeSize.x * 0.5f - tailRotorLeadingEdgeDiameter * 0.5f + (tailRotorLeadingEdgeDiameter - medianTest);
    //testEdgeTrans.x = - tailRotorLeadingEdgeDiameter * 0.5f + (tailRotorLeadingEdgeDiameter - medianTest);
    //testEdgeTrans.x = 0.0f;
    //testEdgeTrans.x = -tailBladeSize.x * 0.5f;
    testEdgeTrans.x = 0.0f;
    testEdgeTrans.y = 0.0f; 
    testEdgeTrans.z = 0.0f;

    m_heliModel.tailRotorBladeEdgeTranslationMatrix1 *= DirectX::SimpleMath::Matrix::CreateTranslation(testEdgeTrans);
    //m_heliModel.tailRotorBladeEdgeTranslationMatrix1 *= DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3((tailBladeSize.x + ((sqrt(3) / 3) * tailBladeSize.z) * 0.5f), 0.0f, 0.0f));

    //const float median = (mainWingTailEdgeDiameter * 0.5f) + ((mainWingTailEdgeDiameter * 0.5f) / 2.0f);
    //mainWingTailEdgeTranslation.x += -mainWingSize.x * 0.5f - mainWingTailEdgeDiameter * 0.5f + (mainWingTailEdgeDiameter - median);


    // tail blade edge 2
    m_heliModel.tailRotorBladeEdgeMatrix2 = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.tailRotorBladeEdgeMatrix2 = DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(-90.0f));
    m_heliModel.tailRotorBladeEdgeMatrix2 *= DirectX::SimpleMath::Matrix::CreateTranslation(tailBladeEdgeTranslation1);
    m_heliModel.localTailRotorBladeEdgeMatrix2 = m_heliModel.tailRotorBladeEdgeMatrix1;
    m_heliModel.tailRotorBladeEdgeTranslationMatrix2 = DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(0.0f));
    m_heliModel.tailRotorBladeEdgeTranslationMatrix2 *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(0.0f));
    //m_heliModel.tailRotorBladeEdgeTranslationMatrix2 *= DirectX::SimpleMath::Matrix::CreateTranslation(tailArmTranslation);
    //m_heliModel.tailRotorBladeEdgeTranslationMatrix2 *= DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(0.0f, 0.0f, tailBladeSize.z + (((sqrt(3) / 3) * tailBladeSize.y) * 0.5f)));
    m_heliModel.tailRotorBladeEdgeTranslationMatrix2 *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(180.0f));
    m_heliModel.tailRotorBladeEdgeTranslationMatrix2 *= DirectX::SimpleMath::Matrix::CreateTranslation(tailArmTranslation);
    m_heliModel.tailRotorBladeEdgeTranslationMatrix2 *= DirectX::SimpleMath::Matrix::CreateTranslation(testEdgeTrans);
}

void Vehicle::InitializeRotorBlades(HeliData& aHeliData)
{
    // set values for main rotor blade
    const int bladeCountMainRotor = 2;
    const float angleBetweetnBladesMainRotor = (Utility::GetPi() * 2.0f) / bladeCountMainRotor;
    const float radiusMainRotor = 7.0f;
    const float heightMainRotor = 0.15f;
    const float lengthMainRotor = 6.57999992f;
    const float widthMainRotor = 0.80f;       
    const float initialRPMMainRotor = 0.0f;
    const float maxPitchAngleMainRotor = Utility::ToRadians(45.0f);
    const float maxInputPitchAngleMainRotor = Utility::ToRadians(45.0f);
    const float minInputPitchAngleMainRotor = Utility::ToRadians(0.0f);
    const float minPitchAngleMainRotor = Utility::ToRadians(0.0f);
    const float neutralAngleMainRotor = Utility::ToRadians(0.0f);
    const float initialPitchAngleMainRotor = neutralAngleMainRotor;
    const float rotorRotationMainRotor = Utility::ToRadians(180.0f);

    aHeliData.mainRotor.angleBetweenBlades = angleBetweetnBladesMainRotor;
    aHeliData.mainRotor.bladeCount = bladeCountMainRotor;
    aHeliData.mainRotor.height = heightMainRotor;
    aHeliData.mainRotor.inputPitchAngleMax = maxInputPitchAngleMainRotor;
    aHeliData.mainRotor.inputPitchAngleMin = minInputPitchAngleMainRotor;
    aHeliData.mainRotor.length = lengthMainRotor;
    aHeliData.mainRotor.neutralAngle = neutralAngleMainRotor;
    aHeliData.mainRotor.pitchAngleMax = maxPitchAngleMainRotor;
    aHeliData.mainRotor.pitchAngleMin = minPitchAngleMainRotor;
    aHeliData.mainRotor.radius = radiusMainRotor;
    aHeliData.mainRotor.rpm = initialRPMMainRotor;
    aHeliData.mainRotor.rotorRotation = rotorRotationMainRotor;
    aHeliData.mainRotor.width = widthMainRotor;
    aHeliData.mainRotor.bladeType = Rotor::BladeType::BLADETYPE_MAINROTOR;

    aHeliData.mainRotor.bladeVec.clear();
    aHeliData.mainRotor.bladeVec.resize(bladeCountMainRotor);
    for (unsigned int i = 0; i < aHeliData.mainRotor.bladeVec.size(); ++i)
    {
        aHeliData.mainRotor.bladeVec[i].pitchAngle = initialPitchAngleMainRotor;
        aHeliData.mainRotor.bladeVec[i].cyclicAngle = 0.0f;
        aHeliData.mainRotor.bladeVec[i].liftMag = 0.0f;
        aHeliData.mainRotor.bladeVec[i].liftNorm = DirectX::SimpleMath::Vector3::Zero;
        aHeliData.mainRotor.bladeVec[i].rotationAngle = static_cast<float>(i) * angleBetweetnBladesMainRotor;
    }

    // set values for tail rotor blade
    const int bladeCountTailRotor = 2;
    const float angleBetweetnBladesTailRotor = (Utility::GetPi() * 2.0f) / bladeCountTailRotor;
    const float radiusTailRotor = 1.325f;
    const float heightTailRotor = 0.1f;
    const float lengthTailRotor = 1.11300004f;
    const float widthTailRotor = 0.300000012f;
    
    const float initialRPMTailRotor = 0.0f;
    const float maxPitchAngleTailRotor = Utility::ToRadians(45.0f);
    const float maxInputPitchAngleTailRotor = Utility::ToRadians(45.0f);
    const float minInputPitchAngleTailRotor = Utility::ToRadians(0.0f);
    const float minPitchAngleTailRotor = Utility::ToRadians(0.0f);
    const float neutralAngleTailRotor = 0.0f;// (maxPitchAngleTailRotor + minInputPitchAngleTailRotor) / 2.0f;
    const float initialPitchAngleTailRotor = neutralAngleTailRotor;
    const float rotorRotationTailBlade = 0.0f;

    aHeliData.tailRotor.angleBetweenBlades = angleBetweetnBladesTailRotor;
    aHeliData.tailRotor.bladeCount = bladeCountTailRotor;
    aHeliData.tailRotor.height = heightTailRotor;
    aHeliData.tailRotor.inputPitchAngleMax = maxInputPitchAngleTailRotor;
    aHeliData.tailRotor.inputPitchAngleMin = minInputPitchAngleTailRotor;
    aHeliData.tailRotor.length = lengthTailRotor;
    aHeliData.tailRotor.neutralAngle = neutralAngleTailRotor;
    aHeliData.tailRotor.pitchAngleMax = maxPitchAngleTailRotor;
    aHeliData.tailRotor.pitchAngleMin = minPitchAngleTailRotor;
    aHeliData.tailRotor.radius = radiusTailRotor;
    aHeliData.tailRotor.rpm = initialRPMTailRotor;
    aHeliData.tailRotor.rotorRotation = rotorRotationTailBlade;
    aHeliData.tailRotor.width = widthTailRotor;
    aHeliData.tailRotor.bladeType = Rotor::BladeType::BLADETYPE_TAILROTOR;

    aHeliData.tailRotor.bladeVec.clear();
    aHeliData.tailRotor.bladeVec.resize(bladeCountMainRotor);
    for (unsigned int i = 0; i < aHeliData.tailRotor.bladeVec.size(); ++i)
    {
        aHeliData.tailRotor.bladeVec[i].pitchAngle = initialPitchAngleTailRotor;
        aHeliData.tailRotor.bladeVec[i].cyclicAngle = 0.0f;
        aHeliData.tailRotor.bladeVec[i].liftMag = 0.0f;
        aHeliData.tailRotor.bladeVec[i].liftNorm = DirectX::SimpleMath::Vector3::Zero;
        aHeliData.tailRotor.bladeVec[i].rotationAngle = static_cast<float>(i) * angleBetweetnBladesTailRotor;
    }
}

void Vehicle::InitializeVehicle(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext)
{
    InitializeFlightControls(m_heli.controlInput);

    // helicopter data

    m_heli.mainRotorRPM = 0.0f;

    m_heli.numEqns = 6;
    m_heli.mass = 1760.0f;
    m_heli.area = 1.94f;
    m_heli.airDensity = 1.2f; // ToDo : pull air density from environment data
    m_heli.dragCoefficient = 0.31f;
    m_heli.airResistance = 0.0f;
    m_heli.totalResistance = m_heli.airResistance;
    m_heli.time = 0.0;  

    m_heli.forward = DirectX::SimpleMath::Vector3::UnitX;
    m_heli.up = DirectX::SimpleMath::Vector3::UnitY;
    m_heli.right = m_heli.forward.Cross(m_heli.up);
    m_heli.alignment = DirectX::SimpleMath::Matrix::CreateLookAt(DirectX::SimpleMath::Vector3::Zero, m_heli.forward, m_heli.up);
    m_heli.alignment = DirectX::SimpleMath::Matrix::Identity;
    //m_heli.alignment = DirectX::SimpleMath::Matrix::CreateFromYawPitchRoll(0.0f, 0.0f, 0.0f);
    m_heli.cameraOrientation = m_heli.alignment;

    m_heli.q.airResistance = DirectX::SimpleMath::Vector3::Zero;
    m_heli.q.bodyTorqueMagnitude = 0.0f;
    m_heli.q.bodyTorqueVec = DirectX::SimpleMath::Vector3::Zero;
    m_heli.q.bodyTorqueForce.axis = DirectX::SimpleMath::Vector3::Zero;
    m_heli.q.bodyTorqueForce.magnitude = 0.0f;

    m_heli.q.pendulumTorqueForceTest.axis = DirectX::SimpleMath::Vector3::Zero;
    m_heli.q.pendulumTorqueForceTest.magnitude = 0.0f;

    m_heli.q.bodyVelocity = DirectX::SimpleMath::Vector3::Zero;
    m_heli.q.engineForce = DirectX::SimpleMath::Vector3::Zero;

    m_heli.q.mainRotorForceNormal = DirectX::SimpleMath::Vector3::UnitY;
    m_heli.q.mainRotorForceMagnitude = 15.0f;
    m_heli.q.position = DirectX::SimpleMath::Vector3::Zero;
    
    m_heli.q.tailRotorForceNormal = -m_heli.right;
    m_heli.q.tailRotorForceMagnitude = 0.0f;
    
    m_heli.q.totalVelocity = DirectX::SimpleMath::Vector3::Zero;
    m_heli.q.velocity = DirectX::SimpleMath::Vector3::Zero;
    m_heli.q.parabolicMomentum = DirectX::SimpleMath::Vector3::Zero;

    m_heli.speed = 0.0;

    m_heli.isVehicleAirborne = false;
    m_heli.isVehicleLanding = false;
    m_heli.isVelocityBackwards = false;

    m_heli.terrainHightAtPos = 0.0;
    m_heli.terrainNormal = DirectX::SimpleMath::Vector3::UnitY;

    // set up rotor blades
    InitializeRotorBlades(m_heli);
    InitializeModel(aContext, m_heli);
    InitializeEngine(m_heli.engine);

    // Intialize key physics hardpoints based on model 
    m_heli.localMainRotorPos = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::Zero, m_heliModel.localMainRotorHubMatrix);
    m_heli.mainRotorPos = m_heli.localMainRotorPos;
    m_heli.localCenterOfMass = m_heli.localMainRotorPos;
    m_heli.localCenterOfMass.y = m_heli.localTailRotorPos.y;  // putting CoM at right angle to tail rotor position to simplify flight model for now
    m_heli.centerOfMass = m_heli.localCenterOfMass;
    m_heli.localLandingGearPos = DirectX::SimpleMath::Vector3::Zero;
    m_heli.localLandingGearPos.y -= 1.5f;
    m_heli.landingGearPos = m_heli.localLandingGearPos;

    DirectX::SimpleMath::Vector3 posShift(0.0f, 0.0f, 0.0f);
    posShift = - m_heli.localCenterOfMass;
    RepositionModelCordinates(posShift, m_heliModel);
}

void Vehicle::InputCollective(const float aCollectiveInput)
{
    const float updatedCollective = (aCollectiveInput * m_heli.controlInput.collectiveInputRate) + m_heli.controlInput.collectiveInput;
    if (updatedCollective > m_heli.controlInput.collectiveInputMax)
    {
        m_heli.controlInput.collectiveInput = m_heli.controlInput.collectiveInputMax;
    }
    else if (updatedCollective < m_heli.controlInput.collectiveInputMin)
    {
        m_heli.controlInput.collectiveInput = m_heli.controlInput.collectiveInputMin;
    }
    else
    {
        m_heli.controlInput.collectiveInput = updatedCollective;
    }
}

void Vehicle::InputCyclicPitch(const float aPitchInput)
{
    m_heli.controlInput.cyclicInputPitchIsPressed = true;
    const float updatedPitch = (aPitchInput * m_heli.controlInput.cyclicInputRate) + m_heli.controlInput.cyclicInputPitch;
    if (updatedPitch > m_heli.controlInput.cyclicInputMax)
    {
        m_heli.controlInput.cyclicInputPitch = m_heli.controlInput.cyclicInputMax;
    }
    else if (updatedPitch < m_heli.controlInput.cyclicInputMin)
    {
        m_heli.controlInput.cyclicInputPitch = m_heli.controlInput.cyclicInputMin;
    }
    else
    {
        m_heli.controlInput.cyclicInputPitch = updatedPitch;
    }

    testPitchInput += aPitchInput * testInputMod;
}

void Vehicle::InputCyclicRoll(const float aRollInput)
{
    m_heli.controlInput.cyclicInputRollIsPressed = true;
    const float updatedRoll = (aRollInput * m_heli.controlInput.cyclicInputRate) + m_heli.controlInput.cyclicInputRoll;
    if (updatedRoll > m_heli.controlInput.cyclicInputMax)
    {
        m_heli.controlInput.cyclicInputRoll = m_heli.controlInput.cyclicInputMax;
    }
    else if (updatedRoll < m_heli.controlInput.cyclicInputMin)
    {
        m_heli.controlInput.cyclicInputRoll = m_heli.controlInput.cyclicInputMin;
    }
    else
    {
        m_heli.controlInput.cyclicInputRoll = updatedRoll;
    }

    testRollInput += aRollInput * testInputMod;
}

void Vehicle::InputDecay(const double aTimeDelta)
{
    const float timeDelta = static_cast<float>(aTimeDelta);

    if (m_heli.controlInput.cyclicInputPitchIsPressed == false)
    {
        // Cyclic Pitch Decay
        if (m_heli.controlInput.cyclicInputPitch > 0.0f)
        {
            if (m_heli.controlInput.cyclicInputPitch - (m_heli.controlInput.cyclicDecayRate * timeDelta) < 0.0f)
            {
                m_heli.controlInput.cyclicInputPitch = 0.0f;
            }
            else
            {
                m_heli.controlInput.cyclicInputPitch -= m_heli.controlInput.cyclicDecayRate * timeDelta;
            }
        }
        else if (m_heli.controlInput.cyclicInputPitch < 0.0f)
        {
            if (m_heli.controlInput.cyclicInputPitch + (m_heli.controlInput.cyclicDecayRate * timeDelta) > 0.0f)
            {
                m_heli.controlInput.cyclicInputPitch = 0.0f;
            }
            else
            {
                m_heli.controlInput.cyclicInputPitch += m_heli.controlInput.cyclicDecayRate * timeDelta;
            }
        }
        else
        {
            m_heli.controlInput.cyclicInputPitch = 0.0f;
        }
    }
    // Cyclic Roll Decay
    if (m_heli.controlInput.cyclicInputRollIsPressed == false)
    {
        if (m_heli.controlInput.cyclicInputRoll > 0.0f)
        {
            if (m_heli.controlInput.cyclicInputRoll - (m_heli.controlInput.cyclicDecayRate * timeDelta) < 0.0f)
            {
                m_heli.controlInput.cyclicInputRoll = 0.0f;
            }
            else
            {
                m_heli.controlInput.cyclicInputRoll -= m_heli.controlInput.cyclicDecayRate * timeDelta;
            }
        }
        else if (m_heli.controlInput.cyclicInputRoll < 0.0f)
        {
            if (m_heli.controlInput.cyclicInputRoll + (m_heli.controlInput.cyclicDecayRate * timeDelta) > 0.0f)
            {
                m_heli.controlInput.cyclicInputRoll = 0.0f;
            }
            else
            {
                m_heli.controlInput.cyclicInputRoll += m_heli.controlInput.cyclicDecayRate * timeDelta;
            }
        }
        else
        {
            m_heli.controlInput.cyclicInputRoll = 0.0f;
        }
    }
    // Yaw Pedal Decay
    if (m_heli.controlInput.yawPedalIsPressed == false)
    {
        if (m_heli.controlInput.yawPedalInput + (m_heli.controlInput.yawPedalDecayRate * timeDelta) < 0.0f)
        {
            if (m_heli.controlInput.yawPedalInput - (m_heli.controlInput.yawPedalDecayRate * timeDelta) > -m_heli.controlInput.inputDeadZone)
            {
                m_heli.controlInput.yawPedalInput = 0.0f;
            }
            else
            {
                m_heli.controlInput.yawPedalInput += m_heli.controlInput.yawPedalDecayRate * timeDelta;
            }
        }
        else if (m_heli.controlInput.yawPedalInput - (m_heli.controlInput.yawPedalDecayRate * timeDelta) > 0.0f)
        {
            if (m_heli.controlInput.yawPedalInput + (m_heli.controlInput.yawPedalDecayRate * timeDelta) < m_heli.controlInput.inputDeadZone)
            {
                m_heli.controlInput.yawPedalInput = 0.0f;
            }
            else
            {
                m_heli.controlInput.yawPedalInput -= m_heli.controlInput.yawPedalDecayRate * timeDelta;
            }
        }
        else
        {
            m_heli.controlInput.yawPedalInput = 0.0f;
        }
    }

    m_heli.controlInput.cyclicInputPitchIsPressed = false;
    m_heli.controlInput.cyclicInputRollIsPressed = false;
    m_heli.controlInput.yawPedalIsPressed = false;
}

void Vehicle::InputThrottle(const float aThrottleInput)
{
    const float updatedThrottle = (aThrottleInput * m_heli.controlInput.throttleInputRate) + m_heli.controlInput.throttleInput;
    if (updatedThrottle > m_heli.controlInput.throttleInputMax)
    {
        m_heli.controlInput.throttleInput = m_heli.controlInput.throttleInputMax;
    }
    else if (updatedThrottle < m_heli.controlInput.throttleInputMin)
    {
        m_heli.controlInput.throttleInput = m_heli.controlInput.throttleInputMin;
    }
    else
    {
        m_heli.controlInput.throttleInput = updatedThrottle;
    }
}

void Vehicle::InputYawPedal(const float aYawInput)
{
    m_heli.controlInput.yawPedalIsPressed = true;
    const float updatedYaw = (aYawInput * m_heli.controlInput.yawPedalInputRate) + m_heli.controlInput.yawPedalInput;
    if (updatedYaw > m_heli.controlInput.yawPedalInputMax)
    {
        m_heli.controlInput.yawPedalInput = m_heli.controlInput.yawPedalInputMax;
    }
    else if (updatedYaw < m_heli.controlInput.yawPedalInputMin)
    {
        m_heli.controlInput.yawPedalInput = m_heli.controlInput.yawPedalInputMin;
    }
    else if (updatedYaw < m_heli.controlInput.inputDeadZone && updatedYaw > -m_heli.controlInput.inputDeadZone)
    {
        m_heli.controlInput.yawPedalInput = 0.0f;
    }
    else
    {
        m_heli.controlInput.yawPedalInput = updatedYaw;
    }

    testYawInput += aYawInput * testInputMod;
}

void Vehicle::Jump()
{
    const float jumpHeight = 100.0f;
    //m_heli.q.velocity.y += jumpHeight;
    m_heli.q.position.y += jumpHeight;
}

void Vehicle::LandVehicle()
{   
    // for now just zero out the velocity and allign vehicle with terrain so the vehicle always lands 'safe'
    m_heli.q.velocity = DirectX::SimpleMath::Vector3::Zero;
    m_heli.up = m_heli.terrainNormal;
    m_heli.isVehicleAirborne = false;
}

void Vehicle::RepositionModelCordinates(const DirectX::SimpleMath::Vector3 aPos, struct HeliModel& aModel)
{
    m_testPos -= aPos;
    m_testPos2 -= aPos;
    m_testPos3 -= aPos;

    aModel.localNoseBodyMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localNoseConeMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localToothMatrix1 *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localToothMatrix2 *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localToothMatrix3 *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localToothMatrix4 *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localToothMatrix5 *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localToothMatrix6 *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localToothMatrix7 *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localToothMatrix8 *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localToothMatrix9 *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localToothMatrix10 *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localToothMatrix11 *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localToothMatrix12 *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localToothBackdropLeftMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localToothBackdropRightMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);

    aModel.localBodyMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localDoorMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localMainWingMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localMainWingLeadingEdgeMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localMainWingTailEdgeMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localWingJetCenterMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localWingJetForwardHousingLeftMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localWingJetForwardHousingRightMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localWingJetForwardCowlLeftMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localWingJetForwardCowlRightMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localWingJetCouplingLeftMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localWingJetCouplingRightMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);

    aModel.localSidestripeMatrix0 *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localSidestripeMatrix1 *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localSidestripeMatrix2 *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localSidestripeMatrix3 *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localSidestripeMatrix4 *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localSidestripeMatrix5 *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localWindShieldMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localNoseCowlMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localSideWindowUpperMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localSideWindowLowerMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localBodyRearMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);

    aModel.localLandingGearFrontMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localLandingGearRearLeftMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localLandingGearRearRightMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);

    aModel.localEngineHousingMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localEngineHousingFrontRightMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localEngineHousingFrontMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localEngineHousingRearMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localEngineHousingFrontLeftMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localEngineHousingRearLeftMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localEngineHousingRearRightMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localEngineHousingSideLeftMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localEngineHousingSideRightMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localEngineExhaustLeftMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localEngineExhaustRightMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);

    aModel.localEngineHousingIntakeLeftMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localEngineHousingIntakeRightMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);

    aModel.localTailBoomMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localTailBoomLowerMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localTailBoomUpperMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localTailBoomEndCapMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localTailFinUpperMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localTailFinUpperTipMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localTailFinLowerMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localTailFinLowerTipMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localTailFinLeadingEdgeLowerMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localTailFinLeadingEdgeUpperMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localTailWingMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localTailWingLeadingEdgeMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localTailWingFinLeftMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localTailWingFinRightMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localMainRotorHubMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localMainRotorBladeMatrix1 *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localMainRotorBladeMatrix2 *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localTailRotorAxelMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localTailRotorHubMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localTailRotorArmMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    
    aModel.tailRotorBladeTranslationMatrix1 *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.tailRotorBladeTranslationMatrix2 *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);

    aModel.tailRotorBladeEdgeTranslationMatrix1 *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.tailRotorBladeEdgeTranslationMatrix2 *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);

    aModel.localMainRotorEdgeBladeMatrix1 *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localMainRotorEdgeBladeMatrix2 *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);

    aModel.localMainRotorAxelMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localMainRotorArmMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);

    m_heli.localMainRotorPos += aPos;
    m_heli.localTailRotorPos += aPos;
    m_heli.localCenterOfMass += aPos;
    m_heli.localLandingGearPos += aPos;
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

    DirectX::SimpleMath::Vector3 rotorForce = aQ->mainRotorForceNormal * aQ->mainRotorForceMagnitude * m_heli.controlInput.collectiveInput;
    rotorForce = UpdateRotorForceRunge();
    rotorForce *= aQ->mainRotorForceMagnitude * m_heli.controlInput.collectiveInput;
    newQ.mainRotorForceNormal = rotorForce;
    newQ.mainRotorForceNormal.Normalize();
    newQ.mainRotorForceMagnitude = aQ->mainRotorForceMagnitude * m_heli.controlInput.collectiveInput;

    //newQ.velocity += rotorForce + static_cast<float>(aQScale) * aDeltaQ->velocity;

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
    DebugPushTestLine(m_heli.q.position, airResistance, 10.0f, 0.0f, DirectX::SimpleMath::Vector4(0.0f, 0.0f, 1.0f, 1.0f));
    if (m_heli.isVelocityBackwards == true)
    {
        airResistance *= -1.0;
    }

    DirectX::SimpleMath::Vector3 gravForce = m_heli.gravity * static_cast<float>(aTimeDelta);
    DirectX::SimpleMath::Vector3 terrainNormalForce = (m_heli.terrainNormal * -m_heli.gravity.y) * static_cast<float>(aTimeDelta);

    DirectX::SimpleMath::Vector3 velocityUpdate = DirectX::SimpleMath::Vector3::Zero;
    Utility::Torque bodyTorqueUpdate;
    if (m_heli.isVehicleAirborne == true)
    {
        terrainNormalForce = DirectX::SimpleMath::Vector3::Zero;

        //velocityUpdate = aQ->velocity + static_cast<float>(aQScale) * aDeltaQ->velocity;
        velocityUpdate = m_heli.gravity * static_cast<float>(aTimeDelta);
        //velocityUpdate =  (m_heli.gravity.Length() * m_heli.mass) * -DirectX::SimpleMath::Vector3::UnitY * static_cast<float>(aTimeDelta);
        velocityUpdate += rotorForce * static_cast<float>(aTimeDelta);
        

        //velocityUpdate = (rotorForce + m_heli.gravity) * static_cast<float>(aTimeDelta);

        Utility::Torque pendTorque;
        pendTorque.axis = DirectX::SimpleMath::Vector3::Zero;
        pendTorque.magnitude = 0.0f;
        UpdatePendulumMotion(pendTorque, velocityUpdate, static_cast<float>(aTimeDelta));
        velocityUpdate += airResistance * static_cast<float>(aTimeDelta);
        bodyTorqueUpdate = UpdateBodyTorqueTestRunge(pendTorque, static_cast<float>(aTimeDelta));
        //bodyTorqueUpdate.axis = DirectX::SimpleMath::Vector3::Zero;
        //bodyTorqueUpdate.magnitude = 0.0f;
    }
    else
    {
        velocityUpdate = DirectX::SimpleMath::Vector3::Zero;
        bodyTorqueUpdate.axis = DirectX::SimpleMath::Vector3::Zero;
        bodyTorqueUpdate.magnitude = 0.0f;
    }

    //  Assign right-hand side values.
    aDQ->airResistance = airResistance;
    aDQ->velocity = velocityUpdate;
    aDQ->totalVelocity = velocityUpdate;
    aDQ->position = static_cast<float>(aTimeDelta) * newQ.velocity;
    aDQ->bodyTorqueForce = bodyTorqueUpdate;
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
    DirectX::SimpleMath::Vector3 totalVelocityUpdate = (dq1.totalVelocity + 2.0 * dq2.totalVelocity + 2.0 * dq3.totalVelocity + dq4.totalVelocity) / numEqns;

    Utility::Torque bodyTorqueUpdate;
    bodyTorqueUpdate.axis = (dq1.bodyTorqueForce.axis + 2.0 * dq2.bodyTorqueForce.axis + 2.0 * dq3.bodyTorqueForce.axis + dq4.bodyTorqueForce.axis) / numEqns;
    bodyTorqueUpdate.magnitude = (dq1.bodyTorqueForce.magnitude + 2.0f * dq2.bodyTorqueForce.magnitude + 2.0f * dq3.bodyTorqueForce.magnitude + dq4.bodyTorqueForce.magnitude) / numEqns;

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
    //q.gravityForce += gravityVelocityUpdate;
    q.totalVelocity += velocityUpdate;
    q.bodyTorqueForce.axis += bodyTorqueUpdate.axis;
    q.bodyTorqueForce.magnitude += bodyTorqueUpdate.magnitude;

    aHeli->q.engineForce = q.engineForce;
    aHeli->q.position = q.position;
    aHeli->q.velocity = q.velocity;
    //aHeli->q.torqueForceMat = q.torqueForceMat;
    aHeli->q.bodyTorqueForce = q.bodyTorqueForce;

    //UpdatePendulumMotion2(static_cast<float>(aTimeDelta));
    //Utility::Torque testTorque = UpdateBodyTorqueTest(static_cast<float>(aTimeDelta));
    /*
    DebugPushUILineDecimalNumber("testTorque.axis.Length() : ", testTorque.axis.Length(), "");
    DebugPushUILineDecimalNumber("testTorque.axis.x,: ", testTorque.axis.x, "");
    DebugPushUILineDecimalNumber("testTorque.axis.y : ", testTorque.axis.y, "");
    DebugPushUILineDecimalNumber("testTorque.axis.z : ", testTorque.axis.z, "");
    DebugPushUILineDecimalNumber("testTorque.magnitude : ", testTorque.magnitude, "");
    */


    //aHeli->q.bodyTorqueForce = testTorque;

    //aHeli->q = q;

    if (aHeli->q.bodyTorqueForce.axis.Length() > 1.5f)
    {
        int testBreak = 0;
        testBreak++;
    }

    //DebugPushTestLine(m_heli.q.position, velocityUpdate, 10.0f, 0.0f, DirectX::SimpleMath::Vector4(0.0f, 1.0f, 0.0f, 1.0f));
    //DebugPushTestLine(m_heli.q.position, q.velocity, 10.0f, 0.0f, DirectX::SimpleMath::Vector4(0.0f, 0.0f, 1.0f, 1.0f));
    DirectX::SimpleMath::Vector3 groundSpeed = q.velocity;
    groundSpeed.y = 0.0f;
    DebugPushUILineDecimalNumber("groundSpeed : ", groundSpeed.Length(), "");
}

void Vehicle::SetEnvironment(Environment* aEnviron)
{
    m_environment = aEnviron;
}

void Vehicle::ToggleLandingGearState()
{
    if (m_heli.landingGear.currentState == LandingGear::LandingGearState::LANDINGGEARSTATE_DOWN || m_heli.landingGear.currentState == LandingGear::LandingGearState::LANDINGGEARSTATE_DESCENDING)
    {
        m_heli.landingGear.currentState = LandingGear::LandingGearState::LANDINGGEARSTATE_ASCENDING;
    }
    else if (m_heli.landingGear.currentState == LandingGear::LandingGearState::LANDINGGEARSTATE_UP || m_heli.landingGear.currentState == LandingGear::LandingGearState::LANDINGGEARSTATE_ASCENDING)
    {
        m_heli.landingGear.currentState = LandingGear::LandingGearState::LANDINGGEARSTATE_DESCENDING;
    }
}

void Vehicle::UpdateBladeLiftForce(const float aTimeStep)
{
    float angleOfAttack = m_heli.mainRotor.bladeVec[0].pitchAngle;
    float rho = m_heli.airDensity;
    float Cl = CalculateLiftCoefficient(angleOfAttack);
    float surface = m_heli.mainRotor.length * m_heli.mainRotor.width;
    float y = m_heli.mainRotor.width;
    const float omega = (m_heli.mainRotor.rpm * 0.10472f) * aTimeStep; // 0.10472 is conversion of RPM to rads per second

    float lift = .5f * rho * omega * omega * Cl * y;
    lift *= 2.0f;
    DebugPushUILineDecimalNumber("lift : ", lift, "");
    // L = .5 * rho * omega^2 * Cl * y integral(r1 -> r2)  r^2 * dr    
}

void Vehicle::UpdateAlignmentTorqueTest()
{
    if (m_heli.isVehicleAirborne == false)
    {
        DirectX::SimpleMath::Vector3 newUp = m_heli.terrainNormal;
        DirectX::SimpleMath::Vector3 oldUp = m_heli.up;
        DirectX::SimpleMath::Vector3 updateUp = DirectX::SimpleMath::Vector3::SmoothStep(oldUp, newUp, 0.2);
        updateUp.Normalize();
        m_heli.up = updateUp;
        m_heli.right = m_heli.forward.Cross(m_heli.up);
        m_heli.right.Normalize();
        m_heli.forward = m_heli.up.Cross(m_heli.right);
        m_heli.alignment = DirectX::SimpleMath::Matrix::CreateWorld(DirectX::SimpleMath::Vector3::Zero, -m_heli.right, m_heli.up);
    }
    else
    {
        DirectX::SimpleMath::Matrix preAlignment = m_heli.alignment;
        DirectX::SimpleMath::Quaternion preAlignmentQuat = DirectX::SimpleMath::Quaternion::CreateFromRotationMatrix(m_heli.alignment);
        DirectX::SimpleMath::Matrix torqueMat;
        const float tol = 0.0001f;
        if (abs(m_heli.q.bodyTorqueForce.magnitude) < tol || m_heli.q.bodyTorqueForce.axis.Length() < tol)
        {
            //torqueMat = DirectX::SimpleMath::Matrix::Identity;
            torqueMat = DirectX::SimpleMath::Matrix::CreateFromAxisAngle(DirectX::SimpleMath::Vector3::UnitX, 0.0f);
        }
        else
        {
            torqueMat = DirectX::SimpleMath::Matrix::CreateFromAxisAngle(m_heli.q.bodyTorqueForce.axis, m_heli.q.bodyTorqueForce.magnitude);
        }

        const float t = 0.5f;
        DirectX::SimpleMath::Quaternion rotQuat = DirectX::SimpleMath::Quaternion::CreateFromRotationMatrix(torqueMat);
        DirectX::SimpleMath::Matrix postAlignment = DirectX::SimpleMath::Matrix::Transform(preAlignment, rotQuat);

        DirectX::SimpleMath::Quaternion postAlignmentQuat = preAlignmentQuat * rotQuat;
        postAlignmentQuat.Normalize();
        DirectX::SimpleMath::Quaternion updateQuat = DirectX::SimpleMath::Quaternion::Slerp(preAlignmentQuat, postAlignmentQuat, t);
        //DirectX::SimpleMath::Quaternion updateQuat = DirectX::SimpleMath::Quaternion::Slerp(preAlignmentQuat, rotQuat, t);
        m_heli.alignment = DirectX::SimpleMath::Matrix::Transform(m_heli.alignment, rotQuat);
        m_heli.alignment = DirectX::SimpleMath::Matrix::Lerp(preAlignment, postAlignment, t);
        //m_heli.alignment = postAlignment;

        //m_heli.alignment = DirectX::SimpleMath::Matrix::CreateFromQuaternion(updateQuat);
        //m_heli.alignment = DirectX::SimpleMath::Matrix::Transform(preAlignment, updateQuat);

        DirectX::SimpleMath::Matrix testMat = DirectX::SimpleMath::Matrix::CreateWorld(DirectX::SimpleMath::Vector3::Zero, -m_heli.right, m_heli.up);
        testMat *= torqueMat;
        //m_heli.alignment *= testMat;
        m_heli.up = DirectX::SimpleMath::Vector3::TransformNormal(DirectX::SimpleMath::Vector3::UnitY, m_heli.alignment);
        m_heli.right = DirectX::SimpleMath::Vector3::TransformNormal(DirectX::SimpleMath::Vector3::UnitZ, m_heli.alignment);
        m_heli.forward = DirectX::SimpleMath::Vector3::TransformNormal(DirectX::SimpleMath::Vector3::UnitX, m_heli.alignment);

        m_heli.alignment = DirectX::SimpleMath::Matrix::CreateWorld(DirectX::SimpleMath::Vector3::Zero, -m_heli.right, m_heli.up);
    }

    m_heli.cameraOrientation = DirectX::XMMatrixLookAtRH(DirectX::SimpleMath::Vector3::Zero, -m_heli.right, m_heli.up);
    m_heli.cameraOrientation = DirectX::SimpleMath::Matrix::CreateWorld(DirectX::SimpleMath::Vector3::Zero, -m_heli.right, m_heli.up);
}

Utility::Torque Vehicle::UpdateBodyTorqueTest(const float aTimeStep)
{
    DirectX::SimpleMath::Vector3 centerMassPos = m_heli.localCenterOfMass;
    centerMassPos = DirectX::SimpleMath::Vector3::Transform(centerMassPos, m_heli.alignment);
    centerMassPos = m_heli.centerOfMass;
    DirectX::SimpleMath::Vector3 rotorPos = m_heli.localMainRotorPos;
    rotorPos = DirectX::SimpleMath::Vector3::Transform(rotorPos, m_heli.alignment);
    rotorPos = m_heli.mainRotorPos;
    DirectX::SimpleMath::Vector3 tailPos = m_heli.localTailRotorPos;
    tailPos = DirectX::SimpleMath::Vector3::Transform(tailPos, m_heli.alignment);
    tailPos = m_heli.tailRotorPos;

    DirectX::SimpleMath::Vector3 mainRotorTorqueArm = rotorPos - centerMassPos;
    DirectX::SimpleMath::Vector3 tailRotorTorqueArm = tailPos - centerMassPos;
    DirectX::SimpleMath::Vector3 gravityTorqueArm = centerMassPos - rotorPos;
    //DirectX::SimpleMath::Vector3 gravityTorqueArm = rotorPos - centerMassPos;

    /*
    DebugPushTestLine(m_heli.mainRotorPos, mainRotorTorqueArm, 10.0f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 0.0f, 0.0f, 1.0f));
    DebugPushTestLine(m_heli.tailRotorPos, tailRotorTorqueArm, 10.0f, 0.0f, DirectX::SimpleMath::Vector4(0.0f, 1.0f, 0.0f, 1.0f));
    DebugPushTestLine(m_heli.centerOfMass, gravityTorqueArm, 10.0f, 0.0f, DirectX::SimpleMath::Vector4(0.0f, 0.0f, 1.0f, 1.0f));
    */

    const float modVal = 0.01f;
    DirectX::SimpleMath::Vector3 mainRotorForce = m_heli.q.mainRotorForceNormal * (m_heli.q.mainRotorForceMagnitude * m_heli.controlInput.collectiveInput) * modVal;
    mainRotorForce = m_heli.controlInput.cyclicStick * (m_heli.q.mainRotorForceMagnitude * m_heli.controlInput.collectiveInput) * modVal;
    mainRotorForce = DirectX::SimpleMath::Vector3::Transform(mainRotorForce, m_heli.alignment);
    DirectX::SimpleMath::Vector3 tailForce = -m_heli.right * (m_heli.controlInput.yawPedalInput) * modVal;
    DirectX::SimpleMath::Vector3 gravityForce = (m_heli.gravity) * modVal;

    /*
    float rotorMag = (m_heli.q.mainRotorForceMagnitude * m_heli.controlInput.collectiveInput) * modVal;
    mainRotorForce = m_heli.controlInput.cyclicStick;
    mainRotorForce = DirectX::SimpleMath::Vector3::Transform(mainRotorForce, m_heli.alignment);
    mainRotorForce.Normalize();
    mainRotorForce = mainRotorForce * rotorMag;
    */
    float gravMag = (m_heli.gravity.y) * modVal;
    gravityForce = m_heli.gravity;
    gravityForce.Normalize();
    gravityForce = gravityForce * (4.8 * modVal);


    if (m_debugToggle == true)
    {
        int testBreak = 0;
        m_debugToggle = false;
    }

    Utility::Torque rotorTorque = Utility::GetTorqueForce(mainRotorTorqueArm, mainRotorForce);
    Utility::Torque tailTorque = Utility::GetTorqueForce(tailRotorTorqueArm, tailForce);
    Utility::Torque gravTorque = Utility::GetTorqueForce(gravityTorqueArm, gravityForce);

    //DirectX::SimpleMath::Vector3 torqueAxis = (rotorTorque.axis * rotorTorque.magnitude) + (tailTorque.axis * tailTorque.magnitude) + (gravTorque.axis * gravTorque.magnitude);
    //DirectX::SimpleMath::Vector3 torqueAxis = (rotorTorque.axis * rotorTorque.magnitude) + (gravTorque.axis * gravTorque.magnitude);
    DirectX::SimpleMath::Vector3 torqueAxis = (rotorTorque.axis * rotorTorque.magnitude) + (tailTorque.axis * tailTorque.magnitude);
    torqueAxis = (rotorTorque.axis * rotorTorque.magnitude) + (tailTorque.axis * tailTorque.magnitude) + (m_heli.q.pendulumTorqueForceTest.axis * m_heli.q.pendulumTorqueForceTest.magnitude);
    torqueAxis.Normalize();
    //const float torqueMag = rotorTorque.magnitude + tailTorque.magnitude + gravTorque.magnitude;
    //const float torqueMag = rotorTorque.magnitude  + gravTorque.magnitude;
    float torqueMag = rotorTorque.magnitude + tailTorque.magnitude;
    torqueMag = rotorTorque.magnitude + tailTorque.magnitude + m_heli.q.pendulumTorqueForceTest.magnitude;

    Utility::Torque updatedTorque;
    updatedTorque.axis = torqueAxis;
    updatedTorque.magnitude = torqueMag;

    /*
    DebugPushTestLine(m_heli.centerOfMass, torqueAxis, 10.0f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 0.0f, 1.0f, 1.0f));
    DebugPushTestLine(m_heli.mainRotorPos, mainRotorForce, 10.0f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    DebugPushTestLine(m_heli.tailRotorPos, tailForce, 10.0f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 0.0f, 0.0f, 1.0f));
    DebugPushTestLine(m_heli.centerOfMass, gravityForce, 10.0f, 0.0f, DirectX::SimpleMath::Vector4(0.0f, 1.0f, 1.0f, 1.0f));
    */
    return updatedTorque;
}

Utility::Torque Vehicle::UpdateBodyTorqueTestRunge(Utility::Torque aPendulumTorque, const float aTimeStep)
{
    DirectX::SimpleMath::Vector3 centerMassPos = m_heli.localCenterOfMass;
    centerMassPos = DirectX::SimpleMath::Vector3::Transform(centerMassPos, m_heli.alignment);
    centerMassPos = m_heli.centerOfMass;
    DirectX::SimpleMath::Vector3 rotorPos = m_heli.localMainRotorPos;
    rotorPos = DirectX::SimpleMath::Vector3::Transform(rotorPos, m_heli.alignment);
    rotorPos = m_heli.mainRotorPos;
    DirectX::SimpleMath::Vector3 tailPos = m_heli.localTailRotorPos;
    tailPos = DirectX::SimpleMath::Vector3::Transform(tailPos, m_heli.alignment);
    tailPos = m_heli.tailRotorPos;

    DirectX::SimpleMath::Vector3 mainRotorTorqueArm = rotorPos - centerMassPos;
    DirectX::SimpleMath::Vector3 tailRotorTorqueArm = tailPos - centerMassPos;
    DirectX::SimpleMath::Vector3 gravityTorqueArm = centerMassPos - rotorPos;
    //DirectX::SimpleMath::Vector3 gravityTorqueArm = rotorPos - centerMassPos;

    /*
    DebugPushTestLine(m_heli.mainRotorPos, mainRotorTorqueArm, 10.0f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 0.0f, 0.0f, 1.0f));
    DebugPushTestLine(m_heli.tailRotorPos, tailRotorTorqueArm, 10.0f, 0.0f, DirectX::SimpleMath::Vector4(0.0f, 1.0f, 0.0f, 1.0f));
    DebugPushTestLine(m_heli.centerOfMass, gravityTorqueArm, 10.0f, 0.0f, DirectX::SimpleMath::Vector4(0.0f, 0.0f, 1.0f, 1.0f));
    */

    const float modVal = 0.01f;
    DirectX::SimpleMath::Vector3 mainRotorForce = m_heli.q.mainRotorForceNormal * (m_heli.q.mainRotorForceMagnitude * m_heli.controlInput.collectiveInput) * modVal;
    mainRotorForce = m_heli.controlInput.cyclicStick * (m_heli.q.mainRotorForceMagnitude * m_heli.controlInput.collectiveInput) * modVal;
    mainRotorForce = DirectX::SimpleMath::Vector3::Transform(mainRotorForce, m_heli.alignment);
    DirectX::SimpleMath::Vector3 tailForce = -m_heli.right * (m_heli.controlInput.yawPedalInput) * modVal;
    DirectX::SimpleMath::Vector3 gravityForce = (m_heli.gravity) * modVal;

    /*
    float rotorMag = (m_heli.q.mainRotorForceMagnitude * m_heli.controlInput.collectiveInput) * modVal;
    mainRotorForce = m_heli.controlInput.cyclicStick;
    mainRotorForce = DirectX::SimpleMath::Vector3::Transform(mainRotorForce, m_heli.alignment);
    mainRotorForce.Normalize();
    mainRotorForce = mainRotorForce * rotorMag;
    */
    float gravMag = (m_heli.gravity.y) * modVal;
    gravityForce = m_heli.gravity;
    gravityForce.Normalize();
    gravityForce = gravityForce * (4.8 * modVal);


    if (m_debugToggle == true)
    {
        int testBreak = 0;
        m_debugToggle = false;
    }

    Utility::Torque rotorTorque = Utility::GetTorqueForce(mainRotorTorqueArm, mainRotorForce);
    Utility::Torque tailTorque = Utility::GetTorqueForce(tailRotorTorqueArm, tailForce);
    Utility::Torque gravTorque = Utility::GetTorqueForce(gravityTorqueArm, gravityForce);

    //DirectX::SimpleMath::Vector3 torqueAxis = (rotorTorque.axis * rotorTorque.magnitude) + (tailTorque.axis * tailTorque.magnitude) + (gravTorque.axis * gravTorque.magnitude);
    //DirectX::SimpleMath::Vector3 torqueAxis = (rotorTorque.axis * rotorTorque.magnitude) + (gravTorque.axis * gravTorque.magnitude);
    DirectX::SimpleMath::Vector3 torqueAxis = (rotorTorque.axis * rotorTorque.magnitude) + (tailTorque.axis * tailTorque.magnitude);
    torqueAxis = (rotorTorque.axis * rotorTorque.magnitude) + (tailTorque.axis * tailTorque.magnitude) + (aPendulumTorque.axis * aPendulumTorque.magnitude);
    torqueAxis.Normalize();
    //const float torqueMag = rotorTorque.magnitude + tailTorque.magnitude + gravTorque.magnitude;
    //const float torqueMag = rotorTorque.magnitude  + gravTorque.magnitude;
    float torqueMag = rotorTorque.magnitude + tailTorque.magnitude;
    torqueMag = rotorTorque.magnitude + tailTorque.magnitude + aPendulumTorque.magnitude;

    Utility::Torque updatedTorque;
    updatedTorque.axis = torqueAxis;
    updatedTorque.magnitude = torqueMag;

    /*
    DebugPushTestLine(m_heli.centerOfMass, torqueAxis, 10.0f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 0.0f, 1.0f, 1.0f));
    DebugPushTestLine(m_heli.mainRotorPos, mainRotorForce, 10.0f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    DebugPushTestLine(m_heli.tailRotorPos, tailForce, 10.0f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 0.0f, 0.0f, 1.0f));
    DebugPushTestLine(m_heli.centerOfMass, gravityForce, 10.0f, 0.0f, DirectX::SimpleMath::Vector4(0.0f, 1.0f, 1.0f, 1.0f));
    */
    return updatedTorque;
}

void Vehicle::UpdateCyclicStick(ControlInput& aInput)
{
    DirectX::SimpleMath::Quaternion cyclicQuat = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(0.0f, aInput.cyclicInputRoll, aInput.cyclicInputPitch);
    DirectX::SimpleMath::Vector3 updatedCyclic = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitY, cyclicQuat);

    float cyclicAngle = Utility::GetAngleBetweenVectors(DirectX::SimpleMath::Vector3::UnitY, updatedCyclic);
    if (cyclicAngle > aInput.cyclicInputMax)
    {
        const float angleProportionToEdgeOfCone = aInput.cyclicInputMax / cyclicAngle;
        cyclicQuat = DirectX::SimpleMath::Quaternion::Slerp(DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(0.0f, 0.0f, 0.0f), cyclicQuat, angleProportionToEdgeOfCone);
        updatedCyclic = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitY, cyclicQuat);
    }

    aInput.cyclicStick = updatedCyclic;
}

void Vehicle::UpdateLandingGear(struct LandingGear& aLandingGear, const double aTimeDelta)
{
    const float timeStep = static_cast<float>(aTimeDelta);

    if (aLandingGear.currentState == LandingGear::LandingGearState::LANDINGGEARSTATE_ASCENDING)
    {
        aLandingGear.angleCurrent += aLandingGear.angleDelta * aTimeDelta;
        if (aLandingGear.angleCurrent >= aLandingGear.angleAtUp)
        {
            aLandingGear.angleCurrent = aLandingGear.angleAtUp;
            aLandingGear.currentState = LandingGear::LandingGearState::LANDINGGEARSTATE_UP;
        }
    }
    if (aLandingGear.currentState == LandingGear::LandingGearState::LANDINGGEARSTATE_DESCENDING)
    {
        aLandingGear.angleCurrent -= aLandingGear.angleDelta * aTimeDelta;
        if (aLandingGear.angleCurrent <= aLandingGear.angleAtDown)
        {
            aLandingGear.angleCurrent = aLandingGear.angleAtDown;
            aLandingGear.currentState = LandingGear::LandingGearState::LANDINGGEARSTATE_DOWN;
        }
    }
}

void Vehicle::UpdateModel()
{
    DirectX::SimpleMath::Matrix updateMat = DirectX::SimpleMath::Matrix::Identity;

    DirectX::SimpleMath::Vector3 translation(0.0f, 0.0f, -2.0f);
    DirectX::SimpleMath::Vector3 antiTrans(0.0f, 0.0f, 2.0f);
    DirectX::SimpleMath::Matrix translationMat = DirectX::SimpleMath::Matrix::CreateTranslation(translation);
    DirectX::SimpleMath::Matrix antiTranslationMat = DirectX::SimpleMath::Matrix::CreateTranslation(antiTrans);
    //updateMat *= translationMat;
    updateMat = DirectX::SimpleMath::Matrix::CreateWorld(m_heli.q.position, -m_heli.right, m_heli.up);
    //updateMat *= antiTranslationMat;
    //updateMat = DirectX::SimpleMath::Matrix::CreateWorld(m_heli.q.position, m_heli.forward, m_heli.up);

    //updateMat = m_heli.alignment;;
    //updateMat *= DirectX::SimpleMath::Matrix::CreateTranslation(m_heli.q.position);

    DirectX::SimpleMath::Matrix updateMat2 = DirectX::SimpleMath::Matrix::CreateWorld(m_prevPos, -m_prevRight, m_prevUp);
    //updateMat = updateMat2;

    m_heliModel.noseConeMatrix = m_heliModel.localNoseConeMatrix;
    m_heliModel.noseConeMatrix *= updateMat;

    m_heliModel.toothMatrix1 = m_heliModel.localToothMatrix1;
    m_heliModel.toothMatrix1 *= updateMat;
    m_heliModel.toothMatrix2 = m_heliModel.localToothMatrix2;
    m_heliModel.toothMatrix2 *= updateMat;
    m_heliModel.toothMatrix3 = m_heliModel.localToothMatrix3;
    m_heliModel.toothMatrix3 *= updateMat;
    m_heliModel.toothMatrix4 = m_heliModel.localToothMatrix4;
    m_heliModel.toothMatrix4 *= updateMat;
    m_heliModel.toothMatrix5 = m_heliModel.localToothMatrix5;
    m_heliModel.toothMatrix5 *= updateMat;
    m_heliModel.toothMatrix6 = m_heliModel.localToothMatrix6;
    m_heliModel.toothMatrix6 *= updateMat;
    m_heliModel.toothMatrix7 = m_heliModel.localToothMatrix7;
    m_heliModel.toothMatrix7 *= updateMat;
    m_heliModel.toothMatrix8 = m_heliModel.localToothMatrix8;
    m_heliModel.toothMatrix8 *= updateMat;
    m_heliModel.toothMatrix9 = m_heliModel.localToothMatrix9;
    m_heliModel.toothMatrix9 *= updateMat;
    m_heliModel.toothMatrix10 = m_heliModel.localToothMatrix10;
    m_heliModel.toothMatrix10 *= updateMat;
    m_heliModel.toothMatrix11 = m_heliModel.localToothMatrix11;
    m_heliModel.toothMatrix11 *= updateMat;
    m_heliModel.toothMatrix12 = m_heliModel.localToothMatrix12;
    m_heliModel.toothMatrix12 *= updateMat;

    m_heliModel.toothBackdropLeftMatrix = m_heliModel.localToothBackdropLeftMatrix;
    m_heliModel.toothBackdropLeftMatrix *= updateMat;
    m_heliModel.toothBackdropRightMatrix = m_heliModel.localToothBackdropRightMatrix;
    m_heliModel.toothBackdropRightMatrix *= updateMat;

    m_heliModel.sideStripeMatrix0 = m_heliModel.localSidestripeMatrix0;
    m_heliModel.sideStripeMatrix0 *= updateMat;
    m_heliModel.sideStripeMatrix1 = m_heliModel.localSidestripeMatrix1;
    m_heliModel.sideStripeMatrix1 *= updateMat;
    m_heliModel.sideStripeMatrix2 = m_heliModel.localSidestripeMatrix2;
    m_heliModel.sideStripeMatrix2 *= updateMat;
    m_heliModel.sideStripeMatrix3 = m_heliModel.localSidestripeMatrix3;
    m_heliModel.sideStripeMatrix3 *= updateMat;
    m_heliModel.sideStripeMatrix4 = m_heliModel.localSidestripeMatrix4;
    m_heliModel.sideStripeMatrix4 *= updateMat;
    m_heliModel.sideStripeMatrix5 = m_heliModel.localSidestripeMatrix5;
    m_heliModel.sideStripeMatrix5 *= updateMat;



    m_heliModel.landingGearFrontMatrix = m_heliModel.localLandingGearFrontMatrix;
    m_heliModel.landingGearFrontMatrix *= updateMat;

    m_heliModel.landingGearRearLeftMatrix = m_heliModel.localLandingGearRearLeftMatrix;
    m_heliModel.landingGearRearLeftMatrix *= updateMat;
    m_heliModel.landingGearRearRightMatrix = m_heliModel.localLandingGearRearRightMatrix;
    m_heliModel.landingGearRearRightMatrix *= updateMat;

    m_heliModel.landingGearUpperArmMatrix = m_heliModel.localLandingGearUperArmMatrix;
    m_heliModel.landingGearLowerArmMatrix = m_heliModel.localLandingGearLowerArmMatrix;
    m_heliModel.landingGearWheelMatrix = m_heliModel.localLandingGearWheelMatrix;

    DirectX::SimpleMath::Matrix landingGearRotationMatrix = DirectX::SimpleMath::Matrix::CreateRotationZ(m_heli.landingGear.angleCurrent);
    m_heliModel.landingGearUpperArmMatrix *= landingGearRotationMatrix;
    m_heliModel.landingGearLowerArmMatrix *= landingGearRotationMatrix;
    m_heliModel.landingGearWheelMatrix *= landingGearRotationMatrix;

    m_heliModel.noseBodyMatrix = m_heliModel.localNoseBodyMatrix;
    m_heliModel.noseBodyMatrix *= updateMat;

    m_heliModel.bodyMatrix = m_heliModel.localBodyMatrix;
    m_heliModel.bodyMatrix *= updateMat;

    m_heliModel.doorMatrix = m_heliModel.localDoorMatrix;
    m_heliModel.doorMatrix *= updateMat;

    m_heliModel.mainWingMatrix = m_heliModel.localMainWingMatrix;
    m_heliModel.mainWingMatrix *= updateMat;

    m_heliModel.mainWingLeadingEdgeMatrix = m_heliModel.localMainWingLeadingEdgeMatrix;
    m_heliModel.mainWingLeadingEdgeMatrix *= updateMat;

    m_heliModel.mainWingTailEdgeMatrix = m_heliModel.localMainWingTailEdgeMatrix;
    m_heliModel.mainWingTailEdgeMatrix *= updateMat;

    m_heliModel.wingJetCenterMatrix = m_heliModel.localWingJetCenterMatrix;
    m_heliModel.wingJetCenterMatrix *= updateMat;

    m_heliModel.wingJetForwardHousingLeftMatrix = m_heliModel.localWingJetForwardHousingLeftMatrix;
    m_heliModel.wingJetForwardHousingLeftMatrix *= updateMat;
    m_heliModel.wingJetForwardHousingRightMatrix = m_heliModel.localWingJetForwardHousingRightMatrix;
    m_heliModel.wingJetForwardHousingRightMatrix *= updateMat;

    m_heliModel.wingJetForwardCowlLeftMatrix = m_heliModel.localWingJetForwardCowlLeftMatrix;
    m_heliModel.wingJetForwardCowlLeftMatrix *= updateMat;
    m_heliModel.wingJetForwardCowlRightMatrix = m_heliModel.localWingJetForwardCowlRightMatrix;
    m_heliModel.wingJetForwardCowlRightMatrix *= updateMat;

    m_heliModel.wingJetCouplingLeftMatrix = m_heliModel.localWingJetCouplingLeftMatrix;
    m_heliModel.wingJetCouplingLeftMatrix *= updateMat;
    m_heliModel.wingJetCouplingRightMatrix = m_heliModel.localWingJetCouplingRightMatrix;
    m_heliModel.wingJetCouplingRightMatrix *= updateMat;

    m_heliModel.windShieldMatrix = m_heliModel.localWindShieldMatrix;
    m_heliModel.windShieldMatrix *= updateMat;

    m_heliModel.noseCowlMatrix = m_heliModel.localNoseCowlMatrix;
    m_heliModel.noseCowlMatrix *= updateMat;

    m_heliModel.sideWindowUpperMatrix = m_heliModel.localSideWindowUpperMatrix;
    m_heliModel.sideWindowUpperMatrix *= updateMat;
    m_heliModel.sideWindowLowerMatrix = m_heliModel.localSideWindowLowerMatrix;
    m_heliModel.sideWindowLowerMatrix *= updateMat;

    m_heliModel.bodyRearMatrix = m_heliModel.localBodyRearMatrix;
    m_heliModel.bodyRearMatrix *= updateMat;

    m_heliModel.engineHousingMatrix = m_heliModel.localEngineHousingMatrix;
    m_heliModel.engineHousingMatrix *= updateMat;

    m_heliModel.engineHousingFrontRightMatrix = m_heliModel.localEngineHousingFrontRightMatrix;
    m_heliModel.engineHousingFrontRightMatrix *= updateMat;

    m_heliModel.engineHousingFrontLeftMatrix = m_heliModel.localEngineHousingFrontLeftMatrix;
    m_heliModel.engineHousingFrontLeftMatrix *= updateMat;

    m_heliModel.engineHousingRearLeftMatrix = m_heliModel.localEngineHousingRearLeftMatrix;
    m_heliModel.engineHousingRearLeftMatrix *= updateMat;

    m_heliModel.engineHousingRearRightMatrix = m_heliModel.localEngineHousingRearRightMatrix;
    m_heliModel.engineHousingRearRightMatrix *= updateMat;

    m_heliModel.engineHousingFrontMatrix = m_heliModel.localEngineHousingFrontMatrix;
    m_heliModel.engineHousingFrontMatrix *= updateMat;

    m_heliModel.engineHousingRearMatrix = m_heliModel.localEngineHousingRearMatrix;
    m_heliModel.engineHousingRearMatrix *= updateMat;

    m_heliModel.engineHousingSideLeftMatrix = m_heliModel.localEngineHousingSideLeftMatrix;
    m_heliModel.engineHousingSideLeftMatrix *= updateMat;

    m_heliModel.engineHousingSideRightMatrix = m_heliModel.localEngineHousingSideRightMatrix;
    m_heliModel.engineHousingSideRightMatrix *= updateMat;

    m_heliModel.engineExhaustLeftMatrix = m_heliModel.localEngineExhaustLeftMatrix;
    m_heliModel.engineExhaustLeftMatrix *= updateMat;
    m_heliModel.engineExhaustRightMatrix = m_heliModel.localEngineExhaustRightMatrix;
    m_heliModel.engineExhaustRightMatrix *= updateMat;

    m_heliModel.engineHousingIntakeLeftMatrix = m_heliModel.localEngineHousingIntakeLeftMatrix;
    m_heliModel.engineHousingIntakeLeftMatrix *= updateMat;
    m_heliModel.engineHousingIntakeRightMatrix = m_heliModel.localEngineHousingIntakeRightMatrix;
    m_heliModel.engineHousingIntakeRightMatrix *= updateMat;

    m_heliModel.tailBoomMatrix = m_heliModel.localTailBoomMatrix;
    m_heliModel.tailBoomMatrix *= updateMat;

    m_heliModel.tailBoomLowerMatrix = m_heliModel.localTailBoomLowerMatrix;
    m_heliModel.tailBoomLowerMatrix *= updateMat;

    m_heliModel.tailBoomUpperMatrix = m_heliModel.localTailBoomUpperMatrix;
    m_heliModel.tailBoomUpperMatrix *= updateMat;

    m_heliModel.tailBoomEndCapMatrix = m_heliModel.localTailBoomEndCapMatrix;
    m_heliModel.tailBoomEndCapMatrix *= updateMat;

    m_heliModel.tailFinUpperMatrix = m_heliModel.localTailFinUpperMatrix;
    m_heliModel.tailFinUpperMatrix *= updateMat;

    m_heliModel.tailFinUpperTipMatrix = m_heliModel.localTailFinUpperTipMatrix;
    m_heliModel.tailFinUpperTipMatrix *= updateMat;

    m_heliModel.tailFinLowerMatrix = m_heliModel.localTailFinLowerMatrix;
    m_heliModel.tailFinLowerMatrix *= updateMat;

    m_heliModel.tailFinLowerTipMatrix = m_heliModel.localTailFinLowerTipMatrix;
    m_heliModel.tailFinLowerTipMatrix *= updateMat;

    m_heliModel.tailFinLeadingEdgeLowerMatrix = m_heliModel.localTailFinLeadingEdgeLowerMatrix;
    m_heliModel.tailFinLeadingEdgeLowerMatrix *= updateMat;
    m_heliModel.tailFinLeadingEdgeUpperMatrix = m_heliModel.localTailFinLeadingEdgeUpperMatrix;
    m_heliModel.tailFinLeadingEdgeUpperMatrix *= updateMat;

    m_heliModel.tailWingMatrix = m_heliModel.localTailWingMatrix;
    m_heliModel.tailWingMatrix *= updateMat;

    m_heliModel.tailWingLeadingEdgeMatrix = m_heliModel.localTailWingLeadingEdgeMatrix;
    m_heliModel.tailWingLeadingEdgeMatrix *= updateMat;

    m_heliModel.tailWingFinLeftMatrix = m_heliModel.localTailWingFinLeftMatrix;
    m_heliModel.tailWingFinLeftMatrix *= updateMat;
    m_heliModel.tailWingFinRightMatrix = m_heliModel.localTailWingFinRightMatrix;
    m_heliModel.tailWingFinRightMatrix *= updateMat;

    m_heliModel.mainRotorAxelMatrix = m_heliModel.localMainRotorAxelMatrix;
    m_heliModel.mainRotorAxelMatrix *= updateMat;

    m_heliModel.mainRotorHubMatrix = m_heliModel.localMainRotorHubMatrix;
    m_heliModel.mainRotorHubMatrix *= updateMat;

    /// Update physics hard point, needs to be moved and updated with tail pos and cog pos
    DirectX::SimpleMath::Vector3 mainRotorPos = DirectX::SimpleMath::Vector3::Zero;
    mainRotorPos = mainRotorPos.Transform(mainRotorPos, m_heliModel.mainRotorHubMatrix);
    //m_heli.mainRotorPos = mainRotorPos;

    // Main rotor
    DirectX::SimpleMath::Matrix mainRotorSpin = DirectX::SimpleMath::Matrix::CreateRotationY(m_heli.mainRotor.rotorRotation);
    m_heliModel.mainRotorArmMatrix = m_heliModel.localMainRotorArmMatrix;
    m_heliModel.mainRotorArmMatrix *= mainRotorSpin;
    m_heliModel.mainRotorArmMatrix *= updateMat;

    // main rotor blade 1
    DirectX::SimpleMath::Matrix mainRotorPitch1 = DirectX::SimpleMath::Matrix::CreateRotationX(m_heli.mainRotor.bladeVec[0].pitchAngle);
    m_heliModel.mainRotorBladeMatrix1 = m_heliModel.mainRotorBladeTranslationMatrix1;
    m_heliModel.mainRotorBladeMatrix1 *= mainRotorPitch1;
    m_heliModel.mainRotorBladeMatrix1 *= m_heliModel.localMainRotorBladeMatrix1;  
    m_heliModel.mainRotorBladeMatrix1 *= mainRotorSpin;
    m_heliModel.mainRotorBladeMatrix1 *= updateMat;
    
    // main rotor edge 1
    m_heliModel.mainRotorBladeEdgeMatrix1 = m_heliModel.mainRotorBladeEdgeTranslationMatrix1;
    m_heliModel.mainRotorBladeEdgeMatrix1 *= mainRotorPitch1;
    m_heliModel.mainRotorBladeEdgeMatrix1 *= m_heliModel.localMainRotorEdgeBladeMatrix1;
    m_heliModel.mainRotorBladeEdgeMatrix1 *= mainRotorSpin;
    m_heliModel.mainRotorBladeEdgeMatrix1 *= updateMat;

    // main rotor blade 2
    DirectX::SimpleMath::Matrix mainRotorPitch2 = DirectX::SimpleMath::Matrix::CreateRotationX(m_heli.mainRotor.bladeVec[1].pitchAngle);
    m_heliModel.mainRotorBladeMatrix2 = m_heliModel.mainRotorBladeTranslationMatrix2;
    m_heliModel.mainRotorBladeMatrix2 *= mainRotorPitch2;
    m_heliModel.mainRotorBladeMatrix2 *= m_heliModel.localMainRotorBladeMatrix2;
    m_heliModel.mainRotorBladeMatrix2 *= mainRotorSpin;
    m_heliModel.mainRotorBladeMatrix2 *= updateMat;
       
    // main rotor edge 2
    m_heliModel.mainRotorBladeEdgeMatrix2 = m_heliModel.mainRotorBladeEdgeTranslationMatrix2;
    DirectX::SimpleMath::Matrix mainEdgePitch2 = DirectX::SimpleMath::Matrix::CreateRotationX(-m_heli.mainRotor.bladeVec[1].pitchAngle);
    m_heliModel.mainRotorBladeEdgeMatrix2 *= mainEdgePitch2;
    m_heliModel.mainRotorBladeEdgeMatrix2 *= m_heliModel.localMainRotorEdgeBladeMatrix2;
    m_heliModel.mainRotorBladeEdgeMatrix2 *= mainRotorSpin;
    m_heliModel.mainRotorBladeEdgeMatrix2 *= updateMat;

    // tail rotor axel
    m_heliModel.tailRotorAxelMatrix = m_heliModel.localTailRotorAxelMatrix;
    m_heliModel.tailRotorAxelMatrix *= updateMat;

    // tail rotor hub
    m_heliModel.tailRotorHubMatrix = m_heliModel.localTailRotorHubMatrix;
    m_heliModel.tailRotorHubMatrix *= updateMat;

    // tail arm 
    DirectX::SimpleMath::Matrix tailArmSpin = DirectX::SimpleMath::Matrix::CreateRotationZ(m_heli.tailRotor.rotorRotation);
    m_heliModel.tailRotorArmMatrix = tailArmSpin;
    m_heliModel.tailRotorArmMatrix *= m_heliModel.localTailRotorArmMatrix;       
    m_heliModel.tailRotorArmMatrix *= updateMat;

    // tail rotor blade 1
    DirectX::SimpleMath::Matrix tailRotorPitch1 = DirectX::SimpleMath::Matrix::CreateRotationY(m_heli.tailRotor.bladeVec[0].pitchAngle);
    m_heliModel.tailRotorBladeMatrix1 = m_heliModel.localTailRotorBladeMatrix1;
    m_heliModel.tailRotorBladeMatrix1 *= tailRotorPitch1;
    m_heliModel.tailRotorBladeMatrix1 *= tailArmSpin;
    m_heliModel.tailRotorBladeMatrix1 *= m_heliModel.tailRotorBladeTranslationMatrix1;  
    m_heliModel.tailRotorBladeMatrix1 *= updateMat;

    // tail rotor edge 1
    //m_heliModel.tailRotorBladeEdgeMatrix1 = m_heliModel.localTailRotorBladeMatrix1;
    m_heliModel.tailRotorBladeEdgeMatrix1 = m_heliModel.localTailRotorBladeEdgeMatrix1;
    m_heliModel.tailRotorBladeEdgeMatrix1 *= tailRotorPitch1;   
    m_heliModel.tailRotorBladeEdgeMatrix1 *= tailArmSpin;
    //m_heliModel.tailRotorBladeEdgeMatrix1 *= m_heliModel.tailRotorBladeTranslationMatrix1;
    m_heliModel.tailRotorBladeEdgeMatrix1 *= m_heliModel.tailRotorBladeEdgeTranslationMatrix1;
    
    m_heliModel.tailRotorBladeEdgeMatrix1 *= updateMat;
    
    /*
    * tailRotorBladeEdgeTranslationMatrix1
    * localTailRotorBladeEdgeMatrix1
    // main rotor edge 1
    m_heliModel.mainRotorBladeEdgeMatrix1 = m_heliModel.mainRotorBladeEdgeTranslationMatrix1;
    m_heliModel.mainRotorBladeEdgeMatrix1 *= mainRotorPitch1;
    m_heliModel.mainRotorBladeEdgeMatrix1 *= m_heliModel.localMainRotorEdgeBladeMatrix1;
    m_heliModel.mainRotorBladeEdgeMatrix1 *= mainRotorSpin;
    m_heliModel.mainRotorBladeEdgeMatrix1 *= updateMat;
    */

    // tail rotor blade 2
    DirectX::SimpleMath::Matrix tailRotorPitch2 = DirectX::SimpleMath::Matrix::CreateRotationY(m_heli.tailRotor.bladeVec[1].pitchAngle);
    m_heliModel.tailRotorBladeMatrix2 = m_heliModel.localTailRotorBladeMatrix2;
    m_heliModel.tailRotorBladeMatrix2 *= tailRotorPitch2;
    m_heliModel.tailRotorBladeMatrix2 *= tailArmSpin;
    m_heliModel.tailRotorBladeMatrix2 *= m_heliModel.tailRotorBladeTranslationMatrix2;
    m_heliModel.tailRotorBladeMatrix2 *= updateMat;

    // tail rotor edge 2
    //m_heliModel.tailRotorBladeEdgeMatrix2 = m_heliModel.localTailRotorBladeMatrix2;
    m_heliModel.tailRotorBladeEdgeMatrix2 = m_heliModel.localTailRotorBladeEdgeMatrix2;
    m_heliModel.tailRotorBladeEdgeMatrix2 *= tailRotorPitch2;
    m_heliModel.tailRotorBladeEdgeMatrix2 *= tailArmSpin;
    //m_heliModel.tailRotorBladeEdgeMatrix2 *= m_heliModel.tailRotorBladeTranslationMatrix2;
    m_heliModel.tailRotorBladeEdgeMatrix2 *= m_heliModel.tailRotorBladeEdgeTranslationMatrix2;
    m_heliModel.tailRotorBladeEdgeMatrix2 *= updateMat;
    
}

void Vehicle::UpdatePendulumMotion(Utility::Torque& aTorque, DirectX::SimpleMath::Vector3& aVelocity, const float aTimeStep)
{
    float rodLength = (m_heli.q.mainRotorForceMagnitude * m_heli.controlInput.collectiveInput);
    DirectX::SimpleMath::Vector3 testRod = m_heli.localMainRotorPos - m_heli.localCenterOfMass;
    testRod = m_heli.mainRotorPos - m_heli.centerOfMass;
    rodLength = testRod.Length();
    DebugPushUILineDecimalNumber("rodLength = ", rodLength, "");

    DirectX::SimpleMath::Vector3 rodUp = m_heli.up;
    DirectX::SimpleMath::Vector3 rodRotor = m_heli.q.mainRotorForceNormal;
    DirectX::SimpleMath::Vector3 rod = rodUp + rodRotor;
    float thetaAngle = Utility::GetAngleBetweenVectors(DirectX::SimpleMath::Vector3::UnitY, rod);

    // body roll torque due to pendulum
    Utility::Torque torqueUpdate;
    torqueUpdate.axis = -rod;
    torqueUpdate.axis = torqueUpdate.axis.Cross(-DirectX::SimpleMath::Vector3::UnitY);;
    torqueUpdate.axis.Normalize();
    torqueUpdate.magnitude = -(m_heli.gravity.y * aTimeStep) * rodLength * sin(thetaAngle);
    torqueUpdate.magnitude *= aTimeStep; // reducing further to eliminate rotational jitters, needs further investigation
    m_heli.q.pendulumTorqueForceTest = torqueUpdate;
    aTorque = torqueUpdate;

    // acceleration due to pendulum motion
    DirectX::SimpleMath::Vector3 accelerationDirection = torqueUpdate.axis;
    accelerationDirection = accelerationDirection.Cross(-rod);
    accelerationDirection.Normalize();
    float accelerationMag = -((m_heli.gravity.y) / rodLength) * sin(thetaAngle);

    // only update with viable data and if not aligned with gravity direction
    if (accelerationMag > 0.00001f && accelerationMag < 20.0f)
    {
        accelerationDirection *= accelerationMag * aTimeStep;
        //accelerationDirection *= accelerationMag;
        accelerationDirection.y = 0.0f;
        aVelocity += accelerationDirection;
    }
}

void Vehicle::UpdatePendulumMotion2(const float aTimeStep)
{
    float rodLength = (m_heli.q.mainRotorForceMagnitude * m_heli.controlInput.collectiveInput);
    DirectX::SimpleMath::Vector3 rodUp = m_heli.up;
    DirectX::SimpleMath::Vector3 rodRotor = m_heli.q.mainRotorForceNormal;
    DirectX::SimpleMath::Vector3 rod = rodUp + rodRotor;
    float thetaAngle = Utility::GetAngleBetweenVectors(DirectX::SimpleMath::Vector3::UnitY, rod);

    // body roll torque due to pendulum
    Utility::Torque torqueUpdate;
    torqueUpdate.axis = -rod;
    torqueUpdate.axis = torqueUpdate.axis.Cross(-DirectX::SimpleMath::Vector3::UnitY);;
    torqueUpdate.axis.Normalize();
    torqueUpdate.magnitude = -(m_heli.gravity.y * aTimeStep) * rodLength * sin(thetaAngle);
    torqueUpdate.magnitude *= aTimeStep; // reducing further to eliminate rotational jitters, needs further investigation
    m_heli.q.pendulumTorqueForceTest = torqueUpdate;

    // acceleration due to pendulum motion
    DirectX::SimpleMath::Vector3 accelerationDirection = torqueUpdate.axis;
    accelerationDirection = accelerationDirection.Cross(-rod);
    float accelerationMag = -((m_heli.gravity.y) / rodLength) * sin(thetaAngle);

    // only update with viable data and if not aligned with gravity direction
    if (accelerationMag > 0.00001f && accelerationMag < 20.0f)
    {
        accelerationDirection *= accelerationMag;
        m_heli.q.velocity += accelerationDirection;
    }
}

void Vehicle::UpdatePhysicsPoints(struct HeliData& aHeli)
{
    DirectX::SimpleMath::Matrix updateMat = DirectX::SimpleMath::Matrix::Identity;

    DirectX::SimpleMath::Vector3 translation(0.0f, 0.0f, -2.0f);
    DirectX::SimpleMath::Vector3 antiTrans(0.0f, 0.0f, 2.0f);
    DirectX::SimpleMath::Matrix translationMat = DirectX::SimpleMath::Matrix::CreateTranslation(translation);
    DirectX::SimpleMath::Matrix antiTranslationMat = DirectX::SimpleMath::Matrix::CreateTranslation(antiTrans);
    //updateMat *= translationMat;
    updateMat = DirectX::SimpleMath::Matrix::CreateWorld(m_heli.q.position, -m_heli.right, m_heli.up);
    //updateMat *= antiTranslationMat;
    //updateMat = DirectX::SimpleMath::Matrix::CreateWorld(m_heli.q.position, m_heli.forward, m_heli.up);

    //updateMat = m_heli.alignment;;
    //updateMat *= DirectX::SimpleMath::Matrix::CreateTranslation(m_heli.q.position);

    //DirectX::SimpleMath::Matrix updateMat2 = DirectX::SimpleMath::Matrix::CreateWorld(m_heli.q.position, -m_prevRight, m_prevUp);
    DirectX::SimpleMath::Matrix updateMat2 = DirectX::SimpleMath::Matrix::CreateWorld(m_heli.q.position, -m_heli.right, m_heli.up);

    updateMat = updateMat2;

    m_heli.centerOfMass = m_heli.localCenterOfMass;
    m_heli.centerOfMass = DirectX::SimpleMath::Vector3::Transform(m_heli.localCenterOfMass, updateMat);
    m_heli.mainRotorPos = m_heli.localMainRotorPos;
    m_heli.mainRotorPos = DirectX::SimpleMath::Vector3::Transform(m_heli.localMainRotorPos, updateMat);
    m_heli.tailRotorPos = m_heli.localTailRotorPos;
    m_heli.tailRotorPos = DirectX::SimpleMath::Vector3::Transform(m_heli.localTailRotorPos, updateMat);

    m_heli.landingGearPos = m_heli.localLandingGearPos;
    m_heli.landingGearPos = DirectX::SimpleMath::Vector3::Transform(m_heli.localLandingGearPos, updateMat);
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

void Vehicle::UpdateRotorData(HeliData& aHeliData, const double aTimer)
{
    UpdateRotorSpin(aHeliData, aTimer);
    UpdateRotorPitch(aHeliData, aTimer);   
}

void Vehicle::UpdateRotorPitch(HeliData& aHeliData, const double aTimer)
{
    float mainRotorPitch = aHeliData.controlInput.collectiveInput * aHeliData.mainRotor.pitchAngleMax;  
    for (unsigned int i = 0; i < aHeliData.mainRotor.bladeVec.size(); ++i)
    {
        aHeliData.mainRotor.bladeVec[i].pitchAngle = mainRotorPitch;
        DirectX::SimpleMath::Vector3 rotorDir = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitX, 
            DirectX::SimpleMath::Matrix::CreateRotationY(aHeliData.mainRotor.rotorRotation + (static_cast<float>(i) * aHeliData.mainRotor.angleBetweenBlades)));

        const float pitchAng = Utility::GetAngleBetweenVectors(aHeliData.controlInput.cyclicStick, rotorDir) - Utility::ToRadians(90.0f);
        aHeliData.mainRotor.bladeVec[i].pitchAngle = mainRotorPitch + pitchAng;
    }
    float tailRotorPitch = aHeliData.tailRotor.neutralAngle + (aHeliData.controlInput.yawPedalInput * (aHeliData.tailRotor.pitchAngleMax * .5f));
    for (unsigned int i = 0; i < aHeliData.tailRotor.bladeVec.size(); ++i)
    {
        aHeliData.tailRotor.bladeVec[i].pitchAngle = tailRotorPitch;
    }  
}

void Vehicle::UpdateRotorSpin(HeliData& aHeliData, const double aTimer)
{
    const float rpmMax = 400.0f;
    const float rpmMin = 0.0f;
    const float prevRPM = aHeliData.mainRotor.rpm;
    const float rpmThrottleSet = aHeliData.controlInput.throttleInput * rpmMax;
    float rpmUpdate = prevRPM;
    const float currentTorqueCurvePos = (prevRPM / rpmMax) + 0.001f; // Small value added so the value can push past 0 rpm value for prevRPM
    float rpmDelta;
    if (currentTorqueCurvePos < 0.333f)
    {
        const float revDeltaRate = 1.9f;
        rpmDelta = revDeltaRate * currentTorqueCurvePos;
    }
    else if (currentTorqueCurvePos < 0.666f)
    {
        const float revDeltaRate = 2.75f;
        rpmDelta = revDeltaRate * currentTorqueCurvePos;
    }
    else
    {
        const float revDeltaRate = 1.85f;
        rpmDelta = revDeltaRate * currentTorqueCurvePos;
    }
    // accelerate
    if (rpmThrottleSet > prevRPM)
    {
        if (prevRPM + rpmDelta > rpmThrottleSet)
        {
            rpmUpdate = rpmThrottleSet;
        }
        else
        {
            rpmUpdate = prevRPM + rpmDelta;
        }
    }
    // deccelerate
    if (rpmThrottleSet < prevRPM)
    {
        if (prevRPM - rpmDelta < rpmThrottleSet)
        {
            rpmUpdate = rpmThrottleSet;
        }
        else
        {
            rpmUpdate = prevRPM - rpmDelta;
        }
    }
    // bracket rpm between min or max rate
    if (rpmUpdate > rpmMax)
    {
        rpmUpdate = rpmMax;
    }
    else if (rpmUpdate < rpmMin)
    {
        rpmUpdate = rpmMin;
    }

    aHeliData.mainRotor.rpm = rpmUpdate;

    const float mainRotorSpinUpdate = (aHeliData.mainRotor.rpm * 0.10472f) * aTimer; // 0.10472 is conversion of RPM to rads per second
    aHeliData.mainRotor.rotorRotation += mainRotorSpinUpdate;
    aHeliData.mainRotor.rotorRotation = Utility::WrapAngle(aHeliData.mainRotor.rotorRotation);

    const float tailRotarGearing = 0.65f;
    const float tailRotorSpinUpdate = mainRotorSpinUpdate * tailRotarGearing;
    aHeliData.tailRotor.rotorRotation += tailRotorSpinUpdate;
    aHeliData.tailRotor.rotorRotation = Utility::WrapAngle(aHeliData.tailRotor.rotorRotation);
}

void Vehicle::UpdateRotorForce()
{
    float pitch = m_heli.controlInput.cyclicInputPitch;
    float roll = m_heli.controlInput.cyclicInputRoll;

    DirectX::SimpleMath::Vector3 swashplate = DirectX::SimpleMath::Vector3::UnitY;
    swashplate = DirectX::SimpleMath::Vector3::Transform(swashplate, DirectX::SimpleMath::Matrix::CreateRotationZ(pitch));
    swashplate = DirectX::SimpleMath::Vector3::Transform(swashplate, DirectX::SimpleMath::Matrix::CreateRotationX(roll));
    float combinedAngle = Utility::GetAngleBetweenVectors(DirectX::SimpleMath::Vector3::UnitY, swashplate);

    if (combinedAngle > m_heli.controlInput.cyclicInputMax)
    {
        float ratio = m_heli.controlInput.cyclicInputMax / combinedAngle;
        pitch = m_heli.controlInput.cyclicInputPitch * ratio;
        roll = m_heli.controlInput.cyclicInputRoll * ratio;

        swashplate = DirectX::SimpleMath::Vector3::UnitY;
        swashplate = DirectX::SimpleMath::Vector3::Transform(swashplate, DirectX::SimpleMath::Matrix::CreateRotationZ(pitch));
        swashplate = DirectX::SimpleMath::Vector3::Transform(swashplate, DirectX::SimpleMath::Matrix::CreateRotationX(roll));
    }

    DirectX::SimpleMath::Matrix pitchRot = DirectX::SimpleMath::Matrix::CreateFromAxisAngle(m_heli.right, pitch);
    DirectX::SimpleMath::Matrix rollRot = DirectX::SimpleMath::Matrix::CreateFromAxisAngle(m_heli.forward, roll);

    DirectX::SimpleMath::Vector3 updateForce = DirectX::SimpleMath::Vector3::TransformNormal(m_heli.up, pitchRot * rollRot);
    updateForce.Normalize();
    m_heli.q.mainRotorForceNormal = updateForce;
}

DirectX::SimpleMath::Vector3 Vehicle::UpdateRotorForceRunge()
{
    float pitch = m_heli.controlInput.cyclicInputPitch;
    float roll = m_heli.controlInput.cyclicInputRoll;

    DirectX::SimpleMath::Vector3 swashplate = DirectX::SimpleMath::Vector3::UnitY;
    swashplate = DirectX::SimpleMath::Vector3::Transform(swashplate, DirectX::SimpleMath::Matrix::CreateRotationZ(pitch));
    swashplate = DirectX::SimpleMath::Vector3::Transform(swashplate, DirectX::SimpleMath::Matrix::CreateRotationX(roll));
    float combinedAngle = Utility::GetAngleBetweenVectors(DirectX::SimpleMath::Vector3::UnitY, swashplate);

    if (combinedAngle > m_heli.controlInput.cyclicInputMax)
    {
        float ratio = m_heli.controlInput.cyclicInputMax / combinedAngle;
        pitch = m_heli.controlInput.cyclicInputPitch * ratio;
        roll = m_heli.controlInput.cyclicInputRoll * ratio;

        swashplate = DirectX::SimpleMath::Vector3::UnitY;
        swashplate = DirectX::SimpleMath::Vector3::Transform(swashplate, DirectX::SimpleMath::Matrix::CreateRotationZ(pitch));
        swashplate = DirectX::SimpleMath::Vector3::Transform(swashplate, DirectX::SimpleMath::Matrix::CreateRotationX(roll));
    }

    DirectX::SimpleMath::Matrix pitchRot = DirectX::SimpleMath::Matrix::CreateFromAxisAngle(m_heli.right, pitch);
    DirectX::SimpleMath::Matrix rollRot = DirectX::SimpleMath::Matrix::CreateFromAxisAngle(m_heli.forward, roll);

    DirectX::SimpleMath::Vector3 updateForce = DirectX::SimpleMath::Vector3::TransformNormal(m_heli.up, pitchRot * rollRot);
    updateForce.Normalize();
    return updateForce;
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

    UpdatePhysicsPoints(m_heli);

    UpdateBladeLiftForce(static_cast<float>(aTimeDelta));

    DirectX::SimpleMath::Vector3 prevVelocity = m_heli.q.velocity;
    DirectX::SimpleMath::Vector3 prevPos = m_heli.q.position;
    m_prevPos = m_heli.q.position;
    m_prevRight = m_heli.right;
    m_prevUp = m_heli.up;

    UpdateCyclicStick(m_heli.controlInput);
    UpdateRotorForce();

    m_heli.isVehicleLanding = false;
    m_heli.terrainHightAtPos = m_environment->GetTerrainHeightAtPos(m_heli.q.position);
    
    DebugPushUILineDecimalNumber("Altitude : ", m_heli.q.position.y - m_heli.terrainHightAtPos, "");
    
    
    //if (m_heli.q.position.y - m_heli.terrainHightAtPos > 0.1)
    if (m_heli.landingGearPos.y - m_heli.terrainHightAtPos > 0.1)
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
        m_heli.q.position.y = m_heli.terrainHightAtPos - m_heli.localLandingGearPos.y;
        int testBrea = 0;
    }
    
    UpdateTerrainNorm();   
    
    RungeKutta4(&m_heli, aTimeDelta);

    UpdateAlignmentTorqueTest();

    UpdateRotorData(m_heli, aTimeDelta);

    if (m_heli.forward.Dot(m_heli.q.velocity) < 0.0)
    {
        m_heli.isVelocityBackwards = true;
    }
    else
    {
        m_heli.isVelocityBackwards = false;
    }

    m_heli.speed = m_heli.q.velocity.Length();
    UpdateModel();

    InputDecay(aTimeDelta);

    m_heli.testAccel = (m_heli.q.velocity.Length() - prevVelocity.Length()) / static_cast<float>(aTimeDelta);
    
    UpdateResistance();

    m_heli.q.bodyTorqueForce.axis = DirectX::SimpleMath::Vector3::Zero;
    m_heli.q.bodyTorqueForce.magnitude = 0.0f;

    m_rotorTimerTest += aTimeDelta;

    DebugPushTestLine(m_testPos, DirectX::SimpleMath::Vector3::UnitX, 3.0f, 0.0f, DirectX::SimpleMath::Vector4(0.0f, 0.0f, 1.0f, 1.0f));
    DebugPushTestLine(m_testPos, DirectX::SimpleMath::Vector3::UnitY, 3.0f, 0.0f, DirectX::SimpleMath::Vector4(0.0f, 0.0f, 1.0f, 1.0f));
    DebugPushTestLine(m_testPos, DirectX::SimpleMath::Vector3::UnitZ, 3.0f, 0.0f, DirectX::SimpleMath::Vector4(0.0f, 0.0f, 1.0f, 1.0f));
    DebugPushTestLine(m_testPos, -DirectX::SimpleMath::Vector3::UnitX, 3.0f, 0.0f, DirectX::SimpleMath::Vector4(0.0f, 0.0f, 1.0f, 1.0f));
    DebugPushTestLine(m_testPos, -DirectX::SimpleMath::Vector3::UnitY, 3.0f, 0.0f, DirectX::SimpleMath::Vector4(0.0f, 0.0f, 1.0f, 1.0f));
    DebugPushTestLine(m_testPos, -DirectX::SimpleMath::Vector3::UnitZ, 3.0f, 0.0f, DirectX::SimpleMath::Vector4(0.0f, 0.0f, 1.0f, 1.0f));

    DebugPushTestLine(m_testPos2, DirectX::SimpleMath::Vector3::UnitX, 3.0f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 0.0f, 0.0f, 1.0f));
    DebugPushTestLine(m_testPos2, DirectX::SimpleMath::Vector3::UnitY, 3.0f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 0.0f, 0.0f, 1.0f));
    DebugPushTestLine(m_testPos2, DirectX::SimpleMath::Vector3::UnitZ, 3.0f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 0.0f, 0.0f, 1.0f));
    DebugPushTestLine(m_testPos2, -DirectX::SimpleMath::Vector3::UnitX, 3.0f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 0.0f, 0.0f, 1.0f));
    DebugPushTestLine(m_testPos2, -DirectX::SimpleMath::Vector3::UnitY, 3.0f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 0.0f, 0.0f, 1.0f));
    DebugPushTestLine(m_testPos2, -DirectX::SimpleMath::Vector3::UnitZ, 3.0f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 0.0f, 0.0f, 1.0f));

    DebugPushTestLine(m_testPos3, DirectX::SimpleMath::Vector3::UnitX, 3.0f, 0.0f, DirectX::SimpleMath::Vector4(0.0f, 1.0f, 0.0f, 1.0f));
    DebugPushTestLine(m_testPos3, DirectX::SimpleMath::Vector3::UnitY, 3.0f, 0.0f, DirectX::SimpleMath::Vector4(0.0f, 1.0f, 0.0f, 1.0f));
    DebugPushTestLine(m_testPos3, DirectX::SimpleMath::Vector3::UnitZ, 3.0f, 0.0f, DirectX::SimpleMath::Vector4(0.0f, 1.0f, 0.0f, 1.0f));
    DebugPushTestLine(m_testPos3, -DirectX::SimpleMath::Vector3::UnitX, 3.0f, 0.0f, DirectX::SimpleMath::Vector4(0.0f, 1.0f, 0.0f, 1.0f));
    DebugPushTestLine(m_testPos3, -DirectX::SimpleMath::Vector3::UnitY, 3.0f, 0.0f, DirectX::SimpleMath::Vector4(0.0f, 1.0f, 0.0f, 1.0f));
    DebugPushTestLine(m_testPos3, -DirectX::SimpleMath::Vector3::UnitZ, 3.0f, 0.0f, DirectX::SimpleMath::Vector4(0.0f, 1.0f, 0.0f, 1.0f));

    //m_heli.landingGear.angleCurrent = cos(m_rotorTimerTest) - Utility::ToRadians(45.0f);
    DebugPushUILineDecimalNumber("m_heli.landingGear.angleCurrent : ", Utility::ToDegrees(m_heli.landingGear.angleCurrent), "");

    if (m_heli.landingGear.currentState == LandingGear::LandingGearState::LANDINGGEARSTATE_ASCENDING || m_heli.landingGear.currentState == LandingGear::LandingGearState::LANDINGGEARSTATE_DESCENDING)
    {
        UpdateLandingGear(m_heli.landingGear, aTimeDelta);
    }

}

void Vehicle::DebugPushTestLine(DirectX::SimpleMath::Vector3 aLineBase, DirectX::SimpleMath::Vector3 aLineEnd, float aLength, float aYOffset, DirectX::SimpleMath::Vector4 aColor)
{
    DirectX::SimpleMath::Vector3 scaledLineBase = aLineBase;
    scaledLineBase.y += aYOffset;
    DirectX::SimpleMath::Vector3 scaledLineEnd = aLineEnd;
    scaledLineEnd.Normalize();
    scaledLineEnd *= aLength;
    scaledLineEnd += scaledLineBase;
    std::tuple<DirectX::SimpleMath::Vector3, DirectX::SimpleMath::Vector3, DirectX::SimpleMath::Vector4> lineTup(scaledLineBase, scaledLineEnd, aColor);
    m_debugLinesVec.push_back(lineTup);
}

void Vehicle::DebugPushTestLineBetweenPoints(DirectX::SimpleMath::Vector3 aPoint1, DirectX::SimpleMath::Vector3 aPoint2, DirectX::SimpleMath::Vector4 aColor)
{
    std::tuple<DirectX::SimpleMath::Vector3, DirectX::SimpleMath::Vector3, DirectX::SimpleMath::Vector4> lineTup(aPoint1, aPoint2, aColor);
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

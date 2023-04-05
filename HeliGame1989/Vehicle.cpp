#include "pch.h"
#include "Vehicle.h"


DirectX::SimpleMath::Vector3 Vehicle::CalculateDragAngularLocal(const DirectX::SimpleMath::Vector3 aAngVelocity)
{
    DirectX::SimpleMath::Vector3 angVelocityNorm = aAngVelocity;
    angVelocityNorm.Normalize();
    DirectX::SimpleMath::Vector3 localAngVelocityNorm = angVelocityNorm;
    //localAngVelocityNorm = DirectX::SimpleMath::Vector3::Transform(localAngVelocityNorm, m_heli.alignmentInverse);

    const float length = m_angDragLength;
    const float width = m_angDragWidth;
    const float height = m_angDragHeight;
    float frontSurfaceArea = width * height;
    float sideSurfaceArea = length * height;
    float topSurfaceArea = length * width;

    float yawDot = localAngVelocityNorm.Dot(DirectX::SimpleMath::Vector3::UnitY);
    float pitchDot = localAngVelocityNorm.Dot(DirectX::SimpleMath::Vector3::UnitZ);
    float rollDot = localAngVelocityNorm.Dot(DirectX::SimpleMath::Vector3::UnitX);

    float yawSurface = (width * abs(yawDot)) + (height * abs(yawDot));
    float pitchSurface = (length * abs(pitchDot)) + (width * abs(pitchDot));
    //float rollSurface = (length * abs(rollDot)) + (height * abs(rollDot));
    float rollSurface = (length * abs(rollDot)) + (width * abs(rollDot));

    float yawRadius = (length * abs(yawDot)) * 0.5f;
    float pitchRadius = (length * abs(pitchDot)) * 0.5f;
    float rollRadius = (width * abs(rollDot)) * 0.5f;

    float airSurfaceArea = yawSurface + pitchSurface + rollSurface;
    float radiusSum = yawRadius + pitchRadius + rollRadius;

    //m_debugData->DebugPushUILineDecimalNumber("airSurfaceArea    = ", airSurfaceArea, "");
    //m_debugData->DebugPushUILineDecimalNumber("radiusSum         = ", radiusSum, "");

    const float angVelocityF = aAngVelocity.Length();
    const float angDragCoefficient = m_angDragCoefficient;
    const float angAirDensity = m_environment->GetAirDensity();
    const float angFrontSurfaceArea = airSurfaceArea;
    const float radius = radiusSum;


    DirectX::SimpleMath::Vector3 angularDrag = angVelocityNorm * (-((0.5f) * (angDragCoefficient * (radius * radius * radius)) * ((angVelocityF * angVelocityF) * angFrontSurfaceArea * angAirDensity)));

    //m_debugData->DebugPushUILineDecimalNumber("angularDrag.L = ", angularDrag.Length(), "");
    return angularDrag;
}

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
    //const float ClMax = 1.7;
    const float ClMax = 1.8;
    //const float inflectionPoint = 0.75f;
    const float inflectionPoint = 0.99f;
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

DirectX::SimpleMath::Vector3 Vehicle::CalculateStabilityTorqueLocal(const HeliData& aHeliData, const float aTimeStep)
{
    m_debugData->DebugClearUI();
    DirectX::SimpleMath::Vector3 testArm = DirectX::SimpleMath::Vector3::UnitY;
    DirectX::SimpleMath::Vector3 testForce = DirectX::SimpleMath::Vector3::UnitZ;
    Utility::Torque testTorque = Utility::GetTorqueForce(DirectX::SimpleMath::Vector3::UnitY, DirectX::SimpleMath::Vector3::UnitZ);

    DirectX::SimpleMath::Vector3 testTorqueVec = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 position = DirectX::SimpleMath::Vector3(0.3, 0.0, 0.3);
    DirectX::SimpleMath::Vector3 point = DirectX::SimpleMath::Vector3(0.0, 1.0, 0.0);
    DirectX::SimpleMath::Vector3 pt = point;
    pt -= position;

    DirectX::SimpleMath::Vector3 torqueAccum = DirectX::SimpleMath::Vector3::Zero;
    testTorqueVec = pt;
    testTorqueVec = testTorqueVec.Cross(testForce);
    //% testForce;

    torqueAccum += testTorqueVec;
    //DirectX::SimpleMath::Quaternion testQuat = aHeliData.q.angularQuatUpdateTest;
    DirectX::SimpleMath::Quaternion testQuat = aHeliData.q.angularQuat;
    /*
    m_debugData->DebugPushUILineDecimalNumber("testQuat.x ", testQuat.x, "");
    m_debugData->DebugPushUILineDecimalNumber("testQuat.y ", testQuat.y, "");
    m_debugData->DebugPushUILineDecimalNumber("testQuat.z ", testQuat.z, "");
    m_debugData->DebugPushUILineDecimalNumber("testQuat.w ", testQuat.w, "");
    */
    //aHeliData.localGravityTorqueArmPos == (0.0. 1.0, 0.0)
    DirectX::SimpleMath::Vector3 cogArm = (aHeliData.localCenterOfMass - aHeliData.localGravityTorqueArmPos) * 3.0f;
    DirectX::SimpleMath::Vector3 cogForce = aHeliData.gravity;
    cogForce = DirectX::SimpleMath::Vector3::Transform(cogForce, aHeliData.alignment);
    cogForce *= m_gravTorqueModTest;
    cogForce = (aHeliData.gravity * m_gravTorqueModTest) * (1.0f * abs(testQuat.w));
    DirectX::SimpleMath::Vector3 cogForceWorld = cogForce;
    //cogForceWorld = DirectX::SimpleMath::Vector3::Transform(cogForceWorld, aHeliData.alignmentInverse);
    cogForceWorld = DirectX::SimpleMath::Vector3::Transform(cogForceWorld, aHeliData.inverseAlignment);

    DirectX::SimpleMath::Vector3 cogTorqueArmWorld = cogArm;
    cogTorqueArmWorld = DirectX::SimpleMath::Vector3::Transform(cogTorqueArmWorld, aHeliData.alignment);
    Utility::Torque cogTorque = Utility::GetTorqueForce(cogTorqueArmWorld, cogForce);


    DirectX::SimpleMath::Vector3 armHinge = cogTorqueArmWorld;
    armHinge *= 1.0f;
    armHinge += aHeliData.q.position;
    //m_debugData->PushDebugLineScaled(armHinge, cogForce, 0.0f, 0.03f, 0.0f, DirectX::Colors::Lavender);
    //m_debugData->PushDebugLine(aHeliData.q.position, cogTorqueArmWorld, 10.0f, 0.0f, DirectX::Colors::Red);
    //m_debugData->PushDebugLineScaled(aHeliData.q.position, cogTorqueArmWorld, 0.0f, 1.0f, 0.0f, DirectX::Colors::Red);

    DirectX::SimpleMath::Vector3 cogArm2 = (DirectX::SimpleMath::Vector3::UnitY * 3.0f) * (1.1f - abs(testQuat.w));
    //m_debugData->DebugPushUILineDecimalNumber("cogArm2.Length() ", cogArm2.Length(), "");
    DirectX::SimpleMath::Vector3 cogForce2 = aHeliData.gravity;
    cogForce2 = DirectX::SimpleMath::Vector3::Transform(cogForce2, aHeliData.alignment);
    cogForce2 *= m_gravTorqueModTest;
    cogForce2 = -aHeliData.gravity * m_gravTorqueModTest;
    DirectX::SimpleMath::Vector3 cogForceWorld2 = cogForce2;
    //cogForceWorld2 = DirectX::SimpleMath::Vector3::Transform(cogForceWorld2, aHeliData.alignmentInverse);
    cogForceWorld2 = DirectX::SimpleMath::Vector3::Transform(cogForceWorld2, aHeliData.inverseAlignment);
    DirectX::SimpleMath::Vector3 cogTorqueArmWorld2 = cogArm2;
    //cogTorqueArmWorld2 = DirectX::SimpleMath::Vector3::Transform(cogTorqueArmWorld2, aHeliData.alignment);
    //cogTorqueArmWorld2 = DirectX::SimpleMath::Vector3::Transform(cogTorqueArmWorld2, aHeliData.alignmentInverse);
    cogTorqueArmWorld2 = DirectX::SimpleMath::Vector3::Transform(cogTorqueArmWorld2, aHeliData.inverseAlignment);
    Utility::Torque cogTorque2 = Utility::GetTorqueForce(cogTorqueArmWorld2, cogForce2);
    DirectX::SimpleMath::Vector3 stabilityTorque2 = cogTorque2.axis * cogTorque2.magnitude;


    DirectX::SimpleMath::Vector3 armHinge2 = cogTorqueArmWorld2;
    armHinge2 *= 1.0f;
    armHinge2 += aHeliData.q.position;
    //m_debugData->PushDebugLine(aHeliData.q.position, cogTorqueArmWorld2, 10.0f, 0.0f, DirectX::Colors::Blue);
    //m_debugData->PushDebugLineScaled(aHeliData.q.position, cogTorqueArmWorld2, 0.0f, 1.0f, 0.0f, DirectX::Colors::Yellow);
    //m_debugData->PushDebugLineScaled(armHinge2, cogForce2, 0.0f, 0.03f, 0.0f, DirectX::Colors::Blue);

    DirectX::SimpleMath::Vector3 stabilityTorque = DirectX::SimpleMath::Vector3::Zero;
    stabilityTorque = cogTorque.axis * cogTorque.magnitude;
    /////////////////////////////////////////////////////////////////
    //DirectX::SimpleMath::Vector3 cogArm = (aHeliData.localCenterOfMass - aHeliData.localGravityTorqueArmPos) * 3.0f;
    DirectX::SimpleMath::Vector3 testStabilityTorque = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 testStabilityForce = DirectX::SimpleMath::Vector3::Zero;
    //rotorPos - centerMassPos;
    //Utility::AddForceAtPoint(mainRotorForce, rotorPos, centerMassPos, testStabilityForce, testStabilityTorque);
    Utility::AddForceAtPoint(cogForceWorld, (aHeliData.localGravityTorqueArmPos * 3.0f), aHeliData.localCenterOfMass, testStabilityForce, testStabilityTorque);

    DirectX::SimpleMath::Vector3 testStabilityTorque2 = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 testStabilityForce2 = DirectX::SimpleMath::Vector3::Zero;
    //rotorPos - centerMassPos;
    //Utility::AddForceAtPoint(mainRotorForce, rotorPos, centerMassPos, testStabilityForce, testStabilityTorque);
    Utility::AddForceAtPoint(cogForceWorld2, cogArm2, aHeliData.localCenterOfMass, testStabilityForce2, testStabilityTorque2);
    testStabilityTorque *= -1.0f;
    testStabilityTorque2 *= -1.0f;
    m_debugData->DebugPushUILineDecimalNumber("testStabilityTorque.Length()  ", testStabilityTorque.Length(), "");
    m_debugData->DebugPushUILineDecimalNumber("stabilityTorque.Length()i      ", stabilityTorque.Length(), "");
    m_debugData->DebugPushUILineDecimalNumber("testStabilityTorque2.Length() ", testStabilityTorque2.Length(), ""); 
    m_debugData->DebugPushUILineDecimalNumber("stabilityTorque2.Length()i     ", stabilityTorque2.Length(), "");


    m_debugData->PushDebugLine(m_heli.q.position, testStabilityTorque, 10.0f, 0.0f, DirectX::Colors::Red);
    m_debugData->PushDebugLine(m_heli.q.position, testStabilityTorque2, 10.0f, 0.5f, DirectX::Colors::Blue);
    m_debugData->PushDebugLine(m_heli.q.position, stabilityTorque, 10.0f, 1.0f, DirectX::Colors::Orange);
    m_debugData->PushDebugLine(m_heli.q.position, stabilityTorque2, 10.0f, 1.5f, DirectX::Colors::Purple);

    /////////////////////////////////////////////////////////////////
    //return stabilityTorque + stabilityTorque2;
    return testStabilityTorque + testStabilityTorque2;
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

void Vehicle::DebugToggle2()
{
    if (m_debugToggle2 == false)
    {
        m_debugToggle2 = true;
    }
    else
    {
        m_debugToggle2 = false;
    }
}

//void Vehicle::DrawModel(const DirectX::SimpleMath::Matrix aView, const DirectX::SimpleMath::Matrix aProj, std::shared_ptr<DirectX::NormalMapEffect> aEffect, Microsoft::WRL::ComPtr<ID3D11InputLayout> aInputLayout)
//void Vehicle::DrawModel(const DirectX::SimpleMath::Matrix aView, const DirectX::SimpleMath::Matrix aProj, std::shared_ptr<DirectX::BasicEffect> aEffect, Microsoft::WRL::ComPtr<ID3D11InputLayout> aInputLayout)
void Vehicle::DrawModel(const DirectX::SimpleMath::Matrix aView, const DirectX::SimpleMath::Matrix aProj)
{
    DirectX::SimpleMath::Matrix testMat = DirectX::SimpleMath::Matrix::Identity;
    //testMat = DirectX::SimpleMath::Matrix::CreateWorld(DirectX::SimpleMath::Vector3(0.0f, 10.0f, 5.0f), DirectX::SimpleMath::Vector3::UnitX, DirectX::SimpleMath::Vector3::UnitY);
    //aEffect->SetView(testMat);
    //aEffect->SetProjection(testMat);
    //aEffect->SetWorld(m_heliModel.bodyMatrix);
    //aEffect->SetMatrices
    //m_heliModel.bodyShape->
    m_heliModel.bodyShape->Draw(m_heliModel.bodyMatrix, aView, aProj, m_heliModel.bodyColor);
    //m_heliModel.bodyShape->Draw(aEffect.get(), aInputLayout.Get());

    m_heliModel.bodyCapShape->Draw(m_heliModel.bodyRearBellyMatrix, aView, aProj, m_heliModel.undersideColor);
    m_heliModel.bodyShape->Draw(m_heliModel.bodyBellyMatrix, aView, aProj, m_heliModel.undersideColor);
    m_heliModel.noseBodyShape->Draw(m_heliModel.noseBodyBellyMatrix, aView, aProj, m_heliModel.undersideColor);
    m_heliModel.noseConeShape->Draw(m_heliModel.noseConeBellyMatrix, aView, aProj, m_heliModel.undersideColor);
    m_heliModel.mainWingShape->Draw(m_heliModel.mainWingBellyMatrix, aView, aProj, m_heliModel.undersideColor);
    m_heliModel.mainWingTailEdgeShape->Draw(m_heliModel.mainWingTailEdgeBellyMatrix, aView, aProj, m_heliModel.undersideColor);
    m_heliModel.mainWingLeadingEdgeShape->Draw(m_heliModel.mainWingLeadingEdgeBellyMatrix, aView, aProj, m_heliModel.undersideColor);
    //m_heliModel.wingJetForwardHousingShape->Draw(m_heliModel.wingJetForwardHousingLeftBellyMatrix, aView, aProj, m_heliModel.undersideColor);
    m_heliModel.tailWingEdgeShape->Draw(m_heliModel.tailWingLeadingEdgeBellyMatrix, aView, aProj, m_heliModel.undersideColor);
    m_heliModel.tailWingEdgeShape->Draw(m_heliModel.tailWingTrailingEdgeBellyMatrix, aView, aProj, m_heliModel.undersideColor);
    m_heliModel.tailWingShape->Draw(m_heliModel.tailWingBellyMatrix, aView, aProj, m_heliModel.undersideColor);

   
    m_heliModel.doorShape->Draw(m_heliModel.doorMatrix, aView, aProj, m_heliModel.bodyColor);
    m_heliModel.mainWingShape->Draw(m_heliModel.mainWingMatrix, aView, aProj, m_heliModel.bodyColor);
    m_heliModel.mainWingLeadingEdgeShape->Draw(m_heliModel.mainWingLeadingEdgeMatrix, aView, aProj, m_heliModel.bodyColor);
    m_heliModel.mainWingTailEdgeShape->Draw(m_heliModel.mainWingTailEdgeMatrix, aView, aProj, m_heliModel.bodyColor);
    //m_heliModel.wingJetCenterShape->Draw(m_heliModel.wingJetCenterMatrix, aView, aProj, m_heliModel.bodyColor);
    m_heliModel.wingJetForwardHousingShape->Draw(m_heliModel.wingJetForwardHousingLeftMatrix, aView, aProj, m_heliModel.bodyColor);
    m_heliModel.wingJetForwardHousingShape->Draw(m_heliModel.wingJetForwardHousingRightMatrix, aView, aProj, m_heliModel.bodyColor);
    m_heliModel.wingJetForwardCowlShape->Draw(m_heliModel.wingJetForwardCowlLeftMatrix, aView, aProj, m_heliModel.bodyColor);
    m_heliModel.wingJetForwardCowlShape->Draw(m_heliModel.wingJetForwardCowlRightMatrix, aView, aProj, m_heliModel.bodyColor);
    m_heliModel.wingJetCouplingShape->Draw(m_heliModel.wingJetCouplingLeftMatrix, aView, aProj, m_heliModel.bodyColor);
    m_heliModel.wingJetCouplingShape->Draw(m_heliModel.wingJetCouplingRightMatrix, aView, aProj, m_heliModel.bodyColor);
    m_heliModel.wingJetRearHousingShape->Draw(m_heliModel.wingJetRearHousingRightMatrix, aView, aProj, m_heliModel.exhaustColor);
    m_heliModel.wingJetRearHousingShape->Draw(m_heliModel.wingJetRearHousingLeftMatrix, aView, aProj, m_heliModel.exhaustColor);

    m_heliModel.wingJetForwardIntakeShape->Draw(m_heliModel.wingJetForwardHousingLeftMatrix, aView, aProj, m_heliModel.shadowColor);
    m_heliModel.wingJetForwardIntakeShape->Draw(m_heliModel.wingJetForwardHousingRightMatrix, aView, aProj, m_heliModel.shadowColor);
    m_heliModel.wingJetExhaustShape->Draw(m_heliModel.wingJetRearHousingLeftMatrix, aView, aProj, m_heliModel.shadowColor);
    m_heliModel.wingJetExhaustShape->Draw(m_heliModel.wingJetRearHousingRightMatrix, aView, aProj, m_heliModel.shadowColor);

    m_heliModel.wingJetExhaustTipShape->Draw(m_heliModel.wingJetExhaustTipLeftMatrix, aView, aProj, m_heliModel.exhaustColor);
    m_heliModel.wingJetExhaustTipShape->Draw(m_heliModel.wingJetExhaustTipRightMatrix, aView, aProj, m_heliModel.exhaustColor);

    m_heliModel.sidePodShape->Draw(m_heliModel.sidePodMatrix, aView, aProj, m_heliModel.bodyColor);

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
    //m_heliModel.sideWindowEyeShape->Draw(m_heliModel.sideWindowEyeMatrix, aView, aProj, m_heliModel.undersideColor);
    m_heliModel.sideWindowLowerShape->Draw(m_heliModel.sideWindowLowerMatrix, aView, aProj, m_heliModel.toothColor);
    m_heliModel.sideWindowLowerShape->Draw(m_heliModel.sideWindowBottomMatrix, aView, aProj, m_heliModel.toothColor);

    m_heliModel.toothShape->Draw(m_heliModel.toothMatrix1, aView, aProj, m_heliModel.toothColor);
    m_heliModel.toothShape->Draw(m_heliModel.toothMatrix2, aView, aProj, m_heliModel.toothColor);
    m_heliModel.toothShape->Draw(m_heliModel.toothMatrix3, aView, aProj, m_heliModel.toothColor);
    m_heliModel.toothShape->Draw(m_heliModel.toothMatrix4, aView, aProj, m_heliModel.toothColor);
    m_heliModel.toothShape->Draw(m_heliModel.toothMatrix5, aView, aProj, m_heliModel.toothColor);
    m_heliModel.toothShape->Draw(m_heliModel.toothMatrix6, aView, aProj, m_heliModel.toothColor);
    m_heliModel.toothShape2->Draw(m_heliModel.toothMatrix7, aView, aProj, m_heliModel.toothColor);
    m_heliModel.toothShape2->Draw(m_heliModel.toothMatrix8, aView, aProj, m_heliModel.toothColor);
    m_heliModel.toothShape2->Draw(m_heliModel.toothMatrix9, aView, aProj, m_heliModel.toothColor);
    m_heliModel.toothShape2->Draw(m_heliModel.toothMatrix10, aView, aProj, m_heliModel.toothColor);
    m_heliModel.toothShape2->Draw(m_heliModel.toothMatrix11, aView, aProj, m_heliModel.toothColor);
    m_heliModel.toothShape2->Draw(m_heliModel.toothMatrix12, aView, aProj, m_heliModel.toothColor);

    //m_heliModel.toothBackdropShape->Draw(m_heliModel.toothBackdropLeftMatrix, aView, aProj, m_heliModel.testColor3);
    //m_heliModel.toothBackdropShape->Draw(m_heliModel.toothBackdropRightMatrix, aView, aProj, m_heliModel.testColor3);
    //m_heliModel.toothBackdropTestShape->Draw(m_heliModel.toothBackdropLeftMatrix, aView, aProj, m_heliModel.testColor3);

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
    //m_heliModel.engineHousingCornerShape->Draw(m_heliModel.engineHousingFrontRightMatrix, aView, aProj, m_heliModel.bodyColor);
    //m_heliModel.engineHousingCornerShape->Draw(m_heliModel.engineHousingFrontLeftMatrix, aView, aProj, m_heliModel.bodyColor);
    m_heliModel.engineHousingCornerShape->Draw(m_heliModel.engineHousingRearLeftMatrix, aView, aProj, m_heliModel.bodyColor);
    m_heliModel.engineHousingCornerShape->Draw(m_heliModel.engineHousingRearRightMatrix, aView, aProj, m_heliModel.bodyColor);

    m_heliModel.engineHousingCornerShape->Draw(m_heliModel.engineHousingIntakeLeftMatrix, aView, aProj, m_heliModel.shadowColor);
    m_heliModel.engineHousingCornerShape->Draw(m_heliModel.engineHousingIntakeRightMatrix, aView, aProj, m_heliModel.shadowColor);

    m_heliModel.engineHousingIntakeCowlShape->Draw(m_heliModel.engineHousingIntakeCowlLeftMatrix, aView, aProj, m_heliModel.bodyColor);
    m_heliModel.engineHousingIntakeCowlShape->Draw(m_heliModel.engineHousingIntakeCowlRightMatrix, aView, aProj, m_heliModel.bodyColor);

    m_heliModel.engineHousingFrontShape->Draw(m_heliModel.engineHousingFrontMatrix, aView, aProj, m_heliModel.bodyColor);
    m_heliModel.engineHousingFrontShape->Draw(m_heliModel.engineHousingRearMatrix, aView, aProj, m_heliModel.bodyColor);
    m_heliModel.engineHousingSideShape->Draw(m_heliModel.engineHousingSideLeftMatrix, aView, aProj, m_heliModel.bodyColor);
    m_heliModel.engineHousingSideShape->Draw(m_heliModel.engineHousingSideRightMatrix, aView, aProj, m_heliModel.bodyColor);

    m_heliModel.engineExhaustExteriorShape->Draw(m_heliModel.engineExhaustLeftMatrix, aView, aProj, m_heliModel.exhaustColor);
    m_heliModel.engineExhaustExteriorShape->Draw(m_heliModel.engineExhaustRightMatrix, aView, aProj, m_heliModel.exhaustColor);
    m_heliModel.engineExhaustInteriorShape->Draw(m_heliModel.engineExhaustLeftMatrix, aView, aProj, m_heliModel.shadowColor);
    m_heliModel.engineExhaustInteriorShape->Draw(m_heliModel.engineExhaustRightMatrix, aView, aProj, m_heliModel.shadowColor);
    m_heliModel.engineExhaustTipShape->Draw(m_heliModel.engineExhaustTipLeftMatrix, aView, aProj, m_heliModel.exhaustColor);
    m_heliModel.engineExhaustTipShape->Draw(m_heliModel.engineExhaustTipRightMatrix, aView, aProj, m_heliModel.exhaustColor);

    m_heliModel.tailBoomShape->Draw(m_heliModel.tailBoomMatrix, aView, aProj, m_heliModel.bodyColor);
    m_heliModel.tailBoomNewShape->Draw(m_heliModel.tailBoomLowerMatrix, aView, aProj, m_heliModel.undersideColor);
    m_heliModel.tailBoomNewShape->Draw(m_heliModel.tailBoomUpperMatrix, aView, aProj, m_heliModel.bodyColor);
    m_heliModel.tailBoomMidEndCapShape->Draw(m_heliModel.tailBoomMidEndCapMatrix, aView, aProj, m_heliModel.bodyColor);
    m_heliModel.tailBoomLowerUpperEndCapShape->Draw(m_heliModel.tailBoomLowerEndCapMatrix, aView, aProj, m_heliModel.undersideColor);
    m_heliModel.tailBoomLowerUpperEndCapShape->Draw(m_heliModel.tailBoomUpperEndCapMatrix, aView, aProj, m_heliModel.bodyColor);

    m_heliModel.tailFinUpperShape->Draw(m_heliModel.tailFinUpperMatrix, aView, aProj, m_heliModel.bodyColor);
    m_heliModel.tailFinUpperTipShape->Draw(m_heliModel.tailFinUpperTipMatrix, aView, aProj, m_heliModel.bodyColor);
    m_heliModel.tailFinLowerShape->Draw(m_heliModel.tailFinLowerMatrix, aView, aProj, m_heliModel.undersideColor);
    m_heliModel.tailFinLowerTipShape->Draw(m_heliModel.tailFinLowerTipMatrix, aView, aProj, m_heliModel.undersideColor);
    m_heliModel.tailFinLeadingEdgeShape->Draw(m_heliModel.tailFinLeadingEdgeLowerMatrix, aView, aProj, m_heliModel.undersideColor);
    m_heliModel.tailFinLeadingEdgeShape->Draw(m_heliModel.tailFinLeadingEdgeUpperMatrix, aView, aProj, m_heliModel.bodyColor);

    m_heliModel.tailWingShape->Draw(m_heliModel.tailWingMatrix, aView, aProj, m_heliModel.bodyColor);
    m_heliModel.tailWingEdgeShape->Draw(m_heliModel.tailWingLeadingEdgeMatrix, aView, aProj, m_heliModel.bodyColor);
    m_heliModel.tailWingEdgeShape->Draw(m_heliModel.tailWingTrailingEdgeMatrix, aView, aProj, m_heliModel.bodyColor);
    m_heliModel.tailWingFinShape->Draw(m_heliModel.tailWingFinLeftMatrix, aView, aProj, m_heliModel.bodyColor);
    m_heliModel.tailWingFinShape->Draw(m_heliModel.tailWingFinRightMatrix, aView, aProj, m_heliModel.bodyColor);

    m_heliModel.tailWingFinEdgeShape->Draw(m_heliModel.tailWingFinEdgeLeftMatrix, aView, aProj, m_heliModel.undersideColor);
    m_heliModel.tailWingFinEdgeShape->Draw(m_heliModel.tailWingFinEdgeRightMatrix, aView, aProj, m_heliModel.undersideColor);

    m_heliModel.mainRotorAxelShape->Draw(m_heliModel.mainRotorAxelMatrix, aView, aProj, m_heliModel.axelColor);
    m_heliModel.mainRotorHubShape->Draw(m_heliModel.mainRotorHubMatrix, aView, aProj, m_heliModel.axelColor);

    m_heliModel.mainRotorArmShape->Draw(m_heliModel.mainRotorArmMatrix, aView, aProj, m_heliModel.rotorColor);

    m_heliModel.mainRotorBladeShape->Draw(m_heliModel.mainRotorBladeMatrix1, aView, aProj, m_heliModel.rotorColor);
    m_heliModel.mainRotorBladeShape->Draw(m_heliModel.mainRotorBladeMatrix2, aView, aProj, m_heliModel.rotorColor);

    m_heliModel.mainRotorBladeInteriorShape->Draw(m_heliModel.mainRotorBladeInteriorMatrix1, aView, aProj, m_heliModel.rotorColor);
    m_heliModel.mainRotorBladeInteriorShape->Draw(m_heliModel.mainRotorBladeInteriorMatrix2, aView, aProj, m_heliModel.rotorColor);
    // stripe 1
    m_heliModel.mainRotorBladeStripe1Shape->Draw(m_heliModel.mainRotorBladeStripe1Matrix1, aView, aProj, m_heliModel.rotorStripeColor);
    m_heliModel.mainRotorBladeStripe1Shape->Draw(m_heliModel.mainRotorBladeStripe1Matrix2, aView, aProj, m_heliModel.rotorStripeColor);
    m_heliModel.mainRotorBladeEdgeStripe1Shape->Draw(m_heliModel.mainRotorBladeEdgeStripe1Matrix1, aView, aProj, m_heliModel.rotorStripeColor);
    m_heliModel.mainRotorBladeEdgeStripe1Shape->Draw(m_heliModel.mainRotorBladeEdgeStripe1Matrix2, aView, aProj, m_heliModel.rotorStripeColor);
    m_heliModel.mainRotorArmStripe1Shape->Draw(m_heliModel.mainRotorArmStripe1Matrix1, aView, aProj, m_heliModel.rotorStripeColor);
    m_heliModel.mainRotorArmStripe1Shape->Draw(m_heliModel.mainRotorArmStripe1Matrix2, aView, aProj, m_heliModel.rotorStripeColor);

    // stripe 2
    m_heliModel.mainRotorArmStripe2Shape->Draw(m_heliModel.mainRotorArmStripe2Matrix1, aView, aProj, m_heliModel.rotorStripeAltColor);
    m_heliModel.mainRotorArmStripe2Shape->Draw(m_heliModel.mainRotorArmStripe2Matrix2, aView, aProj, m_heliModel.rotorStripeAltColor);
    m_heliModel.mainRotorBladeEdgeStripe2Shape->Draw(m_heliModel.mainRotorEdgeStripe2Matrix1, aView, aProj, m_heliModel.rotorStripeAltColor);
    m_heliModel.mainRotorBladeEdgeStripe2Shape->Draw(m_heliModel.mainRotorEdgeStripe2Matrix2, aView, aProj, m_heliModel.rotorStripeAltColor);
    m_heliModel.mainRotorBladeStripe2Shape->Draw(m_heliModel.mainRotorBladeStripe2Matrix1, aView, aProj, m_heliModel.rotorStripeAltColor);
    m_heliModel.mainRotorBladeStripe2Shape->Draw(m_heliModel.mainRotorBladeStripe2Matrix2, aView, aProj, m_heliModel.rotorStripeAltColor);

    m_heliModel.mainRotorBladeEdgeShape->Draw(m_heliModel.mainRotorBladeEdgeMatrix1, aView, aProj, m_heliModel.rotorColor);
    m_heliModel.mainRotorBladeEdgeShape->Draw(m_heliModel.mainRotorBladeEdgeMatrix2, aView, aProj, m_heliModel.rotorColor);

    m_heliModel.mainRotorArmEndCapShape->Draw(m_heliModel.mainRotorArmEndCapMatrix1, aView, aProj, m_heliModel.rotorStripeColor);
    m_heliModel.mainRotorArmEndCapShape->Draw(m_heliModel.mainRotorArmEndCapMatrix2, aView, aProj, m_heliModel.rotorStripeColor);
    m_heliModel.mainRotorBladeEndCapShape->Draw(m_heliModel.mainRotorBladeEndCapMatrix1, aView, aProj, m_heliModel.rotorStripeColor);
    m_heliModel.mainRotorBladeEndCapShape->Draw(m_heliModel.mainRotorBladeEndCapMatrix2, aView, aProj, m_heliModel.rotorStripeColor);
    m_heliModel.mainRotorBladeEdgeEndCapShape->Draw(m_heliModel.mainRotorBladeEdgeEndCapMatrix1, aView, aProj, m_heliModel.rotorStripeColor);
    m_heliModel.mainRotorBladeEdgeEndCapShape->Draw(m_heliModel.mainRotorBladeEdgeEndCapMatrix2, aView, aProj, m_heliModel.rotorStripeColor);

    m_heliModel.tailRotorAxelShape->Draw(m_heliModel.tailRotorAxelMatrix, aView, aProj, m_heliModel.axelColor);
    m_heliModel.tailRotorHubShape->Draw(m_heliModel.tailRotorHubMatrix, aView, aProj, m_heliModel.axelColor);
    m_heliModel.tailRotorArmShape->Draw(m_heliModel.tailRotorArmMatrix, aView, aProj, m_heliModel.rotorColor);
    m_heliModel.tailRotorBladeShape->Draw(m_heliModel.tailRotorBladeMatrix1, aView, aProj, m_heliModel.rotorColor);
    m_heliModel.tailRotorBladeShape->Draw(m_heliModel.tailRotorBladeMatrix2, aView, aProj, m_heliModel.rotorColor);
    m_heliModel.tailRotorBladeEdgeShape->Draw(m_heliModel.tailRotorBladeEdgeMatrix1, aView, aProj, m_heliModel.rotorColor);
    m_heliModel.tailRotorBladeEdgeShape->Draw(m_heliModel.tailRotorBladeEdgeMatrix2, aView, aProj, m_heliModel.rotorColor);

    m_heliModel.tailRotorBladeEndCapShape->Draw(m_heliModel.tailRotorBladeEndCapMatrix1, aView, aProj, m_heliModel.rotorStripeColor);
    m_heliModel.tailRotorBladeEndCapShape->Draw(m_heliModel.tailRotorBladeEndCapMatrix2, aView, aProj, m_heliModel.rotorStripeColor);

    m_heliModel.tailRotorArmEndCapShape->Draw(m_heliModel.tailRotorArmEndCapMatrix1, aView, aProj, m_heliModel.rotorStripeColor);
    m_heliModel.tailRotorArmEndCapShape->Draw(m_heliModel.tailRotorArmEndCapMatrix2, aView, aProj, m_heliModel.rotorStripeColor);

    m_heliModel.tailRotorBladeEdgeEndCapShape->Draw(m_heliModel.tailRotorBladeEdgeEndCapMatrix2, aView, aProj, m_heliModel.rotorStripeColor);
    m_heliModel.tailRotorBladeEdgeEndCapShape->Draw(m_heliModel.tailRotorBladeEdgeEndCapMatrix1, aView, aProj, m_heliModel.rotorStripeColor);

    m_heliModel.tailRotorBladeInteriorShape->Draw(m_heliModel.tailRotorBladeInteriorMatrix1, aView, aProj, m_heliModel.rotorColor);
    m_heliModel.tailRotorBladeInteriorShape->Draw(m_heliModel.tailRotorBladeInteriorMatrix2, aView, aProj, m_heliModel.rotorColor);

    //  tail rotor arm stripe 1
    m_heliModel.tailRotorArmStripe1Shape->Draw(m_heliModel.tailRotorArmStripe1Matrix1, aView, aProj, m_heliModel.rotorStripeColor);
    m_heliModel.tailRotorArmStripe1Shape->Draw(m_heliModel.tailRotorArmStripe1Matrix2, aView, aProj, m_heliModel.rotorStripeColor);
    m_heliModel.tailRotorBladeStripe1Shape->Draw(m_heliModel.tailRotorBladeStripe1Matrix1, aView, aProj, m_heliModel.rotorStripeColor);
    m_heliModel.tailRotorBladeStripe1Shape->Draw(m_heliModel.tailRotorBladeStripe1Matrix2, aView, aProj, m_heliModel.rotorStripeColor);

    //  tail rotor edge stripe 1
    m_heliModel.tailRotorEdgeStripe1Shape->Draw(m_heliModel.tailRotorEdgeStripe1Matrix1, aView, aProj, m_heliModel.rotorStripeColor);
    m_heliModel.tailRotorEdgeStripe1Shape->Draw(m_heliModel.tailRotorEdgeStripe1Matrix2, aView, aProj, m_heliModel.rotorStripeColor);

    //  tail rotor arm stripe 2
    m_heliModel.tailRotorArmStripe2Shape->Draw(m_heliModel.tailRotorArmStripe2Matrix1, aView, aProj, m_heliModel.rotorStripeAltColor);
    m_heliModel.tailRotorArmStripe2Shape->Draw(m_heliModel.tailRotorArmStripe2Matrix2, aView, aProj, m_heliModel.rotorStripeAltColor);

    //  tail rotor blade stripe 2
    m_heliModel.tailRotorBladeStripe2Shape->Draw(m_heliModel.tailRotorBladeStripe2Matrix1, aView, aProj, m_heliModel.rotorStripeAltColor);
    m_heliModel.tailRotorBladeStripe2Shape->Draw(m_heliModel.tailRotorBladeStripe2Matrix2, aView, aProj, m_heliModel.rotorStripeAltColor);

    //  tail rotor edge stripe 2
    m_heliModel.tailRotorEdgeStripe2Shape->Draw(m_heliModel.tailRotorEdgeStripe2Matrix1, aView, aProj, m_heliModel.rotorStripeAltColor);
    m_heliModel.tailRotorEdgeStripe2Shape->Draw(m_heliModel.tailRotorEdgeStripe2Matrix2, aView, aProj, m_heliModel.rotorStripeAltColor);

    // draw shadows
    m_heliModel.bodyShape->Draw(m_heliModel.shadowBaseMat, aView, aProj, m_heliModel.shadowColor);

    m_heliModel.bodyCapShape->Draw(m_heliModel.shadowFrontNoseMat, aView, aProj, m_heliModel.shadowColor);
    m_heliModel.bodyCapShape->Draw(m_heliModel.shadowRearNoseMat, aView, aProj, m_heliModel.shadowColor);
    m_heliModel.mainWingShape->Draw(m_heliModel.shadowMainWingMat, aView, aProj, m_heliModel.shadowColor);

    m_heliModel.mainWingLeadingEdgeShape->Draw(m_heliModel.shadowMainWingFrontMat, aView, aProj, m_heliModel.shadowColor);
    m_heliModel.mainWingTailEdgeShape->Draw(m_heliModel.shadowMainWingRearMat, aView, aProj, m_heliModel.shadowColor);

    m_heliModel.tailBoomShape->Draw(m_heliModel.shadowBoomMat, aView, aProj, m_heliModel.shadowColor);
    m_heliModel.tailBoomMidEndCapShape->Draw(m_heliModel.shadowBoomEndCapMat, aView, aProj, m_heliModel.shadowColor); 
    m_heliModel.tailWingShape->Draw(m_heliModel.shadowTailWingMat, aView, aProj, m_heliModel.shadowColor);

    m_heliModel.tailFinLowerShape->Draw(m_heliModel.shadowTailFinLowerMat, aView, aProj, m_heliModel.shadowColor);
    m_heliModel.tailFinUpperShape->Draw(m_heliModel.shadowTailFinUpperMat, aView, aProj, m_heliModel.shadowColor);

    m_heliModel.engineHousingShape->Draw(m_heliModel.shadowHouseingMainMat, aView, aProj, m_heliModel.shadowColor);
    m_heliModel.engineHousingFrontShape->Draw(m_heliModel.shadowHouseingFrontMat, aView, aProj, m_heliModel.shadowColor);
    m_heliModel.engineHousingFrontShape->Draw(m_heliModel.shadowHouseingRearMat, aView, aProj, m_heliModel.shadowColor);

    m_heliModel.tailWingFinShape->Draw(m_heliModel.shadowRearWingFinLeftMat, aView, aProj, m_heliModel.shadowColor);
    m_heliModel.tailWingFinShape->Draw(m_heliModel.shadowRearWingFinRightMat, aView, aProj, m_heliModel.shadowColor);

    m_heliModel.mainRotorAxelShape->Draw(m_heliModel.shadowMainRotorAxelMat, aView, aProj, m_heliModel.shadowColor);
    m_heliModel.mainRotorHubShape->Draw(m_heliModel.shadowMainRotorHubMat, aView, aProj, m_heliModel.shadowColor);
    m_heliModel.tailRotorAxelShape->Draw(m_heliModel.shadowTailRotorAxelMat, aView, aProj, m_heliModel.shadowColor);
    m_heliModel.tailRotorHubShape->Draw(m_heliModel.shadowTailRotorHubMat, aView, aProj, m_heliModel.shadowColor);
    
    m_heliModel.mainRotorBladeShape->Draw(m_heliModel.shadowMainRotorMat1, aView, aProj, m_heliModel.shadowColor);
    m_heliModel.mainRotorBladeShape->Draw(m_heliModel.shadowMainRotorMat2, aView, aProj, m_heliModel.shadowColor);

    m_heliModel.tailRotorBladeShape->Draw(m_heliModel.shadowTailRotorMat1, aView, aProj, m_heliModel.shadowColor);
    m_heliModel.tailRotorBladeShape->Draw(m_heliModel.shadowTailRotorMat2, aView, aProj, m_heliModel.shadowColor);
    
    m_heliModel.mainRotorArmShape->Draw(m_heliModel.shadowMainRotorArmMat, aView, aProj, m_heliModel.shadowColor);
    m_heliModel.tailRotorArmShape->Draw(m_heliModel.shadowTailRotorArmMat, aView, aProj, m_heliModel.shadowColor);

    m_heliModel.noseConeShape->Draw(m_heliModel.shadowNoseMat, aView, aProj, m_heliModel.shadowColor);
    m_heliModel.noseBodyShape->Draw(m_heliModel.shadowNoseBodyMat, aView, aProj, m_heliModel.shadowColor);

    m_heliModel.engineHousingSideShape->Draw(m_heliModel.shadowHouseingLeftMat, aView, aProj, m_heliModel.shadowColor);
    m_heliModel.engineHousingSideShape->Draw(m_heliModel.shadowHouseingRightMat, aView, aProj, m_heliModel.shadowColor);
    m_heliModel.tailBoomNewShape->Draw(m_heliModel.shadowBoomBottomMat, aView, aProj, m_heliModel.shadowColor);
    m_heliModel.tailBoomNewShape->Draw(m_heliModel.shadowBoomTopMat, aView, aProj, m_heliModel.shadowColor);
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
    DirectX::SimpleMath::Vector4 purple1 = DirectX::SimpleMath::Vector4(0.1411764705882353, 0.0901960784313725, 0.4509803921568627, 1.0f);
    DirectX::SimpleMath::Vector4 purple2 = DirectX::SimpleMath::Vector4(0.1019607843137255, 0.0980392156862745, 0.3725490196078431, 1.0f);
    DirectX::SimpleMath::Vector4 gold1 = DirectX::SimpleMath::Vector4(0.7333333333333333, 0.5764705882352941, 0.0784313725490196, 1.0f);
    DirectX::SimpleMath::Vector4 orange1 = DirectX::SimpleMath::Vector4(0.8745098039215686, 0.2745098039215686, 0.003921568627451, 1.0f);
    DirectX::SimpleMath::Vector4 badRed = DirectX::SimpleMath::Vector4(0.501960814f, 0.0f, 0.0f, 1.0f);
    DirectX::SimpleMath::Vector4 marsRed = DirectX::SimpleMath::Vector4(0.6117647058823529, 0.1803921568627451, 0.207843137254902, 1.0f);

    // set model part colors
    m_heliModel.axelColor = DirectX::SimpleMath::Vector4(0.411764741f, 0.411764741f, 0.411764741f, 1.0f);
    m_heliModel.exhaustColor = DirectX::SimpleMath::Vector4(0.8, 0.8f, 0.8f, 1.0f);
    m_heliModel.landingGearArmColor = DirectX::SimpleMath::Vector4(0.9f, 0.9f, 0.9f, 1.0f);
    m_heliModel.landingGearTireColor = DirectX::SimpleMath::Vector4(0.411764741f, 0.411764741f, 0.411764741f, 1.0f);
    m_heliModel.landingGearWheelColor = DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f);
    m_heliModel.rotorColor = DirectX::SimpleMath::Vector4(0.827451050f, 0.827451050f, 0.827451050f, 1.0f);
    m_heliModel.toothColor = DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f);
    m_heliModel.rotorStripeColor = orange1;
    m_heliModel.rotorStripeAltColor = purple1;
    
    m_heliModel.bodyColor = purple1;
    m_heliModel.undersideColor = orange1;

    m_heliModel.shadowColor = DirectX::SimpleMath::Vector4(0.0, 0.0f, 0.0f, 1.0f);
    m_heliModel.stripeColor0 = DirectX::SimpleMath::Vector4(0.8, 0.8f, 0.8f, 1.0f);
    m_heliModel.stripeColor1 = DirectX::SimpleMath::Vector4(0.0, 0.0f, 0.0f, 1.0f);
    m_heliModel.stripeColor1 = orange1;

    m_heliModel.windshieldColor = DirectX::SimpleMath::Vector4(0.662745118f, 0.662745118f, 0.662745118f, 1.20f);
    m_heliModel.testColor1 = DirectX::SimpleMath::Vector4(1.0, 0.0f, 0.0f, 1.0f);
    m_heliModel.testColor2 = badRed;
    m_heliModel.testColor3 = DirectX::Colors::Goldenrod;
    m_heliModel.testColor3.w = 2.0f;
    m_heliModel.testColor3 = DirectX::SimpleMath::Vector4(.8314, .6863, .2157, 1.2f);

    const float zFightOffset = 0.0007f;

    //const float bellyVertOffset = 0.017f;
    const float bellyVertOffset = 0.017f;
    const DirectX::SimpleMath::Vector3 bellyScale(0.85f, 0.85f, 0.85f);
    const DirectX::SimpleMath::Vector3 bellyWingScale(1.0f, 1.0f, 0.95f);
    const DirectX::SimpleMath::Vector3 bellyTranslation(0.0f, -bellyVertOffset, 0.0f);

    // set model shapes and local positions   
    // main body
    const DirectX::SimpleMath::Vector3 bodySize(5.0f, 3.0f, 3.0f);
    const DirectX::SimpleMath::Vector3 bodyTranslation(0.0f, bodySize.y * 0.5f, 0.0f);
    m_heliModel.bodyShape = DirectX::GeometricPrimitive::CreateBox(aContext.Get(), bodySize);
    m_heliModel.bodyMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.bodyMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(bodyTranslation);
    m_heliModel.localBodyMatrix = m_heliModel.bodyMatrix;


    // main body belly
    m_heliModel.bodyBellyMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.bodyBellyMatrix *= DirectX::SimpleMath::Matrix::CreateScale(DirectX::SimpleMath::Vector3(1.0f, 1.0f, bellyScale.z));
    m_heliModel.bodyBellyMatrix *= m_heliModel.bodyMatrix;
    m_heliModel.bodyBellyMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(bellyTranslation);
    m_heliModel.localBodyBellyMatrix = m_heliModel.bodyBellyMatrix;

    // main wing
    const DirectX::SimpleMath::Vector3 mainWingSize(1.5f, 0.9f, 6.8f);
    const DirectX::SimpleMath::Vector3 mainWingTranslation(-mainWingSize.x * 0.65f, mainWingSize.y * 0.7f, 0.0f);
    m_heliModel.mainWingShape = DirectX::GeometricPrimitive::CreateBox(aContext.Get(), mainWingSize);
    m_heliModel.mainWingMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.mainWingMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(mainWingTranslation);
    m_heliModel.localMainWingMatrix = m_heliModel.mainWingMatrix;

    // main wing belly
    m_heliModel.mainWingBellyMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.mainWingBellyMatrix *= DirectX::SimpleMath::Matrix::CreateScale(DirectX::SimpleMath::Vector3(1.0f, 1.0f, bellyWingScale.z));
    m_heliModel.mainWingBellyMatrix *= m_heliModel.mainWingMatrix;
    m_heliModel.mainWingBellyMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(bellyTranslation);
    m_heliModel.localMainWingBellyMatrix = m_heliModel.mainWingBellyMatrix;

    // main wing leading edge   
    DirectX::SimpleMath::Vector3 mainWingLeadingEdgeSize((mainWingSize.y * 0.5f) * sqrt(2.0f), mainWingSize.z, (mainWingSize.y * 0.5f) * sqrt(2.0f));
    m_heliModel.mainWingLeadingEdgeShape = DirectX::GeometricPrimitive::CreateBox(aContext.Get(), mainWingLeadingEdgeSize);
    DirectX::SimpleMath::Vector3 mainWingLeadingEdgeTranslation(mainWingTranslation.x + mainWingSize.x * 0.5f, mainWingTranslation.y, mainWingTranslation.z);
    m_heliModel.mainWingLeadingEdgeMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.mainWingLeadingEdgeMatrix = DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(45.0f));
    m_heliModel.mainWingLeadingEdgeMatrix *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(90.0f));
    m_heliModel.mainWingLeadingEdgeMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(mainWingLeadingEdgeTranslation);
    m_heliModel.localMainWingLeadingEdgeMatrix = m_heliModel.mainWingLeadingEdgeMatrix;

    // main wing leading belly
    m_heliModel.mainWingLeadingEdgeBellyMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.mainWingLeadingEdgeBellyMatrix *= DirectX::SimpleMath::Matrix::CreateScale(DirectX::SimpleMath::Vector3(1.0f, bellyWingScale.z, 1.0f));
    m_heliModel.mainWingLeadingEdgeBellyMatrix *= m_heliModel.mainWingLeadingEdgeMatrix;
    m_heliModel.mainWingLeadingEdgeBellyMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(bellyTranslation);
    m_heliModel.localMainWingLeadingBellyEdgeMatrix = m_heliModel.mainWingLeadingEdgeBellyMatrix;

    // main wing trail edge
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

    // main wing trail belly
    m_heliModel.mainWingTailEdgeBellyMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.mainWingTailEdgeBellyMatrix *= DirectX::SimpleMath::Matrix::CreateScale(DirectX::SimpleMath::Vector3(1.0f, bellyWingScale.z, 1.0f));
    m_heliModel.mainWingTailEdgeBellyMatrix *= m_heliModel.mainWingTailEdgeMatrix;
    m_heliModel.mainWingTailEdgeBellyMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(bellyTranslation);
    m_heliModel.localMainWingTailEdgeBellyMatrix = m_heliModel.mainWingTailEdgeBellyMatrix;

    // wing jet center
    const DirectX::SimpleMath::Vector3 wingJetCenterSize(mainWingSize.x * 1.0, mainWingSize.y * 1.5f, mainWingSize.z * 0.5f);
    m_heliModel.wingJetCenterShape = DirectX::GeometricPrimitive::CreateBox(aContext.Get(), wingJetCenterSize);
    const DirectX::SimpleMath::Vector3 wingJetCenterTranslation(mainWingTranslation.x + (mainWingLeadingEdgeSize.x * 0.75f), mainWingTranslation.y * 1.2f, mainWingTranslation.z);
    m_heliModel.wingJetCenterMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.wingJetCenterMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(wingJetCenterTranslation);
    m_heliModel.localWingJetCenterMatrix = m_heliModel.wingJetCenterMatrix;

    // wing jet forward housing left
    const float wingJetForwardHousingDiameter = wingJetCenterSize.y;
    const float wingJetForwardHousingHeight = wingJetCenterSize.x;
    m_heliModel.wingJetForwardHousingShape = DirectX::GeometricPrimitive::CreateCylinder(aContext.Get(), wingJetForwardHousingHeight, wingJetForwardHousingDiameter, m_heliModel.circleTessellationVal1);
    m_heliModel.wingJetForwardIntakeShape = DirectX::GeometricPrimitive::CreateCylinder(aContext.Get(), wingJetForwardHousingHeight + 0.01, wingJetForwardHousingDiameter * 0.9f, m_heliModel.circleTessellationVal1);
    const DirectX::SimpleMath::Vector3 wingJetForwardHousingLeftTranslation(wingJetCenterTranslation.x, wingJetCenterTranslation.y, -wingJetCenterSize.z * 0.5f);
    m_heliModel.wingJetForwardHousingLeftMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.wingJetForwardHousingLeftMatrix = DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(90.0f));
    m_heliModel.wingJetForwardHousingLeftMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(wingJetForwardHousingLeftTranslation);
    m_heliModel.localWingJetForwardHousingLeftMatrix = m_heliModel.wingJetForwardHousingLeftMatrix;

    // main wing forward jet left belly
    m_heliModel.wingJetForwardHousingLeftBellyMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.wingJetForwardHousingLeftBellyMatrix *= DirectX::SimpleMath::Matrix::CreateScale(DirectX::SimpleMath::Vector3(1.0f, 1.0f, 1.0f));
    m_heliModel.wingJetForwardHousingLeftBellyMatrix *= m_heliModel.wingJetForwardHousingLeftMatrix;
    m_heliModel.wingJetForwardHousingLeftBellyMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(bellyTranslation);
    m_heliModel.localWingJetForwardHousingLeftBellyMatrix = m_heliModel.wingJetForwardHousingLeftBellyMatrix;

    // wing jet forward housing right
    const DirectX::SimpleMath::Vector3 wingJetForwardHousingRightTranslation(wingJetCenterTranslation.x, wingJetCenterTranslation.y, wingJetCenterSize.z * 0.5f);
    m_heliModel.wingJetForwardHousingRightMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.wingJetForwardHousingRightMatrix = DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(90.0f));
    m_heliModel.wingJetForwardHousingRightMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(wingJetForwardHousingRightTranslation);
    m_heliModel.localWingJetForwardHousingRightMatrix = m_heliModel.wingJetForwardHousingRightMatrix;

    // wing jet forward cowl left  
    const float wingJetForwardCowlThickness = 0.3f;
    const float wingJetCowlScale = 4.0f;
    const float wingJetForwardCowlDiameter = wingJetForwardHousingDiameter - wingJetForwardCowlThickness;
    m_heliModel.wingJetForwardCowlShape = DirectX::GeometricPrimitive::CreateTorus(aContext.Get(), wingJetForwardCowlDiameter, wingJetForwardCowlThickness, m_heliModel.circleTessellationVal1);
    const DirectX::SimpleMath::Vector3 wingJetForwardCowlLeftTranslation(wingJetForwardHousingLeftTranslation.x + wingJetForwardHousingHeight * 0.5f, wingJetForwardHousingLeftTranslation.y, wingJetForwardHousingLeftTranslation.z);
    m_heliModel.wingJetForwardCowlLeftMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.wingJetForwardCowlLeftMatrix = DirectX::SimpleMath::Matrix::CreateScale(1.0f, wingJetCowlScale, 1.0f);
    m_heliModel.wingJetForwardCowlLeftMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(90.0f));
    m_heliModel.wingJetForwardCowlLeftMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(wingJetForwardCowlLeftTranslation);
    m_heliModel.localWingJetForwardCowlLeftMatrix = m_heliModel.wingJetForwardCowlLeftMatrix;

    // wing jet forward cowl right
    const DirectX::SimpleMath::Vector3 wingJetForwardCowlRightTranslation(wingJetForwardHousingRightTranslation.x + wingJetForwardHousingHeight * 0.5f, wingJetForwardHousingRightTranslation.y, wingJetForwardHousingRightTranslation.z);
    m_heliModel.wingJetForwardCowlRightMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.wingJetForwardCowlRightMatrix = DirectX::SimpleMath::Matrix::CreateScale(1.0f, wingJetCowlScale, 1.0f);
    m_heliModel.wingJetForwardCowlRightMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(90.0f));
    m_heliModel.wingJetForwardCowlRightMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(wingJetForwardCowlRightTranslation);
    m_heliModel.localWingJetForwardCowlRightMatrix = m_heliModel.wingJetForwardCowlRightMatrix;

    // wing jet coupling left  
    const float wingJetCouplingHeight = wingJetForwardHousingHeight;
    const float wingJetCouplingDiameter = wingJetForwardHousingDiameter;
    m_heliModel.wingJetCouplingShape = DirectX::GeometricPrimitive::CreateCone(aContext.Get(), wingJetCouplingDiameter, wingJetCouplingHeight, m_heliModel.circleTessellationVal1);
    const DirectX::SimpleMath::Vector3 wingJetCouplingLeftTranslation(wingJetForwardHousingLeftTranslation.x - wingJetForwardHousingHeight, wingJetForwardHousingLeftTranslation.y, wingJetForwardHousingLeftTranslation.z);
    m_heliModel.wingJetCouplingLeftMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.wingJetCouplingLeftMatrix = DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(90.0f));
    m_heliModel.wingJetCouplingLeftMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(wingJetCouplingLeftTranslation);
    m_heliModel.localWingJetCouplingLeftMatrix = m_heliModel.wingJetCouplingLeftMatrix;

    // wing jet coupling right
    const DirectX::SimpleMath::Vector3 wingJetCouplingRightTranslation(wingJetForwardHousingRightTranslation.x - wingJetForwardHousingHeight, wingJetForwardHousingRightTranslation.y, wingJetForwardHousingRightTranslation.z);
    m_heliModel.wingJetCouplingRightMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.wingJetCouplingRightMatrix = DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(90.0f));
    m_heliModel.wingJetCouplingRightMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(wingJetCouplingRightTranslation);
    m_heliModel.localWingJetCouplingRightMatrix = m_heliModel.wingJetCouplingRightMatrix;

    // wing jet rear housing left
    const float wingJetRearHousingDiameter = wingJetCenterSize.y * 0.45f;
    const float wingJetRearHousingHeight = wingJetCenterSize.x * 1.0f;
    m_heliModel.wingJetRearHousingShape = DirectX::GeometricPrimitive::CreateCylinder(aContext.Get(), wingJetRearHousingHeight, wingJetRearHousingDiameter, m_heliModel.circleTessellationVal2);
    const float wingJetExhaustDiameter = wingJetRearHousingDiameter * 0.9f;
    const float wingJetExhaustHeight = wingJetRearHousingHeight + 0.01f;
    m_heliModel.wingJetExhaustShape = DirectX::GeometricPrimitive::CreateCylinder(aContext.Get(), wingJetExhaustHeight, wingJetExhaustDiameter, m_heliModel.circleTessellationVal2);
    const float wingJetRearHousingOffsetX = -1.5f;
    const float wingJetRearHousingOffsetY = 0.17f;
    const float wingJetRearHousingOffsetZ = -0.0f;
    const DirectX::SimpleMath::Vector3 wingJetRearHousingLeftTranslation(wingJetCenterTranslation.x + wingJetRearHousingOffsetX, wingJetCenterTranslation.y + wingJetRearHousingOffsetY, -wingJetCenterSize.z * 0.5f - wingJetRearHousingOffsetZ);
    m_heliModel.wingJetRearHousingLeftMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.wingJetRearHousingLeftMatrix = DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(90.0f));
    m_heliModel.wingJetRearHousingLeftMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(wingJetRearHousingLeftTranslation);
    m_heliModel.localWingJetRearHousingLeftMatrix = m_heliModel.wingJetRearHousingLeftMatrix;

    // wing jet rear housing right
    const DirectX::SimpleMath::Vector3 wingJetRearHousingRightTranslation(wingJetCenterTranslation.x + wingJetRearHousingOffsetX, wingJetCenterTranslation.y + wingJetRearHousingOffsetY, wingJetCenterSize.z * 0.5f + wingJetRearHousingOffsetZ);
    m_heliModel.wingJetRearHousingRightMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.wingJetRearHousingRightMatrix = DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(90.0f));
    m_heliModel.wingJetRearHousingRightMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(wingJetRearHousingRightTranslation);
    m_heliModel.localWingJetRearHousingRightMatrix = m_heliModel.wingJetRearHousingRightMatrix;

    // wing jet exhaust tip
    const float jetExhaustTipThickness = (wingJetRearHousingDiameter - wingJetExhaustDiameter) * 0.5f;
    const float jetExhaustTipDiameter = wingJetRearHousingDiameter - jetExhaustTipThickness;
    m_heliModel.wingJetExhaustTipShape = DirectX::GeometricPrimitive::CreateTorus(aContext.Get(), jetExhaustTipDiameter, jetExhaustTipThickness, m_heliModel.circleTessellationVal2);
    m_heliModel.wingJetExhaustTipLeftMatrix = DirectX::SimpleMath::Matrix::CreateScale(1.0f, 12.0f, 1.0f);
    m_heliModel.wingJetExhaustTipLeftMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(0.0f, wingJetRearHousingHeight * 0.5f, 0.0f));
    m_heliModel.wingJetExhaustTipLeftMatrix *= m_heliModel.wingJetRearHousingLeftMatrix;
    m_heliModel.localWingJetExhaustTipLeftMatrix = m_heliModel.wingJetExhaustTipLeftMatrix;
    m_heliModel.wingJetExhaustTipRightMatrix = DirectX::SimpleMath::Matrix::CreateScale(1.0f, 12.0f, 1.0f);
    m_heliModel.wingJetExhaustTipRightMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(0.0f, wingJetRearHousingHeight * 0.5f, 0.0f));
    m_heliModel.wingJetExhaustTipRightMatrix *= m_heliModel.wingJetRearHousingRightMatrix;
    m_heliModel.localWingJetExhaustTipRightMatrix = m_heliModel.wingJetExhaustTipRightMatrix;

    // side side pod
    const float sidePodWidth = bodySize.z + 1.0f;
    const float sidePodHeight = 0.9f;
    const DirectX::SimpleMath::Vector3 sidePodTranslation(wingJetCenterTranslation.x -0.641f, wingJetCenterTranslation.y + 0.75f, wingJetCenterTranslation.z);
    m_heliModel.sidePodShape = DirectX::GeometricPrimitive::CreateCylinder(aContext.Get(), sidePodHeight, sidePodWidth, 6);
    m_heliModel.sidePodMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.sidePodMatrix = DirectX::SimpleMath::Matrix::CreateScale(1.0f, 1.0f, 1.2f);
    m_heliModel.sidePodMatrix *= DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(90.0f));
    m_heliModel.sidePodMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(sidePodTranslation);
    m_heliModel.localSidePodMatrix = m_heliModel.sidePodMatrix;

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
    const DirectX::SimpleMath::Vector3 landingGearRearLeftTranslation(bodySize.x * -0.34f, landingGearVerticalOffset, bodySize.z * -0.9f);
    m_heliModel.landingGearRearLeftMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.landingGearRearLeftMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(landingGearRearLeftTranslation);
    m_heliModel.localLandingGearRearLeftMatrix = m_heliModel.landingGearRearLeftMatrix;

    // landing gear back right
    const DirectX::SimpleMath::Vector3 landingGearRearRightTranslation(bodySize.x * -0.34f, landingGearVerticalOffset, bodySize.z * 0.9f);
    m_heliModel.landingGearRearRightMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.landingGearRearRightMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(landingGearRearRightTranslation);
    m_heliModel.localLandingGearRearRightMatrix = m_heliModel.landingGearRearRightMatrix;

    /// windshield
    const float windshieldSize = (bodySize.z * 0.5f) * sqrt(2.0f);
    m_heliModel.bodyCapShape = DirectX::GeometricPrimitive::CreateOctahedron(aContext.Get(), windshieldSize);
    const DirectX::SimpleMath::Vector3 windshieldTranslation(bodySize.x * 0.5f, -bodySize.y * 0.0f, 0.0f);
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

    // nose cone belly
    m_heliModel.noseConeBellyMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.noseConeBellyMatrix *= DirectX::SimpleMath::Matrix::CreateScale(DirectX::SimpleMath::Vector3(bellyScale.x, 1.0f, bellyScale.z));
    m_heliModel.noseConeBellyMatrix *= m_heliModel.noseConeMatrix;
    m_heliModel.noseConeBellyMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(bellyTranslation);
    m_heliModel.localNoseConeBellyMatrix = m_heliModel.noseConeBellyMatrix;

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

    // nose body belly
    m_heliModel.noseBodyBellyMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.noseBodyBellyMatrix *= DirectX::SimpleMath::Matrix::CreateScale(DirectX::SimpleMath::Vector3(1.0f, 1.0f, bellyScale.z));
    m_heliModel.noseBodyBellyMatrix *= m_heliModel.noseBodyMatrix;
    m_heliModel.noseBodyBellyMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(bellyTranslation); // 0.017
    m_heliModel.localNoseBodyBellyMatrix = m_heliModel.noseBodyBellyMatrix;

    // nose cowel
    const float noseCowlSize = windshieldSize * 0.42f;
    DirectX::SimpleMath::Vector3 noseCowlTranslation = windshieldTranslation;
    const float cowlPosOffset = 0.01f;
    noseCowlTranslation.x += windshieldSize;
    noseCowlTranslation.x -= noseCowlSize;
    noseCowlTranslation.x += cowlPosOffset;
    noseCowlTranslation.y += cowlPosOffset - 0.01f;
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

    // main body rear belly
    m_heliModel.bodyRearBellyMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.bodyRearBellyMatrix *= DirectX::SimpleMath::Matrix::CreateScale(DirectX::SimpleMath::Vector3(1.0564f, bellyScale.y, bellyScale.z));  
    m_heliModel.bodyRearBellyMatrix *= m_heliModel.bodyRearMatrix;
    m_heliModel.bodyRearBellyMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(-0.0f, bellyTranslation.y * 14.2f, 0.0f));    
    m_heliModel.localBodyRearBellyMatrix = m_heliModel.bodyRearBellyMatrix;

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

    // Side window eye
    const DirectX::SimpleMath::Vector3 sideWindowEyeSize(sideWindowUpperSize.x, sideWindowUpperSize.y * 0.35, sideWindowUpperSize.z + indentOffset); 
    m_heliModel.sideWindowEyeShape = DirectX::GeometricPrimitive::CreateCylinder(aContext.Get(), sideWindowEyeSize.z, sideWindowEyeSize.y, 3);
    const DirectX::SimpleMath::Vector3 sideWindowEyeTranslation(sideWindowUpperTranslation.x - (sideWindowEyeSize.y - sideWindowUpperSize.y) * 0.25f, sideWindowUpperTranslation.y + (sideWindowEyeSize.y - sideWindowUpperSize.y) * 0.2f, sideWindowUpperTranslation.z);
    m_heliModel.sideWindowEyeMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.sideWindowEyeMatrix = DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(30.0f));
    m_heliModel.sideWindowEyeMatrix *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(90.0f));
    m_heliModel.sideWindowEyeMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(sideWindowEyeTranslation);
    m_heliModel.localSideWindowEyeMatrix = m_heliModel.sideWindowEyeMatrix;

    // Side window lower
    const DirectX::SimpleMath::Vector3 sideWindowLowerSize(noseBodySize.x * 0.85f, noseBodySize.y * 0.75f, bodySize.z + indentOffset);
    m_heliModel.sideWindowLowerShape = DirectX::GeometricPrimitive::CreateCylinder(aContext.Get(), sideWindowLowerSize.z, toothDiameter, 3);
    const DirectX::SimpleMath::Vector3 sideWindowLowerTranslation(noseBodyTranslation.x * 0.99f, toothUpperTranslation.y, noseBodyTranslation.z);
    m_heliModel.sideWindowLowerMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.sideWindowLowerMatrix *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(90.0f));
    m_heliModel.sideWindowLowerMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(sideWindowLowerTranslation);
    m_heliModel.localSideWindowLowerMatrix = m_heliModel.sideWindowLowerMatrix;
    m_heliModel.sideWindowBottomMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.sideWindowBottomMatrix *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(90.0f));
    m_heliModel.sideWindowBottomMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(180.0f));
    const DirectX::SimpleMath::Vector3 sideWindowBottomTranslation(noseBodyTranslation.x * 0.99f, toothLowerTranslation.y, noseBodyTranslation.z);
    m_heliModel.sideWindowBottomMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(sideWindowBottomTranslation);
    m_heliModel.localSideWindowBottomMatrix = m_heliModel.sideWindowBottomMatrix;

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
    const float intakeVerticalOffset = -0.05f;
    const float intakeHorizontalOffset = -0.065;
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

    // engine house intake cowl left  
    const float engineIntakeCowlThickness = (engineHousingFrontSize - intakeShapeSize) * 1.5f;
    const float engineIntakeCowlDiamter = 1.11f;
    m_heliModel.engineHousingIntakeCowlShape = DirectX::GeometricPrimitive::CreateTorus(aContext.Get(), engineIntakeCowlDiamter, engineIntakeCowlThickness, 3);
    DirectX::SimpleMath::Vector3 engineIntakeCowlLeftTranslation(engineIntakeCowlDiamter * 0.25 - engineIntakeCowlThickness, engineIntakeCowlDiamter * 0.25f - engineIntakeCowlThickness + 0.72f, -engineIntakeCowlDiamter * 0.25f + engineIntakeCowlThickness);
    engineIntakeCowlLeftTranslation.x = engineHousingFrontSize * 0.25;
    engineIntakeCowlLeftTranslation.y = engineHousingFrontSize * 0.5f - engineIntakeCowlThickness * 0.80f;
    engineIntakeCowlLeftTranslation.z = -engineHousingFrontSize * 0.25f;
    engineIntakeCowlLeftTranslation.x = 0.196;
    engineIntakeCowlLeftTranslation.y = 0.195;
    engineIntakeCowlLeftTranslation.z = -0.196;
    m_heliModel.engineHousingIntakeCowlLeftMatrix = DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(-55.0f));
    m_heliModel.engineHousingIntakeCowlLeftMatrix *= DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(-45.0f));
    m_heliModel.engineHousingIntakeCowlLeftMatrix *= m_heliModel.engineHousingFrontLeftMatrix;
    m_heliModel.engineHousingIntakeCowlLeftMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(engineIntakeCowlLeftTranslation);
    m_heliModel.localEngineHousingIntakeCowlLeftMatrix = m_heliModel.engineHousingIntakeCowlLeftMatrix;
    
    // engine intake cowl right
    DirectX::SimpleMath::Vector3 engineIntakeCowlRightTranslation(engineIntakeCowlLeftTranslation.x, engineIntakeCowlLeftTranslation.y, -engineIntakeCowlLeftTranslation.z);
    m_heliModel.engineHousingIntakeCowlRightMatrix = DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(-125.0f));
    m_heliModel.engineHousingIntakeCowlRightMatrix *= DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(45.0f));
    m_heliModel.engineHousingIntakeCowlRightMatrix *= m_heliModel.engineHousingFrontRightMatrix;
    m_heliModel.engineHousingIntakeCowlRightMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(engineIntakeCowlRightTranslation);
    m_heliModel.localEngineHousingIntakeCowlRightMatrix = m_heliModel.engineHousingIntakeCowlRightMatrix;

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
    const DirectX::SimpleMath::Vector3 exhaustSize(1.23f, engineHousingSize.y * 0.48f, engineHousingSize.z * 0.25f);
    m_heliModel.engineExhaustExteriorShape = DirectX::GeometricPrimitive::CreateCylinder(aContext.Get(), exhaustSize.x, exhaustSize.y, m_heliModel.circleTessellationVal2);
    const DirectX::SimpleMath::Vector3 exhaustLeftTranslation(-engineHousingSize.x * 0.5f, engineHousingSize.y * 0.18f, engineHousingSize.z * 0.35f);
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

    // engine exhaust interior
    const float engineExhaustInteriorHeight = exhaustSize.x * 1.001f;
    const float engineExhaustInteriorDiameter = exhaustSize.y * 0.96f;
    m_heliModel.engineExhaustInteriorShape = DirectX::GeometricPrimitive::CreateCylinder(aContext.Get(), engineExhaustInteriorHeight, engineExhaustInteriorDiameter, m_heliModel.circleTessellationVal2);
    // engine exhaust tip
    //const float engineExhaustTipThickness = (exhaustSize.y - engineExhaustInteriorDiameter) * 0.5f;
    const float engineExhaustTipThickness = 0.04f;
    const float engineExhaustTipDiameter = exhaustSize.y - engineExhaustTipThickness;
    m_heliModel.engineExhaustTipShape = DirectX::GeometricPrimitive::CreateTorus(aContext.Get(), engineExhaustTipDiameter, engineExhaustTipThickness, m_heliModel.circleTessellationVal2);
    m_heliModel.engineExhaustTipLeftMatrix = DirectX::SimpleMath::Matrix::CreateScale(1.0f, 13.0f, 1.0f);
    m_heliModel.engineExhaustTipLeftMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(0.0f, exhaustSize.x * 0.5f, 0.0f));
    m_heliModel.engineExhaustTipLeftMatrix *= m_heliModel.engineExhaustLeftMatrix;
    m_heliModel.localEngineExhaustTipLeftMatrix = m_heliModel.engineExhaustTipLeftMatrix;
    m_heliModel.engineExhaustTipRightMatrix = DirectX::SimpleMath::Matrix::CreateScale(1.0f, 13.0f, 1.0f);
    m_heliModel.engineExhaustTipRightMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(0.0f, exhaustSize.x * 0.5f, 0.0f));
    m_heliModel.engineExhaustTipRightMatrix *= m_heliModel.engineExhaustRightMatrix;
    m_heliModel.localEngineExhaustTipRightMatrix = m_heliModel.engineExhaustTipRightMatrix;

    // tail boom
    const DirectX::SimpleMath::Vector3 tailBoomSize(8.9f, 0.65f, 0.7f);
    float testSize = (tailBoomSize.z * tailBoomSize.z) + (tailBoomSize.z * tailBoomSize.z);
    testSize = sqrt(testSize);
    testSize *= 0.5f;
    DirectX::SimpleMath::Vector3 tailBoomTranslation(-bodySize.x, bodySize.y - (tailBoomSize.y * 1.0f), 0.0f);
    tailBoomTranslation.y = bodySize.y - testSize - testSize;
    //tailBoomTranslation.y += 0.165f; 
    tailBoomTranslation.y += -0.1f;
    m_heliModel.tailBoomShape = DirectX::GeometricPrimitive::CreateBox(aContext.Get(), tailBoomSize);
    m_heliModel.tailBoomMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.tailBoomMatrix *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(0.0));
    m_heliModel.tailBoomMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(tailBoomTranslation);
    m_heliModel.localTailBoomMatrix = m_heliModel.tailBoomMatrix;

    // tail boom lower
    float tailBoomDiameter = 0.8f;
    tailBoomDiameter = tailBoomSize.z;
    tailBoomDiameter = 0.2f;
    const float tailBoomHeight = 8.9f;
    DirectX::SimpleMath::Vector3 tailBoomLowerTranslation(-bodySize.x, tailBoomTranslation.y, 0.0f);
    tailBoomLowerTranslation.y +=  ((tailBoomDiameter * 0.5f) / 2.0f);
    tailBoomLowerTranslation.y -= tailBoomSize.y * 0.5f + tailBoomDiameter * 0.25f;
    DirectX::SimpleMath::Vector3 tailBoomLowerSize(tailBoomHeight, testSize, testSize);
    m_heliModel.tailBoomNewShape = DirectX::GeometricPrimitive::CreateBox(aContext.Get(), tailBoomLowerSize);
    m_heliModel.tailBoomLowerMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.tailBoomLowerMatrix *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(45.0));
    m_heliModel.tailBoomLowerMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(tailBoomLowerTranslation);
    m_heliModel.localTailBoomLowerMatrix = m_heliModel.tailBoomLowerMatrix;

    // tail boom upper
    DirectX::SimpleMath::Vector3 tailBoomUpperTranslation(-bodySize.x, tailBoomTranslation.y, 0.0f);
    tailBoomUpperTranslation.y -= ((tailBoomDiameter * 0.5f) / 2.0f);
    tailBoomUpperTranslation.y += tailBoomDiameter * 0.5f + tailBoomSize.y * 0.5f;
    tailBoomUpperTranslation = tailBoomLowerTranslation;
    tailBoomUpperTranslation.y += tailBoomSize.y;
    m_heliModel.tailBoomUpperMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.tailBoomUpperMatrix *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(-45.0));
    m_heliModel.tailBoomUpperMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(tailBoomUpperTranslation);
    m_heliModel.localTailBoomUpperMatrix = m_heliModel.tailBoomUpperMatrix;

    // tail boom mid end cap
    const DirectX::SimpleMath::Vector3 tailBoomEndCapSize((tailBoomSize.z * 0.5f) * sqrt(2.0f), tailBoomSize.y, (tailBoomSize.z * 0.5f) * sqrt(2.0f));
    DirectX::SimpleMath::Vector3 tailBoomEndCapTranslation(tailBoomTranslation.x - tailBoomSize.x * 0.5f, tailBoomTranslation.y, tailBoomTranslation.z);
    m_heliModel.tailBoomMidEndCapShape = DirectX::GeometricPrimitive::CreateBox(aContext.Get(), tailBoomEndCapSize);
    m_heliModel.tailBoomMidEndCapMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.tailBoomMidEndCapMatrix *= DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(45.0));
    m_heliModel.tailBoomMidEndCapMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(tailBoomEndCapTranslation);
    m_heliModel.localTailBoomMidEndCapMatrix = m_heliModel.tailBoomMidEndCapMatrix;

    // tail boom Lower end cap
    const float tailBooLowerUpperEndCapSize = tailBoomSize.z * 0.5f;
    m_heliModel.tailBoomLowerUpperEndCapShape = DirectX::GeometricPrimitive::CreateOctahedron(aContext.Get(), tailBooLowerUpperEndCapSize);
    DirectX::SimpleMath::Vector3 tailBoomLowerEndCapTranslation(tailBoomEndCapTranslation.x, tailBoomLowerTranslation.y, tailBoomEndCapTranslation.z);
    m_heliModel.tailBoomLowerEndCapMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.tailBoomLowerEndCapMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(tailBoomLowerEndCapTranslation);
    m_heliModel.localTailBoomLowerEndCapMatrix = m_heliModel.tailBoomLowerEndCapMatrix;

    // tail boom upper end cap
    DirectX::SimpleMath::Vector3 tailBoomUpperEndCapTranslation(tailBoomEndCapTranslation.x, tailBoomUpperTranslation.y, tailBoomEndCapTranslation.z);
    m_heliModel.tailBoomUpperEndCapMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.tailBoomUpperEndCapMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(tailBoomUpperEndCapTranslation);
    m_heliModel.localTailBoomUpperEndCapMatrix = m_heliModel.tailBoomUpperEndCapMatrix;

    // tail fin upper
    const float tailfinWidth = 0.2f;
    const DirectX::SimpleMath::Vector3 tailFinOffset(-0.315f, 0.0f, 0.0f);
    const DirectX::SimpleMath::Vector3 tailFinUpperSize(1.0f, 2.6f, tailfinWidth);
    DirectX::SimpleMath::Vector3 tailFinUpperTranslation(tailBoomUpperTranslation.x - (tailBoomHeight * 0.546f) + (tailFinUpperSize.x * 0.5f), tailBoomTranslation.y + (tailBoomSize.y * 1.85f) - 0.13f, 0.0f);
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
    tailFinUpperTipSize.z -= 0.0002f;
    m_heliModel.tailFinUpperTipShape = DirectX::GeometricPrimitive::CreateBox(aContext.Get(), tailFinUpperTipSize);
    DirectX::SimpleMath::Vector3 tailFinUpperTipTranslation(0.0f, tailFinUpperSize.y * 0.5f, 0.0);
    tailFinUpperTipTranslation = DirectX::SimpleMath::Vector3::Transform(tailFinUpperTipTranslation, DirectX::SimpleMath::Matrix::CreateRotationZ(tailFinUpperRotation));
    DirectX::SimpleMath::Vector3 tailFinUpperTipTranslation3 = tailFinUpperTranslation;
    m_heliModel.tailFinUpperTipMatrix = DirectX::SimpleMath::Matrix::CreateTranslation(tailFinUpperTranslation + tailFinUpperTipTranslation + tailFinOffset);
    m_heliModel.localTailFinUpperTipMatrix = m_heliModel.tailFinUpperTipMatrix;

    // tail fin lower
    const DirectX::SimpleMath::Vector3 tailFinLowerSize(1.0f, 1.7f, tailfinWidth * 0.99f);
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

    // tail wing belly
    m_heliModel.tailWingBellyMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.tailWingBellyMatrix *= DirectX::SimpleMath::Matrix::CreateScale(DirectX::SimpleMath::Vector3(1.0f, 1.0f, 1.0f));
    m_heliModel.tailWingBellyMatrix *= m_heliModel.tailWingMatrix;
    m_heliModel.tailWingBellyMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(bellyTranslation);
    m_heliModel.localTailWingBellyMatrix = m_heliModel.tailWingBellyMatrix;

    // tail wing leading edge
    const DirectX::SimpleMath::Vector3 tailWingLeadingEdgeSize((tailWingSize.y * 0.5f) * sqrt(2.0f), (tailWingSize.y * 0.5f) * sqrt(2.0f), tailWingSize.z);
    m_heliModel.tailWingEdgeShape = DirectX::GeometricPrimitive::CreateBox(aContext.Get(), tailWingLeadingEdgeSize);
    const DirectX::SimpleMath::Vector3 tailWingLeadingEdgeTranslation(tailWingTranslation.x + tailWingSize.x * 0.5f, tailWingTranslation.y, tailWingTranslation.z);
    m_heliModel.tailWingLeadingEdgeMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.tailWingLeadingEdgeMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(45.0f));
    m_heliModel.tailWingLeadingEdgeMatrix *= tailWingRotation;
    m_heliModel.tailWingLeadingEdgeMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(tailWingLeadingEdgeTranslation);
    m_heliModel.localTailWingLeadingEdgeMatrix = m_heliModel.tailWingLeadingEdgeMatrix;

    // tail wing leading edge belly
    m_heliModel.tailWingLeadingEdgeBellyMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.tailWingLeadingEdgeBellyMatrix *= DirectX::SimpleMath::Matrix::CreateScale(DirectX::SimpleMath::Vector3(1.0f, 1.0f, 1.0f));
    m_heliModel.tailWingLeadingEdgeBellyMatrix *= m_heliModel.tailWingLeadingEdgeMatrix;
    m_heliModel.tailWingLeadingEdgeBellyMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(bellyTranslation);
    m_heliModel.localTailWingLeadingEdgeBellyMatrix = m_heliModel.tailWingLeadingEdgeBellyMatrix;

    // tail wing trailing edge
    const DirectX::SimpleMath::Vector3 tailWingTrailingEdgeTranslation(tailWingTranslation.x - tailWingSize.x * 0.5f, tailWingTranslation.y, tailWingTranslation.z);
    m_heliModel.tailWingTrailingEdgeMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.tailWingTrailingEdgeMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(45.0f));
    m_heliModel.tailWingTrailingEdgeMatrix *= tailWingRotation;
    m_heliModel.tailWingTrailingEdgeMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(tailWingTrailingEdgeTranslation);
    m_heliModel.localTailWingTrailingEdgeMatrix = m_heliModel.tailWingTrailingEdgeMatrix;

    // tail wing trailing edge belly
    m_heliModel.tailWingTrailingEdgeBellyMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.tailWingTrailingEdgeBellyMatrix *= DirectX::SimpleMath::Matrix::CreateScale(DirectX::SimpleMath::Vector3(1.0f, 1.0f, 1.01f));
    m_heliModel.tailWingTrailingEdgeBellyMatrix *= m_heliModel.tailWingTrailingEdgeMatrix;
    m_heliModel.tailWingTrailingEdgeBellyMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(bellyTranslation);
    m_heliModel.localTailWingTrailingEdgeBellyMatrix = m_heliModel.tailWingTrailingEdgeBellyMatrix;

    // tail wing fin left
    const float tailWingFinDiameter = 1.25f;
    const float tailWingFinHeight = tailWingFinWidth;
    const float tailWingFinTilt = 0.0f;
    m_heliModel.tailWingFinShape = DirectX::GeometricPrimitive::CreateCylinder(aContext.Get(), tailWingFinHeight, tailWingFinDiameter, 3);
    DirectX::SimpleMath::Vector3 tailWingFinLeftTranslation = tailWingTranslation;
    tailWingFinLeftTranslation.x -= 0.075f;
    tailWingFinLeftTranslation.z -= tailWingSize.z * 0.5f + tailWingFinHeight * 0.5f;
    m_heliModel.tailWingFinLeftMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.tailWingFinLeftMatrix = DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(-30.0f));
    m_heliModel.tailWingFinLeftMatrix *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(90.0f + tailWingFinTilt));
    m_heliModel.tailWingFinLeftMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(tailWingFinLeftTranslation);
    m_heliModel.localTailWingFinLeftMatrix = m_heliModel.tailWingFinLeftMatrix;

    // tail wing fin edge left
    const float tailWingFinEdgeHeight = ((tailWingFinDiameter * 0.5f) * cos(Utility::ToRadians(30.0f))) * 2.0f;
    const float tailWingFinEdgeDiameter = tailWingFinWidth / sin(Utility::ToRadians(60.0f));
    m_heliModel.tailWingFinEdgeShape = DirectX::GeometricPrimitive::CreateCylinder(aContext.Get(), tailWingFinEdgeHeight, tailWingFinEdgeDiameter, 3);
    DirectX::SimpleMath::Vector3 tailWingFinEdgeLeftTranslation(tailWingFinLeftTranslation.x - tailWingFinDiameter * 0.25f - tailWingFinEdgeDiameter * 0.25f, tailWingFinLeftTranslation.y, tailWingFinLeftTranslation.z);
    m_heliModel.tailWingFinEdgeLeftMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.tailWingFinEdgeLeftMatrix *= DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(30.0f));
    m_heliModel.tailWingFinEdgeLeftMatrix *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(tailWingFinTilt));
    m_heliModel.tailWingFinEdgeLeftMatrix *= tailWingRotation;
    m_heliModel.tailWingFinEdgeLeftMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(tailWingFinEdgeLeftTranslation);
    m_heliModel.localTailWingFinEdgeLeftMatrix = m_heliModel.tailWingFinEdgeLeftMatrix;

    // tail wing fin edge right
    DirectX::SimpleMath::Vector3 tailWingFinEdgeRightTranslation(tailWingFinEdgeLeftTranslation.x, tailWingFinEdgeLeftTranslation.y, -tailWingFinEdgeLeftTranslation.z);
    m_heliModel.tailWingFinEdgeRightMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.tailWingFinEdgeRightMatrix *= DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(30.0f));
    m_heliModel.tailWingFinEdgeRightMatrix *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(-tailWingFinTilt));
    m_heliModel.tailWingFinEdgeRightMatrix *= tailWingRotation;
    m_heliModel.tailWingFinEdgeRightMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(tailWingFinEdgeRightTranslation);
    m_heliModel.localTailWingFinEdgeRightMatrix = m_heliModel.tailWingFinEdgeRightMatrix;

    // tail wing fin right
    DirectX::SimpleMath::Vector3 tailWingFinRightTranslation = tailWingTranslation;
    tailWingFinRightTranslation.x -= 0.075f;
    tailWingFinRightTranslation.z += tailWingSize.z * 0.5f + tailWingFinHeight * 0.5f;
    m_heliModel.tailWingFinRightMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.tailWingFinRightMatrix = DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(-30.0f));
    m_heliModel.tailWingFinRightMatrix *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(90.0f - tailWingFinTilt));
    m_heliModel.tailWingFinRightMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(tailWingFinRightTranslation);
    m_heliModel.localTailWingFinRightMatrix = m_heliModel.tailWingFinRightMatrix;

    // main rotor axel
    const float mainAxelLength = 0.75f;
    const float mainAxelDiameter = 0.2f;
    const DirectX::SimpleMath::Vector3 mainAxelTranslation(0.0f, bodySize.y + (engineHousingSize.y * 0.5f) + (mainAxelLength * 0.5f), 0.0f);
    m_heliModel.mainRotorAxelShape = DirectX::GeometricPrimitive::CreateCylinder(aContext.Get(), mainAxelLength, mainAxelDiameter);
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
    const float tailArmHeight = aHeliData.tailRotor.height;
    m_heliModel.tailRotorArmShape = DirectX::GeometricPrimitive::CreateCylinder(aContext.Get(), tailArmDiameter, tailArmHeight);
    const DirectX::SimpleMath::Vector3 tailArmTranslation(tailHubTranslation.x, tailHubTranslation.y, tailHubTranslation.z);
    m_heliModel.tailRotorArmMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.tailRotorArmMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(tailArmTranslation);
    m_heliModel.localTailRotorArmMatrix = m_heliModel.tailRotorArmMatrix;

    // main rotor arm
    const float mainArmDiameter = aHeliData.mainRotor.radius * 2.0f;
    const float mainArmHeight = aHeliData.mainRotor.height;
    m_heliModel.mainRotorArmShape = DirectX::GeometricPrimitive::CreateCylinder(aContext.Get(), mainArmDiameter, mainArmHeight);
    const DirectX::SimpleMath::Vector3 rotorArmTranslation(0.0f, mainHubTranslation.y, 0.0f);
    m_heliModel.mainRotorArmMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.mainRotorArmMatrix *= DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(90.0f));
    m_heliModel.mainRotorArmMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(90.0f));
    m_heliModel.mainRotorArmMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(rotorArmTranslation);
    m_heliModel.localMainRotorArmMatrix = m_heliModel.mainRotorArmMatrix;

    //const float rotorStripeOffset = 0.001f;
    const float rotorStripeOffset = 0.002f;
    // main rotor arm end cap
    m_heliModel.mainRotorArmEndCapShape = DirectX::GeometricPrimitive::CreateSphere(aContext.Get(),  mainArmHeight + rotorStripeOffset);
    const DirectX::SimpleMath::Vector3 rotorArmEndCapTranslation1(mainArmDiameter * 0.5f, mainHubTranslation.y, 0.0f);
    m_heliModel.mainRotorArmEndCapMatrix1 = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.mainRotorArmEndCapMatrix1 *= DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(90.0f));
    m_heliModel.mainRotorArmEndCapMatrix1 *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(90.0f));
    m_heliModel.mainRotorArmEndCapMatrix1 *= DirectX::SimpleMath::Matrix::CreateTranslation(rotorArmEndCapTranslation1);
    m_heliModel.localMainRotorArmEndCapMatrix1 = m_heliModel.mainRotorArmEndCapMatrix1;
    const DirectX::SimpleMath::Vector3 rotorArmEndCapTranslation2(-mainArmDiameter * 0.5f, mainHubTranslation.y, 0.0f);
    m_heliModel.mainRotorArmEndCapMatrix2 = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.mainRotorArmEndCapMatrix2 *= DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(90.0f));
    m_heliModel.mainRotorArmEndCapMatrix2 *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(90.0f));
    m_heliModel.mainRotorArmEndCapMatrix2 *= DirectX::SimpleMath::Matrix::CreateTranslation(rotorArmEndCapTranslation2);
    m_heliModel.localMainRotorArmEndCapMatrix2 = m_heliModel.mainRotorArmEndCapMatrix2;

    // main rotor blade 1
    const float mainBladeWidth = aHeliData.mainRotor.width;
    const float mainBladeLength = aHeliData.mainRotor.length - 0.3f;
    const DirectX::SimpleMath::Vector3 rotorBladeSize(mainBladeLength, mainArmHeight, mainBladeWidth);
    const DirectX::SimpleMath::Vector3 rotorBladeTranslation1(((mainArmDiameter * 0.5f) - (rotorBladeSize.x * 0.5f)), mainHubTranslation.y, 0.0f);
    m_heliModel.mainRotorBladeShape = DirectX::GeometricPrimitive::CreateBox(aContext.Get(), rotorBladeSize);
    m_heliModel.mainRotorBladeMatrix1 = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.mainRotorBladeMatrix1 *= DirectX::SimpleMath::Matrix::CreateTranslation(rotorBladeTranslation1);
    m_heliModel.localMainRotorBladeMatrix1 = m_heliModel.mainRotorBladeMatrix1;
    m_heliModel.mainRotorBladeTranslationMatrix1 = DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(0.0f, 0.0f, rotorBladeSize.z * 0.5f));

    // main rotor blade end cap 1
    m_heliModel.mainRotorBladeEndCapShape = DirectX::GeometricPrimitive::CreateCylinder(aContext.Get(), mainBladeWidth, mainArmHeight + rotorStripeOffset);
    const DirectX::SimpleMath::Vector3 rotorBladeEndCapTranslation1(((mainArmDiameter * 0.5f) - (mainBladeWidth * 0.0f)), mainHubTranslation.y * 1.0f + (mainBladeWidth * 0.0f), rotorBladeSize.z * 0.0f);
    m_heliModel.mainRotorBladeEndCapMatrix1 = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.mainRotorBladeEndCapMatrix1 *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(90.0f));
    m_heliModel.mainRotorBladeEndCapMatrix1 *= DirectX::SimpleMath::Matrix::CreateTranslation(rotorBladeEndCapTranslation1);
    m_heliModel.localMainRotorBladeEndCapMatrix1 = m_heliModel.mainRotorBladeEndCapMatrix1;
    m_heliModel.mainRotorBladeEndCapTranslationMatrix1 = DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(0.0f, mainBladeWidth * 0.5f, mainBladeLength * 0.0f));
    // main rotor blade end cap 2
    const DirectX::SimpleMath::Vector3 rotorBladeEndCapTranslation2(((-mainArmDiameter * 0.5f) - (-mainBladeWidth * 0.0f)), mainHubTranslation.y * 1.0f + (mainBladeWidth * 0.0f), 0.0f);
    m_heliModel.mainRotorBladeEndCapMatrix2 = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.mainRotorBladeEndCapMatrix2 *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(90.0f));
    m_heliModel.mainRotorBladeEndCapMatrix2 *= DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(180.0f));
    m_heliModel.mainRotorBladeEndCapMatrix2 *= DirectX::SimpleMath::Matrix::CreateTranslation(rotorBladeEndCapTranslation2);
    m_heliModel.localMainRotorBladeEndCapMatrix2 = m_heliModel.mainRotorBladeEndCapMatrix2;
    m_heliModel.mainRotorBladeEndCapTranslationMatrix2 = DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(0.0f, mainBladeWidth * 0.5f, mainBladeLength * 0.0f));
   
    // main rotor edge  1
    float mainRotorLeadingEdgeDiameter = rotorBladeSize.y / sin(Utility::ToRadians(60.0f));
    float mainRotorLeadingEdgeHeight = mainBladeLength;
    m_heliModel.mainRotorBladeEdgeShape = DirectX::GeometricPrimitive::CreateCylinder(aContext.Get(), mainRotorLeadingEdgeHeight, mainRotorLeadingEdgeDiameter, 3);
    m_heliModel.mainRotorBladeEdgeMatrix1 = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.mainRotorBladeEdgeMatrix1 *= DirectX::SimpleMath::Matrix::CreateTranslation(rotorBladeTranslation1);
    m_heliModel.localMainRotorEdgeBladeMatrix1 = m_heliModel.mainRotorBladeEdgeMatrix1;
    m_heliModel.mainRotorBladeEdgeTranslationMatrix1 *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(90.0f));
    m_heliModel.mainRotorBladeEdgeTranslationMatrix1 *= DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(0.0f, 0.0f, rotorBladeSize.z + (((sqrt(3) / 3) * rotorBladeSize.y) * 0.5f)));
    
    // main rotor blade interior 1
    float testval = (rotorBladeSize.z * 0.5f) / sin(Utility::ToRadians(60.0f));
    testval = 1.07f;
    const float mainBladeInteriorWidth = aHeliData.mainRotor.width;
    const float mainBladeInteriorLength = aHeliData.mainRotor.length;
    const DirectX::SimpleMath::Vector3 rotorBladeInteriorSize(mainBladeInteriorLength, mainArmHeight, mainBladeInteriorWidth);
    const DirectX::SimpleMath::Vector3 rotorBladeInteriorTranslation1(((mainArmDiameter * 0.5f) - (rotorBladeSize.x * 1.0f)), mainHubTranslation.y + 0.0f, 0.0f);
    const float interiorHeight = rotorBladeSize.y - zFightOffset;
    const float interiorDiameter = testval * 1.0f; // rotorBladeSize.z / sin(Utility::ToRadians(60.0f));
    m_heliModel.mainRotorBladeInteriorShape = DirectX::GeometricPrimitive::CreateCylinder(aContext.Get(), interiorHeight, interiorDiameter, 3);
    m_heliModel.mainRotorBladeInteriorMatrix1 = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.mainRotorBladeInteriorMatrix1 *= DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(0.0f));
    m_heliModel.mainRotorBladeInteriorMatrix1 *= DirectX::SimpleMath::Matrix::CreateTranslation(rotorBladeInteriorTranslation1);
    m_heliModel.localMainRotorBladeInteriorMatrix1 = m_heliModel.mainRotorBladeInteriorMatrix1;
    m_heliModel.mainRotorBladeInteriorTranslationMatrix1 = DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(0.0f, 0.0f, interiorDiameter * 0.25f));

    // main rotor blade 1 stripe 1 
    const float rotorStripe1WidthMod = 0.12f;
    const float mainBladeStripe1Width = aHeliData.mainRotor.width;
    const float mainBladeStripe1Length = mainBladeLength * rotorStripe1WidthMod;
    const DirectX::SimpleMath::Vector3 rotorBladeStripe1Size(mainBladeStripe1Length, mainArmHeight + rotorStripeOffset, mainBladeStripe1Width);
    const DirectX::SimpleMath::Vector3 rotorBladeStripe1Translation1(((mainArmDiameter * 0.5f) - (mainBladeStripe1Length * 0.5f)), mainHubTranslation.y, 0.0f);
    m_heliModel.mainRotorBladeStripe1Shape = DirectX::GeometricPrimitive::CreateBox(aContext.Get(), rotorBladeStripe1Size);
    m_heliModel.mainRotorStripe1Matrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.mainRotorStripe1Matrix *= DirectX::SimpleMath::Matrix::CreateTranslation(rotorBladeStripe1Translation1);
    m_heliModel.localMainRotorStripe1Matrix = m_heliModel.mainRotorStripe1Matrix;
    m_heliModel.mainRotorBladeStripe1Matrix1 = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.mainRotorBladeStripe1Matrix1 *= DirectX::SimpleMath::Matrix::CreateTranslation(rotorBladeStripe1Translation1);
    m_heliModel.localMainRotorBladeStripe1Matrix1 = m_heliModel.mainRotorBladeStripe1Matrix1;
    // main rotor edge  stripe 1
    float mainRotorLeadingEdgeStripe1Diameter = rotorBladeSize.y / sin(Utility::ToRadians(60.0f)) + rotorStripeOffset;
    float mainRotorLeadingEdgeStripe1Height = mainBladeLength * rotorStripe1WidthMod;
    m_heliModel.mainRotorBladeEdgeStripe1Shape = DirectX::GeometricPrimitive::CreateCylinder(aContext.Get(), mainRotorLeadingEdgeStripe1Height, mainRotorLeadingEdgeStripe1Diameter, 3);
    m_heliModel.mainRotorBladeEdgeStripe1Matrix1 = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.mainRotorBladeEdgeStripe1Matrix1 *= DirectX::SimpleMath::Matrix::CreateTranslation(rotorBladeStripe1Translation1);
    m_heliModel.localMainRotorEdgeBladeStripe1Matrix1 = m_heliModel.mainRotorBladeEdgeStripe1Matrix1;

    // main rotor arm
    m_heliModel.mainRotorArmStripe1Shape = DirectX::GeometricPrimitive::CreateCylinder(aContext.Get(), mainRotorLeadingEdgeStripe1Height, mainArmHeight + rotorStripeOffset);
    m_heliModel.mainRotorArmStripe1Matrix1 = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.mainRotorArmStripe1Matrix2 = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.mainRotorArmStripe1Matrix1 *= DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(90.0f));
    m_heliModel.mainRotorArmStripe1Matrix1 *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(90.0f));
    m_heliModel.mainRotorArmStripe1Matrix1 *= DirectX::SimpleMath::Matrix::CreateTranslation(rotorBladeStripe1Translation1);
    m_heliModel.localMainRotorArmStripe1Matrix1 = m_heliModel.mainRotorArmStripe1Matrix1;

    // main rotor blade edge end cap 1
    m_heliModel.mainRotorBladeEdgeEndCapShape = DirectX::GeometricPrimitive::CreateCone(aContext.Get(), mainArmHeight + rotorStripeOffset, mainRotorLeadingEdgeDiameter);
    const DirectX::SimpleMath::Vector3 rotorBladeEdgeEndCapTranslation1(((mainArmDiameter * 0.5f) - (mainBladeWidth * 0.0f)), mainHubTranslation.y * 1.0f + (mainBladeWidth * 0.0f), rotorBladeSize.z * 0.0f);
    m_heliModel.mainRotorBladeEdgeEndCapMatrix1 = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.mainRotorBladeEdgeEndCapMatrix1 *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(90.0f));
    m_heliModel.mainRotorBladeEdgeEndCapMatrix1 *= DirectX::SimpleMath::Matrix::CreateTranslation(rotorBladeEdgeEndCapTranslation1);
    m_heliModel.localMainRotorBladeEdgeEndCapMatrix1 = m_heliModel.mainRotorBladeEdgeEndCapMatrix1;
    m_heliModel.mainRotorBladeEdgeEndCapTranslationMatrix1 = DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(0.0f, mainBladeWidth * 0.5f, 0.0f));
    m_heliModel.mainRotorBladeEdgeEndCapTranslationMatrix1 = DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(0.0f, rotorBladeSize.z + (((sqrt(3) / 3) * rotorBladeSize.y) * 0.5f), 0.0f));
    m_heliModel.mainRotorBladeEdgeEndCapTranslationMatrix1 = DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(0.0f, mainBladeWidth + (mainArmHeight * 0.57f), 0.0f));

    // main rotor blade edge end cap 2
    const DirectX::SimpleMath::Vector3 rotorBladeEdgeEndCapTranslation2(((-mainArmDiameter * 0.5f) - (-mainBladeWidth * 0.0f)), mainHubTranslation.y * 1.0f + (mainBladeWidth * 0.0f), 0.0f);
    m_heliModel.mainRotorBladeEdgeEndCapMatrix2 = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.mainRotorBladeEdgeEndCapMatrix2 *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(90.0f));
    m_heliModel.mainRotorBladeEdgeEndCapMatrix2 *= DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(180.0f));
    m_heliModel.mainRotorBladeEdgeEndCapMatrix2 *= DirectX::SimpleMath::Matrix::CreateTranslation(rotorBladeEdgeEndCapTranslation2);
    m_heliModel.localMainRotorBladeEdgeEndCapMatrix2 = m_heliModel.mainRotorBladeEdgeEndCapMatrix2;
    m_heliModel.mainRotorBladeEdgeEndCapTranslationMatrix2 = DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(0.0f, mainBladeWidth + (mainArmHeight * 0.57f), 0.0f));

    // main rotor blade 2
    const DirectX::SimpleMath::Vector3 rotorBladeTranslation2(((-mainArmDiameter * 0.5f) + (rotorBladeSize.x * 0.5f)), mainHubTranslation.y, 0.0f);
    m_heliModel.mainRotorBladeMatrix2 = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.mainRotorBladeMatrix2 *= DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(180.0f));
    m_heliModel.mainRotorBladeMatrix2 *= DirectX::SimpleMath::Matrix::CreateTranslation(rotorBladeTranslation2);
    m_heliModel.localMainRotorBladeMatrix2 = m_heliModel.mainRotorBladeMatrix2;
    m_heliModel.mainRotorBladeTranslationMatrix2 = DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(0.0f, 0.0f, rotorBladeSize.z * 0.5f));

    // main rotor blade interior 2
    const DirectX::SimpleMath::Vector3 rotorBladeInteriorTranslation2(((-mainArmDiameter * 0.5f) + (rotorBladeSize.x * 1.0f)), mainHubTranslation.y + 0.0f, 0.0f);
    m_heliModel.mainRotorBladeInteriorMatrix2 = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.mainRotorBladeInteriorMatrix2 *= DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(180.0f));
    m_heliModel.mainRotorBladeInteriorMatrix2 *= DirectX::SimpleMath::Matrix::CreateTranslation(rotorBladeInteriorTranslation2);   
    m_heliModel.localMainRotorBladeInteriorMatrix2 = m_heliModel.mainRotorBladeInteriorMatrix2;
    m_heliModel.mainRotorBladeInteriorTranslationMatrix2 = DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(0.0f, 0.0f, interiorDiameter * 0.25f));

    // main rotor edge 2
    m_heliModel.mainRotorBladeEdgeMatrix2 = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.mainRotorBladeEdgeMatrix2 *= DirectX::SimpleMath::Matrix::CreateTranslation(rotorBladeTranslation2);
    m_heliModel.localMainRotorEdgeBladeMatrix2 = m_heliModel.mainRotorBladeEdgeMatrix2;
    m_heliModel.mainRotorBladeEdgeTranslationMatrix2 = DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(60.0f));
    m_heliModel.mainRotorBladeEdgeTranslationMatrix2 *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(-90.0f));
    m_heliModel.mainRotorBladeEdgeTranslationMatrix2 *= DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(0.0f, 0.0f, -rotorBladeSize.z + (((sqrt(3) / 3) * -rotorBladeSize.y) * 0.5f)));

    // main rotor stripe 2
    const float rotorStripe2WidthMod = 0.04f;
    const float mainBladeStripe2Length = mainBladeLength * rotorStripe2WidthMod;
    const float mainBladeStripe2Offset = (mainBladeStripe1Length * 1.0f) + (mainBladeStripe2Length * 1.0f) + (mainBladeStripe2Length * 0.0f);
    const DirectX::SimpleMath::Vector3 mainRotorStripe2Translation1(((mainArmDiameter * 0.5f) - (mainBladeStripe2Length * 0.5f) - mainBladeStripe2Offset), mainHubTranslation.y, 0.0f);
    const DirectX::SimpleMath::Vector3 mainRotorStripe2Translation2(((-mainArmDiameter * 0.5f) + (mainBladeStripe2Length * 0.5f) + mainBladeStripe2Offset), mainHubTranslation.y, 0.0f);
    m_heliModel.localMainRotorStripe2Matrix = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.localMainRotorStripe2Matrix *= DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(90.0f));
    m_heliModel.localMainRotorStripe2Matrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(90.0f));
    m_heliModel.localMainRotorStripe2Matrix *= DirectX::SimpleMath::Matrix::CreateTranslation(mainRotorStripe2Translation1);
    m_heliModel.mainRotorArmStripe2Shape = DirectX::GeometricPrimitive::CreateCylinder(aContext.Get(), mainBladeLength * rotorStripe2WidthMod, mainArmHeight + rotorStripeOffset);
    m_heliModel.mainRotorArmStripe2Matrix1 = DirectX::SimpleMath::Matrix::CreateTranslation(mainRotorStripe2Translation1);
    m_heliModel.mainRotorBladeEdgeStripe2Shape = DirectX::GeometricPrimitive::CreateCylinder(aContext.Get(), mainBladeLength * rotorStripe2WidthMod , mainRotorLeadingEdgeStripe1Diameter, 3);
    m_heliModel.mainRotorEdgeStripe2Matrix1 = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.mainRotorEdgeStripe2Matrix1 *= DirectX::SimpleMath::Matrix::CreateTranslation(mainRotorStripe2Translation1);
    m_heliModel.localMainRotorEdgeStripe2Matrix1 = m_heliModel.mainRotorEdgeStripe2Matrix1;
    m_heliModel.mainRotorEdgeStripe2Matrix2 = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.mainRotorEdgeStripe2Matrix2 *= DirectX::SimpleMath::Matrix::CreateTranslation(mainRotorStripe2Translation1);
    m_heliModel.mainRotorEdgeStripe2Matrix2 *= DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(180.0f));
    m_heliModel.localMainRotorEdgeStripe2Matrix2 = m_heliModel.mainRotorEdgeStripe2Matrix2;

    // blade stripe 2
    const DirectX::SimpleMath::Vector3 rotorBladeStripe2Size(mainBladeStripe1Length, mainArmHeight + rotorStripeOffset, mainBladeStripe1Width);
    m_heliModel.mainRotorBladeStripe2Shape = DirectX::GeometricPrimitive::CreateBox(aContext.Get(), DirectX::SimpleMath::Vector3(mainBladeStripe2Length, mainArmHeight + rotorStripeOffset, mainBladeWidth));
    m_heliModel.mainRotorBladeStripe2Matrix1 = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.mainRotorBladeStripe2Matrix1 *= DirectX::SimpleMath::Matrix::CreateTranslation(mainRotorStripe2Translation1);
    m_heliModel.localMainRotorBladeStripe2Matrix1 = m_heliModel.mainRotorBladeStripe2Matrix1;
    m_heliModel.mainRotorBladeStripe2Matrix2 = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.mainRotorBladeStripe2Matrix2 *= DirectX::SimpleMath::Matrix::CreateTranslation(mainRotorStripe2Translation1);
    m_heliModel.mainRotorBladeStripe2Matrix2 *= DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(180.0f));
    m_heliModel.localMainRotorBladeStripe2Matrix2 = m_heliModel.mainRotorBladeStripe2Matrix2;

    // tail rotor blade 1
    const float tailBladeWidth = aHeliData.tailRotor.width;
    const float tailBladeLength = aHeliData.tailRotor.length * 0.93f;
    const DirectX::SimpleMath::Vector3 tailBladeSize(tailBladeWidth, tailBladeLength, tailArmHeight);
    m_heliModel.tailRotorBladeShape = DirectX::GeometricPrimitive::CreateBox(aContext.Get(), tailBladeSize);
    const DirectX::SimpleMath::Vector3 tailBladeTranslation(-tailBladeWidth * 0.5f, -(tailArmDiameter * 0.5f) + (tailBladeSize.y * 0.5f), 0.0f);  
    m_heliModel.tailRotorBladeMatrix1 = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.tailRotorBladeMatrix1 *= DirectX::SimpleMath::Matrix::CreateTranslation(tailBladeTranslation);
    m_heliModel.localTailRotorBladeMatrix1 = m_heliModel.tailRotorBladeMatrix1;
    m_heliModel.tailRotorBladeTranslationMatrix1 *= DirectX::SimpleMath::Matrix::CreateTranslation(tailArmTranslation);

    // tail rotor blade end cap 1
    m_heliModel.tailRotorBladeEndCapShape = DirectX::GeometricPrimitive::CreateCylinder(aContext.Get(), tailBladeWidth, tailArmHeight + rotorStripeOffset);
    const DirectX::SimpleMath::Vector3 tailBladeEndCapTranslation1(-tailBladeWidth * 0.5f, -(tailArmDiameter * 0.5f) - (tailBladeSize.y * 0.0f), 0.0f);
    m_heliModel.tailRotorBladeEndCapMatrix1 = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.tailRotorBladeEndCapMatrix1 *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(90.0f));
    m_heliModel.tailRotorBladeEndCapMatrix1 *= DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(90.0f));
    m_heliModel.tailRotorBladeEndCapMatrix1 *= DirectX::SimpleMath::Matrix::CreateTranslation(tailBladeEndCapTranslation1);
    m_heliModel.localTailRotorBladeEndCapMatrix1 = m_heliModel.tailRotorBladeEndCapMatrix1;
    m_heliModel.tailRotorBladeEndCapTranslationMatrix1 *= DirectX::SimpleMath::Matrix::CreateTranslation(tailArmTranslation);

    // tail rotor blade 2
    m_heliModel.tailRotorBladeMatrix2 = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.tailRotorBladeMatrix2 *= DirectX::SimpleMath::Matrix::CreateTranslation(tailBladeTranslation);
    m_heliModel.localTailRotorBladeMatrix2 = m_heliModel.tailRotorBladeMatrix2;
    m_heliModel.tailRotorBladeTranslationMatrix2 *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(180.0f));
    m_heliModel.tailRotorBladeTranslationMatrix2 *= DirectX::SimpleMath::Matrix::CreateTranslation(tailArmTranslation);

    // tail rotor blade end cap 2
    m_heliModel.tailRotorBladeEndCapMatrix2 = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.tailRotorBladeEndCapMatrix2 *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(90.0f));
    m_heliModel.tailRotorBladeEndCapMatrix2 *= DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(90.0f));
    m_heliModel.tailRotorBladeEndCapMatrix2 *= DirectX::SimpleMath::Matrix::CreateTranslation(tailBladeEndCapTranslation1);
    m_heliModel.localTailRotorBladeEndCapMatrix2 = m_heliModel.tailRotorBladeEndCapMatrix2;
    m_heliModel.tailRotorBladeEndCapTranslationMatrix2 *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(180.0f));
    m_heliModel.tailRotorBladeEndCapTranslationMatrix2 *= DirectX::SimpleMath::Matrix::CreateTranslation(tailArmTranslation);

    // tail rotor arm end cap 1
    m_heliModel.tailRotorArmEndCapShape = DirectX::GeometricPrimitive::CreateSphere(aContext.Get(), tailArmHeight + rotorStripeOffset);
    const DirectX::SimpleMath::Vector3 tailArmEndCapTranslation1(-tailBladeWidth * 0.0f, -(tailArmDiameter * 0.5f) - (tailBladeSize.y * 0.0f), 0.0f);
    m_heliModel.tailRotorArmEndCapMatrix1 = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.tailRotorArmEndCapMatrix1 *= DirectX::SimpleMath::Matrix::CreateTranslation(tailArmEndCapTranslation1);
    m_heliModel.localTailRotorArmEndCapMatrix1 = m_heliModel.tailRotorArmEndCapMatrix1;
    m_heliModel.tailRotorArmEndCapTranslationMatrix1 = DirectX::SimpleMath::Matrix::CreateTranslation(tailArmTranslation);

    // tail rotor arm end cap 2
    const DirectX::SimpleMath::Vector3 tailArmEndCapTranslation2(-tailBladeWidth * 0.0f, +(tailArmDiameter * 0.5f) - (tailBladeSize.y * 0.0f), 0.0f);
    m_heliModel.tailRotorArmEndCapMatrix2 = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.tailRotorArmEndCapMatrix2 *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(180.0f));
    m_heliModel.tailRotorArmEndCapMatrix2 *= DirectX::SimpleMath::Matrix::CreateTranslation(tailArmEndCapTranslation2);
    m_heliModel.localTailRotorArmEndCapMatrix2 = m_heliModel.tailRotorArmEndCapMatrix2;
    m_heliModel.tailRotorArmEndCapTranslationMatrix2 = DirectX::SimpleMath::Matrix::CreateTranslation(tailArmTranslation);

    float tailRotorLeadingEdgeDiameter = tailArmHeight / sin(Utility::ToRadians(60.0f));
    float tailRotorLeadingEdgeHeight = tailBladeLength;
    m_heliModel.tailRotorBladeEdgeShape = DirectX::GeometricPrimitive::CreateCylinder(aContext.Get(), tailRotorLeadingEdgeHeight, tailRotorLeadingEdgeDiameter, 3);
    DirectX::SimpleMath::Vector3 tailBladeEdgeTranslation = tailBladeTranslation;
    const float medianTest = (tailRotorLeadingEdgeDiameter * 0.5f) + ((tailRotorLeadingEdgeDiameter * 0.5f) / 2.0f);
    DirectX::SimpleMath::Vector3 tailBladeEdgeTranslation1(-tailBladeSize.x * 1.0f - tailRotorLeadingEdgeDiameter * 0.5f + (tailRotorLeadingEdgeDiameter - medianTest), tailBladeTranslation.y, 0.0f);
    m_heliModel.tailRotorBladeEdgeMatrix1 = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.tailRotorBladeEdgeMatrix1 = DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(-90.0f));
    m_heliModel.tailRotorBladeEdgeMatrix1 *= DirectX::SimpleMath::Matrix::CreateTranslation(tailBladeEdgeTranslation1);
    m_heliModel.localTailRotorBladeEdgeMatrix1 = m_heliModel.tailRotorBladeEdgeMatrix1;
    m_heliModel.tailRotorBladeEdgeTranslationMatrix1 *= DirectX::SimpleMath::Matrix::CreateTranslation(tailArmTranslation);

    // tail blade edge 2
    m_heliModel.tailRotorBladeEdgeMatrix2 = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.tailRotorBladeEdgeMatrix2 = DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(-90.0f));
    m_heliModel.tailRotorBladeEdgeMatrix2 *= DirectX::SimpleMath::Matrix::CreateTranslation(tailBladeEdgeTranslation1);
    m_heliModel.localTailRotorBladeEdgeMatrix2 = m_heliModel.tailRotorBladeEdgeMatrix1;
    m_heliModel.tailRotorBladeEdgeTranslationMatrix2 *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(180.0f));
    m_heliModel.tailRotorBladeEdgeTranslationMatrix2 *= DirectX::SimpleMath::Matrix::CreateTranslation(tailArmTranslation);

    // tail blade edge end cap 2
    m_heliModel.tailRotorBladeEdgeEndCapShape = DirectX::GeometricPrimitive::CreateCone(aContext.Get(), tailArmHeight + rotorStripeOffset, tailRotorLeadingEdgeDiameter);
    DirectX::SimpleMath::Vector3 tailBladeEdgeEndCapTranslation1(-tailBladeWidth * 1.0f - tailRotorLeadingEdgeDiameter, - tailArmDiameter * 0.5f, 0.0f);
    tailBladeEdgeEndCapTranslation1 = tailBladeEdgeTranslation1;
    tailBladeEdgeEndCapTranslation1.y -= tailBladeLength * 0.5f;
    tailBladeEdgeEndCapTranslation1.x -= tailBladeWidth * 0.095f ;
    m_heliModel.tailRotorBladeEdgeEndCapMatrix2 = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.tailRotorBladeEdgeEndCapMatrix2 = DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(90.0f));
    m_heliModel.tailRotorBladeEdgeEndCapMatrix2 *= DirectX::SimpleMath::Matrix::CreateTranslation(tailBladeEdgeEndCapTranslation1);
    m_heliModel.localTailRotorBladeEdgeEndCapMatrix2 = m_heliModel.tailRotorBladeEdgeEndCapMatrix2;
    m_heliModel.tailRotorBladeEdgeEndCapTranslationMatrix2 *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(180.0f));
    m_heliModel.tailRotorBladeEdgeEndCapTranslationMatrix2 *= DirectX::SimpleMath::Matrix::CreateTranslation(tailArmTranslation);
    // tail blade edge end cap 1
    m_heliModel.tailRotorBladeEdgeEndCapMatrix1 = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.tailRotorBladeEdgeEndCapMatrix1 = DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(90.0f));
    m_heliModel.tailRotorBladeEdgeEndCapMatrix1 *= DirectX::SimpleMath::Matrix::CreateTranslation(tailBladeEdgeEndCapTranslation1);
    m_heliModel.localTailRotorBladeEdgeEndCapMatrix1 = m_heliModel.tailRotorBladeEdgeEndCapMatrix1;
    m_heliModel.tailRotorBladeEdgeEndCapTranslationMatrix1 *= DirectX::SimpleMath::Matrix::CreateTranslation(tailArmTranslation);

    // tail rotor blade interior 1
    float tailIntHeight = tailBladeSize.z - zFightOffset;
    float tailIntDiam = 0.40f;
    m_heliModel.tailRotorBladeInteriorShape = DirectX::GeometricPrimitive::CreateCylinder(aContext.Get(), tailIntHeight, tailIntDiam, 3);
    m_heliModel.tailRotorBladeInteriorMatrix1 = DirectX::SimpleMath::Matrix::Identity; 
    m_heliModel.tailRotorBladeInteriorMatrix1 *= DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(30.0f));
    m_heliModel.tailRotorBladeInteriorMatrix1 *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(90.0f));
    m_heliModel.tailRotorBladeInteriorMatrix1 *= DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(-tailIntDiam * 0.25f, -(tailArmDiameter * 0.5f) + (tailBladeSize.y * 1.0f), 0.0f));
    m_heliModel.localTailRotorBladeInteriorMatrix1 = m_heliModel.tailRotorBladeInteriorMatrix1;
    m_heliModel.tailRotorBladeInteriorTranslationMatrix1 = DirectX::SimpleMath::Matrix::CreateTranslation(tailArmTranslation);
    // tail rotor blade interior 2
    m_heliModel.tailRotorBladeInteriorMatrix2 = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.tailRotorBladeInteriorMatrix2 *= DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(30.0f));
    m_heliModel.tailRotorBladeInteriorMatrix2 *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(90.0f));
    m_heliModel.tailRotorBladeInteriorMatrix2 *= DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(-tailIntDiam * 0.25f, -(tailArmDiameter * 0.5f) + (tailBladeSize.y * 1.0f), 0.0f));
    m_heliModel.localTailRotorBladeInteriorMatrix2 = m_heliModel.tailRotorBladeInteriorMatrix2;
    m_heliModel.tailRotorBladeInteriorTranslationMatrix2 *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(180.0f));
    m_heliModel.tailRotorBladeInteriorTranslationMatrix2 *= DirectX::SimpleMath::Matrix::CreateTranslation(tailArmTranslation);

    // tail rotor arm stripe 1 
    const float tailStripe1WidthMod = 0.2f;
    const float tailBladeStripe1Width = tailBladeWidth;
    const float tailBladeStripe1Length = tailBladeLength * tailStripe1WidthMod;
    const DirectX::SimpleMath::Vector3 tailStripe1Size(tailBladeStripe1Length, tailArmHeight + rotorStripeOffset, tailBladeStripe1Width);
    const DirectX::SimpleMath::Vector3 Stripe1Translation1(((tailArmDiameter * 0.5f) - (tailBladeStripe1Length * 0.5f)), tailHubTranslation.y, 0.0f);
    m_heliModel.tailRotorArmStripe1Shape = DirectX::GeometricPrimitive::CreateCylinder(aContext.Get(), tailBladeStripe1Length, tailArmHeight + rotorStripeOffset);
    const DirectX::SimpleMath::Vector3 tailArmStripe1Translation1(-tailBladeWidth * 0.0f, -(tailArmDiameter * 0.5f) + (tailBladeStripe1Length * 0.5f), 0.0f);
    m_heliModel.tailRotorArmStripe1Matrix1 = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.tailRotorArmStripe1Matrix1 *= DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(-tailBladeWidth * 0.0f, -(tailArmDiameter * 0.5f) + (tailBladeStripe1Length * 0.5f), 0.0f));
    m_heliModel.localTailRotorArmStripe1Matrix1 = m_heliModel.tailRotorArmStripe1Matrix1;
    m_heliModel.tailRotorArmStripe1TranslationMatrix1 = DirectX::SimpleMath::Matrix::CreateTranslation(tailArmTranslation);
    // main rotor arm 1 stripe 2
    m_heliModel.tailRotorArmStripe1Matrix2 = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.tailRotorArmStripe1Matrix2 *= DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(-tailBladeWidth * 0.0f, -(tailArmDiameter * 0.5f) + (tailBladeStripe1Length * 0.5f), 0.0f));
    m_heliModel.localTailRotorArmStripe1Matrix2 = m_heliModel.tailRotorArmStripe1Matrix2;

    // tail arm 1 stripe 2
    const float tailStripe2WidthMod = 0.06f;
    const float tailBladeStripe2Length = tailBladeLength * tailStripe2WidthMod;
    const float tailBladeStripe2Offset = (tailBladeStripe1Length * 1.0f) + (tailBladeStripe2Length * 1.0f);
    m_heliModel.tailRotorArmStripe2Shape = DirectX::GeometricPrimitive::CreateCylinder(aContext.Get(), tailBladeStripe2Length, tailArmHeight + rotorStripeOffset);
    m_heliModel.tailRotorArmStripe2Matrix1 = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.tailRotorArmStripe2Matrix1 *= DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(-tailBladeWidth * 0.0f, (-tailArmDiameter * 0.5f) + (tailBladeStripe2Length * 0.5f) + tailBladeStripe2Offset, 0.0f));
    m_heliModel.localTailRotorArmStripe2Matrix1 = m_heliModel.tailRotorArmStripe2Matrix1;
    m_heliModel.localTailRotorArmStripe2Matrix1 = m_heliModel.tailRotorArmStripe2Matrix1;
    // tail arm 2 stripe 2
    m_heliModel.tailRotorArmStripe2Matrix2 = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.tailRotorArmStripe2Matrix2 *= DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(-tailBladeWidth * 0.0f, (-tailArmDiameter * 0.5f) + (tailBladeStripe2Length * 0.5f) + tailBladeStripe2Offset, 0.0f));
    m_heliModel.localTailRotorArmStripe2Matrix2 = m_heliModel.tailRotorArmStripe2Matrix2;

    // tail rotor blade 1 stripe 1 
    m_heliModel.tailRotorBladeStripe1Shape = DirectX::GeometricPrimitive::CreateBox(aContext.Get(), DirectX::SimpleMath::Vector3(tailBladeStripe1Width, tailBladeStripe1Length, tailArmHeight + rotorStripeOffset));
    m_heliModel.tailRotorBladeStripe1Matrix1 = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.tailRotorBladeStripe1Matrix1 *= DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(-tailBladeWidth * 0.5f, -(tailArmDiameter * 0.5f) + (tailBladeStripe1Length * 0.5f), 0.0f));
    m_heliModel.localTailRotorBladeStripe1Matrix1 = m_heliModel.tailRotorBladeStripe1Matrix1;
    // tail rotor blade 2 stripe 1 
    m_heliModel.tailRotorBladeStripe1Matrix2 = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.tailRotorBladeStripe1Matrix2 *= DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(-tailBladeWidth * 0.5f, -(tailArmDiameter * 0.5f) + (tailBladeStripe1Length * 0.5f), 0.0f));
    m_heliModel.localTailRotorBladeStripe1Matrix2 = m_heliModel.tailRotorBladeStripe1Matrix2;

    // tail rotor blade 1 stripe 2
    m_heliModel.tailRotorBladeStripe2Shape = DirectX::GeometricPrimitive::CreateBox(aContext.Get(), DirectX::SimpleMath::Vector3(tailBladeStripe1Width, tailBladeStripe2Length, tailArmHeight + rotorStripeOffset));
    m_heliModel.tailRotorBladeStripe2Matrix1 = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.tailRotorBladeStripe2Matrix1 *= DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(-tailBladeWidth * 0.5f, -(tailArmDiameter * 0.5f) + (tailBladeStripe2Length * 0.5f) + tailBladeStripe2Offset, 0.0f));
    m_heliModel.localTailRotorBladeStripe2Matrix1 = m_heliModel.tailRotorBladeStripe2Matrix1;
    // tail rotor blade 2 stripe 2
    m_heliModel.tailRotorBladeStripe2Matrix2 = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.tailRotorBladeStripe2Matrix2 *= DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(-tailBladeWidth * 0.5f, -(tailArmDiameter * 0.5f) + (tailBladeStripe2Length * 0.5f) + tailBladeStripe2Offset, 0.0f));
    m_heliModel.localTailRotorBladeStripe2Matrix2 = m_heliModel.tailRotorBladeStripe2Matrix2;

    // tail rotor edge 1 stripe 1 
    m_heliModel.tailRotorEdgeStripe1Shape = DirectX::GeometricPrimitive::CreateCylinder(aContext.Get(), tailBladeStripe1Length, tailBladeSize.z / sin(Utility::ToRadians(60.0f)) + rotorStripeOffset, 3);
    m_heliModel.tailRotorEdgeStripe1Matrix1 = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.tailRotorEdgeStripe1Matrix1 = DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(-90.0f));
    m_heliModel.tailRotorEdgeStripe1Matrix1 *= DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(-tailBladeSize.x * 1.0f - tailRotorLeadingEdgeDiameter * 0.5f + (tailRotorLeadingEdgeDiameter - medianTest), -(tailArmDiameter * 0.5f) + (tailBladeStripe1Length * 0.5f), 0.0f));
    m_heliModel.localTailRotorEdgeStripe1Matrix1 = m_heliModel.tailRotorEdgeStripe1Matrix1;
    // tail rotor edge 2 stripe 1 
    m_heliModel.tailRotorEdgeStripe1Matrix2 = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.tailRotorEdgeStripe1Matrix2 = DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(-90.0f));
    m_heliModel.tailRotorEdgeStripe1Matrix2 *= DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(-tailBladeSize.x * 1.0f - tailRotorLeadingEdgeDiameter * 0.5f + (tailRotorLeadingEdgeDiameter - medianTest), -(tailArmDiameter * 0.5f) + (tailBladeStripe1Length * 0.5f), 0.0f));
    m_heliModel.localTailRotorEdgeStripe1Matrix2 = m_heliModel.tailRotorEdgeStripe1Matrix2;

    // tail rotor edge 1 stripe 2
    m_heliModel.tailRotorEdgeStripe2Shape = DirectX::GeometricPrimitive::CreateCylinder(aContext.Get(), tailBladeStripe2Length, tailBladeSize.z / sin(Utility::ToRadians(60.0f)) + rotorStripeOffset, 3);
    m_heliModel.tailRotorEdgeStripe2Matrix1 = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.tailRotorEdgeStripe2Matrix1 = DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(-90.0f));
    m_heliModel.tailRotorEdgeStripe2Matrix1 *= DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(-tailBladeSize.x * 1.0f - tailRotorLeadingEdgeDiameter * 0.5f + (tailRotorLeadingEdgeDiameter - medianTest), -(tailArmDiameter * 0.5f) + (tailBladeStripe2Length * 0.5f) + tailBladeStripe2Offset, 0.0f));
    m_heliModel.localTailRotorEdgeStripe2Matrix1 = m_heliModel.tailRotorEdgeStripe2Matrix1;   
    // tail rotor edge 2 stripe 2
    m_heliModel.tailRotorEdgeStripe2Matrix2 = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.tailRotorEdgeStripe2Matrix2 = DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(-90.0f));
    m_heliModel.tailRotorEdgeStripe2Matrix2 *= DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(-tailBladeSize.x * 1.0f - tailRotorLeadingEdgeDiameter * 0.5f + (tailRotorLeadingEdgeDiameter - medianTest), -(tailArmDiameter * 0.5f) + (tailBladeStripe2Length * 0.5f) + tailBladeStripe2Offset, 0.0f));
    m_heliModel.localTailRotorEdgeStripe2Matrix2 = m_heliModel.tailRotorEdgeStripe2Matrix2;

    m_heliModel.tailRotorTranslationMatrix1 = DirectX::SimpleMath::Matrix::CreateTranslation(tailArmTranslation);
    m_heliModel.tailRotorTranslationMatrix2 = DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(180.0f));
    m_heliModel.tailRotorTranslationMatrix2 *= DirectX::SimpleMath::Matrix::CreateTranslation(tailArmTranslation);

    // shadows
    DirectX::SimpleMath::Vector3 baseShadowSize = DirectX::SimpleMath::Vector3(5.0f, 10.0f, 5.0f);;
    baseShadowSize.y *= 0.5f;
    m_heliModel.shadowBaseShape = DirectX::GeometricPrimitive::CreateBox(aContext.Get(), baseShadowSize);
    m_heliModel.localShadowBaseMat = DirectX::SimpleMath::Matrix::Identity;
    m_heliModel.localShadowBaseMat *= DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(baseShadowSize.x * 0.04f, baseShadowSize.y * 0.45f, 0.0f));

}

void Vehicle::InitializeRotorBlades(HeliData& aHeliData)
{
    // set values for main rotor blade
    const int bladeCountMainRotor = 2;
    const float angleBetweetnBladesMainRotor = (Utility::GetPi() * 2.0f) / bladeCountMainRotor;
    const float radiusMainRotor = 8.0f;
    const float heightMainRotor = 0.15f;
    const float lengthMainRotor = 7.4f;
    const float widthMainRotor = 0.80f;       
    const float initialRPMMainRotor = 0.0f;
    const float maxPitchAngleMainRotor = Utility::ToRadians(20.0f);
    const float maxInputPitchAngleMainRotor = Utility::ToRadians(15.0f);
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
        aHeliData.mainRotor.bladeVec[i].liftForcePerSecond = 0.0f;
        aHeliData.mainRotor.bladeVec[i].liftMag = 0.0f;
        aHeliData.mainRotor.bladeVec[i].liftNorm = DirectX::SimpleMath::Vector3::Zero;
        aHeliData.mainRotor.bladeVec[i].rotationAngle = static_cast<float>(i) * angleBetweetnBladesMainRotor;
    }

    // set values for tail rotor blade
    const int bladeCountTailRotor = 2;
    const float angleBetweetnBladesTailRotor = (Utility::GetPi() * 2.0f) / bladeCountTailRotor;
    const float radiusTailRotor = 1.425f;
    const float heightTailRotor = 0.1f;
    const float lengthTailRotor = 1.25f;
    const float widthTailRotor = 0.300000012f;
    
    const float initialRPMTailRotor = 0.0f;
    const float maxPitchAngleTailRotor = Utility::ToRadians(45.0f);
    const float maxInputPitchAngleTailRotor = Utility::ToRadians(45.0f);
    const float minInputPitchAngleTailRotor = Utility::ToRadians(0.0f);
    const float minPitchAngleTailRotor = Utility::ToRadians(0.0f);
    const float neutralAngleTailRotor = (maxPitchAngleTailRotor + minInputPitchAngleTailRotor) / 2.0f;
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
        aHeliData.tailRotor.bladeVec[i].liftForcePerSecond = 0.0f;
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
    m_heli.mainRotorMaxRPM = 500.0f;
    m_heli.numEqns = 6;
    m_heli.mass = 1700.0f;
    m_heli.massTest = 2000.0f;
    m_heli.area = 14.67;
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
    m_heli.inverseAlignment = m_heli.alignment;
    m_heli.inverseAlignment = m_heli.inverseAlignment.Invert();
    m_heli.alignmentQuat = DirectX::SimpleMath::Quaternion::CreateFromRotationMatrix(m_heli.alignment);
    m_heli.inverseAlignmentQuat = m_heli.alignmentQuat;
    m_heli.inverseAlignmentQuat.Inverse(m_heli.inverseAlignmentQuat);
    m_heli.cameraOrientation = m_heli.alignment;
    m_heli.cameraOrientationPrevious = m_heli.cameraOrientation;

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
    m_heli.altitude = 0.0f;
    m_heli.groundPlane.x = 0.0f;
    m_heli.groundPlane.y = -1.0f;
    m_heli.groundPlane.z = 0.0f;
    m_heli.groundPlane.w = 0.5f;
    m_heli.terrainHightAtPos = 0.0;
    m_heli.terrainNormal = DirectX::SimpleMath::Vector3::UnitY;

    m_heli.q.tailRotorForceNormal = -m_heli.right;
    m_heli.q.tailRotorForceMagnitude = 0.0f;
    
    m_heli.q.totalVelocity = DirectX::SimpleMath::Vector3::Zero;
    m_heli.q.velocity = DirectX::SimpleMath::Vector3::Zero;
    m_heli.q.parabolicMomentum = DirectX::SimpleMath::Vector3::Zero;

    m_heli.speed = 0.0;

    m_heli.isVehicleAirborne = false;
    m_heli.isVehicleLanding = false;
    m_heli.isVelocityBackwards = false;

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

    DirectX::SimpleMath::Vector3 posShift(0.0f, 0.0f, 0.0f);
    posShift = - m_heli.localCenterOfMass;

    m_heli.localLandingGearPos = DirectX::SimpleMath::Vector3::Zero;
    m_heli.localLandingGearPos.y -= 0.869f;
    m_heli.landingGearPos = m_heli.localLandingGearPos;

    m_heli.localGravityTorqueArmPos = m_heli.localCenterOfMass;
    m_heli.localGravityTorqueArmPos.y += 1.0f;
    m_heli.gravityTorqueArmPos = m_heli.localGravityTorqueArmPos;

    RepositionModelCordinates(posShift, m_heliModel);

    // initialize tensors
    float xExtent = m_inertiaModelX;
    float yExtent = m_inertiaModelY;
    float zExtent = m_inertiaModelZ;

    float mass = m_inertiaMass;
    // cuboid
    m_heli.inertiaMatrix._11 = (1.0f / 12.0f) * (mass) * ((yExtent * yExtent) + (zExtent * zExtent));
    m_heli.inertiaMatrix._22 = (1.0f / 12.0f) * (mass) * ((xExtent * xExtent) + (zExtent * zExtent));
    m_heli.inertiaMatrix._33 = (1.0f / 12.0f) * (mass) * ((xExtent * xExtent) + (yExtent * yExtent));

    m_heli.localInertiaMatrix = m_heli.inertiaMatrix;
    m_heli.inverseInertiaMatrix = m_heli.inertiaMatrix;
    m_heli.inverseInertiaMatrix = m_heli.inverseInertiaMatrix.Invert();
    m_heli.localInverseInertiaMatrix = m_heli.inverseInertiaMatrix;
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
    DirectX::SimpleMath::Vector3 jumpVec(50.0f, 0.0f, 0.0f);
    jumpVec = DirectX::SimpleMath::Vector3::Transform(jumpVec, DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(45.0f)));
    //m_heli.q.velocity += jumpVec;
    const float jumpHeight = 10.0f;
    //m_heli.q.velocity.y += 50.0f;
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
    //m_heli.localLandingGearPos += aPos;

    aModel.localBodyRearBellyMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localBodyBellyMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localNoseBodyBellyMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localNoseConeBellyMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localMainWingBellyMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localMainWingTailEdgeBellyMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localMainWingLeadingBellyEdgeMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localWingJetForwardHousingLeftBellyMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localTailWingLeadingEdgeBellyMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localTailWingTrailingEdgeBellyMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localTailWingBellyMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);

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
    aModel.localWingJetRearHousingLeftMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localWingJetRearHousingRightMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localWingJetExhaustTipLeftMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localWingJetExhaustTipRightMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localSidePodMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    
    aModel.localSidestripeMatrix0 *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localSidestripeMatrix1 *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localSidestripeMatrix2 *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localSidestripeMatrix3 *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localSidestripeMatrix4 *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localSidestripeMatrix5 *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localWindShieldMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localNoseCowlMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localSideWindowUpperMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localSideWindowEyeMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localSideWindowLowerMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localSideWindowBottomMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localBodyRearMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);

    aModel.localLandingGearFrontMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localLandingGearRearLeftMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localLandingGearRearRightMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);

    aModel.localEngineHousingMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localEngineHousingFrontRightMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localEngineHousingFrontMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localEngineHousingRearMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localEngineHousingFrontLeftMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localEngineHousingIntakeLeftMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localEngineHousingIntakeRightMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);

    aModel.localEngineHousingIntakeCowlLeftMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localEngineHousingIntakeCowlRightMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);

    aModel.localEngineHousingRearLeftMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localEngineHousingRearRightMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localEngineHousingSideLeftMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localEngineHousingSideRightMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localEngineExhaustLeftMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localEngineExhaustRightMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);

    aModel.localEngineExhaustTipLeftMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localEngineExhaustTipRightMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);

    aModel.localTailBoomMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localTailBoomLowerMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localTailBoomUpperMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localTailBoomMidEndCapMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localTailBoomLowerEndCapMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localTailBoomUpperEndCapMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);

    aModel.localTailFinUpperMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localTailFinUpperTipMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localTailFinLowerMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localTailFinLowerTipMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localTailFinLeadingEdgeLowerMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localTailFinLeadingEdgeUpperMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localTailWingMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localTailWingLeadingEdgeMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localTailWingTrailingEdgeMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localTailWingFinLeftMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localTailWingFinRightMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localTailWingFinEdgeLeftMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localTailWingFinEdgeRightMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localMainRotorHubMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localMainRotorBladeMatrix1 *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);

    aModel.localMainRotorBladeInteriorMatrix1 *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localMainRotorBladeInteriorMatrix2 *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    // stripe 1
    aModel.localMainRotorBladeStripe1Matrix1 *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localMainRotorEdgeBladeStripe1Matrix1 *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localMainRotorStripe1Matrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localMainRotorArmStripe1Matrix1 *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localMainRotorStripe1Matrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    //
    // stripe 2
    aModel.localMainRotorStripe2Matrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localMainRotorBladeStripe2Matrix1 *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localMainRotorBladeStripe2Matrix2 *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localMainRotorEdgeStripe2Matrix1 *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localMainRotorEdgeStripe2Matrix2 *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localMainRotorArmStripe2Matrix1 *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localMainRotorArmStripe2Matrix2 *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    //
    aModel.localMainRotorBladeMatrix2 *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localMainRotorBladeEndCapMatrix1 *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localMainRotorBladeEndCapMatrix2 *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localMainRotorBladeEdgeEndCapMatrix1 *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localMainRotorBladeEdgeEndCapMatrix2 *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);

    aModel.localTailRotorAxelMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localTailRotorHubMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localTailRotorArmMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.tailRotorArmEndCapTranslationMatrix1 *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.tailRotorArmEndCapTranslationMatrix2 *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);

    aModel.tailRotorBladeTranslationMatrix1 *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.tailRotorBladeTranslationMatrix2 *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.tailRotorBladeEndCapTranslationMatrix1 *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.tailRotorBladeEndCapTranslationMatrix2 *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);

    aModel.tailRotorBladeEdgeTranslationMatrix1 *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.tailRotorBladeEdgeTranslationMatrix2 *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);

    aModel.tailRotorBladeEdgeEndCapTranslationMatrix1 *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.tailRotorBladeEdgeEndCapTranslationMatrix2 *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);

    aModel.localMainRotorEdgeBladeMatrix1 *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localMainRotorEdgeBladeMatrix2 *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);

    aModel.localMainRotorAxelMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localMainRotorArmMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localMainRotorArmEndCapMatrix1 *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.localMainRotorArmEndCapMatrix2 *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);

    aModel.tailRotorBladeInteriorTranslationMatrix1 *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.tailRotorBladeInteriorTranslationMatrix2 *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);

    aModel.tailRotorArmStripe1TranslationMatrix1 *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);

    aModel.tailRotorTranslationMatrix1 *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.tailRotorTranslationMatrix2 *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
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
    //rotorForce = aQ->mainRotorForceNormal;
    //rotorForce *= aQ->mainRotorForceMagnitude * m_heli.controlInput.collectiveInput;
    //rotorForce *= (m_heli.mainRotor.bladeVec[0].liftForcePerSecond + m_heli.mainRotor.bladeVec[1].liftForcePerSecond) / m_heli.mass;
    rotorForce *= (m_heli.mainRotor.bladeVec[0].liftForcePerSecond + m_heli.mainRotor.bladeVec[1].liftForcePerSecond);

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

    //DirectX::SimpleMath::Vector3 airResistance = velocityNorm * (static_cast<float>(aTimeDelta) * (-frontDragResistance / mass));
    DirectX::SimpleMath::Vector3 airResistance = velocityNorm * (-frontDragResistance);
    if (m_heli.isVelocityBackwards == true)
    {
        //airResistance *= -1.0;
    }

    DirectX::SimpleMath::Vector3 gravForce = m_heli.gravity * static_cast<float>(aTimeDelta);
    DirectX::SimpleMath::Vector3 terrainNormalForce = (m_heli.terrainNormal * -m_heli.gravity.y) * static_cast<float>(aTimeDelta);

    DirectX::SimpleMath::Vector3 velocityUpdate = DirectX::SimpleMath::Vector3::Zero;

    terrainNormalForce = DirectX::SimpleMath::Vector3::Zero;
    //velocityUpdate = aQ->velocity + static_cast<float>(aQScale) * aDeltaQ->velocity;
    velocityUpdate += m_heli.gravity * m_heli.mass;
    //velocityUpdate =  (m_heli.gravity.Length() * m_heli.mass) * -DirectX::SimpleMath::Vector3::UnitY * static_cast<float>(aTimeDelta);
    velocityUpdate += rotorForce;
    //velocityUpdate = (rotorForce + m_heli.gravity) * static_cast<float>(aTimeDelta);

    Utility::Torque pendTorque;
    pendTorque.axis = DirectX::SimpleMath::Vector3::Zero;
    pendTorque.magnitude = 0.0f;
    //DirectX::SimpleMath::Vector3 pendulumVelocity = DirectX::SimpleMath::Vector3::Zero;
    UpdatePendulumMotion(pendTorque, velocityUpdate, static_cast<float>(aTimeDelta));

    
    //UpdatePendulumMotion(pendTorque, pendulumVelocity, static_cast<float>(aTimeDelta));
    velocityUpdate += airResistance;
    Utility::Torque bodyTorqueUpdate = UpdateBodyTorqueTestRunge(pendTorque, static_cast<float>(aTimeDelta));


    //velocityUpdate = DirectX::SimpleMath::Vector3::Zero;
    //velocityUpdate = aQ->velocity + static_cast<float>(aQScale) * aDeltaQ->velocity;
    //velocityUpdate.y += (m_heli.gravity.y * m_heli.mass);// *static_cast<float>(aTimeDelta);
    //velocityUpdate =  (m_heli.gravity.Length() * m_heli.mass) * -DirectX::SimpleMath::Vector3::UnitY * static_cast<float>(aTimeDelta);
    //velocityUpdate += rotorForce * static_cast<float>(aTimeDelta);
    //velocityUpdate = (rotorForce + m_heli.gravity) * static_cast<float>(aTimeDelta);

    /*
    velocityUpdate = DirectX::SimpleMath::Vector3::Zero;
    velocityUpdate += ((rotorForce + (m_heli.gravity * 5.0f) + airResistance ) / 20.0f) * static_cast<float>(aTimeDelta);
    UpdatePendulumMotion(pendTorque, velocityUpdate, static_cast<float>(aTimeDelta));
    bodyTorqueUpdate = UpdateBodyTorqueTestRunge(pendTorque, static_cast<float>(aTimeDelta));
    */
    /// //
    /*
    velocityUpdate += pendulumVelocity;
    velocityUpdate += rotorForce;
    velocityUpdate += (m_heli.gravity) ;
    UpdatePendulumMotion(pendTorque, velocityUpdate, static_cast<float>(aTimeDelta));
    */
    //////////////////

    DirectX::SimpleMath::Vector3 momentumUpdate = newQ.angularMomentum;
    momentumUpdate *= m_angularDamping;
    momentumUpdate *= 0.0f;
    Utility::Torque bodyTorqueUpdateLocal = bodyTorqueUpdate;
    bodyTorqueUpdateLocal.axis = DirectX::SimpleMath::Vector3::Transform(bodyTorqueUpdateLocal.axis, m_heli.inverseAlignment);

    DirectX::SimpleMath::Vector3 localStabilityTorque = CalculateStabilityTorqueLocal(m_heli, static_cast<float>(aTimeDelta));
    if (m_debugToggle == true || m_debugToggle2 == true)
    {
        //localStabilityTorque = DirectX::SimpleMath::Vector3::Zero;
    }
    DirectX::SimpleMath::Vector3 torqueForce = bodyTorqueUpdateLocal.axis * bodyTorqueUpdateLocal.magnitude;;
    torqueForce += momentumUpdate;
    //torqueForce -= momentumUpdate * m_angularDamping;
    torqueForce += localStabilityTorque;
    torqueForce += (m_heli.angularDrag / aTimeDelta);
    DirectX::SimpleMath::Vector3 zero = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 one = DirectX::SimpleMath::Vector3::Zero;
    //DirectX::SimpleMath::Vector3 AirplanevAngularVelocity = newQ.angularVelocity;
    DirectX::SimpleMath::Vector3 AirplanevAngularVelocity = newQ.angularMomentum;
    AirplanevAngularVelocity = momentumUpdate;
    DirectX::SimpleMath::Vector3 testAngVel = DirectX::SimpleMath::Vector3::Zero;
    one = AirplanevAngularVelocity;
    one = DirectX::SimpleMath::Vector3::Transform(one, m_heli.localInertiaMatrix);
    zero = torqueForce - (AirplanevAngularVelocity.Cross(one));
    testAngVel += DirectX::SimpleMath::Vector3::Transform(zero, m_heli.localInverseInertiaMatrix);
    DirectX::SimpleMath::Vector3 momentsInput = momentumUpdate;
    DirectX::SimpleMath::Vector3 vAngularVelocity = torqueForce;

    DirectX::SimpleMath::Vector3 toCrossVelocity = vAngularVelocity;
    toCrossVelocity = DirectX::SimpleMath::Vector3::Transform(toCrossVelocity, m_heli.localInertiaMatrix); 

    DirectX::SimpleMath::Vector3 toInverseInertiaVelocity = vAngularVelocity;
    DirectX::SimpleMath::Vector3 testCross = vAngularVelocity;
    testCross = testCross.Cross(toCrossVelocity);

    toInverseInertiaVelocity = (momentsInput - (vAngularVelocity.Cross(toCrossVelocity)));

    toInverseInertiaVelocity = DirectX::SimpleMath::Vector3::Transform(toInverseInertiaVelocity, m_heli.localInverseInertiaMatrix);
    vAngularVelocity = toInverseInertiaVelocity;

    DirectX::SimpleMath::Vector3 updatedAngularVelocity = torqueForce;
    updatedAngularVelocity = testAngVel;
    //updatedAngularVelocity = vAngularVelocity;

    aDQ->angularVelocity = static_cast<float>(aTimeDelta) * updatedAngularVelocity;
    aDQ->angularMomentum = static_cast<float>(aTimeDelta) * torqueForce;
    //////////////////
    
    //  Assign right-hand side values.
    aDQ->velocity = static_cast<float>(aTimeDelta) * (velocityUpdate / m_heli.mass);
    aDQ->airResistance = airResistance;
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
        //m_heli.q.velocity = DirectX::SimpleMath::Vector3::Zero;
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
    
    DirectX::SimpleMath::Vector3 velocityUpdate = (dq1.velocity + 2.0 * dq2.velocity + 2.0 * dq3.velocity + dq4.velocity) / numEqns;

    DirectX::SimpleMath::Vector3 bodyVelocityyUpdate = (dq1.bodyVelocity + 2.0 * dq2.bodyVelocity + 2.0 * dq3.bodyVelocity + dq4.bodyVelocity) / numEqns;
    DirectX::SimpleMath::Vector3 engineVelocityUpdate = (dq1.engineForce + 2.0 * dq2.engineForce + 2.0 * dq3.engineForce + dq4.engineForce) / numEqns;
    DirectX::SimpleMath::Vector3 airResistnaceVelocityUpdate = (dq1.airResistance + 2.0 * dq2.airResistance + 2.0 * dq3.airResistance + dq4.airResistance) / numEqns;
    DirectX::SimpleMath::Vector3 totalVelocityUpdate = (dq1.totalVelocity + 2.0 * dq2.totalVelocity + 2.0 * dq3.totalVelocity + dq4.totalVelocity) / numEqns;

    DirectX::SimpleMath::Vector3 angularVelocityUpdate = (dq1.angularVelocity + 2.0 * dq2.angularVelocity + 2.0 * dq3.angularVelocity + dq4.angularVelocity) / numEqns;
    DirectX::SimpleMath::Vector3 angularMomentumUpdate = (dq1.angularMomentum + 2.0 * dq2.angularMomentum + 2.0 * dq3.angularMomentum + dq4.angularMomentum) / numEqns;

    Utility::Torque bodyTorqueUpdate;
    bodyTorqueUpdate.axis = (dq1.bodyTorqueForce.axis + 2.0 * dq2.bodyTorqueForce.axis + 2.0 * dq3.bodyTorqueForce.axis + dq4.bodyTorqueForce.axis) / numEqns;
    bodyTorqueUpdate.magnitude = (dq1.bodyTorqueForce.magnitude + 2.0f * dq2.bodyTorqueForce.magnitude + 2.0f * dq3.bodyTorqueForce.magnitude + dq4.bodyTorqueForce.magnitude) / numEqns;

    if (dq1.velocity.Length() != dq2.velocity.Length())
    {
        float v1 = dq1.velocity.Length();
        float v2 = dq2.velocity.Length();
        int testBreak = 0;
        testBreak++;
    }
    // To prevent the vehicle from continuing to roll forward if vehicle velocity is less than the tolerance value and update velocity is zero
    const float stopTolerance = 0.1;
    if (q.velocity.Length() < stopTolerance && velocityUpdate == DirectX::SimpleMath::Vector3::Zero)
    {
        //q.velocity = DirectX::SimpleMath::Vector3::Zero;
    }
    else
    {
        //q.velocity += velocityUpdate;
    }

    if (q.position.y + posUpdate.y <= aHeli->terrainHightAtPos - aHeli->localLandingGearPos.y)
    {
        if (q.velocity.y + velocityUpdate.y <= aHeli->gravity.y * aTimeDelta)
        {
            velocityUpdate = DirectX::SimpleMath::Vector3::Zero;
            q.velocity = DirectX::SimpleMath::Vector3::Zero;
            posUpdate = DirectX::SimpleMath::Vector3::Zero;
            aHeli->q.position.y = aHeli->terrainHightAtPos + - aHeli->localLandingGearPos.y;
            q.position.y = aHeli->terrainHightAtPos + -aHeli->localLandingGearPos.y;
            bodyTorqueUpdate.magnitude = 0.0f;
            q.bodyTorqueForce.magnitude = 0.0f;

        }
    }
    
    q.velocity += velocityUpdate;
    q.engineForce += engineVelocityUpdate;
    q.airResistance += airResistnaceVelocityUpdate;
    //q.gravityForce += gravityVelocityUpdate;
    q.totalVelocity += velocityUpdate;
    q.bodyTorqueForce.axis += bodyTorqueUpdate.axis;
    q.bodyTorqueForce.magnitude += bodyTorqueUpdate.magnitude;
    q.position += posUpdate;
    
    q.angularVelocity += angularVelocityUpdate;
    q.angularMomentum += angularMomentumUpdate;

    if (m_heli.isVehicleAirborne == false && q.velocity.y <= 0.0f)
    {
        aHeli->q.velocity = DirectX::SimpleMath::Vector3::Zero;
    }
    else
    {
        aHeli->q.velocity = q.velocity;
        aHeli->q.position = q.position;
    }
    
    aHeli->q.engineForce = q.engineForce;
    //aHeli->q.position = q.position;
    //aHeli->q.velocity = q.velocity;
    //aHeli->q.torqueForceMat = q.torqueForceMat;
    aHeli->q.bodyTorqueForce = q.bodyTorqueForce;
    aHeli->q.angularVelocity = q.angularVelocity;
    aHeli->q.angularMomentum = q.angularMomentum;
}

void Vehicle::SetDebugData(std::shared_ptr<DebugData> aDebugPtr)
{
    m_debugData = aDebugPtr;
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
    surface *= 1.0f;
    float y = m_heli.mainRotor.width;
    const float omega = (m_heli.mainRotor.rpm * 0.10472f) * aTimeStep; // 0.10472 is conversion of RPM to rads per second
    const float omega2 = (m_heli.mainRotor.rpm * 0.10472f);
    float lift = .5f * rho * omega * omega * Cl * y;
   
    m_heli.mainRotor.bladeVec[0].liftMag = lift;
    m_heli.mainRotor.bladeVec[0].liftForcePerSecond = .5f * rho * omega2 * omega2 * Cl * surface;
    Cl = CalculateLiftCoefficient(m_heli.mainRotor.bladeVec[1].pitchAngle);
    lift = .5f * rho * omega * omega * Cl * y;
    m_heli.mainRotor.bladeVec[1].liftMag = lift;
    m_heli.mainRotor.bladeVec[1].liftForcePerSecond = .5f * rho * omega2 * omega2 * Cl * surface;

    lift = .5f * rho * omega2 * omega2 * Cl * y;
    
    m_heli.mainRotor.bladeVec[0].liftForcePerSecond = UpdateGroundEffectForce(m_heli.mainRotor.bladeVec[0].liftForcePerSecond);
    m_heli.mainRotor.bladeVec[1].liftForcePerSecond = UpdateGroundEffectForce(m_heli.mainRotor.bladeVec[1].liftForcePerSecond);
    //UpdateGroundEffectForce(m_heli.mainRotor.bladeVec[0].liftForcePerSecond + m_heli.mainRotor.bladeVec[1].liftForcePerSecond);
    // L = .5 * rho * omega^2 * Cl * y integral(r1 -> r2)  r^2 * dr    
}

void Vehicle::UpdateAlignmentTorqueTest(const float aTimeDelta)
{
    m_heli.cameraOrientationPrevious = m_heli.cameraOrientation;

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

        ////////////////////
        if (m_isUseNewPhysicsTrue == true)
        {
            DirectX::SimpleMath::Quaternion updateAlignQuat;
            updateAlignQuat.x = m_heli.q.angularVelocity.x;
            updateAlignQuat.y = m_heli.q.angularVelocity.y;
            updateAlignQuat.z = m_heli.q.angularVelocity.z;
            updateAlignQuat.w = 0.0f;
            DirectX::SimpleMath::Quaternion updateAlignOutputQuat = m_heli.q.angularQuat;
            updateAlignOutputQuat.Normalize();
            m_heli.q.angularQuat.Normalize();
            updateAlignOutputQuat += (0.5f * aTimeDelta) * (updateAlignQuat * m_heli.q.angularQuat);
            updateAlignOutputQuat.Normalize();
            m_heli.q.angularQuat = updateAlignOutputQuat;

            m_heli.alignment = DirectX::SimpleMath::Matrix::CreateFromQuaternion(m_heli.q.angularQuat);
            float rotVal = Utility::ToRadians(90.0f);
            DirectX::SimpleMath::Matrix rotMat = DirectX::SimpleMath::Matrix::CreateFromYawPitchRoll(0.0, rotVal, 0.0);
            //m_heli.alignment = rotMat;
            m_heli.inverseAlignment = m_heli.alignment;
            m_heli.inverseAlignment = m_heli.inverseAlignment.Invert();
            m_heli.alignmentQuat = m_heli.q.angularQuat;
            m_heli.inverseAlignmentQuat = m_heli.alignmentQuat;
            m_heli.inverseAlignmentQuat.Inverse(m_heli.inverseAlignmentQuat);
        }
        ////////////////////

        m_heli.up = DirectX::SimpleMath::Vector3::TransformNormal(DirectX::SimpleMath::Vector3::UnitY, m_heli.alignment);
        m_heli.right = DirectX::SimpleMath::Vector3::TransformNormal(DirectX::SimpleMath::Vector3::UnitZ, m_heli.alignment);
        m_heli.forward = DirectX::SimpleMath::Vector3::TransformNormal(DirectX::SimpleMath::Vector3::UnitX, m_heli.alignment);

        m_heli.alignment = DirectX::SimpleMath::Matrix::CreateWorld(DirectX::SimpleMath::Vector3::Zero, -m_heli.right, m_heli.up);
    }

    m_heli.cameraOrientation = DirectX::XMMatrixLookAtRH(DirectX::SimpleMath::Vector3::Zero, -m_heli.right, m_heli.up);
    m_heli.cameraOrientation = DirectX::SimpleMath::Matrix::CreateWorld(DirectX::SimpleMath::Vector3::Zero, -m_heli.right, m_heli.up);
}

void Vehicle::UpdateAlignmentCamera()
{
    //m_debugLinesVec.clear();
    float yaw = Utility::GetAngleBetweenVectors(DirectX::SimpleMath::Vector3::UnitX, m_heli.forward);
    float pitch = Utility::GetAngleBetweenVectors(DirectX::SimpleMath::Vector3::UnitY, m_heli.up);
    float roll = Utility::GetAngleBetweenVectors(-DirectX::SimpleMath::Vector3::UnitZ, m_heli.right);
    roll = 0.0f;
    pitch = asin(m_heli.forward.y);
    yaw = atan2(-m_heli.forward.z, m_heli.forward.x);
    //DebugPushUILineDecimalNumber("yaw : ", Utility::ToDegrees(yaw), "");
    //DebugPushUILineDecimalNumber("pitch: ", Utility::ToDegrees(pitch), "");
    //DebugPushUILineDecimalNumber("roll : ", Utility::ToDegrees(roll), "");
    
    //DirectX::SimpleMath::Matrix alignMat = DirectX::SimpleMath::Matrix::CreateFromYawPitchRoll(yaw, pitch, roll);
    //DirectX::SimpleMath::Matrix alignMat = DirectX::SimpleMath::Matrix::CreateFromYawPitchRoll(yaw, 0.0f, pitch);
    DirectX::SimpleMath::Matrix alignMat = DirectX::SimpleMath::Matrix::CreateFromYawPitchRoll(yaw, 0.0f, 0.0f);
    DirectX::SimpleMath::Vector3 testLine = DirectX::SimpleMath::Vector3::UnitX;
    testLine = DirectX::SimpleMath::Vector3::Transform(testLine, alignMat);
    //DebugPushTestLine(m_heli.q.position, DirectX::SimpleMath::Vector3::UnitX, 25.0f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 0.0f, 0.0f, 1.0f));
    //DebugPushTestLine(m_heli.q.position, DirectX::SimpleMath::Vector3::UnitZ, 25.0f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 0.0f, 0.0f, 1.0f));
    //DebugPushTestLine(m_heli.q.position, testLine, 25.0f, 2.0f, DirectX::SimpleMath::Vector4(0.0f, 1.0f, 0.0f, 1.0f));
    //DebugPushTestLine(m_heli.q.position, m_heli.forward, 25.0f, 0.0f, DirectX::SimpleMath::Vector4(0.0f, 0.0f, 1.0f, 1.0f));
    testLine = DirectX::SimpleMath::Vector3::UnitX;
    testLine = DirectX::SimpleMath::Vector3::Transform(testLine, m_heli.cameraOrientation);
    //DebugPushTestLine(m_heli.q.position, testLine, 25.0f, 1.0f, DirectX::SimpleMath::Vector4(1.0f, 0.0f, 0.0f, 1.0f));
    
    m_heli.cameraOrientation = alignMat;
}

DirectX::SimpleMath::Vector3 Vehicle::GetFollowPos() const
{
    DirectX::SimpleMath::Vector3 followPos = -m_heli.forward;
    followPos.y = 0.0f;
    followPos.Normalize();
    return followPos;
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

    //mainRotorForce = m_heli.controlInput.cyclicStick * ((m_heli.mainRotor.bladeVec[0].liftForcePerSecond + m_heli.mainRotor.bladeVec[1].liftForcePerSecond) / 200.0) * modVal;
    
    mainRotorForce = UpdateRotorForceRunge();
    //rotorForce *= aQ->mainRotorForceMagnitude * m_heli.controlInput.collectiveInput;
    //mainRotorForce *= ((m_heli.mainRotor.bladeVec[0].liftForcePerSecond + m_heli.mainRotor.bladeVec[1].liftForcePerSecond) / m_heli.mass) * modVal;
    //mainRotorForce *= ((m_heli.mainRotor.bladeVec[0].liftForcePerSecond + m_heli.mainRotor.bladeVec[1].liftForcePerSecond) / m_heli.mass) * modVal;
    if (m_isUseNewPhysicsTrue == true)
    {
        //mainRotorForce *= ((m_heli.mainRotor.bladeVec[0].liftForcePerSecond + m_heli.mainRotor.bladeVec[1].liftForcePerSecond) / m_heli.mass);
        mainRotorForce *= ((m_heli.mainRotor.bladeVec[0].liftForcePerSecond + m_heli.mainRotor.bladeVec[1].liftForcePerSecond));// / m_heli.mass);
    }
    else
    {
        mainRotorForce *= ((m_heli.mainRotor.bladeVec[0].liftForcePerSecond + m_heli.mainRotor.bladeVec[1].liftForcePerSecond) / m_heli.mass) * modVal;
    }
    //m_debugData->PushDebugLine(m_heli.mainRotorPos, mainRotorForce, 10.0f, 0.0f, DirectX::Colors::Red);
    //m_debugData->DebugPushUILineDecimalNumber("mainRotorForce.Length()", mainRotorForce.Length(), "");
    //mainRotorForce = DirectX::SimpleMath::Vector3::Transform(mainRotorForce, m_heli.alignment);
    const float windVaning = WindVaningVal(m_heli, aTimeStep);
    m_debugData->DebugPushUILineDecimalNumber("windVaning", windVaning, "");
    //DirectX::SimpleMath::Vector3 tailForce = -m_heli.right * (m_heli.controlInput.yawPedalInput + windVaning) * modVal;
    DirectX::SimpleMath::Vector3 tailForce = -m_heli.right * (m_heli.controlInput.yawPedalInput + windVaning) * m_heli.tailRotorForceMagMax;

    if (m_debugToggle == true)
    {
        tailForce = -m_heli.right * (m_heli.controlInput.yawPedalInput) * m_heli.tailRotorForceMagMax;
    }
    if (m_debugToggle2 == true)
    {
        tailForce = -m_heli.right * (m_heli.controlInput.yawPedalInput + (windVaning * 0.2f)) * m_heli.tailRotorForceMagMax;
    }

    m_debugData->DebugPushUILineDecimalNumber("tailForce.Length() ", tailForce.Length(), "");
    //m_debugData->PushDebugLine(m_heli.tailRotorPos, tailForce, 10.0f, 0.0f, DirectX::Colors::White);

    /*
    float rotorMag = (m_heli.q.mainRotorForceMagnitude * m_heli.controlInput.collectiveInput) * modVal;
    mainRotorForce = m_heli.controlInput.cyclicStick;
    mainRotorForce = DirectX::SimpleMath::Vector3::Transform(mainRotorForce, m_heli.alignment);
    mainRotorForce.Normalize();
    mainRotorForce = mainRotorForce * rotorMag;
    */
    DirectX::SimpleMath::Vector3 gravityForce = (m_heli.gravity) * modVal;
    float gravMag = (m_heli.gravity.y) * modVal;
    gravityForce = m_heli.gravity;
    gravityForce.Normalize();
    gravityForce = gravityForce * (4.8 * modVal);
    if (m_isUseNewPhysicsTrue == true)
    {
        gravityForce = m_heli.gravity * m_heli.mass;
    }

    Utility::Torque rotorTorque = Utility::GetTorqueForce(mainRotorTorqueArm, mainRotorForce);
    Utility::Torque tailTorque = Utility::GetTorqueForce(tailRotorTorqueArm, tailForce);
    Utility::Torque gravTorque = Utility::GetTorqueForce(gravityTorqueArm, gravityForce);

    //Utility::Torque testWindVaning = WindVaning(m_heli, aTimeStep);
    //DirectX::SimpleMath::Vector3 torqueAxis = (rotorTorque.axis * rotorTorque.magnitude) + (tailTorque.axis * tailTorque.magnitude) + (gravTorque.axis * gravTorque.magnitude);
    //DirectX::SimpleMath::Vector3 torqueAxis = (rotorTorque.axis * rotorTorque.magnitude) + (gravTorque.axis * gravTorque.magnitude);
    DirectX::SimpleMath::Vector3 torqueAxis = (rotorTorque.axis * rotorTorque.magnitude) + (tailTorque.axis * tailTorque.magnitude);
    //torqueAxis = (rotorTorque.axis * rotorTorque.magnitude) + (tailTorque.axis * tailTorque.magnitude) + (aPendulumTorque.axis * aPendulumTorque.magnitude) + (testWindVaning.axis * testWindVaning.magnitude);
    torqueAxis = (rotorTorque.axis * rotorTorque.magnitude) + (tailTorque.axis * tailTorque.magnitude) + (aPendulumTorque.axis * aPendulumTorque.magnitude);
    torqueAxis.Normalize();
    //const float torqueMag = rotorTorque.magnitude + tailTorque.magnitude + gravTorque.magnitude;
    //const float torqueMag = rotorTorque.magnitude  + gravTorque.magnitude;
    float torqueMag = rotorTorque.magnitude + tailTorque.magnitude;
    //torqueMag = rotorTorque.magnitude + tailTorque.magnitude + aPendulumTorque.magnitude + testWindVaning.magnitude;
    torqueMag = rotorTorque.magnitude + tailTorque.magnitude + aPendulumTorque.magnitude;

    DirectX::SimpleMath::Vector3 testRotorTorque = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 testRotorForce = DirectX::SimpleMath::Vector3::Zero;
    //rotorPos - centerMassPos;
    Utility::AddForceAtPoint(mainRotorForce, rotorPos, centerMassPos, testRotorForce, testRotorTorque);

    //m_debugData->PushDebugLine(m_heli.q.position, testRotorTorque, 10.0f, 0.0f, DirectX::Colors::White);
    //m_debugData->PushDebugLine(m_heli.q.position, (rotorTorque.axis * rotorTorque.magnitude), 10.0f, 0.5f, DirectX::Colors::White);
    m_debugData->DebugPushUILineDecimalNumber("testRotorTorque.Length() ", testRotorTorque.Length(), "");
    m_debugData->DebugPushUILineDecimalNumber("(rotor.axis * .magnitude) ", (rotorTorque.axis * rotorTorque.magnitude).Length(), "");

    
    //torqueAxis = (rotorTorque.axis * rotorTorque.magnitude) + (tailTorque.axis * tailTorque.magnitude);
    torqueAxis = (testRotorTorque) + (tailTorque.axis * tailTorque.magnitude);
    torqueAxis.Normalize();
    //torqueMag = rotorTorque.magnitude + tailTorque.magnitude;
    torqueMag = testRotorTorque.Length() + tailTorque.magnitude;

    /*
    torqueAxis = (rotorTorque.axis * rotorTorque.magnitude) + (tailTorque.axis * tailTorque.magnitude) + (aPendulumTorque.axis * aPendulumTorque.magnitude) 
        + (m_heli.centrifugalForce);
    torqueAxis.Normalize();
    torqueMag = rotorTorque.magnitude + tailTorque.magnitude + aPendulumTorque.magnitude + m_heli.centrifugalForce.Length();
    */

    /*
    if (m_debugToggle == true)
    {
        torqueAxis = (rotorTorque.axis * rotorTorque.magnitude) + (tailTorque.axis * tailTorque.magnitude) + (m_heli.centrifugalForce);
        torqueAxis.Normalize();
        torqueMag = rotorTorque.magnitude + tailTorque.magnitude + m_heli.centrifugalForce.Length();
    }

    if (m_debugToggle2 == true)
    {
        torqueAxis = (rotorTorque.axis * rotorTorque.magnitude) + (tailTorque.axis * tailTorque.magnitude) + (aPendulumTorque.axis * aPendulumTorque.magnitude);
        torqueAxis.Normalize();
        torqueMag = rotorTorque.magnitude + tailTorque.magnitude + aPendulumTorque.magnitude;
    }
    */

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

Utility::Torque Vehicle::UpdateBodyTorqueTestRungeOld(Utility::Torque aPendulumTorque, const float aTimeStep)
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

    //mainRotorForce = m_heli.controlInput.cyclicStick * ((m_heli.mainRotor.bladeVec[0].liftForcePerSecond + m_heli.mainRotor.bladeVec[1].liftForcePerSecond) / 200.0) * modVal;

    mainRotorForce = UpdateRotorForceRunge();
    //rotorForce *= aQ->mainRotorForceMagnitude * m_heli.controlInput.collectiveInput;
    //mainRotorForce *= ((m_heli.mainRotor.bladeVec[0].liftForcePerSecond + m_heli.mainRotor.bladeVec[1].liftForcePerSecond) / m_heli.mass) * modVal;
    //mainRotorForce *= ((m_heli.mainRotor.bladeVec[0].liftForcePerSecond + m_heli.mainRotor.bladeVec[1].liftForcePerSecond) / m_heli.mass) * modVal;
    if (m_isUseNewPhysicsTrue == true)
    {
        mainRotorForce *= ((m_heli.mainRotor.bladeVec[0].liftForcePerSecond + m_heli.mainRotor.bladeVec[1].liftForcePerSecond) / m_heli.mass);
    }
    else
    {
        mainRotorForce *= ((m_heli.mainRotor.bladeVec[0].liftForcePerSecond + m_heli.mainRotor.bladeVec[1].liftForcePerSecond) / m_heli.mass) * modVal;
    }

    //mainRotorForce = DirectX::SimpleMath::Vector3::Transform(mainRotorForce, m_heli.alignment);
    const float windVaning = WindVaningVal(m_heli, aTimeStep);
    DirectX::SimpleMath::Vector3 tailForce = -m_heli.right * (m_heli.controlInput.yawPedalInput + windVaning) * modVal;
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
    if (m_isUseNewPhysicsTrue == true)
    {
        gravityForce = m_heli.gravity * m_heli.mass;
    }

    Utility::Torque rotorTorque = Utility::GetTorqueForce(mainRotorTorqueArm, mainRotorForce);
    Utility::Torque tailTorque = Utility::GetTorqueForce(tailRotorTorqueArm, tailForce);
    Utility::Torque gravTorque = Utility::GetTorqueForce(gravityTorqueArm, gravityForce);

    //Utility::Torque testWindVaning = WindVaning(m_heli, aTimeStep);
    //DirectX::SimpleMath::Vector3 torqueAxis = (rotorTorque.axis * rotorTorque.magnitude) + (tailTorque.axis * tailTorque.magnitude) + (gravTorque.axis * gravTorque.magnitude);
    //DirectX::SimpleMath::Vector3 torqueAxis = (rotorTorque.axis * rotorTorque.magnitude) + (gravTorque.axis * gravTorque.magnitude);
    DirectX::SimpleMath::Vector3 torqueAxis = (rotorTorque.axis * rotorTorque.magnitude) + (tailTorque.axis * tailTorque.magnitude);
    //torqueAxis = (rotorTorque.axis * rotorTorque.magnitude) + (tailTorque.axis * tailTorque.magnitude) + (aPendulumTorque.axis * aPendulumTorque.magnitude) + (testWindVaning.axis * testWindVaning.magnitude);
    torqueAxis = (rotorTorque.axis * rotorTorque.magnitude) + (tailTorque.axis * tailTorque.magnitude) + (aPendulumTorque.axis * aPendulumTorque.magnitude);
    torqueAxis.Normalize();
    //const float torqueMag = rotorTorque.magnitude + tailTorque.magnitude + gravTorque.magnitude;
    //const float torqueMag = rotorTorque.magnitude  + gravTorque.magnitude;
    float torqueMag = rotorTorque.magnitude + tailTorque.magnitude;
    //torqueMag = rotorTorque.magnitude + tailTorque.magnitude + aPendulumTorque.magnitude + testWindVaning.magnitude;
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

void Vehicle::UpdateCentrifugalForce(const float aTimeStep)
{
    DirectX::SimpleMath::Vector3 prevFrame = m_heli.centrifugalForcePrevFrame;
    DirectX::SimpleMath::Quaternion prevQuatFrame = m_heli.centrifugalForcePrevQuatFrame;
    DirectX::SimpleMath::Quaternion r = prevQuatFrame - m_heli.q.angularQuat;
    float rLength = r.Length();
    float massAtADistanceR = m_heli.mass;
    DirectX::SimpleMath::Vector3 angVel = m_heli.q.angularVelocity;
    DirectX::SimpleMath::Vector3 updateForce = m_heli.mass * (angVel * angVel) * r;
    updateForce /= aTimeStep;

    m_heli.centrifugalForce = updateForce;
    m_heli.centrifugalForcePrevFrame = m_heli.q.angularVelocity;
    m_heli.centrifugalForcePrevQuatFrame = m_heli.q.angularQuat;

    m_debugData->DebugPushUILineDecimalNumber("m_heli.centrifugalForce.Length()", m_heli.centrifugalForce.Length(), "");
    m_debugData->PushDebugLine(m_heli.q.position, updateForce, 15.0f, 0.0f, DirectX::Colors::Red);

    /////////////////////////////////////////////////////////////////////////////////////////////////////////

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
    DirectX::SimpleMath::Vector3 mainRotorForce = UpdateRotorForceRunge();
    //rotorForce *= aQ->mainRotorForceMagnitude * m_heli.controlInput.collectiveInput;
    //mainRotorForce *= ((m_heli.mainRotor.bladeVec[0].liftForcePerSecond + m_heli.mainRotor.bladeVec[1].liftForcePerSecond) / m_heli.mass) * modVal;
    //mainRotorForce *= ((m_heli.mainRotor.bladeVec[0].liftForcePerSecond + m_heli.mainRotor.bladeVec[1].liftForcePerSecond) / m_heli.mass) * modVal;
    //mainRotorForce *= ((m_heli.mainRotor.bladeVec[0].liftForcePerSecond + m_heli.mainRotor.bladeVec[1].liftForcePerSecond) / m_heli.mass);
    mainRotorForce *= ((m_heli.mainRotor.bladeVec[0].liftForcePerSecond + m_heli.mainRotor.bladeVec[1].liftForcePerSecond));// / m_heli.mass);


   // m_debugData->DebugPushUILineDecimalNumber("mainRotorForce.Length()        ", mainRotorForce.Length(), "");
    //m_debugData->PushDebugLine(m_heli.q.position, mainRotorForce, 15.0f, 0.0f, DirectX::Colors::Blue);
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DirectX::SimpleMath::Vector3 bodyTorqueForce = m_heli.q.bodyTorqueForce.axis * m_heli.q.bodyTorqueForce.magnitude;
    //m_debugData->DebugPushUILineDecimalNumber("bodyTorqueForce.Length()        ", bodyTorqueForce.Length(), "");
    //m_debugData->PushDebugLine(m_heli.q.position, bodyTorqueForce, 15.0f, 0.0f, DirectX::Colors::Yellow);

    DirectX::SimpleMath::Vector3 pendTorqueForce = m_heli.q.pendulumTorqueForceTest.axis * m_heli.q.pendulumTorqueForceTest.magnitude;
    m_debugData->DebugPushUILineDecimalNumber("pendTorqueForce.Length()        ", pendTorqueForce.Length(), "");
    m_debugData->PushDebugLine(m_heli.q.position, pendTorqueForce, 15.0f, 1.0f, DirectX::Colors::Blue);

    m_debugData->DebugPushUILineDecimalNumber("m_heli.angularDrag.Length()       ", m_heli.angularDrag.Length(), "");
    m_debugData->PushDebugLine(m_heli.q.position, m_heli.angularDrag, 15.0f, 2.0f, DirectX::Colors::Yellow);

    m_debugData->DebugPushUILineDecimalNumber("m_heli.angularDrag2.Length()       ", m_heli.angularDrag2.Length(), "");
    m_debugData->PushDebugLine(m_heli.q.position, m_heli.angularDrag2, 15.0f, 4.0f, DirectX::Colors::Aqua);

    DirectX::SimpleMath::Vector3 stabilityTorqueLocal = CalculateStabilityTorqueLocal(m_heli, aTimeStep);
    DirectX::SimpleMath::Vector3 stabilityTorqueWorld = stabilityTorqueLocal;
    stabilityTorqueWorld.Transform(stabilityTorqueWorld, m_heli.alignment);

    m_debugData->DebugPushUILineDecimalNumber("stabilityTorqueWorld.Length()       ", stabilityTorqueWorld.Length(), "");
    m_debugData->PushDebugLine(m_heli.q.position, stabilityTorqueWorld, 15.0f, 3.0f, DirectX::Colors::White);

    m_debugData->DebugPushUILineWholeNumber("m_debugToggle = ", m_debugToggle, "");
    m_debugData->DebugPushUILineWholeNumber("m_debugToggle2 = ", m_debugToggle2, "");

    m_debugData->DebugClearUI();
}

float Vehicle::WindVaningVal(const HeliData& aHeliData, const float aTimeStep)
{
    DirectX::SimpleMath::Vector3 localizedAirVelocity = -aHeliData.q.velocity;
    DirectX::SimpleMath::Matrix localizeMat = aHeliData.alignment;
    localizeMat = localizeMat.Invert();
    localizedAirVelocity = DirectX::SimpleMath::Vector3::Transform(localizedAirVelocity, localizeMat);

    DirectX::SimpleMath::Vector3 airVelocityNormXZ = localizedAirVelocity;
    airVelocityNormXZ.y = 0.0f;
    airVelocityNormXZ.Normalize();
    float ratio = airVelocityNormXZ.Dot(DirectX::SimpleMath::Vector3::UnitZ);

    float v = aHeliData.q.velocity.Length();
    float airDensity = aHeliData.airDensity;
    float dragCoefficient = aHeliData.dragCoefficient;
    float surfaceArea = aHeliData.area;
    float dragResistance = 0.5f * airDensity * surfaceArea * dragCoefficient * v * v;
    //float windVaning = -ratio * dragResistance * 0.0000005f;
    float windVaning = (-ratio * dragResistance) / 2000.0f;
    return windVaning;
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

float Vehicle::UpdateGroundEffectForce(const float aLiftForce)
{
    const float wingSpan = m_heli.mainRotor.length * 3.0f;
    const float altitude = m_heli.q.position.y - m_heli.terrainHightAtPos - 3.0f; 
    const float groundEffectFactor = ((16.0f * altitude / wingSpan) * (16.0f * altitude / wingSpan)) / (1.0f + ((16.0f * altitude / wingSpan) * (16.0f * altitude / wingSpan)));

    float liftMod = abs(1.0f - groundEffectFactor);
    float groundEffectLift = aLiftForce * liftMod;
    groundEffectLift += aLiftForce;
    return groundEffectLift;
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
    DirectX::SimpleMath::Matrix updateMat = DirectX::SimpleMath::Matrix::CreateWorld(m_heli.q.position, -m_heli.right, m_heli.up);

    // belly shapes
    m_heliModel.bodyRearBellyMatrix = m_heliModel.localBodyRearBellyMatrix;
    m_heliModel.bodyRearBellyMatrix *= updateMat;
    m_heliModel.bodyBellyMatrix = m_heliModel.localBodyBellyMatrix;
    m_heliModel.bodyBellyMatrix *= updateMat;
    m_heliModel.noseBodyBellyMatrix = m_heliModel.localNoseBodyBellyMatrix;
    m_heliModel.noseBodyBellyMatrix *= updateMat;
    m_heliModel.noseConeBellyMatrix = m_heliModel.localNoseConeBellyMatrix;
    m_heliModel.noseConeBellyMatrix *= updateMat;
    m_heliModel.mainWingBellyMatrix = m_heliModel.localMainWingBellyMatrix;
    m_heliModel.mainWingBellyMatrix *= updateMat;
    m_heliModel.mainWingTailEdgeBellyMatrix = m_heliModel.localMainWingTailEdgeBellyMatrix;
    m_heliModel.mainWingTailEdgeBellyMatrix *= updateMat;
    m_heliModel.mainWingLeadingEdgeBellyMatrix = m_heliModel.localMainWingLeadingBellyEdgeMatrix;
    m_heliModel.mainWingLeadingEdgeBellyMatrix *= updateMat;
    m_heliModel.wingJetForwardHousingLeftBellyMatrix = m_heliModel.localWingJetForwardHousingLeftBellyMatrix;
    m_heliModel.wingJetForwardHousingLeftBellyMatrix *= updateMat;

    m_heliModel.tailWingLeadingEdgeBellyMatrix = m_heliModel.localTailWingLeadingEdgeBellyMatrix;
    m_heliModel.tailWingLeadingEdgeBellyMatrix *= updateMat;

    m_heliModel.tailWingTrailingEdgeBellyMatrix = m_heliModel.localTailWingTrailingEdgeBellyMatrix;
    m_heliModel.tailWingTrailingEdgeBellyMatrix *= updateMat;

    m_heliModel.tailWingBellyMatrix = m_heliModel.localTailWingBellyMatrix;
    m_heliModel.tailWingBellyMatrix *= updateMat;

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

    m_heliModel.wingJetRearHousingLeftMatrix = m_heliModel.localWingJetRearHousingLeftMatrix;
    m_heliModel.wingJetRearHousingLeftMatrix *= updateMat;
    m_heliModel.wingJetRearHousingRightMatrix = m_heliModel.localWingJetRearHousingRightMatrix;
    m_heliModel.wingJetRearHousingRightMatrix *= updateMat;
    m_heliModel.wingJetExhaustTipLeftMatrix = m_heliModel.localWingJetExhaustTipLeftMatrix;
    m_heliModel.wingJetExhaustTipLeftMatrix *= updateMat;
    m_heliModel.wingJetExhaustTipRightMatrix = m_heliModel.localWingJetExhaustTipRightMatrix;
    
    m_heliModel.sidePodMatrix = m_heliModel.localSidePodMatrix;
    m_heliModel.sidePodMatrix *= updateMat;

    m_heliModel.wingJetExhaustTipRightMatrix *= updateMat;

    m_heliModel.windShieldMatrix = m_heliModel.localWindShieldMatrix;
    m_heliModel.windShieldMatrix *= updateMat;

    m_heliModel.noseCowlMatrix = m_heliModel.localNoseCowlMatrix;
    m_heliModel.noseCowlMatrix *= updateMat;

    m_heliModel.sideWindowUpperMatrix = m_heliModel.localSideWindowUpperMatrix;
    m_heliModel.sideWindowUpperMatrix *= updateMat;
    m_heliModel.sideWindowEyeMatrix = m_heliModel.localSideWindowEyeMatrix;
    m_heliModel.sideWindowEyeMatrix *= updateMat;
    m_heliModel.sideWindowLowerMatrix = m_heliModel.localSideWindowLowerMatrix;
    m_heliModel.sideWindowLowerMatrix *= updateMat;
    m_heliModel.sideWindowBottomMatrix = m_heliModel.localSideWindowBottomMatrix;
    m_heliModel.sideWindowBottomMatrix *= updateMat;

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

    m_heliModel.engineHousingIntakeCowlLeftMatrix = m_heliModel.localEngineHousingIntakeCowlLeftMatrix;
    m_heliModel.engineHousingIntakeCowlLeftMatrix *= updateMat;
    m_heliModel.engineHousingIntakeCowlRightMatrix = m_heliModel.localEngineHousingIntakeCowlRightMatrix;
    m_heliModel.engineHousingIntakeCowlRightMatrix *= updateMat;

    m_heliModel.engineExhaustTipLeftMatrix = m_heliModel.localEngineExhaustTipLeftMatrix;
    m_heliModel.engineExhaustTipLeftMatrix *= updateMat;
    m_heliModel.engineExhaustTipRightMatrix = m_heliModel.localEngineExhaustTipRightMatrix;
    m_heliModel.engineExhaustTipRightMatrix *= updateMat;

    m_heliModel.tailBoomMatrix = m_heliModel.localTailBoomMatrix;
    m_heliModel.tailBoomMatrix *= updateMat;

    m_heliModel.tailBoomLowerMatrix = m_heliModel.localTailBoomLowerMatrix;
    m_heliModel.tailBoomLowerMatrix *= updateMat;

    m_heliModel.tailBoomUpperMatrix = m_heliModel.localTailBoomUpperMatrix;
    m_heliModel.tailBoomUpperMatrix *= updateMat;

    m_heliModel.tailBoomMidEndCapMatrix = m_heliModel.localTailBoomMidEndCapMatrix;
    m_heliModel.tailBoomMidEndCapMatrix *= updateMat;
    m_heliModel.tailBoomLowerEndCapMatrix = m_heliModel.localTailBoomLowerEndCapMatrix;
    m_heliModel.tailBoomLowerEndCapMatrix *= updateMat;
    m_heliModel.tailBoomUpperEndCapMatrix = m_heliModel.localTailBoomUpperEndCapMatrix;
    m_heliModel.tailBoomUpperEndCapMatrix *= updateMat;

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
    m_heliModel.tailWingTrailingEdgeMatrix = m_heliModel.localTailWingTrailingEdgeMatrix;
    m_heliModel.tailWingTrailingEdgeMatrix *= updateMat;

    m_heliModel.tailWingFinLeftMatrix = m_heliModel.localTailWingFinLeftMatrix;
    m_heliModel.tailWingFinLeftMatrix *= updateMat;
    m_heliModel.tailWingFinRightMatrix = m_heliModel.localTailWingFinRightMatrix;
    m_heliModel.tailWingFinRightMatrix *= updateMat;

    m_heliModel.tailWingFinEdgeLeftMatrix = m_heliModel.localTailWingFinEdgeLeftMatrix;
    m_heliModel.tailWingFinEdgeLeftMatrix *= updateMat;
    m_heliModel.tailWingFinEdgeRightMatrix = m_heliModel.localTailWingFinEdgeRightMatrix;
    m_heliModel.tailWingFinEdgeRightMatrix *= updateMat;

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

    // main rotor arm end cap 1
    m_heliModel.mainRotorArmEndCapMatrix1 = m_heliModel.localMainRotorArmEndCapMatrix1;
    m_heliModel.mainRotorArmEndCapMatrix1 *= mainRotorSpin;
    m_heliModel.mainRotorArmEndCapMatrix1 *= updateMat;

    // main rotor arm end cap 2
    m_heliModel.mainRotorArmEndCapMatrix2 = m_heliModel.localMainRotorArmEndCapMatrix2;
    m_heliModel.mainRotorArmEndCapMatrix2 *= mainRotorSpin;
    m_heliModel.mainRotorArmEndCapMatrix2 *= updateMat;

    // main rotor blade 1
    DirectX::SimpleMath::Matrix mainRotorPitch1 = DirectX::SimpleMath::Matrix::CreateRotationX(m_heli.mainRotor.bladeVec[0].pitchAngle);
    m_heliModel.mainRotorBladeMatrix1 = m_heliModel.mainRotorBladeTranslationMatrix1;
    m_heliModel.mainRotorBladeMatrix1 *= mainRotorPitch1;
    m_heliModel.mainRotorBladeMatrix1 *= m_heliModel.localMainRotorBladeMatrix1;  
    m_heliModel.mainRotorBladeMatrix1 *= mainRotorSpin;
    m_heliModel.mainRotorBladeMatrix1 *= updateMat;

    // main rotor blade interior 1
    m_heliModel.mainRotorBladeInteriorMatrix1 = m_heliModel.mainRotorBladeInteriorTranslationMatrix1;
    m_heliModel.mainRotorBladeInteriorMatrix1 *= mainRotorPitch1;
    m_heliModel.mainRotorBladeInteriorMatrix1 *= m_heliModel.localMainRotorBladeInteriorMatrix1;
    m_heliModel.mainRotorBladeInteriorMatrix1 *= mainRotorSpin;
    m_heliModel.mainRotorBladeInteriorMatrix1 *= updateMat;

    // main rotor blade 1 stripe 1
    m_heliModel.mainRotorBladeStripe1Matrix1 = m_heliModel.mainRotorBladeTranslationMatrix1;
    m_heliModel.mainRotorBladeStripe1Matrix1 *= mainRotorPitch1;
    m_heliModel.mainRotorBladeStripe1Matrix1 *= m_heliModel.localMainRotorBladeStripe1Matrix1;
    m_heliModel.mainRotorBladeStripe1Matrix1 *= mainRotorSpin;
    m_heliModel.mainRotorBladeStripe1Matrix1 *= updateMat;
    
    // main rotor edge 1
    m_heliModel.mainRotorBladeEdgeMatrix1 = m_heliModel.mainRotorBladeEdgeTranslationMatrix1;
    m_heliModel.mainRotorBladeEdgeMatrix1 *= mainRotorPitch1;
    m_heliModel.mainRotorBladeEdgeMatrix1 *= m_heliModel.localMainRotorEdgeBladeMatrix1;
    m_heliModel.mainRotorBladeEdgeMatrix1 *= mainRotorSpin;
    m_heliModel.mainRotorBladeEdgeMatrix1 *= updateMat;

    // main rotor edge 1 stripe 1
    m_heliModel.mainRotorBladeEdgeStripe1Matrix1 = m_heliModel.mainRotorBladeEdgeTranslationMatrix1;
    m_heliModel.mainRotorBladeEdgeStripe1Matrix1 *= mainRotorPitch1;
    m_heliModel.mainRotorBladeEdgeStripe1Matrix1 *= m_heliModel.localMainRotorEdgeBladeStripe1Matrix1;
    m_heliModel.mainRotorBladeEdgeStripe1Matrix1 *= mainRotorSpin;
    m_heliModel.mainRotorBladeEdgeStripe1Matrix1 *= updateMat;

    // main rotor arm 1 stripe 1
    m_heliModel.mainRotorArmStripe1Matrix1 = m_heliModel.localMainRotorArmMatrix;
    m_heliModel.mainRotorArmStripe1Matrix1 = m_heliModel.localMainRotorArmStripe1Matrix1;
    m_heliModel.mainRotorArmStripe1Matrix1 *= mainRotorSpin;
    m_heliModel.mainRotorArmStripe1Matrix1 *= updateMat;
    // main rotor arm 2 stripe 1
    m_heliModel.mainRotorArmStripe1Matrix2 = m_heliModel.localMainRotorArmMatrix;
    m_heliModel.mainRotorArmStripe1Matrix2 = m_heliModel.localMainRotorArmStripe1Matrix1;
    m_heliModel.mainRotorArmStripe1Matrix2 *= DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(180.0f));
    m_heliModel.mainRotorArmStripe1Matrix2 *= mainRotorSpin;
    m_heliModel.mainRotorArmStripe1Matrix2 *= updateMat;

    // main rotor blade end cap 1
    m_heliModel.mainRotorBladeEndCapMatrix1 = m_heliModel.mainRotorBladeEndCapTranslationMatrix1;
    m_heliModel.mainRotorBladeEndCapMatrix1 *= mainRotorPitch1;
    m_heliModel.mainRotorBladeEndCapMatrix1 *= m_heliModel.localMainRotorBladeEndCapMatrix1;
    m_heliModel.mainRotorBladeEndCapMatrix1 *= mainRotorSpin;
    m_heliModel.mainRotorBladeEndCapMatrix1 *= updateMat;

    // main rotor edge end cap 1
    m_heliModel.mainRotorBladeEdgeEndCapMatrix1 = m_heliModel.mainRotorBladeEdgeEndCapTranslationMatrix1;
    m_heliModel.mainRotorBladeEdgeEndCapMatrix1 *= mainRotorPitch1;
    m_heliModel.mainRotorBladeEdgeEndCapMatrix1 *= m_heliModel.localMainRotorBladeEdgeEndCapMatrix1;
    m_heliModel.mainRotorBladeEdgeEndCapMatrix1 *= mainRotorSpin;
    m_heliModel.mainRotorBladeEdgeEndCapMatrix1 *= updateMat;

    // main rotor blade 2
    DirectX::SimpleMath::Matrix mainRotorPitch2 = DirectX::SimpleMath::Matrix::CreateRotationX(m_heli.mainRotor.bladeVec[1].pitchAngle);
    m_heliModel.mainRotorBladeMatrix2 = m_heliModel.mainRotorBladeTranslationMatrix2;
    m_heliModel.mainRotorBladeMatrix2 *= mainRotorPitch2;
    m_heliModel.mainRotorBladeMatrix2 *= m_heliModel.localMainRotorBladeMatrix2;
    m_heliModel.mainRotorBladeMatrix2 *= mainRotorSpin;
    m_heliModel.mainRotorBladeMatrix2 *= updateMat;
    
    // main rotor blade interior 2
    m_heliModel.mainRotorBladeInteriorMatrix2 = m_heliModel.mainRotorBladeInteriorTranslationMatrix2;
    m_heliModel.mainRotorBladeInteriorMatrix2 *= mainRotorPitch2;
    m_heliModel.mainRotorBladeInteriorMatrix2 *= m_heliModel.localMainRotorBladeInteriorMatrix2;
    m_heliModel.mainRotorBladeInteriorMatrix2 *= mainRotorSpin;
    m_heliModel.mainRotorBladeInteriorMatrix2 *= updateMat;

    // main rotor stripe 2
    m_heliModel.mainRotorArmStripe2Matrix1 = m_heliModel.localMainRotorArmMatrix;
    m_heliModel.mainRotorArmStripe2Matrix1 *= m_heliModel.localMainRotorStripe2Matrix;
    m_heliModel.mainRotorArmStripe2Matrix1 = m_heliModel.localMainRotorStripe2Matrix;
    m_heliModel.mainRotorArmStripe2Matrix1 *= mainRotorSpin;
    m_heliModel.mainRotorArmStripe2Matrix1 *= updateMat;
    m_heliModel.mainRotorArmStripe2Matrix2 = m_heliModel.localMainRotorStripe2Matrix;
    m_heliModel.mainRotorArmStripe2Matrix2 *= DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(180.0f));
    m_heliModel.mainRotorArmStripe2Matrix2 *= mainRotorSpin;
    m_heliModel.mainRotorArmStripe2Matrix2 *= updateMat;
    // main rotor blade stripe 2
    m_heliModel.mainRotorBladeStripe2Matrix1 = m_heliModel.mainRotorBladeTranslationMatrix1;
    m_heliModel.mainRotorBladeStripe2Matrix1 *= mainRotorPitch1;
    m_heliModel.mainRotorBladeStripe2Matrix1 *= m_heliModel.localMainRotorBladeStripe2Matrix1;
    m_heliModel.mainRotorBladeStripe2Matrix1 *= mainRotorSpin;
    m_heliModel.mainRotorBladeStripe2Matrix1 *= updateMat;
    m_heliModel.mainRotorBladeStripe2Matrix2 = m_heliModel.mainRotorBladeTranslationMatrix2;
    m_heliModel.mainRotorBladeStripe2Matrix2 *= mainRotorPitch2;
    m_heliModel.mainRotorBladeStripe2Matrix2 *= m_heliModel.localMainRotorBladeStripe2Matrix2;
    m_heliModel.mainRotorBladeStripe2Matrix2 *= mainRotorSpin;
    m_heliModel.mainRotorBladeStripe2Matrix2 *= updateMat;
    // main rotor edge stripe 2
    m_heliModel.mainRotorEdgeStripe2Matrix1 = m_heliModel.localMainRotorStripe2Matrix;
    m_heliModel.mainRotorEdgeStripe2Matrix1 *= mainRotorPitch1;
    m_heliModel.mainRotorEdgeStripe2Matrix1 *= m_heliModel.localMainRotorEdgeBladeMatrix1;
    m_heliModel.mainRotorEdgeStripe2Matrix1 *= mainRotorSpin;
    m_heliModel.mainRotorEdgeStripe2Matrix1 *= updateMat;

    m_heliModel.mainRotorEdgeStripe2Matrix1 = m_heliModel.mainRotorBladeEdgeTranslationMatrix1;
    m_heliModel.mainRotorEdgeStripe2Matrix1 *= mainRotorPitch1;
    m_heliModel.mainRotorEdgeStripe2Matrix1 *= m_heliModel.localMainRotorEdgeStripe2Matrix1;
    m_heliModel.mainRotorEdgeStripe2Matrix1 *= mainRotorSpin;
    m_heliModel.mainRotorEdgeStripe2Matrix1 *= updateMat;

    m_heliModel.mainRotorEdgeStripe2Matrix2 = m_heliModel.mainRotorBladeEdgeTranslationMatrix1;
    m_heliModel.mainRotorEdgeStripe2Matrix2 *= mainRotorPitch2;
    m_heliModel.mainRotorEdgeStripe2Matrix2 *= m_heliModel.localMainRotorEdgeStripe2Matrix2;
    m_heliModel.mainRotorEdgeStripe2Matrix2 *= mainRotorSpin;
    m_heliModel.mainRotorEdgeStripe2Matrix2 *= updateMat;

    // main rotor blade 2 stripe 1
    m_heliModel.mainRotorBladeStripe1Matrix2 = m_heliModel.mainRotorBladeTranslationMatrix1; 
    m_heliModel.mainRotorBladeStripe1Matrix2 *= mainRotorPitch2;
    m_heliModel.mainRotorBladeStripe1Matrix2 *= m_heliModel.localMainRotorBladeStripe1Matrix1;
    m_heliModel.mainRotorBladeStripe1Matrix2 *= DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(180.0f));
    m_heliModel.mainRotorBladeStripe1Matrix2 *= mainRotorSpin;
    m_heliModel.mainRotorBladeStripe1Matrix2 *= updateMat;

    // main rotor edge 2 stripe 1
    m_heliModel.mainRotorBladeEdgeStripe1Matrix2 = m_heliModel.mainRotorBladeEdgeTranslationMatrix1;
    m_heliModel.mainRotorBladeEdgeStripe1Matrix2 *= mainRotorPitch2;
    m_heliModel.mainRotorBladeEdgeStripe1Matrix2 *= m_heliModel.localMainRotorEdgeBladeStripe1Matrix1;
    m_heliModel.mainRotorBladeEdgeStripe1Matrix2 *= DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(180.0f));
    m_heliModel.mainRotorBladeEdgeStripe1Matrix2 *= mainRotorSpin;
    m_heliModel.mainRotorBladeEdgeStripe1Matrix2 *= updateMat;

    // main rotor blade end cap 2
    m_heliModel.mainRotorBladeEndCapMatrix2 = m_heliModel.mainRotorBladeEndCapTranslationMatrix2;
    m_heliModel.mainRotorBladeEndCapMatrix2 *= mainRotorPitch2;
    m_heliModel.mainRotorBladeEndCapMatrix2 *= m_heliModel.localMainRotorBladeEndCapMatrix2;
    m_heliModel.mainRotorBladeEndCapMatrix2 *= mainRotorSpin;
    m_heliModel.mainRotorBladeEndCapMatrix2 *= updateMat;

    // main rotor edge 2
    m_heliModel.mainRotorBladeEdgeMatrix2 = m_heliModel.mainRotorBladeEdgeTranslationMatrix2;
    DirectX::SimpleMath::Matrix mainEdgePitch2 = DirectX::SimpleMath::Matrix::CreateRotationX(-m_heli.mainRotor.bladeVec[1].pitchAngle);
    m_heliModel.mainRotorBladeEdgeMatrix2 *= mainEdgePitch2;
    m_heliModel.mainRotorBladeEdgeMatrix2 *= m_heliModel.localMainRotorEdgeBladeMatrix2;
    m_heliModel.mainRotorBladeEdgeMatrix2 *= mainRotorSpin;
    m_heliModel.mainRotorBladeEdgeMatrix2 *= updateMat;

    // main rotor edge end cap 2
    m_heliModel.mainRotorBladeEdgeEndCapMatrix2 = m_heliModel.mainRotorBladeEdgeEndCapTranslationMatrix2;
    m_heliModel.mainRotorBladeEdgeEndCapMatrix2 *= mainRotorPitch2;
    m_heliModel.mainRotorBladeEdgeEndCapMatrix2 *= m_heliModel.localMainRotorBladeEdgeEndCapMatrix2;
    m_heliModel.mainRotorBladeEdgeEndCapMatrix2 *= mainRotorSpin;
    m_heliModel.mainRotorBladeEdgeEndCapMatrix2 *= updateMat;

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

    // tail arm end cap 1
    m_heliModel.tailRotorArmEndCapMatrix1 = m_heliModel.localTailRotorArmEndCapMatrix1;
    m_heliModel.tailRotorArmEndCapMatrix1 *= tailArmSpin;
    m_heliModel.tailRotorArmEndCapMatrix1 *= m_heliModel.tailRotorArmEndCapTranslationMatrix1;
    m_heliModel.tailRotorArmEndCapMatrix1 *= updateMat;
    
    // tail arm end cap 2
    m_heliModel.tailRotorArmEndCapMatrix2 = m_heliModel.localTailRotorArmEndCapMatrix2;
    m_heliModel.tailRotorArmEndCapMatrix2 *= tailArmSpin;
    m_heliModel.tailRotorArmEndCapMatrix2 *= m_heliModel.tailRotorArmEndCapTranslationMatrix2;
    m_heliModel.tailRotorArmEndCapMatrix2 *= updateMat;

    // tail rotor blade 1
    DirectX::SimpleMath::Matrix tailRotorPitch1 = DirectX::SimpleMath::Matrix::CreateRotationY(m_heli.tailRotor.bladeVec[0].pitchAngle);
    m_heliModel.tailRotorBladeMatrix1 = m_heliModel.localTailRotorBladeMatrix1;
    m_heliModel.tailRotorBladeMatrix1 *= tailRotorPitch1;
    m_heliModel.tailRotorBladeMatrix1 *= tailArmSpin;
    m_heliModel.tailRotorBladeMatrix1 *= m_heliModel.tailRotorBladeTranslationMatrix1;  
    m_heliModel.tailRotorBladeMatrix1 *= updateMat;

    // tail rotor blade end cap1
    m_heliModel.tailRotorBladeEndCapMatrix1 = m_heliModel.localTailRotorBladeEndCapMatrix1;
    m_heliModel.tailRotorBladeEndCapMatrix1 *= tailRotorPitch1;
    m_heliModel.tailRotorBladeEndCapMatrix1 *= tailArmSpin;
    m_heliModel.tailRotorBladeEndCapMatrix1 *= m_heliModel.tailRotorBladeEndCapTranslationMatrix1;
    m_heliModel.tailRotorBladeEndCapMatrix1 *= updateMat;

    // tail rotor edge 1
    m_heliModel.tailRotorBladeEdgeMatrix1 = m_heliModel.localTailRotorBladeEdgeMatrix1;
    m_heliModel.tailRotorBladeEdgeMatrix1 *= tailRotorPitch1;   
    m_heliModel.tailRotorBladeEdgeMatrix1 *= tailArmSpin;
    m_heliModel.tailRotorBladeEdgeMatrix1 *= m_heliModel.tailRotorBladeEdgeTranslationMatrix1;   
    m_heliModel.tailRotorBladeEdgeMatrix1 *= updateMat;

    // tail rotor blade 2
    DirectX::SimpleMath::Matrix tailRotorPitch2 = DirectX::SimpleMath::Matrix::CreateRotationY(m_heli.tailRotor.bladeVec[1].pitchAngle);
    m_heliModel.tailRotorBladeMatrix2 = m_heliModel.localTailRotorBladeMatrix2;
    m_heliModel.tailRotorBladeMatrix2 *= tailRotorPitch2;
    m_heliModel.tailRotorBladeMatrix2 *= tailArmSpin;
    m_heliModel.tailRotorBladeMatrix2 *= m_heliModel.tailRotorBladeTranslationMatrix2;
    m_heliModel.tailRotorBladeMatrix2 *= updateMat;

    // tail rotor blade end cap2
    m_heliModel.tailRotorBladeEndCapMatrix2 = m_heliModel.localTailRotorBladeEndCapMatrix2;
    m_heliModel.tailRotorBladeEndCapMatrix2 *= tailRotorPitch2;
    m_heliModel.tailRotorBladeEndCapMatrix2 *= tailArmSpin;
    m_heliModel.tailRotorBladeEndCapMatrix2 *= m_heliModel.tailRotorBladeEndCapTranslationMatrix2;
    m_heliModel.tailRotorBladeEndCapMatrix2 *= updateMat;

    // tail rotor edge 2
    m_heliModel.tailRotorBladeEdgeMatrix2 = m_heliModel.localTailRotorBladeEdgeMatrix2;
    m_heliModel.tailRotorBladeEdgeMatrix2 *= tailRotorPitch2;
    m_heliModel.tailRotorBladeEdgeMatrix2 *= tailArmSpin;
    m_heliModel.tailRotorBladeEdgeMatrix2 *= m_heliModel.tailRotorBladeEdgeTranslationMatrix2;
    m_heliModel.tailRotorBladeEdgeMatrix2 *= updateMat;

    // tail rotor edge end cap 1
    m_heliModel.tailRotorBladeEdgeEndCapMatrix1 = m_heliModel.localTailRotorBladeEdgeEndCapMatrix1;
    m_heliModel.tailRotorBladeEdgeEndCapMatrix1 *= tailRotorPitch2;
    m_heliModel.tailRotorBladeEdgeEndCapMatrix1 *= tailArmSpin;
    m_heliModel.tailRotorBladeEdgeEndCapMatrix1 *= m_heliModel.tailRotorBladeEdgeEndCapTranslationMatrix1;
    m_heliModel.tailRotorBladeEdgeEndCapMatrix1 *= updateMat;

    // tail rotor edge end cap 2
    m_heliModel.tailRotorBladeEdgeEndCapMatrix2 = m_heliModel.localTailRotorBladeEdgeEndCapMatrix2;
    m_heliModel.tailRotorBladeEdgeEndCapMatrix2 *= tailRotorPitch2;
    m_heliModel.tailRotorBladeEdgeEndCapMatrix2 *= tailArmSpin;
    m_heliModel.tailRotorBladeEdgeEndCapMatrix2 *= m_heliModel.tailRotorBladeEdgeEndCapTranslationMatrix2;
    m_heliModel.tailRotorBladeEdgeEndCapMatrix2 *= updateMat; 

    // tail rotor blade interior 1
    m_heliModel.tailRotorBladeInteriorMatrix1 = m_heliModel.localTailRotorBladeInteriorMatrix1;
    m_heliModel.tailRotorBladeInteriorMatrix1 *= tailRotorPitch1;
    m_heliModel.tailRotorBladeInteriorMatrix1 *= tailArmSpin;
    m_heliModel.tailRotorBladeInteriorMatrix1 *= m_heliModel.tailRotorBladeInteriorTranslationMatrix1; 
    m_heliModel.tailRotorBladeInteriorMatrix1 *= updateMat;

    // tail rotor blade interior 2   
    m_heliModel.tailRotorBladeInteriorMatrix2 = m_heliModel.localTailRotorBladeInteriorMatrix2;
    m_heliModel.tailRotorBladeInteriorMatrix2 *= tailRotorPitch2;
    m_heliModel.tailRotorBladeInteriorMatrix2 *= tailArmSpin;
    m_heliModel.tailRotorBladeInteriorMatrix2 *= m_heliModel.tailRotorBladeInteriorTranslationMatrix2;   
    m_heliModel.tailRotorBladeInteriorMatrix2 *= updateMat;

    // tail rotor arm 1 stripe 1   
    m_heliModel.tailRotorArmStripe1Matrix1 = m_heliModel.localTailRotorArmStripe1Matrix1;
    m_heliModel.tailRotorArmStripe1Matrix1 *= tailRotorPitch1;
    m_heliModel.tailRotorArmStripe1Matrix1 *= tailArmSpin;
    m_heliModel.tailRotorArmStripe1Matrix1 *= m_heliModel.tailRotorArmStripe1TranslationMatrix1;
    m_heliModel.tailRotorArmStripe1Matrix1 *= updateMat;
    // tail rotor arm 2 stripe 1
    m_heliModel.tailRotorArmStripe1Matrix2 = m_heliModel.localTailRotorArmStripe1Matrix2;
    m_heliModel.tailRotorArmStripe1Matrix2 *= tailRotorPitch2;
    m_heliModel.tailRotorArmStripe1Matrix2 *= tailArmSpin;
    m_heliModel.tailRotorArmStripe1Matrix2 *= m_heliModel.tailRotorTranslationMatrix2;
    m_heliModel.tailRotorArmStripe1Matrix2 *= updateMat;

    // tail rotor arm 1 stripe 2   
    m_heliModel.tailRotorArmStripe2Matrix1 = m_heliModel.localTailRotorArmStripe2Matrix1;
    m_heliModel.tailRotorArmStripe2Matrix1 *= tailRotorPitch1;
    m_heliModel.tailRotorArmStripe2Matrix1 *= tailArmSpin;
    m_heliModel.tailRotorArmStripe2Matrix1 *= m_heliModel.tailRotorTranslationMatrix1;
    m_heliModel.tailRotorArmStripe2Matrix1 *= updateMat;
    // tail rotor arm 2 stripe 2
    m_heliModel.tailRotorArmStripe2Matrix2 = m_heliModel.localTailRotorArmStripe2Matrix2;
    m_heliModel.tailRotorArmStripe2Matrix2 *= tailRotorPitch2;
    m_heliModel.tailRotorArmStripe2Matrix2 *= tailArmSpin;
    m_heliModel.tailRotorArmStripe2Matrix2 *= m_heliModel.tailRotorTranslationMatrix2;
    m_heliModel.tailRotorArmStripe2Matrix2 *= updateMat;

    // tail rotor blade 1 stripe 1   
    m_heliModel.tailRotorBladeStripe1Matrix1 = m_heliModel.localTailRotorBladeStripe1Matrix1;
    m_heliModel.tailRotorBladeStripe1Matrix1 *= tailRotorPitch1;
    m_heliModel.tailRotorBladeStripe1Matrix1 *= tailArmSpin;
    m_heliModel.tailRotorBladeStripe1Matrix1 *= m_heliModel.tailRotorTranslationMatrix1;
    m_heliModel.tailRotorBladeStripe1Matrix1 *= updateMat;
    // tail rotor blade 2 stripe 1
    m_heliModel.tailRotorBladeStripe1Matrix2 = m_heliModel.localTailRotorBladeStripe1Matrix2;
    m_heliModel.tailRotorBladeStripe1Matrix2 *= tailRotorPitch1;
    m_heliModel.tailRotorBladeStripe1Matrix2 *= tailArmSpin;
    m_heliModel.tailRotorBladeStripe1Matrix2 *= m_heliModel.tailRotorTranslationMatrix2;
    m_heliModel.tailRotorBladeStripe1Matrix2 *= updateMat;

    // tail rotor blade 1 stripe 2
    m_heliModel.tailRotorBladeStripe2Matrix1 = m_heliModel.localTailRotorBladeStripe2Matrix1;
    m_heliModel.tailRotorBladeStripe2Matrix1 *= tailRotorPitch1;
    m_heliModel.tailRotorBladeStripe2Matrix1 *= tailArmSpin;
    m_heliModel.tailRotorBladeStripe2Matrix1 *= m_heliModel.tailRotorArmStripe1TranslationMatrix1;
    m_heliModel.tailRotorBladeStripe2Matrix1 *= updateMat;
    // tail rotor blade 2 stripe 2
    m_heliModel.tailRotorBladeStripe2Matrix2 = m_heliModel.localTailRotorBladeStripe2Matrix2;
    m_heliModel.tailRotorBladeStripe2Matrix2 *= tailRotorPitch1;
    m_heliModel.tailRotorBladeStripe2Matrix2 *= tailArmSpin;
    m_heliModel.tailRotorBladeStripe2Matrix2 *= m_heliModel.tailRotorTranslationMatrix2;
    m_heliModel.tailRotorBladeStripe2Matrix2 *= updateMat;

    // tail rotor edge 1 stripe 1   
    m_heliModel.tailRotorEdgeStripe1Matrix1 = m_heliModel.localTailRotorEdgeStripe1Matrix1;
    m_heliModel.tailRotorEdgeStripe1Matrix1 *= tailRotorPitch1;
    m_heliModel.tailRotorEdgeStripe1Matrix1 *= tailArmSpin;
    m_heliModel.tailRotorEdgeStripe1Matrix1 *= m_heliModel.tailRotorArmStripe1TranslationMatrix1;
    m_heliModel.tailRotorEdgeStripe1Matrix1 *= updateMat;
    // tail rotor edge 2 stripe 1
    m_heliModel.tailRotorEdgeStripe1Matrix2 = m_heliModel.localTailRotorEdgeStripe1Matrix2;
    m_heliModel.tailRotorEdgeStripe1Matrix2 *= tailRotorPitch1;
    m_heliModel.tailRotorEdgeStripe1Matrix2 *= tailArmSpin;
    m_heliModel.tailRotorEdgeStripe1Matrix2 *= m_heliModel.tailRotorTranslationMatrix2;
    m_heliModel.tailRotorEdgeStripe1Matrix2 *= updateMat;
    
    // tail rotor edge 1 stripe 2
    m_heliModel.tailRotorEdgeStripe2Matrix1 = m_heliModel.localTailRotorEdgeStripe2Matrix1;
    m_heliModel.tailRotorEdgeStripe2Matrix1 *= tailRotorPitch1;
    m_heliModel.tailRotorEdgeStripe2Matrix1 *= tailArmSpin;
    m_heliModel.tailRotorEdgeStripe2Matrix1 *= m_heliModel.tailRotorTranslationMatrix1;
    m_heliModel.tailRotorEdgeStripe2Matrix1 *= updateMat;
    // tail rotor edge 2 stripe 2
    m_heliModel.tailRotorEdgeStripe2Matrix2 = m_heliModel.localTailRotorEdgeStripe2Matrix2;
    m_heliModel.tailRotorEdgeStripe2Matrix2 *= tailRotorPitch2;
    m_heliModel.tailRotorEdgeStripe2Matrix2 *= tailArmSpin;
    m_heliModel.tailRotorEdgeStripe2Matrix2 *= m_heliModel.tailRotorTranslationMatrix2;
    m_heliModel.tailRotorEdgeStripe2Matrix2 *= updateMat;

    // update shadow model
    DirectX::SimpleMath::Vector3 lightDir = m_environment->GetLightDirectionPrime();
    DirectX::SimpleMath::Plane groundPlane = m_heli.groundPlane;

    DirectX::SimpleMath::Vector3 zFightOffSet = groundPlane.Normal() * 0.1f;
    DirectX::SimpleMath::Matrix planeTrans = DirectX::SimpleMath::Matrix::Identity;
    planeTrans *= DirectX::SimpleMath::Matrix::CreateTranslation(zFightOffSet);
    DirectX::SimpleMath::Matrix planeTrans2 = planeTrans;
    planeTrans2 = planeTrans2.Transpose();

    groundPlane = DirectX::SimpleMath::Plane::Transform(groundPlane, planeTrans2);
    
    float alt = m_heli.altitude + 0.0000000001f; // padding added because shadows won't show if altitude = 0
    DirectX::SimpleMath::Matrix shadowMat = DirectX::SimpleMath::Matrix::CreateShadow(lightDir, groundPlane);

    const float maxShadowRange = m_environment->GetMaxShadowCastRange();
    float shadowScale;
    float inverseShadowScale;
    if (alt > maxShadowRange)
    {
        shadowScale = 0.0f;
        inverseShadowScale = 1.0f;
    }
    else
    {
        inverseShadowScale = alt / maxShadowRange;
        shadowScale = 1.0f - inverseShadowScale;
    }
    DirectX::SimpleMath::Matrix shadowScaleMat = DirectX::SimpleMath::Matrix::CreateScale(DirectX::SimpleMath::Vector3(shadowScale, shadowScale, shadowScale));

    m_heliModel.shadowBaseMat = m_heliModel.localBodyMatrix;
    m_heliModel.shadowBaseMat *= inverseShadowScale;
    m_heliModel.shadowBaseMat *= shadowScaleMat;
    m_heliModel.shadowBaseMat *= updateMat;
    m_heliModel.shadowBaseMat *= shadowMat;

    m_heliModel.shadowFrontNoseMat = m_heliModel.localWindShieldMatrix;
    m_heliModel.shadowFrontNoseMat *= inverseShadowScale;
    m_heliModel.shadowFrontNoseMat *= shadowScaleMat;
    m_heliModel.shadowFrontNoseMat *= updateMat;
    m_heliModel.shadowFrontNoseMat *= shadowMat;

    m_heliModel.shadowNoseMat = m_heliModel.localNoseConeMatrix;
    m_heliModel.shadowNoseMat *= inverseShadowScale;
    m_heliModel.shadowNoseMat *= shadowScaleMat;
    m_heliModel.shadowNoseMat *= updateMat;
    m_heliModel.shadowNoseMat *= shadowMat;

    m_heliModel.shadowNoseBodyMat = m_heliModel.localNoseBodyMatrix;
    m_heliModel.shadowNoseBodyMat *= inverseShadowScale;
    m_heliModel.shadowNoseBodyMat *= shadowScaleMat;
    m_heliModel.shadowNoseBodyMat *= updateMat;
    m_heliModel.shadowNoseBodyMat *= shadowMat;

    m_heliModel.shadowRearNoseMat = m_heliModel.localBodyRearMatrix;
    m_heliModel.shadowRearNoseMat *= inverseShadowScale;
    m_heliModel.shadowRearNoseMat *= shadowScaleMat;
    m_heliModel.shadowRearNoseMat *= updateMat;
    m_heliModel.shadowRearNoseMat *= shadowMat;

    m_heliModel.shadowBoomMat = m_heliModel.localTailBoomMatrix;
    m_heliModel.shadowBoomMat *= inverseShadowScale;
    m_heliModel.shadowBoomMat *= shadowScaleMat;
    m_heliModel.shadowBoomMat *= updateMat;
    m_heliModel.shadowBoomMat *= shadowMat;

    m_heliModel.shadowBoomBottomMat = m_heliModel.localTailBoomLowerMatrix;
    m_heliModel.shadowBoomBottomMat *= inverseShadowScale;
    m_heliModel.shadowBoomBottomMat *= shadowScaleMat;
    m_heliModel.shadowBoomBottomMat *= updateMat;
    m_heliModel.shadowBoomBottomMat *= shadowMat;

    m_heliModel.shadowBoomTopMat = m_heliModel.localTailBoomUpperMatrix;
    m_heliModel.shadowBoomTopMat *= inverseShadowScale;
    m_heliModel.shadowBoomTopMat *= shadowScaleMat;
    m_heliModel.shadowBoomTopMat *= updateMat;
    m_heliModel.shadowBoomTopMat *= shadowMat;

    m_heliModel.shadowBoomEndCapMat = m_heliModel.localTailBoomMidEndCapMatrix;
    m_heliModel.shadowBoomEndCapMat *= inverseShadowScale;
    m_heliModel.shadowBoomEndCapMat *= shadowScaleMat;
    m_heliModel.shadowBoomEndCapMat *= updateMat;
    m_heliModel.shadowBoomEndCapMat *= shadowMat;

    m_heliModel.shadowMainWingMat = m_heliModel.localMainWingMatrix;
    m_heliModel.shadowMainWingMat *= inverseShadowScale;
    m_heliModel.shadowMainWingMat *= shadowScaleMat;
    m_heliModel.shadowMainWingMat *= updateMat;
    m_heliModel.shadowMainWingMat *= shadowMat;

    m_heliModel.shadowMainWingFrontMat = m_heliModel.localMainWingLeadingEdgeMatrix;
    m_heliModel.shadowMainWingFrontMat *= inverseShadowScale;
    m_heliModel.shadowMainWingFrontMat *= shadowScaleMat;
    m_heliModel.shadowMainWingFrontMat *= updateMat;
    m_heliModel.shadowMainWingFrontMat *= shadowMat;

    m_heliModel.shadowMainWingRearMat = m_heliModel.localMainWingTailEdgeMatrix;
    m_heliModel.shadowMainWingRearMat *= inverseShadowScale;
    m_heliModel.shadowMainWingRearMat *= shadowScaleMat;
    m_heliModel.shadowMainWingRearMat *= updateMat;
    m_heliModel.shadowMainWingRearMat *= shadowMat;

    m_heliModel.shadowTailWingMat = m_heliModel.localTailWingMatrix;
    m_heliModel.shadowTailWingMat *= inverseShadowScale;
    m_heliModel.shadowTailWingMat *= shadowScaleMat;
    m_heliModel.shadowTailWingMat *= updateMat;
    m_heliModel.shadowTailWingMat *= shadowMat;

    m_heliModel.shadowTailFinLowerMat = m_heliModel.localTailFinLowerMatrix;
    m_heliModel.shadowTailFinLowerMat *= inverseShadowScale;
    m_heliModel.shadowTailFinLowerMat *= shadowScaleMat;
    m_heliModel.shadowTailFinLowerMat *= updateMat;
    m_heliModel.shadowTailFinLowerMat *= shadowMat;

    m_heliModel.shadowTailFinUpperMat = m_heliModel.localTailFinUpperMatrix;
    m_heliModel.shadowTailFinUpperMat *= inverseShadowScale;
    m_heliModel.shadowTailFinUpperMat *= shadowScaleMat;
    m_heliModel.shadowTailFinUpperMat *= updateMat;
    m_heliModel.shadowTailFinUpperMat *= shadowMat;

    m_heliModel.shadowTailRotorHubMat = m_heliModel.localTailRotorHubMatrix;
    m_heliModel.shadowTailRotorHubMat *= inverseShadowScale;
    m_heliModel.shadowTailRotorHubMat *= shadowScaleMat;
    m_heliModel.shadowTailRotorHubMat *= updateMat;
    m_heliModel.shadowTailRotorHubMat *= shadowMat;

    m_heliModel.shadowTailRotorAxelMat = m_heliModel.localTailRotorAxelMatrix;
    m_heliModel.shadowTailRotorAxelMat *= inverseShadowScale;
    m_heliModel.shadowTailRotorAxelMat *= shadowScaleMat;
    m_heliModel.shadowTailRotorAxelMat *= updateMat;
    m_heliModel.shadowTailRotorAxelMat *= shadowMat;

    m_heliModel.shadowMainRotorAxelMat = m_heliModel.localMainRotorAxelMatrix;
    m_heliModel.shadowMainRotorAxelMat *= inverseShadowScale;
    m_heliModel.shadowMainRotorAxelMat *= shadowScaleMat;
    m_heliModel.shadowMainRotorAxelMat *= updateMat;
    m_heliModel.shadowMainRotorAxelMat *= shadowMat;

    m_heliModel.shadowMainRotorHubMat = m_heliModel.localMainRotorHubMatrix;
    m_heliModel.shadowMainRotorHubMat *= inverseShadowScale;
    m_heliModel.shadowMainRotorHubMat *= shadowScaleMat;
    m_heliModel.shadowMainRotorHubMat *= updateMat;
    m_heliModel.shadowMainRotorHubMat *= shadowMat;

    m_heliModel.shadowHouseingMainMat = m_heliModel.localEngineHousingMatrix;
    m_heliModel.shadowHouseingMainMat *= inverseShadowScale;
    m_heliModel.shadowHouseingMainMat *= shadowScaleMat;
    m_heliModel.shadowHouseingMainMat *= updateMat;
    m_heliModel.shadowHouseingMainMat *= shadowMat;

    m_heliModel.shadowHouseingFrontMat = m_heliModel.localEngineHousingFrontMatrix;
    m_heliModel.shadowHouseingFrontMat *= inverseShadowScale;
    m_heliModel.shadowHouseingFrontMat *= shadowScaleMat;
    m_heliModel.shadowHouseingFrontMat *= updateMat;
    m_heliModel.shadowHouseingFrontMat *= shadowMat;

    m_heliModel.shadowHouseingLeftMat = m_heliModel.localEngineHousingSideLeftMatrix;
    m_heliModel.shadowHouseingLeftMat *= inverseShadowScale;
    m_heliModel.shadowHouseingLeftMat *= shadowScaleMat;
    m_heliModel.shadowHouseingLeftMat *= updateMat;
    m_heliModel.shadowHouseingLeftMat *= shadowMat;

    m_heliModel.shadowHouseingRightMat = m_heliModel.localEngineHousingSideRightMatrix;
    m_heliModel.shadowHouseingRightMat *= inverseShadowScale;
    m_heliModel.shadowHouseingRightMat *= shadowScaleMat;
    m_heliModel.shadowHouseingRightMat *= updateMat;
    m_heliModel.shadowHouseingRightMat *= shadowMat;

    m_heliModel.shadowHouseingRearMat = m_heliModel.localEngineHousingRearMatrix;
    m_heliModel.shadowHouseingRearMat *= inverseShadowScale;
    m_heliModel.shadowHouseingRearMat *= shadowScaleMat;
    m_heliModel.shadowHouseingRearMat *= updateMat;
    m_heliModel.shadowHouseingRearMat *= shadowMat;

    m_heliModel.shadowRearWingFinLeftMat = m_heliModel.localTailWingFinLeftMatrix;
    m_heliModel.shadowRearWingFinLeftMat *= inverseShadowScale;
    m_heliModel.shadowRearWingFinLeftMat *= shadowScaleMat;
    m_heliModel.shadowRearWingFinLeftMat *= updateMat;
    m_heliModel.shadowRearWingFinLeftMat *= shadowMat;

    m_heliModel.shadowRearWingFinRightMat = m_heliModel.localTailWingFinRightMatrix;
    m_heliModel.shadowRearWingFinRightMat *= inverseShadowScale;
    m_heliModel.shadowRearWingFinRightMat *= shadowScaleMat;
    m_heliModel.shadowRearWingFinRightMat *= updateMat;
    m_heliModel.shadowRearWingFinRightMat *= shadowMat;

    m_heliModel.shadowMainRotorMat1 = m_heliModel.mainRotorBladeTranslationMatrix1;
    m_heliModel.shadowMainRotorMat1 *= mainRotorPitch1;
    m_heliModel.shadowMainRotorMat1 *= m_heliModel.localMainRotorBladeMatrix1;
    m_heliModel.shadowMainRotorMat1 *= mainRotorSpin;
    m_heliModel.shadowMainRotorMat1 *= inverseShadowScale;
    m_heliModel.shadowMainRotorMat1 *= shadowScaleMat;
    m_heliModel.shadowMainRotorMat1 *= updateMat;
    m_heliModel.shadowMainRotorMat1 *= shadowMat;

    m_heliModel.shadowMainRotorMat2 = m_heliModel.mainRotorBladeTranslationMatrix2;
    m_heliModel.shadowMainRotorMat2 *= mainRotorPitch2;
    m_heliModel.shadowMainRotorMat2 *= m_heliModel.localMainRotorBladeMatrix2;
    m_heliModel.shadowMainRotorMat2 *= mainRotorSpin;
    m_heliModel.shadowMainRotorMat2 *= inverseShadowScale;
    m_heliModel.shadowMainRotorMat2 *= shadowScaleMat;
    m_heliModel.shadowMainRotorMat2 *= updateMat;
    m_heliModel.shadowMainRotorMat2 *= shadowMat;

    // tail rotor blade 1
    m_heliModel.shadowTailRotorMat1 = m_heliModel.localTailRotorBladeMatrix1;
    m_heliModel.shadowTailRotorMat1 *= tailRotorPitch1;
    m_heliModel.shadowTailRotorMat1 *= tailArmSpin;
    m_heliModel.shadowTailRotorMat1 *= m_heliModel.tailRotorBladeTranslationMatrix1;
    m_heliModel.shadowTailRotorMat1 *= inverseShadowScale;
    m_heliModel.shadowTailRotorMat1 *= shadowScaleMat;
    m_heliModel.shadowTailRotorMat1 *= updateMat;
    m_heliModel.shadowTailRotorMat1 *= shadowMat;

    // tail rotor blade 2
    m_heliModel.shadowTailRotorMat2 = m_heliModel.localTailRotorBladeMatrix2;
    m_heliModel.shadowTailRotorMat2 *= tailRotorPitch2;
    m_heliModel.shadowTailRotorMat2 *= tailArmSpin;
    m_heliModel.shadowTailRotorMat2 *= m_heliModel.tailRotorBladeTranslationMatrix2;
    m_heliModel.shadowTailRotorMat2 *= inverseShadowScale;
    m_heliModel.shadowTailRotorMat2 *= shadowScaleMat;
    m_heliModel.shadowTailRotorMat2 *= updateMat;
    m_heliModel.shadowTailRotorMat2 *= shadowMat;

    m_heliModel.shadowMainRotorArmMat = m_heliModel.localMainRotorArmMatrix;
    m_heliModel.shadowMainRotorArmMat *= mainRotorSpin;
    m_heliModel.shadowMainRotorArmMat *= inverseShadowScale;
    m_heliModel.shadowMainRotorArmMat *= shadowScaleMat;
    m_heliModel.shadowMainRotorArmMat *= updateMat;
    m_heliModel.shadowMainRotorArmMat *= shadowMat;

    m_heliModel.shadowTailRotorArmMat = tailArmSpin;
    m_heliModel.shadowTailRotorArmMat *= m_heliModel.localTailRotorArmMatrix;
    m_heliModel.shadowTailRotorArmMat *= inverseShadowScale;
    m_heliModel.shadowTailRotorArmMat *= shadowScaleMat;
    m_heliModel.shadowTailRotorArmMat *= updateMat;
    m_heliModel.shadowTailRotorArmMat *= shadowMat;
}

void Vehicle::UpdatePendulumMotion(Utility::Torque& aTorque, DirectX::SimpleMath::Vector3& aVelocity, const float aTimeStep)
{
    float rodLength = (m_heli.q.mainRotorForceMagnitude * m_heli.controlInput.collectiveInput);
    DirectX::SimpleMath::Vector3 testRod = m_heli.localMainRotorPos - m_heli.localCenterOfMass;
    testRod = m_heli.mainRotorPos - m_heli.centerOfMass;
    rodLength = testRod.Length();

    DirectX::SimpleMath::Vector3 rodUp = m_heli.up;
    DirectX::SimpleMath::Vector3 rodRotor = m_heli.q.mainRotorForceNormal;
    DirectX::SimpleMath::Vector3 rod = rodUp + rodRotor;
    rod = m_heli.mainRotorPos - m_heli.centerOfMass;
    float thetaAngle = Utility::GetAngleBetweenVectors(DirectX::SimpleMath::Vector3::UnitY, rod);

    // body roll torque due to pendulum
    Utility::Torque torqueUpdate;
    torqueUpdate.axis = -rod;
    torqueUpdate.axis = torqueUpdate.axis.Cross(-DirectX::SimpleMath::Vector3::UnitY);;
    torqueUpdate.axis.Normalize();
    //torqueUpdate.magnitude = -(m_heli.gravity.y * aTimeStep) * rodLength * sin(thetaAngle);
    if (m_isUseNewPhysicsTrue == true)
    {
        //torqueUpdate.magnitude = -(m_heli.gravity.y) * rodLength * sin(thetaAngle);
        torqueUpdate.magnitude = -(m_heli.gravity.y * m_testMass) * rodLength * sin(thetaAngle);
    }
    else
    {
        torqueUpdate.magnitude = -(m_heli.gravity.y * aTimeStep) * rodLength * sin(thetaAngle);
        torqueUpdate.magnitude *= aTimeStep; // reducing further to eliminate rotational jitters, needs further investigation
    }
    m_heli.q.pendulumTorqueForceTest = torqueUpdate;
    aTorque = torqueUpdate;

    DirectX::SimpleMath::Vector3 torqueVec = torqueUpdate.axis * torqueUpdate.magnitude;
    //m_debugData->PushDebugLine(m_heli.q.position, torqueVec, 20.0f, 0.0f, DirectX::Colors::Red);
    //m_debugData->PushDebugLine(m_heli.q.position, -rod, 20.0f, 0.0f, DirectX::Colors::Red);
    //m_debugData->PushDebugLinePositionIndicator(m_heli.q.position + (rod), 10.0f, 0.0f, DirectX::Colors::White);
    //DebugPushUILineDecimalNumber("torqueUpdate.magnitude", torqueUpdate.magnitude, "");
    //m_debugData->DebugPushUILineDecimalNumber("torqueUpdate.magnitude", torqueUpdate.magnitude, "");
    //DebugPushUILineDecimalNumber("rod.Length()", rod.Length(), "");
    // acceleration due to pendulum motion
    DirectX::SimpleMath::Vector3 accelerationDirection = torqueUpdate.axis;
    accelerationDirection = accelerationDirection.Cross(-rod);
    accelerationDirection.Normalize();
    float accelerationMag = -((m_heli.gravity.y) / rodLength) * sin(thetaAngle);

    // only update with viable data and if not aligned with gravity direction
    if (accelerationMag > 0.00001f && accelerationMag < 20.0f)
    {
        accelerationDirection *= accelerationMag * aTimeStep;
        accelerationDirection *= accelerationMag;
        //accelerationDirection *= accelerationMag;
        //accelerationDirection.y = 0.0f;
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
    DirectX::SimpleMath::Matrix updateMat = DirectX::SimpleMath::Matrix::CreateWorld(m_heli.q.position, -m_heli.right, m_heli.up);

    m_heli.centerOfMass = m_heli.localCenterOfMass;
    m_heli.centerOfMass = DirectX::SimpleMath::Vector3::Transform(m_heli.localCenterOfMass, updateMat);
    m_heli.mainRotorPos = m_heli.localMainRotorPos;
    m_heli.mainRotorPos = DirectX::SimpleMath::Vector3::Transform(m_heli.localMainRotorPos, updateMat);
    m_heli.tailRotorPos = m_heli.localTailRotorPos;
    m_heli.tailRotorPos = DirectX::SimpleMath::Vector3::Transform(m_heli.localTailRotorPos, updateMat);

    m_heli.landingGearPos = m_heli.localLandingGearPos;
    m_heli.landingGearPos = DirectX::SimpleMath::Vector3::Transform(m_heli.localLandingGearPos, updateMat);

    m_heli.gravityTorqueArmPos = m_heli.localGravityTorqueArmPos;
    m_heli.gravityTorqueArmPos = DirectX::SimpleMath::Vector3::Transform(m_heli.gravityTorqueArmPos, updateMat);
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
        if (mainRotorPitch + pitchAng > 0.0f)
        {
            aHeliData.mainRotor.bladeVec[i].pitchAngle = mainRotorPitch + pitchAng;
        }
        else
        {
            aHeliData.mainRotor.bladeVec[i].pitchAngle = 0.0f;
        }
    }
    float tailRotorPitch = aHeliData.tailRotor.neutralAngle + (aHeliData.controlInput.yawPedalInput * (aHeliData.tailRotor.pitchAngleMax * .5f));
    for (unsigned int i = 0; i < aHeliData.tailRotor.bladeVec.size(); ++i)
    {
        aHeliData.tailRotor.bladeVec[i].pitchAngle = tailRotorPitch;
    }  
}

void Vehicle::UpdateRotorSpin(HeliData& aHeliData, const double aTimer)
{
    const float rpmMax = aHeliData.mainRotorMaxRPM;
    const float rpmMin = 0.0f;
    const float prevRPM = aHeliData.mainRotor.rpm;
    const float rpmThrottleSet = aHeliData.controlInput.throttleInput * rpmMax;
    float rpmUpdate = prevRPM;
    const float currentTorqueCurvePos = (prevRPM / rpmMax) + 0.001f; // Small value added so the value can push past 0 rpm value for prevRPM
    float rpmDelta;
    if (currentTorqueCurvePos < 0.333f)
    {
        const float revDeltaRate = 20.4f;
        //const float revDeltaRate = 100.9f;
        rpmDelta = revDeltaRate * currentTorqueCurvePos;
    }
    else if (currentTorqueCurvePos < 0.666f)
    {
        const float revDeltaRate = 2.75f;
        //const float revDeltaRate = 32.75f;
        rpmDelta = revDeltaRate * currentTorqueCurvePos;
    }
    else
    {
        const float revDeltaRate = 1.85f;
        //const float revDeltaRate = 21.85f;
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

    const float tailRotarGearing = 0.75f;
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

void Vehicle::UpdateTerrainData()
{
    bool isVehicleInPlayUpdate = m_environment->GetVehicleUpdateData(m_heli.q.position, m_heli.terrainNormal,
        m_heli.terrainHightAtPos, m_heli.groundPlane);
    if (isVehicleInPlayUpdate == false)
    {
        // to do: add error handling if out of play
    }

    m_heli.altitude = m_heli.landingGearPos.y - m_heli.terrainHightAtPos;
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
    m_testTimer2 += static_cast<float>(aTimeDelta);
    
    //DebugPushTestLinePositionIndicator(m_heli.q.position, 4.0f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 0.0f, 0.0f, 1.0f));
    UpdatePhysicsPoints(m_heli);
    
    UpdateBladeLiftForce(static_cast<float>(aTimeDelta));  

    m_heli.q.bodyTorqueForce.axis = DirectX::SimpleMath::Vector3::Zero;
    m_heli.q.bodyTorqueForce.magnitude = 0.0f;
    
    DirectX::SimpleMath::Vector3 prevVelocity = m_heli.q.velocity;
    DirectX::SimpleMath::Vector3 prevPos = m_heli.q.position;
    m_prevPos = m_heli.q.position;
    m_prevRight = m_heli.right;
    m_prevUp = m_heli.up;

    UpdateCyclicStick(m_heli.controlInput);
    //UpdateRotorForce();

    m_heli.isVehicleLanding = false;
    m_heli.terrainHightAtPos = m_environment->GetTerrainHeightAtPos(m_heli.q.position);
    
    //m_heli.isVehicleAirborne = true;
    //if (m_heli.q.position.y - m_heli.terrainHightAtPos > 0.1)
    if (m_heli.landingGearPos.y - m_heli.terrainHightAtPos > 0.1)
    {
        m_heli.isVehicleAirborne = true;
        m_testTimer += aTimeDelta;
    }
    else
    {
        if (m_heli.isVehicleAirborne == true)
        {
            LandVehicle();
            m_heli.isVehicleLanding = true;
            m_testTimer2 = m_testTimer;
            m_testTimer = 0.0f;
        }
        m_heli.isVehicleAirborne = false;
        //m_heli.q.position.y = m_heli.terrainHightAtPos - m_heli.localLandingGearPos.y;
    }

    //UpdateTerrainNorm();      
    UpdateTerrainData();

    UpdateCentrifugalForce(static_cast<float>(aTimeDelta));
    m_heli.angularDrag = CalculateDragAngularLocal(m_heli.q.angularVelocity);
    m_heli.angularDrag2 = CalculateDragAngularLocal(m_heli.q.angularMomentum);
    RungeKutta4(&m_heli, aTimeDelta);
    UpdateRotorData(m_heli, aTimeDelta);
    
    if (m_heli.forward.Dot(m_heli.q.velocity) < 0.0)
    {
        m_heli.isVelocityBackwards = true;
    }
    else
    {
        m_heli.isVelocityBackwards = false;
    }

    DirectX::SimpleMath::Vector3 speed = m_heli.q.velocity;
    speed.y = 0.0f;
    m_heli.speed = speed.Length();

    UpdateModel();
    InputDecay(aTimeDelta);

    m_heli.testAccel = (m_heli.q.velocity.Length() - prevVelocity.Length()) / static_cast<float>(aTimeDelta);
    m_heli.testAccelVec = (m_heli.q.velocity - prevVelocity) / static_cast<float>(aTimeDelta);

    UpdateResistance();
    
    //m_heli.q.bodyTorqueForce.axis = DirectX::SimpleMath::Vector3::Zero;
    //m_heli.q.bodyTorqueForce.magnitude = 0.0f;

    m_rotorTimerTest += aTimeDelta;

    UpdateAlignmentTorqueTest(static_cast<float>(aTimeDelta));
    UpdateAlignmentCamera();

    if (m_heli.landingGear.currentState == LandingGear::LandingGearState::LANDINGGEARSTATE_ASCENDING || m_heli.landingGear.currentState == LandingGear::LandingGearState::LANDINGGEARSTATE_DESCENDING)
    {
        UpdateLandingGear(m_heli.landingGear, aTimeDelta);
    }

    /*
    const float x = 5.0f;
    const float y = 2.0f;
    const float z = 2.0f;
    DirectX::SimpleMath::Vector3 pos1 = DirectX::SimpleMath::Vector3(x, 0.0, 0.0);
    DirectX::SimpleMath::Vector3 pos2 = DirectX::SimpleMath::Vector3(0.0, y, 0.0);
    DirectX::SimpleMath::Vector3 pos3 = DirectX::SimpleMath::Vector3(0.0, 0.0, z);
    DirectX::SimpleMath::Vector3 pos4 = DirectX::SimpleMath::Vector3(-x, 0.0, 0.0);
    DirectX::SimpleMath::Vector3 pos5 = DirectX::SimpleMath::Vector3(0.0, -y, 0.0);
    m_debugData->PushDebugLinePositionIndicator(m_heli.q.position + pos1, 10.0f, 0.0f, DirectX::Colors::Red);
    m_debugData->PushDebugLinePositionIndicator(m_heli.q.position + pos2, 10.0f, 0.0f, DirectX::Colors::Red);
    m_debugData->PushDebugLinePositionIndicator(m_heli.q.position + pos3, 10.0f, 0.0f, DirectX::Colors::Red);
    m_debugData->PushDebugLinePositionIndicator(m_heli.q.position + pos4, 10.0f, 0.0f, DirectX::Colors::Red);
    //m_debugData->PushDebugLinePositionIndicator(m_heli.q.position + pos5, 10.0f, 0.0f, DirectX::Colors::Red);
    //m_debugData->PushDebugLinePositionIndicator(m_heli.centerOfMass, 10.0f, 0.0f, DirectX::Colors::Red);
    //m_debugData->PushDebugLinePositionIndicator(m_heli.mainRotorPos, 10.0f, 0.0f, DirectX::Colors::Red);
    */

    m_debugData->DebugPushUILineWholeNumber("m_debugToggle = ", m_debugToggle, "");
    m_debugData->DebugPushUILineWholeNumber("m_debugToggle2 = ", m_debugToggle2, "");

    DirectX::SimpleMath::Vector3 cyclicTest = UpdateRotorForceRunge();
    m_debugData->DebugPushUILineDecimalNumber("cyclicTest.x = ", cyclicTest.x, "");
    m_debugData->DebugPushUILineDecimalNumber("cyclicTest.y = ", cyclicTest.y, "");
    m_debugData->DebugPushUILineDecimalNumber("cyclicTest.z = ", cyclicTest.z, "");


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

void Vehicle::DebugPushTestLinePositionIndicator(const DirectX::SimpleMath::Vector3 aPoint, const float aLineLength, const float aOffset, const DirectX::SimpleMath::Vector4 aColor)
{
    DebugPushTestLine(aPoint, DirectX::SimpleMath::Vector3::UnitX, aLineLength, aOffset, aColor);
    DebugPushTestLine(aPoint, DirectX::SimpleMath::Vector3::UnitY, aLineLength, aOffset, aColor);
    DebugPushTestLine(aPoint, DirectX::SimpleMath::Vector3::UnitZ, aLineLength, aOffset, aColor);
    DebugPushTestLine(aPoint, -DirectX::SimpleMath::Vector3::UnitX, aLineLength, aOffset, aColor);
    DebugPushTestLine(aPoint, -DirectX::SimpleMath::Vector3::UnitY, aLineLength, aOffset, aColor);
    DebugPushTestLine(aPoint, -DirectX::SimpleMath::Vector3::UnitZ, aLineLength, aOffset, aColor);
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

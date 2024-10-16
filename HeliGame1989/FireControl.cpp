#include "pch.h"
#include "FireControl.h"

void FireControl::DeleteProjectileFromVec(const unsigned int aIndex)
{
    if (aIndex > m_projectileVec.size())
    {
        // add error handling here
    }
    else
    {
        std::vector<ProjectileData>::iterator it;
        it = m_projectileVec.begin() + aIndex;
        m_projectileVec.erase(it);
    }
}

void FireControl::DrawProjectile(const DirectX::SimpleMath::Matrix aView, const DirectX::SimpleMath::Matrix aProj)
{
    //m_heliModel.mainRotorHaloShape->Draw(m_heliModel.mainRotorHaloMatrix, aView, aProj, m_heliModel.bodyColor);
    //DirectX::SimpleMath::Matrix updateMat = DirectX::SimpleMath::Matrix::CreateWorld(m_heli.q.position, -m_heli.right, m_heli.up);
    //m_heliModel.cannonBarrelMatrix = m_heliModel.scaleCannonBarrelMatrix;
    //m_heliModel.cannonBarrelMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(0.0f));
    //m_heliModel.cannonBarrelMatrix *= m_heliModel.localCannonBarrelMatrix;
    //m_heliModel.cannonBarrelMatrix *= updateMat;
    DirectX::SimpleMath::Vector4 projectileColor(1.0f, 1.0f, 1.0f, 1.0f);
    //AmmoData projData = m_ballAmmo;
    //ProjectileModel projAmmo = m_ballAmmo->ammoModel;
    //projAmmo.projectileShape = m_ballAmmo.ammoModel.projectileShape;

    for (unsigned int i = 0; i < m_projectileVec.size(); ++i)
    {
        DirectX::SimpleMath::Matrix projMat = m_ballAmmo.ammoModel.localProjectileMatrix;
        projMat *= DirectX::SimpleMath::Matrix::CreateTranslation(m_projectileVec[i].q.position);
        m_ballAmmo.ammoModel.projectileShape->Draw(projMat, aView, aProj, projectileColor);
    }
}

void FireControl::FireProjectile(AmmoType aAmmoType, const DirectX::SimpleMath::Vector3 aLaunchPos, const DirectX::SimpleMath::Vector3 aLaunchDirection, const DirectX::SimpleMath::Vector3 aLauncherVelocity)
{
    ProjectileData firedProjectile;
    firedProjectile.projectileAmmoType = aAmmoType;
    firedProjectile.q.position = aLaunchPos;
    firedProjectile.q.velocity = (m_ballAmmo.launchVelocity * aLaunchDirection) + aLauncherVelocity;
    firedProjectile.time = 0.0f;
    m_projectileVec.push_back(firedProjectile);
}

void FireControl::InitializeAmmo(AmmoData& aAmmo)
{
    aAmmo.ammoType = AmmoType::AMMOTYPE_BALL01;
    aAmmo.airDensity = 1.25f;
    aAmmo.dragCoefficient = 0.3f;
    aAmmo.gravity = -9.8f;
    aAmmo.launchVelocity = 25.0f;
    aAmmo.mass = 0.2f;
    aAmmo.surfaceArea = 0.15f;
    aAmmo.radius = 0.12f;
}

void FireControl::InitializeFireControl(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext, const DirectX::SimpleMath::Vector3 aLaunchPos, const DirectX::SimpleMath::Vector3 aLaunchDirection, Environment const* aEnvironment)
{
    m_projectileVec.clear();
    m_environment = aEnvironment;
    InitializeAmmo(m_ballAmmo);
    InitializeProjectileModel(aContext, m_ballAmmo);
    InitializeLauncherData(m_launcherData, aLaunchPos, aLaunchDirection);
}

void FireControl::InitializeLauncherData(LauncherData& aLauncher, const DirectX::SimpleMath::Vector3 aPosition, const DirectX::SimpleMath::Vector3 aDirection)
{
    aLauncher.launchDirectionNorm = aDirection;
    //aLauncher.launchVelocity = 10.0f;
    aLauncher.launcherPosition = aPosition;
    aLauncher.reloadCoolDown = 3.0f;
    aLauncher.coolDownTimer = 0.0f;
}

void FireControl::InitializeProjectileModel(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext, AmmoData& aAmmo)
{
    const float ammoSize = aAmmo.radius;
    aAmmo.ammoModel.projectileShape = DirectX::GeometricPrimitive::CreateSphere(aContext.Get(), ammoSize);
    aAmmo.ammoModel.projectileMatrix = DirectX::SimpleMath::Matrix::Identity;
    aAmmo.ammoModel.localProjectileMatrix = aAmmo.ammoModel.projectileMatrix;
}


void FireControl::RightHandSide(struct ProjectileData* aProjectile, ProjectileMotion* aQ, ProjectileMotion* aDeltaQ, double aTimeDelta, float aQScale, ProjectileMotion* aDQ)
{
    //  Compute the intermediate values of the 
    //  dependent variables.
    ProjectileMotion newQ;
    newQ.velocity = aQ->velocity + static_cast<float>(aQScale) * aDeltaQ->velocity;
    newQ.position = aQ->position + static_cast<float>(aQScale) * aDeltaQ->position;

    DirectX::SimpleMath::Vector3 velocityUpdate = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 gravForce = DirectX::SimpleMath::Vector3(0.0f, -9.8f, 0.0f);
    velocityUpdate += gravForce;

    float mass = aProjectile->mass;

    aDQ->velocity = static_cast<float>(aTimeDelta) * (velocityUpdate / mass);
    aDQ->position = static_cast<float>(aTimeDelta) * newQ.velocity;
}

void FireControl::RungeKutta4(struct ProjectileData* aProjectile, double aTimeDelta)
{
    //  Define a convenience variables
    //const float numEqns = static_cast<float>(aHeli->numEqns);
    const float numEqns = static_cast<float>(6);
    //  Retrieve the current values of the dependent and independent variables.
    ProjectileMotion q = aProjectile->q;
    ProjectileMotion dq1;
    ProjectileMotion dq2;
    ProjectileMotion dq3;
    ProjectileMotion dq4;

    // Compute the four Runge-Kutta steps, The return 
    // value of RightHandSide method is an array
    // of delta-q values for each of the four steps.
    RightHandSide(aProjectile, &q, &q, aTimeDelta, 0.0, &dq1);
    RightHandSide(aProjectile, &q, &dq1, aTimeDelta, 0.5, &dq2);
    RightHandSide(aProjectile, &q, &dq2, aTimeDelta, 0.5, &dq3);
    RightHandSide(aProjectile, &q, &dq3, aTimeDelta, 1.0, &dq4);
    aProjectile->time = aProjectile->time + static_cast<float>(aTimeDelta);

    DirectX::SimpleMath::Vector3 posUpdate = (dq1.position + 2.0 * dq2.position + 2.0 * dq3.position + dq4.position) / numEqns;
    DirectX::SimpleMath::Vector3 velocityUpdate = (dq1.velocity + 2.0 * dq2.velocity + 2.0 * dq3.velocity + dq4.velocity) / numEqns;

    q.velocity += velocityUpdate;
    q.position += posUpdate;
    aProjectile->q.velocity = q.velocity;
    aProjectile->q.position = q.position;
}

void FireControl::UpdateProjectileVec(double aTimeDelta)
{
    for (unsigned int i = 0; i < m_projectileVec.size(); ++i)
    {
        RungeKutta4(&m_projectileVec[i], aTimeDelta);
    }

    for (unsigned int i = 0; i < m_projectileVec.size(); ++i)
    {
        if (m_projectileVec[i].time > 30.0f)
        {
            DeleteProjectileFromVec(i);
        }
    }
}
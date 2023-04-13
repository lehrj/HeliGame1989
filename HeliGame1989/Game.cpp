//
// Game.cpp
//
#include <stdlib.h>  // for random numbers
#include <time.h>    // timer for random numbers

#include "pch.h"
#include "Game.h"

extern void ExitGame() noexcept;

using namespace DirectX;
using Microsoft::WRL::ComPtr;

Game::Game() noexcept :
    m_window(nullptr),
    m_outputWidth(1600),
    m_outputHeight(900),

    m_featureLevel(D3D_FEATURE_LEVEL_9_1)
{
    m_outputWidth = m_outputWidthDefault;
    m_outputHeight = m_outputHeightDefault;
    //srand(time(NULL));
    srand(0);

    m_debugData = std::make_shared<DebugData>();
    m_environment = new Environment();

    m_camera = new Camera(m_outputWidth, m_outputHeight);
    m_camera->InintializePreSwingCamera(DirectX::SimpleMath::Vector3::Zero, 0.0);
    m_lighting = new Lighting();
    m_vehicle = new Vehicle();
    m_vehicle->SetEnvironment(m_environment);
    m_camera->SetVehicleFocus(m_vehicle);
    m_camera->SetCameraEnvironment(m_environment);

    if (m_isInDebugMode == false)
    {
        m_currentGameState = GameState::GAMESTATE_INTROSCREEN;
    }
    else
    {
        m_currentGameState = GameState::GAMESTATE_STARTSCREEN;
    }

    m_currentGameState = GameState::GAMESTATE_GAMEPLAY;
    //m_currentGameState = GameState::GAMESTATE_GAMEPLAYSTART;
    //m_currentGameState = GameState::GAMESTATE_INTROSCREEN;
    //m_currentGameState = GameState::GAMESTATE_STARTSCREEN;
    m_lighting->SetLighting(Lighting::LightingState::LIGHTINGSTATE_TEST01);
    //m_lighting->SetLighting(Lighting::LightingState::LIGHTINGSTATE_STARTSCREEN);
    //m_lighting->SetLightingNormColorTextureVertex(Lighting::LightingState::LIGHTINGSTATE_TEST01);
    //m_lighting->SetLightingNormColorVertex2(Lighting::LightingState::LIGHTINGSTATE_TEST01);
    //m_lighting->SetLightingColorVertex3(Lighting::LightingState::LIGHTINGSTATE_TEST01);

    m_currentUiState = UiState::UISTATE_SWING;
    InitializeWorldGrid();
}

Game::~Game()
{
    if (m_audioEngine)
    {
        m_audioEngine->Suspend();
    }
    m_audioMusicStream.reset();
    m_audioEffectStream.reset();

    delete m_camera;
    delete m_environment;
    delete m_lighting;
    delete m_vehicle;

    /*
    delete[] m_terrainVertexArray;
    m_terrainVertexArray = 0;
    delete[] m_terrainVertexArrayBase;
    m_terrainVertexArrayBase = 0;

    delete[] m_terrainVertexArray2;
    m_terrainVertexArray2 = 0;
    delete[] m_terrainVertexArrayBase2;
    m_terrainVertexArrayBase2 = 0;
    */

    /*
    delete[] m_terrainVertexArrayGamePlay;
    m_terrainVertexArrayGamePlay = 0;
    delete[] m_terrainVertexArrayBaseGamePlay;
    m_terrainVertexArrayBaseGamePlay = 0;

    delete[] m_terrainVertexArrayStartScreen;
    m_terrainVertexArrayStartScreen = 0;
    delete[] m_terrainVertexArrayBaseStartScreen;
    m_terrainVertexArrayBaseStartScreen = 0;
    */

    delete[] m_terrainGamePlay.terrainVertexArray;
    m_terrainGamePlay.terrainVertexArray = 0;
    delete[] m_terrainGamePlay.terrainVertexArrayBase;
    m_terrainGamePlay.terrainVertexArrayBase = 0;

    delete[] m_terrainStartScreen.terrainVertexArray;
    m_terrainStartScreen.terrainVertexArray = 0;
    delete[] m_terrainStartScreen.terrainVertexArrayBase;
    m_terrainStartScreen.terrainVertexArrayBase = 0;
}

void Game::AudioPlayMusic(XACT_WAVEBANK_AUDIOBANK aSFX)
{
    m_audioMusicStream = m_audioBank->CreateStreamInstance(aSFX);

    if (m_audioMusicStream)
    {
        m_audioMusicStream->SetVolume(m_musicVolume);
        m_audioMusicStream->Play(true);
    }
}

void Game::AudioPlaySFX(XACT_WAVEBANK_AUDIOBANK aSFX)
{
    m_audioEffectStream = m_audioBank->CreateStreamInstance(aSFX);
    if (m_audioEffectStream)
    {
        m_audioEffectStream->SetVolume(m_sfxVolume);
        m_audioEffectStream->Play();
    }
}

// Helper method to clear the back buffers.
void Game::Clear()
{
    // Clear the views.
    m_d3dContext->ClearRenderTargetView(m_renderTargetView.Get(), DirectX::Colors::Black);
    m_d3dContext->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    m_d3dContext->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), m_depthStencilView.Get());

    // Set the viewport.
    CD3D11_VIEWPORT viewport(0.0f, 0.0f, static_cast<float>(m_outputWidth), static_cast<float>(m_outputHeight));
    m_d3dContext->RSSetViewports(1, &viewport);
}

// These are the resources that depend on the device.
void Game::CreateDevice()
{
    UINT creationFlags = 0;

#ifdef _DEBUG
    creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    static const D3D_FEATURE_LEVEL featureLevels[] =
    {
        // TODO: Modify for supported Direct3D feature levels
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
        D3D_FEATURE_LEVEL_9_3,
        D3D_FEATURE_LEVEL_9_2,
        D3D_FEATURE_LEVEL_9_1
    };

    // Create the DX11 API device object, and get a corresponding context.
    ComPtr<ID3D11Device> device;
    ComPtr<ID3D11DeviceContext> context;
    DX::ThrowIfFailed(D3D11CreateDevice(
        nullptr,                            // specify nullptr to use the default adapter
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        creationFlags,
        featureLevels,
        _countof(featureLevels),
        D3D11_SDK_VERSION,
        device.ReleaseAndGetAddressOf(),    // returns the Direct3D device created
        &m_featureLevel,                    // returns feature level of device created
        context.ReleaseAndGetAddressOf()    // returns the device immediate context
    ));

#ifndef NDEBUG
    ComPtr<ID3D11Debug> d3dDebug;
    if (SUCCEEDED(device.As(&d3dDebug)))
    {
        ComPtr<ID3D11InfoQueue> d3dInfoQueue;
        if (SUCCEEDED(d3dDebug.As(&d3dInfoQueue)))
        {
#ifdef _DEBUG
            d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_CORRUPTION, true);
            d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_ERROR, true);
#endif
            D3D11_MESSAGE_ID hide[] =
            {
                D3D11_MESSAGE_ID_SETPRIVATEDATA_CHANGINGPARAMS,
                // TODO: Add more message IDs here as needed.
            };
            D3D11_INFO_QUEUE_FILTER filter = {};
            filter.DenyList.NumIDs = _countof(hide);
            filter.DenyList.pIDList = hide;
            d3dInfoQueue->AddStorageFilterEntries(&filter);
        }
    }
#endif

    DX::ThrowIfFailed(device.As(&m_d3dDevice));
    DX::ThrowIfFailed(context.As(&m_d3dContext));

    // Jackson Industry textures
    DX::ThrowIfFailed(CreateWICTextureFromFile(m_d3dDevice.Get(), L"../HeliGame1989/Art/Textures/skyTexture.jpg", nullptr, m_textureSky.ReleaseAndGetAddressOf()));

    DX::ThrowIfFailed(CreateWICTextureFromFile(m_d3dDevice.Get(), L"../HeliGame1989/Art/Textures/blankTexture.jpg", nullptr, m_texture.ReleaseAndGetAddressOf()));
    DX::ThrowIfFailed(CreateWICTextureFromFile(m_d3dDevice.Get(), L"../HeliGame1989/Art/NormalMaps/blankNormal.jpg", nullptr, m_normalMap.ReleaseAndGetAddressOf()));
    DX::ThrowIfFailed(CreateWICTextureFromFile(m_d3dDevice.Get(), L"../HeliGame1989/Art/SpecularMaps/blankSpecular.jpg", nullptr, m_specular.ReleaseAndGetAddressOf()));
    //DX::ThrowIfFailed(CreateWICTextureFromFile(m_d3dDevice.Get(), L"../HeliGame1989/Art/NormalMaps/normBMW2.png", nullptr, m_normalMap.ReleaseAndGetAddressOf()));
    //DX::ThrowIfFailed(CreateWICTextureFromFile(m_d3dDevice.Get(), L"../HeliGame1989/Art/SpecularMaps/specularJI.png", nullptr, m_specular.ReleaseAndGetAddressOf()));
    // test textures
    DX::ThrowIfFailed(CreateWICTextureFromFile(m_d3dDevice.Get(), L"../HeliGame1989/Art/Textures/TestOP.png", nullptr, m_textureTest.ReleaseAndGetAddressOf()));
    DX::ThrowIfFailed(CreateWICTextureFromFile(m_d3dDevice.Get(), L"../HeliGame1989/Art/NormalMaps/TestOP.png", nullptr, m_normalMapTest.ReleaseAndGetAddressOf()));
    DX::ThrowIfFailed(CreateWICTextureFromFile(m_d3dDevice.Get(), L"../HeliGame1989/Art/SpecularMaps/SpecularTeaser1.png", nullptr, m_specularTest.ReleaseAndGetAddressOf()));

    // Jackson Industry textures
    DX::ThrowIfFailed(CreateWICTextureFromFile(m_d3dDevice.Get(), L"../HeliGame1989/Art/Textures/logoJI2.png", nullptr, m_textureJI.ReleaseAndGetAddressOf()));
    DX::ThrowIfFailed(CreateWICTextureFromFile(m_d3dDevice.Get(), L"../HeliGame1989/Art/NormalMaps/normJI.png", nullptr, m_normalMapJI.ReleaseAndGetAddressOf()));
    DX::ThrowIfFailed(CreateWICTextureFromFile(m_d3dDevice.Get(), L"../HeliGame1989/Art/SpecularMaps/specularJI2.png", nullptr, m_specularJI.ReleaseAndGetAddressOf()));

    // BMW textures
    DX::ThrowIfFailed(CreateWICTextureFromFile(m_d3dDevice.Get(), L"../HeliGame1989/Art/Textures/logoBMW.png", nullptr, m_textureBMW.ReleaseAndGetAddressOf()));
    DX::ThrowIfFailed(CreateWICTextureFromFile(m_d3dDevice.Get(), L"../HeliGame1989/Art/NormalMaps/normBMW.png", nullptr, m_normalMapBMW.ReleaseAndGetAddressOf()));
    DX::ThrowIfFailed(CreateWICTextureFromFile(m_d3dDevice.Get(), L"../HeliGame1989/Art/NormalMaps/normBMW.png", nullptr, m_normalMapBMW2.ReleaseAndGetAddressOf()));
    DX::ThrowIfFailed(CreateWICTextureFromFile(m_d3dDevice.Get(), L"../HeliGame1989/Art/SpecularMaps/specularBMW.png", nullptr, m_specularBMW.ReleaseAndGetAddressOf()));
    // Start screen textures
    DX::ThrowIfFailed(CreateWICTextureFromFile(m_d3dDevice.Get(), L"../HeliGame1989/Art/Textures/textureUV.png", nullptr, m_textureAutoGame.ReleaseAndGetAddressOf()));
    DX::ThrowIfFailed(CreateWICTextureFromFile(m_d3dDevice.Get(), L"../HeliGame1989/Art/NormalMaps/normalUV.png", nullptr, m_normalMapAutoGame.ReleaseAndGetAddressOf()));
    DX::ThrowIfFailed(CreateWICTextureFromFile(m_d3dDevice.Get(), L"../HeliGame1989/Art/SpecularMaps/specularUV.png", nullptr, m_specularAutoGame.ReleaseAndGetAddressOf()));

    // Textures for teaser trailer    
    DX::ThrowIfFailed(CreateWICTextureFromFile(m_d3dDevice.Get(), L"../HeliGame1989/Art/Textures/LogoTeaser.png", nullptr, m_textureTeaser.ReleaseAndGetAddressOf()));
    DX::ThrowIfFailed(CreateWICTextureFromFile(m_d3dDevice.Get(), L"../HeliGame1989/Art/NormalMaps/NormalMapTeaser.png", nullptr, m_normalMapTeaser.ReleaseAndGetAddressOf()));
    DX::ThrowIfFailed(CreateWICTextureFromFile(m_d3dDevice.Get(), L"../HeliGame1989/Art/SpecularMaps/SpecularTeaser.png", nullptr, m_specularTeaser.ReleaseAndGetAddressOf()));

    // TODO: Initialize device dependent objects here (independent of window size).
    m_world = DirectX::SimpleMath::Matrix::Identity;
    m_states = std::make_unique<CommonStates>(m_d3dDevice.Get());
    m_effect = std::make_unique<NormalMapEffect>(m_d3dDevice.Get());

    // Make sure you called CreateDDSTextureFromFile and CreateWICTextureFromFile before this point!
    m_effect->SetTexture(m_texture.Get());
    m_effect->SetNormalTexture(m_normalMap.Get());
    m_effect->SetLightDiffuseColor(0, Colors::White);
    m_effect->SetAlpha(1.0);

    m_effect->SetFogEnabled(false);
    m_effect->SetFogColor(DirectX::Colors::Black);
    m_effect->SetFogStart(1.0);
    m_effect->SetFogEnd(4.0);

    m_effect2 = std::make_unique<BasicEffect>(m_d3dDevice.Get());
    //m_effect2->SetTextureEnabled(false);
    m_effect2->SetVertexColorEnabled(true);
    m_effect2->EnableDefaultLighting();
    m_effect2->SetLightDiffuseColor(0, Colors::Gray);

    m_effect3 = std::make_unique<BasicEffect>(m_d3dDevice.Get());
    m_effect3->SetVertexColorEnabled(true);

    void const* shaderByteCode2;
    size_t byteCodeLength2;
    m_effect2->GetVertexShaderBytecode(&shaderByteCode2, &byteCodeLength2);
    DX::ThrowIfFailed(m_d3dDevice->CreateInputLayout(VertexType2::InputElements, VertexType2::InputElementCount, shaderByteCode2, byteCodeLength2, m_inputLayout.ReleaseAndGetAddressOf()));
    const int maxVertices = 8192;
    const int maxIndices = maxVertices * 3;
    m_batch2 = std::make_unique<PrimitiveBatch<VertexType2>>(m_d3dContext.Get(), maxIndices, maxVertices);

    void const* shaderByteCode;
    size_t byteCodeLength;
    m_effect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);
    DX::ThrowIfFailed(m_d3dDevice->CreateInputLayout(VertexType::InputElements, VertexType::InputElementCount, shaderByteCode, byteCodeLength, m_inputLayout.ReleaseAndGetAddressOf()));
    m_batch = std::make_unique<PrimitiveBatch<VertexType>>(m_d3dContext.Get());

    void const* shaderByteCode3;
    size_t byteCodeLength3;
    m_effect3->GetVertexShaderBytecode(&shaderByteCode3, &byteCodeLength3);
    DX::ThrowIfFailed(m_d3dDevice->CreateInputLayout(VertexType3::InputElements, VertexType3::InputElementCount, shaderByteCode3, byteCodeLength3, m_inputLayout.ReleaseAndGetAddressOf()));
    m_batch3 = std::make_unique<PrimitiveBatch<VertexType3>>(m_d3dContext.Get());

    //m_shape = GeometricPrimitive::CreateCube(m_d3dContext.Get(), 20000.0f, false);
    m_skyShape = GeometricPrimitive::CreateSphere(m_d3dContext.Get(), 200000.0f, 32, false);
    m_skyShape->CreateInputLayout(m_effect.get(), m_inputLayout.ReleaseAndGetAddressOf());

    
    CD3D11_RASTERIZER_DESC rastDesc(D3D11_FILL_SOLID, D3D11_CULL_NONE, FALSE,
        D3D11_DEFAULT_DEPTH_BIAS, D3D11_DEFAULT_DEPTH_BIAS_CLAMP,
        D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS, TRUE, FALSE, FALSE, TRUE);
    
     // For multisampling rendering
    /*
    CD3D11_RASTERIZER_DESC rastDesc(D3D11_FILL_SOLID, D3D11_CULL_NONE, FALSE,
        D3D11_DEFAULT_DEPTH_BIAS, D3D11_DEFAULT_DEPTH_BIAS_CLAMP,
        D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS, TRUE, FALSE, TRUE, TRUE); // Multisampling
    */
    DX::ThrowIfFailed(m_d3dDevice->CreateRasterizerState(&rastDesc, m_raster.ReleaseAndGetAddressOf()));

    m_font = std::make_unique<SpriteFont>(m_d3dDevice.Get(), L"Art/Fonts/myfile.spritefont");
    m_titleFont = std::make_unique<SpriteFont>(m_d3dDevice.Get(), L"Art/Fonts/titleFont.spritefont");
    m_bitwiseFont = std::make_unique<SpriteFont>(m_d3dDevice.Get(), L"Art/Fonts/bitwise24.spritefont");
    m_spriteBatch = std::make_unique<SpriteBatch>(m_d3dContext.Get());

    // Start Texture
    ComPtr<ID3D11Resource> resource;

    // Character Select Textures
    DX::ThrowIfFailed(CreateWICTextureFromFile(m_d3dDevice.Get(), L"../HeliGame1989/Art/Textures/PacSprite.png", nullptr, m_pacTexture.ReleaseAndGetAddressOf()));
    m_pacSprite = std::make_unique<AnimatedTexture>();
    m_pacSprite->Load(m_pacTexture.Get(), 4, 6);

    DX::ThrowIfFailed(CreateWICTextureFromFile(m_d3dDevice.Get(), L"../HeliGame1989/Art/Textures/ChacterSpriteSheet.png", nullptr, m_characterTexture.ReleaseAndGetAddressOf()));
    m_character = std::make_unique<AnimatedTexture>();
    m_character->Load(m_characterTexture.Get(), 4, 6);

    DX::ThrowIfFailed(CreateWICTextureFromFile(m_d3dDevice.Get(), L"../HeliGame1989/Art/Textures/Chacter0SpriteSheet.png", nullptr, m_character0Texture.ReleaseAndGetAddressOf()));
    m_character0 = std::make_unique<AnimatedTexture>();
    m_character0->Load(m_character0Texture.Get(), 4, 6);

    DX::ThrowIfFailed(CreateWICTextureFromFile(m_d3dDevice.Get(), L"../HeliGame1989/Art/Textures/Chacter1SpriteSheet.png", nullptr, m_character1Texture.ReleaseAndGetAddressOf()));
    m_character1 = std::make_unique<AnimatedTexture>();
    m_character1->Load(m_character1Texture.Get(), 4, 6);

    DX::ThrowIfFailed(CreateWICTextureFromFile(m_d3dDevice.Get(), L"../HeliGame1989/Art/Textures/Chacter2SpriteSheet.png", nullptr, m_character2Texture.ReleaseAndGetAddressOf()));
    m_character2 = std::make_unique<AnimatedTexture>();
    m_character2->Load(m_character2Texture.Get(), 4, 6);

    DX::ThrowIfFailed(CreateWICTextureFromFile(m_d3dDevice.Get(), L"../HeliGame1989/Art/Textures/CharacterBackground.png", resource.GetAddressOf(), m_characterBackgroundTexture.ReleaseAndGetAddressOf()));
    ComPtr<ID3D11Texture2D> characterBackground;
    DX::ThrowIfFailed(resource.As(&characterBackground));
    CD3D11_TEXTURE2D_DESC characterBackgroundDesc;
    characterBackground->GetDesc(&characterBackgroundDesc);
    m_characterBackgroundOrigin.x = float(characterBackgroundDesc.Width / 2);
    m_characterBackgroundOrigin.y = float(characterBackgroundDesc.Height / 2);

    // Environment Select textures
    DX::ThrowIfFailed(CreateWICTextureFromFile(m_d3dDevice.Get(), L"../HeliGame1989/Art/Textures/EnvironmentSelectCalm.png", nullptr, m_environSelectCalmTexture.ReleaseAndGetAddressOf()));
    DX::ThrowIfFailed(CreateWICTextureFromFile(m_d3dDevice.Get(), L"../HeliGame1989/Art/Textures/EnvironmentSelectCalm.png", resource.GetAddressOf(), m_environSelectCalmTexture.ReleaseAndGetAddressOf()));
    ComPtr<ID3D11Texture2D> pEnvironmentSelectCalm;
    DX::ThrowIfFailed(resource.As(&pEnvironmentSelectCalm));
    CD3D11_TEXTURE2D_DESC EnvironmentSelectCalmDesc;
    pEnvironmentSelectCalm->GetDesc(&EnvironmentSelectCalmDesc);
    m_environSelectCalmOrigin.x = float(EnvironmentSelectCalmDesc.Width / 2);
    m_environSelectCalmOrigin.y = float(EnvironmentSelectCalmDesc.Height / 2);

    DX::ThrowIfFailed(CreateWICTextureFromFile(m_d3dDevice.Get(), L"../HeliGame1989/Art/Textures/EnvironmentSelectBreezy.png", nullptr, m_environSelectBreezyTexture.ReleaseAndGetAddressOf()));
    DX::ThrowIfFailed(CreateWICTextureFromFile(m_d3dDevice.Get(), L"../HeliGame1989/Art/Textures/EnvironmentSelectBreezy.png", resource.GetAddressOf(), m_environSelectBreezyTexture.ReleaseAndGetAddressOf()));
    ComPtr<ID3D11Texture2D> pEnvironmentSelectBreezy;
    DX::ThrowIfFailed(resource.As(&pEnvironmentSelectBreezy));
    CD3D11_TEXTURE2D_DESC EnvironmentSelectBreezyDesc;
    pEnvironmentSelectBreezy->GetDesc(&EnvironmentSelectBreezyDesc);
    m_environSelectBreezyOrigin.x = float(EnvironmentSelectBreezyDesc.Width / 2);
    m_environSelectBreezyOrigin.y = float(EnvironmentSelectBreezyDesc.Height / 2);

    DX::ThrowIfFailed(CreateWICTextureFromFile(m_d3dDevice.Get(), L"../HeliGame1989/Art/Textures/EnvironmentSelectAlien.jpg", nullptr, m_environSelectAlienTexture.ReleaseAndGetAddressOf()));
    DX::ThrowIfFailed(CreateWICTextureFromFile(m_d3dDevice.Get(), L"../HeliGame1989/Art/Textures/EnvironmentSelectAlien.jpg", resource.GetAddressOf(), m_environSelectAlienTexture.ReleaseAndGetAddressOf()));
    ComPtr<ID3D11Texture2D> pEnvironmentSelectAlien;
    DX::ThrowIfFailed(resource.As(&pEnvironmentSelectAlien));
    CD3D11_TEXTURE2D_DESC EnvironmentSelectAlienDesc;
    pEnvironmentSelectAlien->GetDesc(&EnvironmentSelectAlienDesc);
    m_environSelectAlienOrigin.x = float(EnvironmentSelectAlienDesc.Width / 2);
    m_environSelectAlienOrigin.y = float(EnvironmentSelectAlienDesc.Height / 2);

    // Intro screen textures
    DX::ThrowIfFailed(CreateWICTextureFromFile(m_d3dDevice.Get(), L"../HeliGame1989/Art/Textures/logoBMW.png", nullptr, m_bmwLogoTexture.ReleaseAndGetAddressOf()));
    DX::ThrowIfFailed(CreateWICTextureFromFile(m_d3dDevice.Get(), L"../HeliGame1989/Art/Textures/logoBMW.png", resource.GetAddressOf(), m_bmwLogoTexture.ReleaseAndGetAddressOf()));
    ComPtr<ID3D11Texture2D> logoBMW;
    DX::ThrowIfFailed(resource.As(&logoBMW));
    CD3D11_TEXTURE2D_DESC logoBMWDesc;
    logoBMW->GetDesc(&logoBMWDesc);
    m_bmwLogoOrigin.x = float(logoBMWDesc.Width / 2);
    m_bmwLogoOrigin.y = float(logoBMWDesc.Height / 2);

    DX::ThrowIfFailed(CreateWICTextureFromFile(m_d3dDevice.Get(), L"../HeliGame1989/Art/Textures/logoJI.png", nullptr, m_jiLogoTexture.ReleaseAndGetAddressOf()));
    DX::ThrowIfFailed(CreateWICTextureFromFile(m_d3dDevice.Get(), L"../HeliGame1989/Art/Textures/logoJI.png", resource.GetAddressOf(), m_jiLogoTexture.ReleaseAndGetAddressOf()));
    ComPtr<ID3D11Texture2D> logoJI;
    DX::ThrowIfFailed(resource.As(&logoJI));
    CD3D11_TEXTURE2D_DESC logoJIDesc;
    logoJI->GetDesc(&logoJIDesc);
    m_jiLogoOrigin.x = float(logoJIDesc.Width / 2);
    m_jiLogoOrigin.y = float(logoJIDesc.Height / 2);

}

// Allocate all memory resources that change on a window SizeChanged event.
void Game::CreateResources()
{
    // Clear the previous window size specific context.
    ID3D11RenderTargetView* nullViews[] = { nullptr };
    m_d3dContext->OMSetRenderTargets(_countof(nullViews), nullViews, nullptr);
    m_renderTargetView.Reset();
    m_depthStencilView.Reset();
    m_d3dContext->Flush();

    const UINT backBufferWidth = static_cast<UINT>(m_outputWidth);
    const UINT backBufferHeight = static_cast<UINT>(m_outputHeight);
    const DXGI_FORMAT backBufferFormat = DXGI_FORMAT_B8G8R8A8_UNORM;
    const DXGI_FORMAT depthBufferFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
    constexpr UINT backBufferCount = 2;

    // If the swap chain already exists, resize it, otherwise create one.
    if (m_swapChain)
    {
        HRESULT hr = m_swapChain->ResizeBuffers(backBufferCount, backBufferWidth, backBufferHeight, backBufferFormat, 0);

        if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
        {
            // If the device was removed for any reason, a new device and swap chain will need to be created.
            OnDeviceLost();

            // Everything is set up now. Do not continue execution of this method. OnDeviceLost will reenter this method 
            // and correctly set up the new device.
            return;
        }
        else
        {
            DX::ThrowIfFailed(hr);
        }
    }
    else
    {
        // First, retrieve the underlying DXGI Device from the D3D Device.
        ComPtr<IDXGIDevice1> dxgiDevice;
        DX::ThrowIfFailed(m_d3dDevice.As(&dxgiDevice));

        // Identify the physical adapter (GPU or card) this device is running on.
        ComPtr<IDXGIAdapter> dxgiAdapter;
        DX::ThrowIfFailed(dxgiDevice->GetAdapter(dxgiAdapter.GetAddressOf()));

        // And obtain the factory object that created it.
        ComPtr<IDXGIFactory2> dxgiFactory;
        DX::ThrowIfFailed(dxgiAdapter->GetParent(IID_PPV_ARGS(dxgiFactory.GetAddressOf())));

        // Create a descriptor for the swap chain.
        DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
        swapChainDesc.Width = backBufferWidth;
        swapChainDesc.Height = backBufferHeight;
        swapChainDesc.Format = backBufferFormat;
        swapChainDesc.SampleDesc.Count = 1;
        swapChainDesc.SampleDesc.Quality = 0;
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.BufferCount = backBufferCount;

        DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsSwapChainDesc = {};
        fsSwapChainDesc.Windowed = TRUE;

        // Create a SwapChain from a Win32 window.
        DX::ThrowIfFailed(dxgiFactory->CreateSwapChainForHwnd(
            m_d3dDevice.Get(),
            m_window,
            &swapChainDesc,
            &fsSwapChainDesc,
            nullptr,
            m_swapChain.ReleaseAndGetAddressOf()
        ));

        // This template does not support exclusive fullscreen mode and prevents DXGI from responding to the ALT+ENTER shortcut.
        DX::ThrowIfFailed(dxgiFactory->MakeWindowAssociation(m_window, DXGI_MWA_NO_ALT_ENTER));
    }

    // Obtain the backbuffer for this window which will be the final 3D rendertarget.
    ComPtr<ID3D11Texture2D> backBuffer;
    DX::ThrowIfFailed(m_swapChain->GetBuffer(0, IID_PPV_ARGS(backBuffer.GetAddressOf())));

    // Create a view interface on the rendertarget to use on bind.
    DX::ThrowIfFailed(m_d3dDevice->CreateRenderTargetView(backBuffer.Get(), nullptr, m_renderTargetView.ReleaseAndGetAddressOf()));

    // Allocate a 2-D surface as the depth/stencil buffer and
    // create a DepthStencil view on this surface to use on bind.
    CD3D11_TEXTURE2D_DESC depthStencilDesc(depthBufferFormat, backBufferWidth, backBufferHeight, 1, 1, D3D11_BIND_DEPTH_STENCIL);
    //CD3D11_TEXTURE2D_DESC depthStencilDesc(depthBufferFormat, backBufferWidth, backBufferHeight, 1, 1, D3D11_BIND_DEPTH_STENCIL, D3D11_USAGE_DEFAULT, 0, 8, 0);  // For multisampling rendering

    ComPtr<ID3D11Texture2D> depthStencil;
    DX::ThrowIfFailed(m_d3dDevice->CreateTexture2D(&depthStencilDesc, nullptr, depthStencil.GetAddressOf()));

    CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2D);
    //CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2DMS);  //for multisampling RenderTesting

    DX::ThrowIfFailed(m_d3dDevice->CreateDepthStencilView(depthStencil.Get(), &depthStencilViewDesc, m_depthStencilView.ReleaseAndGetAddressOf()));

    // TODO: Initialize windows-size dependent objects here.

    m_view = DirectX::SimpleMath::Matrix::CreateLookAt(DirectX::SimpleMath::Vector3(2.f, 2.f, 2.f), DirectX::SimpleMath::Vector3::Zero, DirectX::SimpleMath::Vector3::UnitY);
    m_proj = DirectX::SimpleMath::Matrix::CreatePerspectiveFieldOfView(XM_PI / 4.f, float(backBufferWidth) / float(backBufferHeight), m_camera->GetViewPlaneNear(), m_camera->GetViewPlaneFar());

    m_effect->SetView(m_view);
    m_effect->SetProjection(m_proj);
    m_effect2->SetView(m_view);
    m_effect2->SetProjection(m_proj);
    m_effect3->SetView(m_view);
    m_effect3->SetProjection(m_proj);
    // world end

    // UI font positions
    m_fontPos.x = backBufferWidth / 2.f;
    m_fontPos.y = backBufferHeight / 2.f;
    m_fontPos2.x = backBufferWidth - 5.f;
    m_fontPos2.y = 35;
    m_fontPosDebug.x = 480;
    m_fontPosDebug.y = 35;
    m_fontMenuPos.x = backBufferWidth / 2.f;
    m_fontMenuPos.y = 35;
    m_bitwiseFontPos.x = backBufferWidth / 2.f;
    m_bitwiseFontPos.y = backBufferHeight / 2.f;

    // Character textures  
    m_pacPos.x = backBufferWidth / 2.f;
    m_pacPos.y = backBufferHeight / 2.f - 55.f;

    m_characterPos.x = backBufferWidth / 2.f;
    m_characterPos.y = backBufferHeight / 2.f;
    m_character0Pos.x = backBufferWidth / 2.f;
    m_character0Pos.y = backBufferHeight / 2.f;
    m_character1Pos.x = backBufferWidth / 2.f;
    m_character1Pos.y = backBufferHeight / 2.f;
    m_character2Pos.x = backBufferWidth / 2.f;
    m_character2Pos.y = backBufferHeight / 2.f;
    DX::ThrowIfFailed(CreateWICTextureFromFile(m_d3dDevice.Get(), L"../HeliGame1989/Art/Textures/CharacterBackground.png", nullptr, m_characterBackgroundTexture.ReleaseAndGetAddressOf()));
    m_characterBackgroundPos.x = backBufferWidth / 2.f;
    m_characterBackgroundPos.y = backBufferHeight / 2.f;

    // Environment select textures
    m_environSelectCalmPos.x = backBufferWidth / 2.f;
    m_environSelectCalmPos.y = backBufferHeight / 2.f;

    m_environSelectBreezyPos.x = backBufferWidth / 2.f;
    m_environSelectBreezyPos.y = backBufferHeight / 2.f;

    m_environSelectAlienPos.x = backBufferWidth / 2.f;
    m_environSelectAlienPos.y = backBufferHeight / 2.f;

    // Intro Sceen textures
    m_bmwLogoPos.x = backBufferWidth / 2.f;
    m_bmwLogoPos.y = backBufferHeight / 2.f;

    m_jiLogoPos.x = backBufferWidth / 2.f;
    m_jiLogoPos.y = backBufferHeight / 2.f;
    // End Texture
}

void Game::DrawCameraFocus()
{
    const float line = .25f;
    DirectX::XMVECTORF32 lineColor = DirectX::Colors::Yellow;
    DirectX::SimpleMath::Vector3 focalPoint = m_camera->GetTargetPos();
    m_cameraFocusPos = focalPoint;
    if (m_moveLightWithCameraFocus3 == true)
    {
        lineColor = DirectX::Colors::White;
        DirectX::SimpleMath::Vector3 posNormalized = m_cameraFocusPos;
        posNormalized.Normalize();
        m_lightPos2 = posNormalized;
    }
    if (m_moveLightWithCameraFocus2 == true)
    {
        lineColor = DirectX::Colors::White;
        DirectX::SimpleMath::Vector3 posNormalized = m_cameraFocusPos;
        posNormalized.Normalize();
        m_lightPos1 = posNormalized;
    }
    if (m_moveLightWithCameraFocus1 == true)
    {
        lineColor = DirectX::Colors::White;
        DirectX::SimpleMath::Vector3 posNormalized = m_cameraFocusPos;
        posNormalized.Normalize();
        m_lightPos0 = posNormalized;
    }

    DirectX::SimpleMath::Vector3 yLine = focalPoint;
    yLine.y += line;
    DirectX::SimpleMath::Vector3 xLine = focalPoint;
    xLine.x += line;
    DirectX::SimpleMath::Vector3 zLine = focalPoint;
    zLine.z += line;

    DirectX::SimpleMath::Vector3 negZLine = focalPoint;
    negZLine.z -= line;
    DirectX::SimpleMath::Vector3 negXLine = focalPoint;
    negXLine.x -= line;

    DirectX::SimpleMath::Vector3 lineNormal = DirectX::SimpleMath::Vector3::UnitX;
    VertexPositionNormalColor origin(focalPoint, lineNormal, lineColor);
    VertexPositionNormalColor yOffset(yLine, lineNormal, lineColor);
    VertexPositionNormalColor xOffset(xLine, lineNormal, lineColor);
    VertexPositionNormalColor zOffset(zLine, lineNormal, lineColor);
    m_batch2->DrawLine(origin, yOffset);
    m_batch2->DrawLine(origin, xOffset);
    m_batch2->DrawLine(origin, zOffset);
}

void Game::DrawDebugDataUI()
{
    std::vector<std::string> uiVector = m_debugData->DebugGetUIVector();
    DirectX::SimpleMath::Vector2 textLinePos = m_fontPos2;
    textLinePos.x = 200;
    //textLinePos.y += 30;
    for (int i = 0; i < uiVector.size(); ++i)
    {
        std::string textLine = uiVector[i];
        DirectX::SimpleMath::Vector2 textLineOrigin = m_bitwiseFont->MeasureString(textLine.c_str()) / 2.f;
        textLinePos.x = textLineOrigin.x + 20;
        m_bitwiseFont->DrawString(m_spriteBatch.get(), textLine.c_str(), textLinePos, Colors::White, 0.f, textLineOrigin);
        textLinePos.y += 30;
    }


    float rpm = m_vehicle->GetRPM() * 1.0f;
    std::string rpmLine = "Rotor RPM: " + std::to_string(static_cast<int>(rpm)) + " ";
    DirectX::SimpleMath::Vector2 rpmLineOrigin = m_bitwiseFont->MeasureString(rpmLine.c_str()) / 2.f;
    textLinePos.x = rpmLineOrigin.x + 20;
    m_bitwiseFont->DrawString(m_spriteBatch.get(), rpmLine.c_str(), textLinePos, Colors::White, 0.f, rpmLineOrigin);
    textLinePos.y += 30;

    float collective = m_vehicle->GetCollective() * 100.0f;
    std::string collectiveLine = "Collective: " + std::to_string(static_cast<int>(collective)) + "% ";
    DirectX::SimpleMath::Vector2 collectiveLineOrigin = m_bitwiseFont->MeasureString(collectiveLine.c_str()) / 2.f;
    textLinePos.x = collectiveLineOrigin.x + 20;
    m_bitwiseFont->DrawString(m_spriteBatch.get(), collectiveLine.c_str(), textLinePos, Colors::White, 0.f, collectiveLineOrigin);
    textLinePos.y += 30;

    float throttle = m_vehicle->GetThrottle() * 100.0f;
    std::string throttleLine = "Throttle: " + std::to_string(static_cast<int>(throttle)) + "% ";
    DirectX::SimpleMath::Vector2 throttleLineOrigin = m_bitwiseFont->MeasureString(throttleLine.c_str()) / 2.f;
    textLinePos.x = throttleLineOrigin.x + 20;
    m_bitwiseFont->DrawString(m_spriteBatch.get(), throttleLine.c_str(), textLinePos, Colors::White, 0.f, throttleLineOrigin);
    textLinePos.y += 30;

    // Draw speed with formatting
    float speed = m_vehicle->GetGroundSpeed() * 2.23694f;
    std::string speedLine = "Ground Speed: " + std::to_string(static_cast<int>(speed)) + " MPH";
    DirectX::SimpleMath::Vector2 speedLineOrigin = m_bitwiseFont->MeasureString(speedLine.c_str()) / 2.f;
    textLinePos.x = speedLineOrigin.x + 20;
    m_bitwiseFont->DrawString(m_spriteBatch.get(), speedLine.c_str(), textLinePos, Colors::White, 0.f, speedLineOrigin);
    textLinePos.y += 30;

    speed = m_vehicle->GetAirSpeed() * 2.23694f;
    speedLine = "Air Speed: " + std::to_string(static_cast<int>(speed)) + " MPH";
    speedLineOrigin = m_bitwiseFont->MeasureString(speedLine.c_str()) / 2.f;
    textLinePos.x = speedLineOrigin.x + 20;
    m_bitwiseFont->DrawString(m_spriteBatch.get(), speedLine.c_str(), textLinePos, Colors::White, 0.f, speedLineOrigin);
    textLinePos.y += 30;

    float altitude = m_vehicle->GetAltitude();
    std::string altitudeLine = "Altitude: " + std::to_string(static_cast<int>(altitude)) + "m ";
    DirectX::SimpleMath::Vector2 altitudeLineOrigin = m_bitwiseFont->MeasureString(altitudeLine.c_str()) / 2.f;
    textLinePos.x = altitudeLineOrigin.x + 20;
    m_bitwiseFont->DrawString(m_spriteBatch.get(), altitudeLine.c_str(), textLinePos, Colors::White, 0.f, altitudeLineOrigin);
    textLinePos.y += 30;

    /*
    // Draw FPS  
    std::string textLine = "FPS  " + std::to_string(m_timer.GetFramesPerSecond());
    DirectX::SimpleMath::Vector2 textLineOrigin = m_bitwiseFont->MeasureString(textLine.c_str()) / 2.f;
    textLinePos.x = textLineOrigin.x + 20;
    m_bitwiseFont->DrawString(m_spriteBatch.get(), textLine.c_str(), textLinePos, Colors::White, 0.f, textLineOrigin);
    textLinePos.y += 30;

    // Draw Timer  
    textLine = "Timer = " + std::to_string(m_timer.GetTotalSeconds());
    textLineOrigin = m_bitwiseFont->MeasureString(textLine.c_str()) / 2.f;
    textLinePos.x = textLineOrigin.x + 20;
    m_bitwiseFont->DrawString(m_spriteBatch.get(), textLine.c_str(), textLinePos, Colors::White, 0.f, textLineOrigin);
    textLinePos.y += 30;
    */
}

void Game::DrawDebugLines(const  DirectX::SimpleMath::Vector3 aPos, const DirectX::XMVECTORF32 aColor)
{
    const float line = .25f;
    DirectX::XMVECTORF32 lineColor = aColor;
    DirectX::SimpleMath::Vector3 focalPoint = aPos;

    DirectX::SimpleMath::Vector3 yLine = focalPoint;
    yLine.y += line;
    DirectX::SimpleMath::Vector3 xLine = focalPoint;
    xLine.x += line;
    DirectX::SimpleMath::Vector3 zLine = focalPoint;
    zLine.z += line;

    DirectX::SimpleMath::Vector3 negZLine = focalPoint;
    negZLine.z -= line;
    DirectX::SimpleMath::Vector3 negXLine = focalPoint;
    negXLine.x -= line;

    VertexPositionColor origin(focalPoint, lineColor);
    VertexPositionColor yOffset(yLine, lineColor);
    VertexPositionColor xOffset(xLine, lineColor);
    VertexPositionColor zOffset(zLine, lineColor);
    m_batch3->DrawLine(origin, yOffset);
    m_batch3->DrawLine(origin, xOffset);
    m_batch3->DrawLine(origin, zOffset);
}

void Game::DrawDebugNormalLines(const  DirectX::SimpleMath::Vector3 aPos, const DirectX::XMVECTORF32 aColor)
{
    const float line = .25f;
    DirectX::XMVECTORF32 lineColor = aColor;
    DirectX::XMVECTORF32 normColor = DirectX::Colors::White;
    DirectX::SimpleMath::Vector3 focalPoint = aPos;
    focalPoint.y = m_environment->GetTerrainHeightAtPos(focalPoint);
    DirectX::SimpleMath::Vector3 normalVec = m_environment->GetTerrainNormal(aPos);
    normalVec *= 5;
    DirectX::SimpleMath::Vector3 yLine = focalPoint;
    yLine.y += line;
    DirectX::SimpleMath::Vector3 xLine = focalPoint;
    xLine.x += line;
    DirectX::SimpleMath::Vector3 zLine = focalPoint;
    zLine.z += line;
    DirectX::SimpleMath::Vector3 normLine = focalPoint;
    normLine += normalVec;

    DirectX::SimpleMath::Vector3 negZLine = focalPoint;
    negZLine.z -= line;
    DirectX::SimpleMath::Vector3 negXLine = focalPoint;
    negXLine.x -= line;

    VertexPositionColor origin(focalPoint, lineColor);
    VertexPositionColor yOffset(yLine, lineColor);
    VertexPositionColor xOffset(xLine, lineColor);
    VertexPositionColor zOffset(zLine, lineColor);
    VertexPositionColor normVertex(normLine, normColor);
    m_batch3->DrawLine(origin, yOffset);
    m_batch3->DrawLine(origin, xOffset);
    m_batch3->DrawLine(origin, zOffset);
    m_batch3->DrawLine(origin, normVertex);
}

void Game::DrawDebugLinesVector()
{
    std::vector<std::tuple<DirectX::SimpleMath::Vector3, DirectX::SimpleMath::Vector3, DirectX::SimpleMath::Vector4>> lineTup = m_vehicle->DebugGetTestLines();
    for (unsigned int i = 0; i < lineTup.size(); ++i)
    {
        DirectX::SimpleMath::Vector4 lineColor = std::get<2>(lineTup[i]);
        VertexPositionColor lineStart(std::get<0>(lineTup[i]), lineColor);
        VertexPositionColor lineEnd(std::get<1>(lineTup[i]), lineColor);
        m_batch3->DrawLine(lineStart, lineEnd);
    }
}

void Game::DrawDebugLinesVectorNew()
{
    std::vector<std::tuple<DirectX::SimpleMath::Vector3, DirectX::SimpleMath::Vector3, DirectX::SimpleMath::Vector4>> lineTup = m_debugData->DebugGetTestLines();
    for (unsigned int i = 0; i < lineTup.size(); ++i)
    {
        DirectX::SimpleMath::Vector4 lineColor = std::get<2>(lineTup[i]);
        VertexPositionColor lineStart(std::get<0>(lineTup[i]), lineColor);
        VertexPositionColor lineEnd(std::get<1>(lineTup[i]), lineColor);
        m_batch3->DrawLine(lineStart, lineEnd);
    }

    std::vector<std::tuple<DirectX::SimpleMath::Vector3, DirectX::SimpleMath::Vector3, DirectX::XMVECTORF32>> lineTupVec = m_debugData->GetDebugLinesVec();
    for (unsigned int i = 0; i < lineTupVec.size(); ++i)
    {
        DirectX::XMVECTORF32 lineColor = std::get<2>(lineTupVec[i]);
        VertexPositionColor lineStart(std::get<0>(lineTupVec[i]), lineColor);
        VertexPositionColor lineEnd(std::get<1>(lineTupVec[i]), lineColor);
        m_batch3->DrawLine(lineStart, lineEnd);
    }
}

void Game::DrawDebugVehicleData()
{
    std::vector<std::string> uiVector = m_vehicle->DebugGetUIVector();
    int vecSize = uiVector.size();
    DirectX::SimpleMath::Vector2 textLinePos = m_fontPos2;
    textLinePos.x = 200;
    for (int i = 0; i < vecSize; ++i)
    {
        std::string textLine = uiVector[i];
        DirectX::SimpleMath::Vector2 textLineOrigin = m_bitwiseFont->MeasureString(textLine.c_str()) / 2.f;
        textLinePos.x = textLineOrigin.x + 20;
        m_bitwiseFont->DrawString(m_spriteBatch.get(), textLine.c_str(), textLinePos, Colors::White, 0.f, textLineOrigin);
        textLinePos.y += 30;
    }

    // Draw speed with formatting
    float speed = m_vehicle->GetGroundSpeed() * 2.23694f;
    std::string speedLine = "Ground Speed: " + std::to_string(static_cast<int>(speed)) + " MPH";
    DirectX::SimpleMath::Vector2 speedLineOrigin = m_bitwiseFont->MeasureString(speedLine.c_str()) / 2.f;
    textLinePos.x = speedLineOrigin.x + 20;
    m_bitwiseFont->DrawString(m_spriteBatch.get(), speedLine.c_str(), textLinePos, Colors::White, 0.f, speedLineOrigin);
    textLinePos.y += 30;

    speed = m_vehicle->GetAirSpeed() * 2.23694f;
    speedLine = "Air Speed: " + std::to_string(static_cast<int>(speed)) + " MPH";
    speedLineOrigin = m_bitwiseFont->MeasureString(speedLine.c_str()) / 2.f;
    textLinePos.x = speedLineOrigin.x + 20;
    m_bitwiseFont->DrawString(m_spriteBatch.get(), speedLine.c_str(), textLinePos, Colors::White, 0.f, speedLineOrigin);
    textLinePos.y += 30;

    
    float rpm = m_vehicle->GetRPM() * 1.0f;
    std::string rpmLine = "Rotor RPM: " + std::to_string(static_cast<int>(rpm)) + " ";
    DirectX::SimpleMath::Vector2 rpmLineOrigin = m_bitwiseFont->MeasureString(rpmLine.c_str()) / 2.f;
    textLinePos.x = rpmLineOrigin.x + 20;
    m_bitwiseFont->DrawString(m_spriteBatch.get(), rpmLine.c_str(), textLinePos, Colors::White, 0.f, rpmLineOrigin);
    textLinePos.y += 30;
    
    float collective = m_vehicle->GetCollective() * 100.0f;
    std::string collectiveLine = "Collective: " + std::to_string(static_cast<int>(collective)) + "% ";
    DirectX::SimpleMath::Vector2 collectiveLineOrigin = m_bitwiseFont->MeasureString(collectiveLine.c_str()) / 2.f;
    textLinePos.x = collectiveLineOrigin.x + 20;
    m_bitwiseFont->DrawString(m_spriteBatch.get(), collectiveLine.c_str(), textLinePos, Colors::White, 0.f, collectiveLineOrigin);
    textLinePos.y += 30;

    float throttle = m_vehicle->GetThrottle() * 100.0f;
    std::string throttleLine = "Throttle: " + std::to_string(static_cast<int>(throttle)) + "% ";
    DirectX::SimpleMath::Vector2 throttleLineOrigin = m_bitwiseFont->MeasureString(throttleLine.c_str()) / 2.f;
    textLinePos.x = throttleLineOrigin.x + 20;
    m_bitwiseFont->DrawString(m_spriteBatch.get(), throttleLine.c_str(), textLinePos, Colors::White, 0.f, throttleLineOrigin);
    textLinePos.y += 30;

    float altitude = m_vehicle->GetAltitude();
    std::string altitudeLine = "Altitude: " + std::to_string(static_cast<int>(altitude)) + "m ";
    DirectX::SimpleMath::Vector2 altitudeLineOrigin = m_bitwiseFont->MeasureString(altitudeLine.c_str()) / 2.f;
    textLinePos.x = altitudeLineOrigin.x + 20;
    m_bitwiseFont->DrawString(m_spriteBatch.get(), altitudeLine.c_str(), textLinePos, Colors::White, 0.f, altitudeLineOrigin);
    textLinePos.y += 30;

    /*
    DirectX::SimpleMath::Vector3 camPos = m_camera->GetPos();
   
    speedLine = "CameraPos x = " + std::to_string(camPos.x) + " ";
    speedLineOrigin = m_bitwiseFont->MeasureString(speedLine.c_str()) / 2.f;
    textLinePos.x = speedLineOrigin.x + 20;
    m_bitwiseFont->DrawString(m_spriteBatch.get(), speedLine.c_str(), textLinePos, Colors::White, 0.f, speedLineOrigin);
    textLinePos.y += 30;

    speedLine = "CameraPos y = " + std::to_string(camPos.y) + " ";
    speedLineOrigin = m_bitwiseFont->MeasureString(speedLine.c_str()) / 2.f;
    textLinePos.x = speedLineOrigin.x + 20;
    m_bitwiseFont->DrawString(m_spriteBatch.get(), speedLine.c_str(), textLinePos, Colors::White, 0.f, speedLineOrigin);
    textLinePos.y += 30;

    speedLine = "CameraPos z = " + std::to_string(camPos.z) + " ";
    speedLineOrigin = m_bitwiseFont->MeasureString(speedLine.c_str()) / 2.f;
    textLinePos.x = speedLineOrigin.x + 20;
    m_bitwiseFont->DrawString(m_spriteBatch.get(), speedLine.c_str(), textLinePos, Colors::White, 0.f, speedLineOrigin);
    textLinePos.y += 30;
    */

    /*
    // Draw Timer  
    std::string textLine = "Timer  " + std::to_string(m_timer.GetTotalSeconds());
    DirectX::SimpleMath::Vector2 textLineOrigin = m_bitwiseFont->MeasureString(textLine.c_str()) / 2.f;
    textLinePos.x = textLineOrigin.x + 20;
    m_bitwiseFont->DrawString(m_spriteBatch.get(), textLine.c_str(), textLinePos, Colors::White, 0.f, textLineOrigin);
    textLinePos.y += 30;
    */

    // Draw FPS  
    std::string textLine = "FPS  " + std::to_string(m_timer.GetFramesPerSecond());
    DirectX::SimpleMath::Vector2 textLineOrigin = m_bitwiseFont->MeasureString(textLine.c_str()) / 2.f;
    textLinePos.x = textLineOrigin.x + 20;
    m_bitwiseFont->DrawString(m_spriteBatch.get(), textLine.c_str(), textLinePos, Colors::White, 0.f, textLineOrigin);
    textLinePos.y += 30;

}

void Game::DrawDebugValue()
{
    std::vector<std::pair<std::string, float>> uiVector = m_vehicle->DebugGetUI();
    int vecSize = uiVector.size();
    DirectX::SimpleMath::Vector2 textLinePos = m_fontPos2;
    textLinePos.x = 200;
    for (int i = 0; i < vecSize; ++i)
    {
        std::string textLine = uiVector[i].first + " = " + std::to_string(uiVector[i].second);
        DirectX::SimpleMath::Vector2 textLineOrigin = m_bitwiseFont->MeasureString(textLine.c_str()) / 2.f;
        textLinePos.x = textLineOrigin.x + 20;
        m_bitwiseFont->DrawString(m_spriteBatch.get(), textLine.c_str(), textLinePos, Colors::White, 0.f, textLineOrigin);
        textLinePos.y += 30;
    }
}

void Game::DrawGridForStartScreen()
{
    float timeStamp = static_cast<float>(m_testTimer);

    DirectX::XMVECTORF32 gridColor = DirectX::Colors::LawnGreen;
    DirectX::XMVECTORF32 baseColor = DirectX::Colors::Black;
    DirectX::SimpleMath::Vector4 gridColor2(0.486274540f, 0.988235354f, 0.000000000f, 1.000000000f);// = DirectX::Colors::LawnGreen;
    gridColor2 = m_startScreenGridDimmerColor;
    
    const float xBase = 1.1;
    const float yBase = 0.0;
    const float zBase = -4.0;
    const float xLength = 4.0;
    const float zSpacing = 0.2;
    const int verticleLineCount = 70;

    const float zLength = zSpacing * verticleLineCount;
    const float xSpacing = 0.2;
    const int horizontalLineCount = 25;
    const float baseOffset = -0.01;
    DirectX::SimpleMath::Vector3 verticleStart(xBase, yBase, zBase);
    for (int i = 0; i <= verticleLineCount; ++i)
    {
        DirectX::SimpleMath::Vector3 verticleEnd = verticleStart;
        verticleEnd.x -= xLength;
        DirectX::VertexPositionColor vertexStart(verticleStart, gridColor2);
        DirectX::VertexPositionColor vertexEnd(verticleEnd, gridColor2);

        m_batch3->DrawLine(vertexStart, vertexEnd);
        verticleStart.z += zSpacing;
    }

    DirectX::SimpleMath::Vector3 horizontalStart(xBase, yBase, zBase);
    float horizontalLinePos = fmod(timeStamp, xLength) * .2f;
    horizontalLinePos += 3.0;
    for (int i = 0; i < horizontalLineCount; ++i)
    {
        horizontalStart.x = -horizontalLinePos + (xSpacing * i);

        DirectX::SimpleMath::Vector3 horizontalEnd = horizontalStart;
        horizontalEnd.z += zLength;
        DirectX::VertexPositionColor vertexStart(horizontalStart, gridColor2);
        DirectX::VertexPositionColor vertexEnd(horizontalEnd, gridColor2);

        if (horizontalStart.x < xBase && horizontalStart.x > xBase - xLength)
        {
            m_batch3->DrawLine(vertexStart, vertexEnd);
        }
    }

    DirectX::SimpleMath::Vector3 nw(xBase, yBase + baseOffset, zBase);
    DirectX::SimpleMath::Vector3 ne = nw;
    ne.z += zLength;

    DirectX::SimpleMath::Vector3 sw = nw;
    sw.x -= xLength;
    DirectX::SimpleMath::Vector3 se = nw;
    se.z += zLength;
    se.x -= xLength;

    nw.y -= baseOffset;
    ne.y -= baseOffset;
    DirectX::VertexPositionColor v1(nw, baseColor);
    DirectX::VertexPositionColor v2(ne, baseColor);
    DirectX::VertexPositionColor v3(se, baseColor);
    DirectX::VertexPositionColor v4(sw, baseColor);
    m_batch3->DrawQuad(v1, v2, v3, v4);

    nw.y += baseOffset;
    ne.y += baseOffset;

    // draw scrolling grid outline
    nw.y -= baseOffset;
    ne.y -= baseOffset;
    se.y -= baseOffset;
    sw.y -= baseOffset;

    v1 = DirectX::VertexPositionColor(nw, gridColor2);
    v2 = DirectX::VertexPositionColor(ne, gridColor2);
    v3 = DirectX::VertexPositionColor(se, gridColor2);
    v4 = DirectX::VertexPositionColor(sw, gridColor2);
    /*
    m_batch3->DrawLine(v1, v2);
    m_batch3->DrawLine(v2, v3);
    m_batch3->DrawLine(v3, v4);
    m_batch3->DrawLine(v4, v1);
    */
    m_batch3->Draw(D3D_PRIMITIVE_TOPOLOGY_LINELIST, m_terrainVertexArray, m_terrainVertexCount);
    //m_batch3->Draw(D3D_PRIMITIVE_TOPOLOGY_LINELIST, m_terrainStartScreen.terrainVertexArray, m_terrainStartScreen.terrainVertexCount);
}

void Game::DrawGamePlayStart()
{
    TerrainDimmer();
    /*
    const float fogGap1 = 0.0f;
    const float fogGap2 = 10.0f;
    const float fadeDuration = 14.0;
    const float fadeInStart = 0.0;
    const float fadeInEnd = fadeInStart + fadeDuration + 0.0f;
    //float fadeInEnd = fadeInStart + fadeDuration + 0.0f;
    const float fullViewDuration = 5.0f;
    */
    const float fogGap1 = 0.0f;
    const float fogGap2 = 10.0f;
    const float fadeDuration = 3.5;
    const float fadeInStart = 0.0;
    const float fadeInEnd = fadeInStart + fadeDuration + 0.0f;
    //float fadeInEnd = fadeInStart + fadeDuration + 0.0f;
    const float fullViewDuration = 4.0f;

    
    const float fadeOutStart = fadeInEnd + fullViewDuration;

    const float fadeOutEnd = fadeOutStart + fadeDuration;
    //const float timeStamp = static_cast<float>(m_testTimer + m_debugStartTime);
    const float timeStamp = static_cast<float>(m_testTimer) - m_gamePlayStartOffSetTimer;

    m_loadScreenTimerStart += static_cast<float>(m_timer.GetElapsedSeconds());

    /////////////////////////////////////
    /// Render GamePlay Start Screen  ///
    /////////////////////////////////////
    if (timeStamp < fadeInStart)
    {
        // render nothing
        DirectX::SimpleMath::Vector3 preZoomPos = m_startScreenCamZoomPos;
        preZoomPos.y = 0.0;

        m_camera->SetPos(m_gamePlayStartCamPos1);
        m_camera->SetTargetPos(m_gamePlayStartCamTarg1);

        m_lighting->SetLighting(Lighting::LightingState::LIGHTINGSTATE_TEST01);

        m_currentGameState = GameState::GAMESTATE_GAMEPLAYSTART;

        m_effect->SetFogStart(-1.0);
        m_effect->SetFogEnd(5.0);
        m_effect->SetFogEnabled(true);

        m_effect2->SetFogStart(-1.0);
        m_effect2->SetFogEnd(5.0);
        m_effect2->SetFogEnabled(true);

        m_effect3->SetFogStart(-1.0);
        m_effect3->SetFogEnd(5.0);
        m_effect3->SetFogEnabled(true);

    }
    else if (timeStamp < fadeOutEnd)  // Render GamePlay Start Screen
    {
        if (timeStamp < fadeInEnd)  // fade in
        {
            float distance = DirectX::SimpleMath::Vector3::Distance(m_camera->GetPos(), m_gamePlayStartCamPos2);
            float speed = distance / (fadeDuration);
            
            m_camera->SetTransitionSpeed(speed);
            m_camera->SetCameraStartPos(m_camera->GetPos());
            m_camera->SetCameraEndPos(m_gamePlayStartCamPos2);
            m_camera->SetDestinationPos(m_gamePlayStartCamPos2);
            m_camera->SetTargetStartPos(m_camera->GetTargetPos());
            m_camera->SetTargetEndPos(m_gamePlayStartCamTarg2);
            m_camera->SetCameraState(CameraState::CAMERASTATE_TRANSITION);
 
            float colorIntensity = (timeStamp - fadeInStart) / (fadeDuration);
            float fogStart = colorIntensity + fogGap1;
            float fogEnd = colorIntensity + fogGap2;

            m_debugValue1 = colorIntensity;
            m_debugValue2 = fogStart;
            m_debugValue3 = fogEnd;
        }
        else if (timeStamp < fadeOutStart)
        {
            if (m_camera->GetCameraState() != CameraState::CAMERASTATE_GAMEPLAYSTARTSPIN)
            {
                m_camera->SetCameraState(CameraState::CAMERASTATE_GAMEPLAYSTARTSPIN);
                m_camera->SetSpinCameraStartGamePlayStart(fadeOutStart - fadeInEnd);
            }
            DirectX::SimpleMath::Vector3 testCameraPos = m_camera->GetPos();
        }
        else if (timeStamp > fadeOutStart && timeStamp < fadeOutEnd) // fade out
        {
            //m_camera->SetCameraState(CameraState::CAMERASTATE_SPINCAMERA);
            DirectX::SimpleMath::Vector3 testCameraPos = m_camera->GetPos();

            float colorIntensity = (fadeOutEnd - timeStamp) / (fadeDuration);
            float fogStart = colorIntensity + fogGap1;
            float fogEnd = colorIntensity + fogGap2;

            m_debugValue1 = colorIntensity;
            m_debugValue2 = fogStart;
            m_debugValue3 = fogEnd;
            m_currentGameState = GameState::GAMESTATE_GAMEPLAY;
        }
        else
        {
            m_effect2->SetFogEnabled(false);
            m_currentGameState = GameState::GAMESTATE_GAMEPLAY;
        }
    }
    m_effect->SetFogEnabled(false);
    m_effect2->SetFogEnabled(false);
    m_effect3->SetFogEnabled(false);
}

void Game::DrawIntroScene()
{
    DirectX::SimpleMath::Vector3 testFogTarget1(1.1, 0.0, 0.0);
    DirectX::SimpleMath::Vector3 testFogTarget2(1.1, .75, 0.0);
    DirectX::SimpleMath::Vector3 testFogTarget3(0.5, 0.0, 0.0);
    DirectX::SimpleMath::Vector3 testFogTarget4(3.1, 0.0, 0.0);

    const float fadeDuration = m_fadeDuration;
    const float logoDisplayDuration = m_logoDisplayDuration;
    const float logoDisplayGap = m_logoDisplayGap;
    const float startDelay = m_startDelay;

    const float fogGap1 = m_fogGap1;
    const float fogGap2 = m_fogGap2;

    const float timeStamp = static_cast<float>(m_testTimer + m_debugStartTime);

    const float fadeInStart1 = startDelay;
    const float fadeInStart2 = startDelay + logoDisplayDuration + logoDisplayGap;
    const float fadeInStart3 = startDelay + logoDisplayDuration + logoDisplayGap + logoDisplayDuration + logoDisplayGap;
    //const float fadeInStart4 = startDelay + logoDisplayDuration + logoDisplayGap + logoDisplayDuration + logoDisplayGap + logoDisplayDuration + logoDisplayGap + m_startScreenTimerMod;

    const float fadeInEnd1 = startDelay + fadeDuration;
    const float fadeInEnd2 = startDelay + logoDisplayDuration + logoDisplayGap + fadeDuration;
    const float fadeInEnd3 = startDelay + logoDisplayDuration + logoDisplayGap + fadeDuration + logoDisplayDuration + logoDisplayGap;
    //const float fadeInEnd4 = startDelay + logoDisplayDuration + logoDisplayGap + fadeDuration + logoDisplayDuration + logoDisplayGap + logoDisplayDuration + logoDisplayGap + m_startScreenTimerMod;

    const float fadeOutStart1 = startDelay + logoDisplayDuration - fadeDuration;
    const float fadeOutStart2 = startDelay + logoDisplayDuration + logoDisplayGap + logoDisplayDuration - fadeDuration;
    const float fadeOutStart3 = startDelay + logoDisplayDuration + logoDisplayGap + logoDisplayDuration + logoDisplayGap + logoDisplayDuration - fadeDuration + m_startScreenTimerMod;
    //const float fadeOutStart4 = startDelay + logoDisplayDuration + logoDisplayGap + logoDisplayDuration + logoDisplayGap + logoDisplayDuration + logoDisplayGap + logoDisplayDuration - fadeDuration + m_startScreenTimerMod;

    const float fadeOutEnd1 = startDelay + logoDisplayDuration;
    const float fadeOutEnd2 = startDelay + logoDisplayDuration + logoDisplayGap + logoDisplayDuration;
    //const float fadeOutEnd3 = startDelay + logoDisplayDuration + logoDisplayGap + logoDisplayDuration + logoDisplayGap + logoDisplayDuration + m_startScreenTimerMod;
    //const float fadeOutEnd4 = startDelay + logoDisplayDuration + logoDisplayGap + logoDisplayDuration + logoDisplayGap + logoDisplayDuration + logoDisplayGap + logoDisplayDuration + m_startScreenTimerMod;

    //float fadeOutStart3 = 0.0;
    float fadeOutEnd3 = fadeOutStart3 + 3.0f;

    //float fadeInStart4 = fadeOutEnd3 + 0.3f;
    float fadeInStart4 = fadeOutEnd3 + 0.05f;
    float fadeInEnd4 = fadeInStart4 + 0.1f;
    float fadeOutStart4 = fadeInEnd4 + 0.1f;
    float fadeOutEnd4 = fadeOutStart4 + 0.1f;

    m_gamePlayStartOffSetTimer = fadeOutEnd4;

    if (timeStamp < fadeInStart1)
    {
        // Render nothing
        m_effect->SetFogEnabled(true);
        m_effect->SetFogStart(0.0);
        m_effect->SetFogEnd(1.0);
        m_lighting->SetLighting(Lighting::LightingState::LIGHTINGSTATE_JI);

        m_camera->SetPos(m_introCamPos);
        m_camera->SetTargetPos(m_introCamTarg);
    }
    //////////////////////////////////////
    /// Render Jackson Industries Logo ///
    //////////////////////////////////////
    else if (timeStamp < fadeOutEnd1)  // Render Jackson Industries Logo
    {
        m_lighting->SetLighting(Lighting::LightingState::LIGHTINGSTATE_JI);
        m_effect->SetTexture(m_textureJI.Get());
        m_effect->SetNormalTexture(m_normalMapJI.Get());
        m_effect->SetSpecularTexture(m_specularJI.Get());
        if (timeStamp < fadeInEnd1)  // fade in
        {
            float colorIntensity = (timeStamp - fadeInStart1) / fadeDuration;
            float fogStart = colorIntensity + fogGap1;
            float fogEnd = colorIntensity + fogGap2;

            SetFogVals(testFogTarget1, colorIntensity);

            m_debugValue1 = colorIntensity;
            m_debugValue2 = fogStart;
            m_debugValue3 = fogEnd;
        }
        else if (timeStamp > fadeOutStart1) // fade out
        {
            float colorIntensity = (fadeOutEnd1 - timeStamp) / (fadeDuration);
            float fogStart = colorIntensity + fogGap1;
            float fogEnd = colorIntensity + fogGap2;

            SetFogVals(testFogTarget1, colorIntensity);

            m_debugValue1 = colorIntensity;
            m_debugValue2 = fogStart;
            m_debugValue3 = fogEnd;
        }
        else // display at full intesity
        {
            //m_effect->SetFogEnabled(false);
        }
    }
    ///////////////////////////////
    ///    Render BMW Logo      /// 
    ///////////////////////////////
    else if (timeStamp < fadeInStart2)
    {
        // render nothing
        m_lighting->SetLighting(Lighting::LightingState::LIGHTINGSTATE_BMW);
        m_camera->SetPos(m_introCamPos2);
        m_camera->SetTargetPos(m_introCamTarg2);
    }
    else if (timeStamp < fadeOutEnd2) // Render BMW Logo
    {
        m_lighting->SetLighting(Lighting::LightingState::LIGHTINGSTATE_BMW);
        m_effect->SetTexture(m_textureBMW.Get());
        m_effect->SetNormalTexture(m_normalMapBMW.Get());
        m_effect->SetSpecularTexture(m_specularBMW.Get());
        if (timeStamp < fadeInEnd2)  // fade in
        {
            float colorIntensity = (timeStamp - fadeInStart2) / (fadeDuration);
            float fogStart = colorIntensity + fogGap1;
            float fogEnd = colorIntensity + fogGap2;
            m_effect->SetFogStart(fogStart);
            m_effect->SetFogEnd(fogEnd);

            SetFogVals(testFogTarget1, colorIntensity);

            m_debugValue1 = colorIntensity;
            m_debugValue2 = fogStart;
            m_debugValue3 = fogEnd;
        }
        else if (timeStamp > fadeOutStart2) // fade out
        {
            float colorIntensity = (fadeOutEnd2 - timeStamp) / (fadeDuration);
            float fogStart = colorIntensity + fogGap1;
            float fogEnd = colorIntensity + fogGap2;
            m_effect->SetFogStart(fogStart);
            m_effect->SetFogEnd(fogEnd);

            SetFogVals(testFogTarget1, colorIntensity);
            SetFogVals2(testFogTarget2, colorIntensity);
            SetFogVals3(testFogTarget3, colorIntensity);

            m_debugValue1 = colorIntensity;
            m_debugValue2 = fogStart;
            m_debugValue3 = fogEnd;
        }
        else
        {
            //m_effect->SetFogEnabled(false);
        }
    }
    ///////////////////////////
    /// Render Start Screen ///
    /////////////////////////// 
    else if (timeStamp < fadeInStart3)
    {
        // render nothing
        float colorIntensity = (fadeInEnd3 - timeStamp) / (fadeDuration);
        colorIntensity = 0.0;
        SetFogVals(testFogTarget2, colorIntensity);
        SetFogVals2(testFogTarget2, colorIntensity);
        SetFogVals3(testFogTarget3, colorIntensity);
        SetTerrainGridDimmer(colorIntensity);

        m_camera->SetCameraStartPos(m_introCamPos);
        m_camera->SetCameraEndPos(m_startScreenCamPos);
        m_camera->SetDestinationPos(m_startScreenCamPos);
        m_camera->SetTargetStartPos(m_introCamTarg);
        m_camera->SetTargetEndPos(m_startScreenCamTarg);
        m_camera->SetCameraState(CameraState::CAMERASTATE_TRANSITION);

        m_lighting->SetLighting(Lighting::LightingState::LIGHTINGSTATE_STARTSCREEN);
        m_currentGameState = GameState::GAMESTATE_STARTSCREEN;
    }
    else if (timeStamp < fadeOutEnd3) // Render Start Screen
    {
        m_lighting->SetLighting(Lighting::LightingState::LIGHTINGSTATE_STARTSCREEN);
        m_effect->SetTexture(m_textureAutoGame.Get());
        m_effect->SetNormalTexture(m_normalMapAutoGame.Get());
        m_effect->SetSpecularTexture(m_specularAutoGame.Get());

        if (timeStamp < fadeInEnd3)  // fade in
        {
            float colorIntensity = (timeStamp - fadeInStart3) / (fadeDuration);
            float fogStart = colorIntensity + fogGap1;
            float fogEnd = colorIntensity + fogGap2;

            SetFogVals(testFogTarget1, colorIntensity);
            SetFogVals2(testFogTarget2, colorIntensity);
            SetFogVals3(testFogTarget3, colorIntensity);
            SetTerrainGridDimmer(colorIntensity);

            m_debugValue1 = colorIntensity;
            m_debugValue2 = fogStart;
            m_debugValue3 = fogEnd;
        }
        else if (timeStamp > fadeOutStart3) // fade out
        {
            float colorIntensity = (fadeOutEnd3 - timeStamp) / (fadeDuration);
            float fogStart = colorIntensity + fogGap1;
            float fogEnd = colorIntensity + fogGap2;

            SetFogVals(testFogTarget1, colorIntensity);
            SetFogVals2(testFogTarget2, colorIntensity);
            SetFogVals3(testFogTarget3, colorIntensity);
            //SetTerrainGridDimmer(colorIntensity);

            m_debugValue1 = colorIntensity;
            m_debugValue2 = fogStart;
            m_debugValue3 = fogEnd;

            float distance = DirectX::SimpleMath::Vector3::Distance(m_camera->GetPos(), m_startScreenCamZoomPos);
            float speed = distance / (fadeOutEnd3 - fadeOutStart3);

            m_camera->SetTransitionSpeed(speed);
            m_camera->SetCameraStartPos(m_startScreenCamPos);
            m_camera->SetCameraEndPos(m_startScreenCamZoomPos);
            m_camera->SetDestinationPos(m_startScreenCamZoomPos);
            m_camera->SetTargetStartPos(m_startScreenCamPos);
            m_camera->SetTargetEndPos(m_teaserCamTarg);
            m_camera->SetCameraState(CameraState::CAMERASTATE_TRANSITION);
        }
        else
        {
            //m_effect->SetFogEnabled(false);
        }
    }
    /////////////////////////////////////
    /// Render GamePlay Start Screen  ///
    /////////////////////////////////////
    else if (timeStamp < fadeInStart4)
    {
        // render nothing
        DirectX::SimpleMath::Vector3 preZoomPos = m_startScreenCamZoomPos;
        preZoomPos.y = 0.0;

        m_camera->SetPos(m_gamePlayStartCamPos1);
        m_camera->SetTargetPos(m_gamePlayStartCamTarg1);

        m_lighting->SetLighting(Lighting::LightingState::LIGHTINGSTATE_TEST01);

        //m_currentGameState = GameState::GAMESTATE_GAMEPLAYSTART;


        m_effect->SetFogStart(1.0);
        m_effect->SetFogEnd(5.0);
        m_effect->SetFogEnabled(true);

        m_effect2->SetFogEnabled(false);
        m_effect3->SetFogEnabled(false);
    }
    else if (timeStamp < fadeOutEnd4)  // Render GamePlay Start Screen
    {
        if (timeStamp < fadeInEnd4)  // fade in
        {
            m_currentGameState = GameState::GAMESTATE_GAMEPLAYSTART;
            m_camera->SetPos(m_gamePlayStartCamPos1);
            m_camera->SetTargetPos(m_gamePlayStartCamTarg1);
            float distance = DirectX::SimpleMath::Vector3::Distance(m_camera->GetPos(), m_teaserCamPos);
            float speed = distance / (fadeInEnd4 - fadeInStart4);
            /*
            m_camera->SetTransitionSpeed(speed);
            m_camera->SetCameraStartPos(m_camera->GetPos());
            m_camera->SetCameraEndPos(m_teaserCamPos);
            m_camera->SetDestinationPos(m_teaserCamPos);
            m_camera->SetTargetStartPos(m_startScreenZCamZoomTarg);
            m_camera->SetTargetEndPos(m_teaserCamTarg);
            m_camera->SetCameraState(CameraState::CAMERASTATE_TRANSITION);
            */
            float colorIntensity = (timeStamp - fadeInStart4) / (fadeDuration);
            float fogStart = colorIntensity + fogGap1;
            float fogEnd = colorIntensity + fogGap2;

            m_debugValue1 = colorIntensity;
            m_debugValue2 = fogStart;
            m_debugValue3 = fogEnd;
        }
        else if (timeStamp > fadeOutStart4) // fade out
        {
            DirectX::SimpleMath::Vector3 testCameraPos = m_camera->GetPos();

            float colorIntensity = (fadeOutEnd4 - timeStamp) / (fadeDuration);
            float fogStart = colorIntensity + fogGap1;
            float fogEnd = colorIntensity + fogGap2;

            m_debugValue1 = colorIntensity;
            m_debugValue2 = fogStart;
            m_debugValue3 = fogEnd;
        }
        else
        {
            m_effect2->SetFogEnabled(false);
            //m_currentGameState = GameState::GAMESTATE_GAMEPLAYSTART;
        }
    }

    if (timeStamp > fadeOutEnd4)
    {

    }

    if (m_currentGameState == GameState::GAMESTATE_INTROSCREEN)
    {
        DrawLogoScreen();
    }
    else if (m_currentGameState == GameState::GAMESTATE_STARTSCREEN)
    {
        DrawStartScreen();
    }
    else if (m_currentGameState == GameState::GAMESTATE_TEASERSCREEN)
    {
        //DrawTeaserScreen();
    }
}

void Game::DrawLightBar()
{
    DirectX::XMVECTORF32 color1 = DirectX::Colors::Red;
    DirectX::XMVECTORF32 color2 = DirectX::Colors::Black;
    const float timeStamp = static_cast<float>(m_testTimer);
    float focusPoint = cosf(timeStamp * 3.f) * .7f;

    DirectX::SimpleMath::Vector3 normal = -DirectX::SimpleMath::Vector3::UnitX;
    float x = m_teaserScreenDistance - 0.01f;
    float y = -0.1f;
    float z = .7f;
    DirectX::SimpleMath::Vector3 left(x, y, -z);
    DirectX::SimpleMath::Vector3 right(x, y, z);
    DirectX::SimpleMath::Vector3 focus(x, y, focusPoint);

    const int lineCount = 10;
    float spacing = .0035f;
    for (int i = 0; i < lineCount; ++i)
    {
        left.y += spacing;
        right.y += spacing;
        focus.y += spacing;
        VertexPositionNormalColor v0(left, normal, color2);
        VertexPositionNormalColor v1(right, normal, color2);
        VertexPositionNormalColor v2(focus, normal, color1);

        m_batch2->DrawLine(v0, v2);
        m_batch2->DrawLine(v2, v1);
    }
}

void Game::DrawLightFocus1()
{
    const float line = .25f;
    DirectX::SimpleMath::Vector3 focalPoint = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 yLine = focalPoint;
    yLine.y += line;
    DirectX::SimpleMath::Vector3 yLine2 = focalPoint;
    yLine2.y -= line;
    DirectX::SimpleMath::Vector3 xLine = focalPoint;
    xLine.x += line;
    DirectX::SimpleMath::Vector3 xLine2 = focalPoint;
    xLine2.x -= line;
    DirectX::SimpleMath::Vector3 zLine = focalPoint;
    zLine.z += line;
    DirectX::SimpleMath::Vector3 zLine2 = focalPoint;
    zLine2.z -= line;

    DirectX::SimpleMath::Matrix rotMatrix1 = DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(45.0));
    DirectX::SimpleMath::Matrix rotMatrix2 = DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(45.0));
    DirectX::SimpleMath::Matrix rotMatrix = rotMatrix1 * rotMatrix2;

    DirectX::SimpleMath::Vector3::Transform(yLine, rotMatrix, yLine);
    DirectX::SimpleMath::Vector3::Transform(xLine, rotMatrix, xLine);
    DirectX::SimpleMath::Vector3::Transform(zLine, rotMatrix, zLine);
    DirectX::SimpleMath::Vector3::Transform(yLine2, rotMatrix, yLine2);
    DirectX::SimpleMath::Vector3::Transform(xLine2, rotMatrix, xLine2);
    DirectX::SimpleMath::Vector3::Transform(zLine2, rotMatrix, zLine2);

    focalPoint = m_lightPos0;
    yLine += focalPoint;
    xLine += focalPoint;
    zLine += focalPoint;
    yLine2 += focalPoint;
    xLine2 += focalPoint;
    zLine2 += focalPoint;

    VertexPositionNormalColor origin(focalPoint, DirectX::SimpleMath::Vector3::UnitX, Colors::Yellow);
    VertexPositionNormalColor yOffset(yLine, DirectX::SimpleMath::Vector3::UnitX, Colors::Yellow);
    VertexPositionNormalColor xOffset(xLine, DirectX::SimpleMath::Vector3::UnitZ, Colors::Yellow);
    VertexPositionNormalColor zOffset(zLine, DirectX::SimpleMath::Vector3::UnitY, Colors::Yellow);

    VertexPositionNormalColor yOffset2(yLine2, DirectX::SimpleMath::Vector3::UnitX, Colors::Yellow);
    VertexPositionNormalColor xOffset2(xLine2, DirectX::SimpleMath::Vector3::UnitZ, Colors::Yellow);
    VertexPositionNormalColor zOffset2(zLine2, DirectX::SimpleMath::Vector3::UnitY, Colors::Yellow);
    m_batch2->DrawLine(origin, yOffset);
    m_batch2->DrawLine(origin, xOffset);
    m_batch2->DrawLine(origin, zOffset);

    m_batch2->DrawLine(origin, yOffset2);
    m_batch2->DrawLine(origin, xOffset2);
    m_batch2->DrawLine(origin, zOffset2);
}

void Game::DrawLightFocus2()
{
    const float line = .25f;

    DirectX::SimpleMath::Vector3 focalPoint = m_lightPos1;
    DirectX::SimpleMath::Vector3 yLine = focalPoint;
    yLine.y += line;
    DirectX::SimpleMath::Vector3 yLine2 = focalPoint;
    yLine2.y -= line;
    DirectX::SimpleMath::Vector3 xLine = focalPoint;
    xLine.x += line;
    DirectX::SimpleMath::Vector3 xLine2 = focalPoint;
    xLine2.x -= line;
    DirectX::SimpleMath::Vector3 zLine = focalPoint;
    zLine.z += line;
    DirectX::SimpleMath::Vector3 zLine2 = focalPoint;
    zLine2.z -= line;
    VertexPositionNormalColor origin(focalPoint, DirectX::SimpleMath::Vector3::UnitX, Colors::Yellow);
    VertexPositionNormalColor yOffset(yLine, DirectX::SimpleMath::Vector3::UnitX, Colors::Yellow);
    VertexPositionNormalColor xOffset(xLine, DirectX::SimpleMath::Vector3::UnitZ, Colors::Yellow);
    VertexPositionNormalColor zOffset(zLine, DirectX::SimpleMath::Vector3::UnitY, Colors::Yellow);

    VertexPositionNormalColor yOffset2(yLine2, DirectX::SimpleMath::Vector3::UnitX, Colors::Yellow);
    VertexPositionNormalColor xOffset2(xLine2, DirectX::SimpleMath::Vector3::UnitZ, Colors::Yellow);
    VertexPositionNormalColor zOffset2(zLine2, DirectX::SimpleMath::Vector3::UnitY, Colors::Yellow);
    m_batch2->DrawLine(origin, yOffset);
    m_batch2->DrawLine(origin, xOffset);
    m_batch2->DrawLine(origin, zOffset);

    m_batch2->DrawLine(origin, yOffset2);
    m_batch2->DrawLine(origin, xOffset2);
    m_batch2->DrawLine(origin, zOffset2);
}

void Game::DrawLightFocus3()
{
    const float line = .25f;

    DirectX::SimpleMath::Vector3 focalPoint = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 yLine = focalPoint;
    yLine.y += line;
    DirectX::SimpleMath::Vector3 yLine2 = focalPoint;
    yLine2.y -= line;
    DirectX::SimpleMath::Vector3 xLine = focalPoint;
    xLine.x += line;
    DirectX::SimpleMath::Vector3 xLine2 = focalPoint;
    xLine2.x -= line;
    DirectX::SimpleMath::Vector3 zLine = focalPoint;
    zLine.z += line;
    DirectX::SimpleMath::Vector3 zLine2 = focalPoint;
    zLine2.z -= line;

    DirectX::SimpleMath::Matrix rotMatrix1 = DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(45.0));
    DirectX::SimpleMath::Matrix rotMatrix2 = DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(45.0));

    DirectX::SimpleMath::Matrix rotMatrix = rotMatrix1 * rotMatrix2;

    DirectX::SimpleMath::Vector3::Transform(yLine, rotMatrix, yLine);
    DirectX::SimpleMath::Vector3::Transform(xLine, rotMatrix, xLine);
    DirectX::SimpleMath::Vector3::Transform(zLine, rotMatrix, zLine);
    DirectX::SimpleMath::Vector3::Transform(yLine2, rotMatrix, yLine2);
    DirectX::SimpleMath::Vector3::Transform(xLine2, rotMatrix, xLine2);
    DirectX::SimpleMath::Vector3::Transform(zLine2, rotMatrix, zLine2);

    focalPoint = m_lightPos2;
    yLine += focalPoint;
    xLine += focalPoint;
    zLine += focalPoint;
    yLine2 += focalPoint;
    xLine2 += focalPoint;
    zLine2 += focalPoint;

    VertexPositionNormalColor origin(focalPoint, DirectX::SimpleMath::Vector3::UnitX, Colors::Yellow);
    VertexPositionNormalColor yOffset(yLine, DirectX::SimpleMath::Vector3::UnitX, Colors::Yellow);
    VertexPositionNormalColor xOffset(xLine, DirectX::SimpleMath::Vector3::UnitZ, Colors::Yellow);
    VertexPositionNormalColor zOffset(zLine, DirectX::SimpleMath::Vector3::UnitY, Colors::Yellow);

    VertexPositionNormalColor yOffset2(yLine2, DirectX::SimpleMath::Vector3::UnitX, Colors::Yellow);
    VertexPositionNormalColor xOffset2(xLine2, DirectX::SimpleMath::Vector3::UnitZ, Colors::Yellow);
    VertexPositionNormalColor zOffset2(zLine2, DirectX::SimpleMath::Vector3::UnitY, Colors::Yellow);
    m_batch2->DrawLine(origin, yOffset);
    m_batch2->DrawLine(origin, xOffset);
    m_batch2->DrawLine(origin, zOffset);

    m_batch2->DrawLine(origin, yOffset2);
    m_batch2->DrawLine(origin, xOffset2);
    m_batch2->DrawLine(origin, zOffset2);
}

void Game::DrawLoadScreen()
{
    DirectX::SimpleMath::Vector3 camPos = m_camera->GetPos();
    DirectX::SimpleMath::Vector3 targPos = m_camera->GetTargetPos();
    DirectX::SimpleMath::Matrix rotMat = DirectX::SimpleMath::Matrix::CreateLookAt(camPos, -targPos, m_camera->GetUp());
    rotMat = m_camera->GetViewMatrix();
    rotMat = m_camera->GetProjectionMatrix();
    DirectX::SimpleMath::Vector3 targDir = camPos - targPos;
    //targDir *= 0.5f;
    //targPos = camPos + targDir;
    
    rotMat = DirectX::SimpleMath::Matrix::CreateLookAt(DirectX::SimpleMath::Vector3::Zero, targDir, m_camera->GetUp());
    //rotMat = DirectX::SimpleMath::Matrix::CreateLookAt(targDir, DirectX::SimpleMath::Vector3::Zero, m_camera->GetUp());
    //rotMat = DirectX::SimpleMath::Matrix::CreateWorld(targPos, -targDir, m_camera->GetUp());
    rotMat = DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(-90.0f));
    //targPos = DirectX::SimpleMath::Vector3::Transform(targPos, rotMat);

    //const DirectX::SimpleMath::Vector3 vertexColor(1.000000000f, 1.000000000f, 1.000000000f);// = DirectX::Colors::White;
    //DirectX::SimpleMath::Vector4 vertexColor(1.000000000f, 0.000000000f, 1.000000000f, 1.0f);
    float screenColor = m_loadScreenTimerStart * 0.02f;
    DirectX::SimpleMath::Vector4 vertexColor(screenColor, screenColor, screenColor, 1.0f);
    vertexColor = m_testColor;
    //vertexColor = DirectX::Colors::Red;
    const float height = 2.5f;
    const float width = 4.888888888f;
    const float distance = 0.0f;

    DirectX::SimpleMath::Vector3 topLeft(distance, height, -width);
    DirectX::SimpleMath::Vector3 topRight(distance, height, width);
    DirectX::SimpleMath::Vector3 bottomRight(distance, -height, width);
    DirectX::SimpleMath::Vector3 bottomLeft(distance, -height, -width);

    topLeft = DirectX::SimpleMath::Vector3::Transform(topLeft, rotMat);
    topRight = DirectX::SimpleMath::Vector3::Transform(topRight, rotMat);
    bottomRight = DirectX::SimpleMath::Vector3::Transform(bottomRight, rotMat);
    bottomLeft = DirectX::SimpleMath::Vector3::Transform(bottomLeft, rotMat);

    DirectX::SimpleMath::Matrix rotMat2 = DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(180.0f));

    topLeft = DirectX::SimpleMath::Vector3::Transform(topLeft, rotMat2);
    topRight = DirectX::SimpleMath::Vector3::Transform(topRight, rotMat2);
    bottomRight = DirectX::SimpleMath::Vector3::Transform(bottomRight, rotMat2);
    bottomLeft = DirectX::SimpleMath::Vector3::Transform(bottomLeft, rotMat2);

    DirectX::SimpleMath::Vector3 transVec(8.0f, 0.7f, 0.0f);

    topLeft += transVec;
    topRight += transVec;
    bottomRight += transVec;
    bottomLeft += transVec;

    //camPos = DirectX::SimpleMath::Vector3::UnitX;
    /*
    topLeft += camPos;
    topRight += camPos;
    bottomRight += camPos;
    bottomLeft += camPos;
    */
    /*
    topLeft += targPos;
    topRight += targPos;
    bottomRight += targPos;
    bottomLeft += targPos;
    */
    const DirectX::SimpleMath::Vector3 vertexNormal = DirectX::SimpleMath::Vector3::UnitY;
    VertexPositionNormalColor vertTopLeft(topLeft, vertexNormal, vertexColor);
    VertexPositionNormalColor vertTopRight(topRight, vertexNormal, vertexColor);
    VertexPositionNormalColor vertBottomRight(bottomRight, vertexNormal, vertexColor);
    VertexPositionNormalColor vertBottomLeft(bottomLeft, vertexNormal, vertexColor);


    //m_batch2->DrawQuad(vertTopLeft, vertTopRight, vertBottomRight, vertBottomLeft);
    
}

void Game::DrawLogoScreen()
{
    const DirectX::SimpleMath::Vector3 vertexNormal = -DirectX::SimpleMath::Vector3::UnitX;
    const DirectX::SimpleMath::Vector3 vertexColor(1.000000000f, 1.000000000f, 1.000000000f);// = DirectX::Colors::White;
    const float height = .5f;
    const float width = .888888888f;
    const float distance = 1.1f;

    DirectX::SimpleMath::Vector3 topLeft(distance, height, -width);
    DirectX::SimpleMath::Vector3 topRight(distance, height, width);
    DirectX::SimpleMath::Vector3 bottomRight(distance, -height, width);
    DirectX::SimpleMath::Vector3 bottomLeft(distance, -height, -width);

    VertexPositionNormalColorTexture vertTopLeft(topLeft, vertexNormal, vertexColor, DirectX::SimpleMath::Vector2(0, 0));
    VertexPositionNormalColorTexture vertTopRight(topRight, vertexNormal, vertexColor, DirectX::SimpleMath::Vector2(1, 0));
    VertexPositionNormalColorTexture vertBottomRight(bottomRight, vertexNormal, vertexColor, DirectX::SimpleMath::Vector2(1, 1));
    VertexPositionNormalColorTexture vertBottomLeft(bottomLeft, vertexNormal, vertexColor, DirectX::SimpleMath::Vector2(0, 1));

    m_batch->DrawQuad(vertTopLeft, vertTopRight, vertBottomRight, vertBottomLeft);
}

void Game::DrawMenuEnvironmentSelect()
{

}

void Game::DrawMenuMain()
{
    float lineDrawY = m_fontMenuPos.y + 15;
    float lineDrawSpacingY = 15;
    std::string menuTitle = "Main Menu";
    float menuTitlePosX = m_fontMenuPos.x;
    float menuTitlePosY = lineDrawY;
    DirectX::SimpleMath::Vector2 menuTitlePos(menuTitlePosX, menuTitlePosY);
    DirectX::SimpleMath::Vector2 menuOrigin = m_titleFont->MeasureString(menuTitle.c_str()) / 2.f;
    DirectX::SimpleMath::Vector4 color1(1.000000000f, 0.549019635f, 0.000000000f, 1.000000000f);// = DirectX::Colors::DarkOrange;
    DirectX::SimpleMath::Vector4 color2(1.000000000f, 0.647058845f, 0.000000000f, 1.000000000f);// = DirectX::Colors::Orange;
    m_titleFont->DrawString(m_spriteBatch.get(), menuTitle.c_str(), menuTitlePos + DirectX::SimpleMath::Vector2(4.f, 4.f), color1, 0.f, menuOrigin);
    m_titleFont->DrawString(m_spriteBatch.get(), menuTitle.c_str(), menuTitlePos + DirectX::SimpleMath::Vector2(3.f, 3.f), color1, 0.f, menuOrigin);
    m_titleFont->DrawString(m_spriteBatch.get(), menuTitle.c_str(), menuTitlePos + DirectX::SimpleMath::Vector2(2.f, 2.f), color1, 0.f, menuOrigin);
    m_titleFont->DrawString(m_spriteBatch.get(), menuTitle.c_str(), menuTitlePos + DirectX::SimpleMath::Vector2(1.f, 1.f), color1, 0.f, menuOrigin);
    m_titleFont->DrawString(m_spriteBatch.get(), menuTitle.c_str(), menuTitlePos + DirectX::SimpleMath::Vector2(-1.f, -1.f), color2, 0.f, menuOrigin);
    m_titleFont->DrawString(m_spriteBatch.get(), menuTitle.c_str(), menuTitlePos, Colors::White, 0.f, menuOrigin);

    lineDrawY += menuTitlePosY + lineDrawSpacingY;
    std::string menuObj0String = "Play!";
    DirectX::SimpleMath::Vector2 menuObj0Pos(menuTitlePosX, lineDrawY);
    DirectX::SimpleMath::Vector2 menuObj0Origin = m_font->MeasureString(menuObj0String.c_str()) / 2.f;

    lineDrawY += menuObj0Pos.y;
    std::string menuObj1String = "Vehicle Select";
    DirectX::SimpleMath::Vector2 menuObj1Pos(menuTitlePosX, lineDrawY);
    DirectX::SimpleMath::Vector2 menuObj1Origin = m_font->MeasureString(menuObj1String.c_str()) / 2.f;

    lineDrawY += menuObj0Pos.y;
    std::string menuObj2String = "Environment Select";
    DirectX::SimpleMath::Vector2 menuObj2Pos(menuTitlePosX, lineDrawY);
    DirectX::SimpleMath::Vector2 menuObj2Origin = m_font->MeasureString(menuObj2String.c_str()) / 2.f;

    // Demo
    ////////////////////////////
    /*
    lineDrawY += menuObj0Pos.y;
    std::string menuObjHydraString = "Hole 12 Golden Bell";
    DirectX::SimpleMath::Vector2 menuObjHydraPos(menuTitlePosX, lineDrawY);
    DirectX::SimpleMath::Vector2 menuObjHydraOrigin = m_font->MeasureString(menuObjHydraString.c_str()) / 2.f;
    */
    ///////////////////////////

    lineDrawY += menuObj0Pos.y;
    std::string menuObj3String = "Quit";
    DirectX::SimpleMath::Vector2 menuObj3Pos(menuTitlePosX, lineDrawY);
    DirectX::SimpleMath::Vector2 menuObj3Origin = m_font->MeasureString(menuObj3String.c_str()) / 2.f;

    if (m_menuSelect < 0 || m_menuSelect > 3)
    {
        m_menuSelect = 0;
    }
    if (m_menuSelect == 0)
    {
        m_font->DrawString(m_spriteBatch.get(), menuObj0String.c_str(), menuObj0Pos + DirectX::SimpleMath::Vector2(4.f, 4.f), Colors::White, 0.f, menuObj0Origin);
        m_font->DrawString(m_spriteBatch.get(), menuObj0String.c_str(), menuObj0Pos + DirectX::SimpleMath::Vector2(-4.f, 4.f), Colors::White, 0.f, menuObj0Origin);
        m_font->DrawString(m_spriteBatch.get(), menuObj0String.c_str(), menuObj0Pos + DirectX::SimpleMath::Vector2(-4.f, -4.f), Colors::White, 0.f, menuObj0Origin);
        m_font->DrawString(m_spriteBatch.get(), menuObj0String.c_str(), menuObj0Pos + DirectX::SimpleMath::Vector2(4.f, -4.f), Colors::White, 0.f, menuObj0Origin);

        m_font->DrawString(m_spriteBatch.get(), menuObj0String.c_str(), menuObj0Pos + DirectX::SimpleMath::Vector2(2.f, 2.f), Colors::Black, 0.f, menuObj0Origin);
        m_font->DrawString(m_spriteBatch.get(), menuObj0String.c_str(), menuObj0Pos + DirectX::SimpleMath::Vector2(-2.f, 2.f), Colors::Black, 0.f, menuObj0Origin);
        m_font->DrawString(m_spriteBatch.get(), menuObj0String.c_str(), menuObj0Pos + DirectX::SimpleMath::Vector2(-2.f, -2.f), Colors::Black, 0.f, menuObj0Origin);
        m_font->DrawString(m_spriteBatch.get(), menuObj0String.c_str(), menuObj0Pos + DirectX::SimpleMath::Vector2(2.f, -2.f), Colors::Black, 0.f, menuObj0Origin);

        m_font->DrawString(m_spriteBatch.get(), menuObj0String.c_str(), menuObj0Pos, Colors::White, 0.f, menuObj0Origin);
    }
    else
    {
        m_font->DrawString(m_spriteBatch.get(), menuObj0String.c_str(), menuObj0Pos, Colors::White, 0.f, menuObj0Origin);
    }

    if (m_menuSelect == 1)
    {
        m_font->DrawString(m_spriteBatch.get(), menuObj1String.c_str(), menuObj1Pos + DirectX::SimpleMath::Vector2(4.f, 4.f), Colors::White, 0.f, menuObj1Origin);
        m_font->DrawString(m_spriteBatch.get(), menuObj1String.c_str(), menuObj1Pos + DirectX::SimpleMath::Vector2(-4.f, 4.f), Colors::White, 0.f, menuObj1Origin);
        m_font->DrawString(m_spriteBatch.get(), menuObj1String.c_str(), menuObj1Pos + DirectX::SimpleMath::Vector2(-4.f, -4.f), Colors::White, 0.f, menuObj1Origin);
        m_font->DrawString(m_spriteBatch.get(), menuObj1String.c_str(), menuObj1Pos + DirectX::SimpleMath::Vector2(4.f, -4.f), Colors::White, 0.f, menuObj1Origin);

        m_font->DrawString(m_spriteBatch.get(), menuObj1String.c_str(), menuObj1Pos + DirectX::SimpleMath::Vector2(2.f, 2.f), Colors::Black, 0.f, menuObj1Origin);
        m_font->DrawString(m_spriteBatch.get(), menuObj1String.c_str(), menuObj1Pos + DirectX::SimpleMath::Vector2(-2.f, 2.f), Colors::Black, 0.f, menuObj1Origin);
        m_font->DrawString(m_spriteBatch.get(), menuObj1String.c_str(), menuObj1Pos + DirectX::SimpleMath::Vector2(-2.f, -2.f), Colors::Black, 0.f, menuObj1Origin);
        m_font->DrawString(m_spriteBatch.get(), menuObj1String.c_str(), menuObj1Pos + DirectX::SimpleMath::Vector2(2.f, -2.f), Colors::Black, 0.f, menuObj1Origin);

        m_font->DrawString(m_spriteBatch.get(), menuObj1String.c_str(), menuObj1Pos, Colors::White, 0.f, menuObj1Origin);
    }
    else
    {
        m_font->DrawString(m_spriteBatch.get(), menuObj1String.c_str(), menuObj1Pos, Colors::White, 0.f, menuObj1Origin);
    }

    if (m_menuSelect == 2)
    {
        m_font->DrawString(m_spriteBatch.get(), menuObj2String.c_str(), menuObj2Pos + DirectX::SimpleMath::Vector2(4.f, 4.f), Colors::White, 0.f, menuObj2Origin);
        m_font->DrawString(m_spriteBatch.get(), menuObj2String.c_str(), menuObj2Pos + DirectX::SimpleMath::Vector2(-4.f, 4.f), Colors::White, 0.f, menuObj2Origin);
        m_font->DrawString(m_spriteBatch.get(), menuObj2String.c_str(), menuObj2Pos + DirectX::SimpleMath::Vector2(-4.f, -4.f), Colors::White, 0.f, menuObj2Origin);
        m_font->DrawString(m_spriteBatch.get(), menuObj2String.c_str(), menuObj2Pos + DirectX::SimpleMath::Vector2(4.f, -4.f), Colors::White, 0.f, menuObj2Origin);

        m_font->DrawString(m_spriteBatch.get(), menuObj2String.c_str(), menuObj2Pos + DirectX::SimpleMath::Vector2(2.f, 2.f), Colors::Black, 0.f, menuObj2Origin);
        m_font->DrawString(m_spriteBatch.get(), menuObj2String.c_str(), menuObj2Pos + DirectX::SimpleMath::Vector2(-2.f, 2.f), Colors::Black, 0.f, menuObj2Origin);
        m_font->DrawString(m_spriteBatch.get(), menuObj2String.c_str(), menuObj2Pos + DirectX::SimpleMath::Vector2(-2.f, -2.f), Colors::Black, 0.f, menuObj2Origin);
        m_font->DrawString(m_spriteBatch.get(), menuObj2String.c_str(), menuObj2Pos + DirectX::SimpleMath::Vector2(2.f, -2.f), Colors::Black, 0.f, menuObj2Origin);

        m_font->DrawString(m_spriteBatch.get(), menuObj2String.c_str(), menuObj2Pos, Colors::White, 0.f, menuObj2Origin);
    }
    else
    {
        m_font->DrawString(m_spriteBatch.get(), menuObj2String.c_str(), menuObj2Pos, Colors::White, 0.f, menuObj2Origin);
    }
    if (m_menuSelect == 3)
    {
        m_font->DrawString(m_spriteBatch.get(), menuObj3String.c_str(), menuObj3Pos + DirectX::SimpleMath::Vector2(4.f, 4.f), Colors::White, 0.f, menuObj3Origin);
        m_font->DrawString(m_spriteBatch.get(), menuObj3String.c_str(), menuObj3Pos + DirectX::SimpleMath::Vector2(-4.f, 4.f), Colors::White, 0.f, menuObj3Origin);
        m_font->DrawString(m_spriteBatch.get(), menuObj3String.c_str(), menuObj3Pos + DirectX::SimpleMath::Vector2(-4.f, -4.f), Colors::White, 0.f, menuObj3Origin);
        m_font->DrawString(m_spriteBatch.get(), menuObj3String.c_str(), menuObj3Pos + DirectX::SimpleMath::Vector2(4.f, -4.f), Colors::White, 0.f, menuObj3Origin);

        m_font->DrawString(m_spriteBatch.get(), menuObj3String.c_str(), menuObj3Pos + DirectX::SimpleMath::Vector2(2.f, 2.f), Colors::Black, 0.f, menuObj3Origin);
        m_font->DrawString(m_spriteBatch.get(), menuObj3String.c_str(), menuObj3Pos + DirectX::SimpleMath::Vector2(-2.f, 2.f), Colors::Black, 0.f, menuObj3Origin);
        m_font->DrawString(m_spriteBatch.get(), menuObj3String.c_str(), menuObj3Pos + DirectX::SimpleMath::Vector2(-2.f, -2.f), Colors::Black, 0.f, menuObj3Origin);
        m_font->DrawString(m_spriteBatch.get(), menuObj3String.c_str(), menuObj3Pos + DirectX::SimpleMath::Vector2(2.f, -2.f), Colors::Black, 0.f, menuObj3Origin);

        m_font->DrawString(m_spriteBatch.get(), menuObj3String.c_str(), menuObj3Pos, Colors::White, 0.f, menuObj3Origin);
    }
    else
    {
        m_font->DrawString(m_spriteBatch.get(), menuObj3String.c_str(), menuObj3Pos, Colors::White, 0.f, menuObj3Origin);
    }
}

void Game::DrawSky()
{
    /* // static sky 
    DirectX::SimpleMath::Matrix rotMat = DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(-30.0f));
    rotMat *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(0.0f));
    rotMat *= DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(10.0f));
    m_skyShape->Draw(rotMat, m_camera->GetViewMatrix(), m_camera->GetProjectionMatrix(), DirectX::SimpleMath::Vector4(1.0, 1.0, 1.0, 2.0f), m_textureSky.Get());
    */

    m_skyRotation += static_cast<float>(m_timer.GetElapsedSeconds()) * 0.19f;
    DirectX::SimpleMath::Matrix rotMat = DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(-m_skyRotation));
    rotMat *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(30.0f));
    rotMat *= DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(30.0f));
    m_skyShape->Draw(rotMat, m_camera->GetViewMatrix(), m_camera->GetProjectionMatrix(), DirectX::SimpleMath::Vector4(1.0, 1.0, 1.0, 2.0f), m_textureSky.Get());
}

void Game::DrawStartScreen()
{
    const float timeStamp = static_cast<float>(m_testTimer);
    m_lighting->SetLighting(Lighting::LightingState::LIGHTINGSTATE_STARTSCREEN);

    m_effect->SetTexture(m_textureAutoGame.Get());
    m_effect->SetNormalTexture(m_normalMapAutoGame.Get());
    m_effect->SetSpecularTexture(m_specularAutoGame.Get());

    const DirectX::SimpleMath::Vector3 vertexColor(1.000000000f, 1.000000000f, 1.000000000f);// = DirectX::Colors::White;
    DirectX::SimpleMath::Vector3 testNorm(0.0, 0.0, 1.0);
    testNorm.Normalize();

    auto time = static_cast<float>(m_timer.GetTotalSeconds());
    float yaw = time * 0.4f;
    float pitch = time * 0.7f;
    float roll = time * 1.1f;
    auto quat0 = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(0.0, roll, 0.0);

    auto norm01 = XMVector3Rotate(m_lightPos0, quat0);
    testNorm = norm01;

    DirectX::SimpleMath::Vector3 vertexNormal = DirectX::SimpleMath::Vector3::UnitX;

    // start background drawing
    const float distance = 1.1f;
    const float height = 1.5f;
    const float width = 1.77777777;
    const float heightBase = 0.0f;
    DirectX::SimpleMath::Vector3 topLeft(distance, height, -width);
    DirectX::SimpleMath::Vector3 topRight(distance, height, width);
    DirectX::SimpleMath::Vector3 bottomRight(distance, heightBase, width);
    DirectX::SimpleMath::Vector3 bottomLeft(distance, heightBase, -width);

    float uStart = 0.0;
    float uStop = 1.0;
    float vStart = 0.25;
    float vStop = 1.0;
    /////////////////////////////

    vertexNormal = -m_lightPos0 + testNorm;

    vertexNormal = -m_lightPos0;

    vertexNormal.Normalize();

    vertexNormal = m_testNorm + m_lightPos0;
    vertexNormal.Normalize();
    vertexNormal += m_lightPos0;
    vertexNormal.Normalize();
    vertexNormal = -vertexNormal;

    /////////////////////////////

    //vertexNormal = testNorm;
    vertexNormal = -DirectX::SimpleMath::Vector3::UnitX;

    VertexPositionNormalColorTexture vertTopLeft(topLeft, vertexNormal, vertexColor, DirectX::SimpleMath::Vector2(uStart, vStart));
    VertexPositionNormalColorTexture vertTopRight(topRight, vertexNormal, vertexColor, DirectX::SimpleMath::Vector2(uStop, vStart));
    VertexPositionNormalColorTexture vertBottomRight(bottomRight, vertexNormal, vertexColor, DirectX::SimpleMath::Vector2(uStop, vStop));
    VertexPositionNormalColorTexture vertBottomLeft(bottomLeft, vertexNormal, vertexColor, DirectX::SimpleMath::Vector2(uStart, vStop));

    // Start moon drawing
    const float moonHeight = 0.2;
    const float moonWidth = 0.2;
    const float moonSize = 0.2;
    const float moonOriginY = 0.7;
    const float moonOriginZ = -1.1;
    const float moonDepth = -0.01;
    DirectX::SimpleMath::Vector3 moonOrigin(distance, moonOriginY, moonOriginZ);
    topLeft = DirectX::SimpleMath::Vector3(moonDepth + distance, moonOriginY, moonOriginZ);
    topRight = DirectX::SimpleMath::Vector3(moonDepth, 0.0, moonSize);
    bottomRight = DirectX::SimpleMath::Vector3(moonDepth, -moonSize, moonSize);
    bottomLeft = DirectX::SimpleMath::Vector3(moonDepth, -moonSize, 0.0);

    topRight += moonOrigin;
    bottomRight += moonOrigin;
    bottomLeft += moonOrigin;

    uStart = 0.0;
    uStop = 0.1588541666666667;
    vStart = 0.0;
    vStop = 0.2824074074074074;

    vertexNormal = -DirectX::SimpleMath::Vector3::UnitX;

    vertTopLeft = VertexPositionNormalColorTexture(topLeft, vertexNormal, vertexColor, DirectX::SimpleMath::Vector2(uStart, vStart));
    vertTopRight = VertexPositionNormalColorTexture(topRight, vertexNormal, vertexColor, DirectX::SimpleMath::Vector2(uStop, vStart));
    vertBottomRight = VertexPositionNormalColorTexture(bottomRight, vertexNormal, vertexColor, DirectX::SimpleMath::Vector2(uStop, vStop));
    vertBottomLeft = VertexPositionNormalColorTexture(bottomLeft, vertexNormal, vertexColor, DirectX::SimpleMath::Vector2(uStart, vStop));

    // testing moon lighting    
    auto moonLight = dynamic_cast<IEffectLights*>(m_effect.get());
    if (moonLight)
    {
        moonLight->SetLightEnabled(0, true);
        moonLight->SetLightEnabled(1, true);
        moonLight->SetLightEnabled(2, true);

        roll = time * 1.1f;

        auto quat = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(-roll, 0.0, 0.0);
        DirectX::SimpleMath::Vector3 axis = -DirectX::SimpleMath::Vector3::UnitZ;

        DirectX::SimpleMath::Vector3 light0 = XMVector3Rotate(axis, quat);
        DirectX::SimpleMath::Vector3 light1 = XMVector3Rotate(axis, quat);
        DirectX::SimpleMath::Vector3 light2 = XMVector3Rotate(axis, quat);

        float val = 0.1;
        DirectX::SimpleMath::Vector4 test(val, val, val, 1.0);
        m_effect->SetAmbientLightColor(test);
       
        moonLight->SetLightDirection(0, light0);
        moonLight->SetLightDirection(1, light1);
        moonLight->SetLightDirection(2, light2);
    }
    
    m_effect->Apply(m_d3dContext.Get());
    // end moon lighting
    m_batch->DrawQuad(vertTopLeft, vertTopRight, vertBottomRight, vertBottomLeft);

    m_batch->End();
    m_batch->Begin();

    ////////////////////////////////
    // Start Text drawing
    const float titleWidth = 0.6;
    const float titleHeight = titleWidth * 0.111864406779661;
    const float titleSize = 0.2;
    const float titleOriginY = 0.5;
    const float titleOriginZ = 0.0;
    const float titleDepth = distance - 0.01;
    DirectX::SimpleMath::Vector3 titleOrigin(0.0, titleOriginY, titleOriginZ);
    topLeft = DirectX::SimpleMath::Vector3(titleDepth, titleHeight, -titleWidth);
    topRight = DirectX::SimpleMath::Vector3(titleDepth, titleHeight, titleWidth);
    bottomRight = DirectX::SimpleMath::Vector3(titleDepth, -titleHeight, titleWidth);
    bottomLeft = DirectX::SimpleMath::Vector3(titleDepth, -titleHeight, -titleWidth);

    topLeft += titleOrigin;
    topRight += titleOrigin;
    bottomRight += titleOrigin;
    bottomLeft += titleOrigin;

    uStart = 0.6927083333333333;
    uStop = 0.989;
    vStart = 0.0;
    vStop = 0.0611111111111111;

    vertexNormal = -DirectX::SimpleMath::Vector3::UnitX;
    vertTopLeft = VertexPositionNormalColorTexture(topLeft, vertexNormal, vertexColor, DirectX::SimpleMath::Vector2(uStart, vStart));
    vertTopRight = VertexPositionNormalColorTexture(topRight, vertexNormal, vertexColor, DirectX::SimpleMath::Vector2(uStop, vStart));
    vertBottomRight = VertexPositionNormalColorTexture(bottomRight, vertexNormal, vertexColor, DirectX::SimpleMath::Vector2(uStop, vStop));
    vertBottomLeft = VertexPositionNormalColorTexture(bottomLeft, vertexNormal, vertexColor, DirectX::SimpleMath::Vector2(uStart, vStop));

    DirectX::SimpleMath::Vector3 testLight0 = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 testLight1 = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 testLight2 = DirectX::SimpleMath::Vector3::Zero;
    ////////////////////////
        // BMW effect
        // text effect
    auto ilights = dynamic_cast<IEffectLights*>(m_effect.get());
    if (ilights)
    {
        ilights->SetLightEnabled(0, true);
        ilights->SetLightEnabled(1, true);
        ilights->SetLightEnabled(2, true);

        yaw = time * 0.4f;
        pitch = time * 0.7f;
        roll = time * 1.1f;

        quat0 = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(0.0, roll, 0.0);
        auto quat1 = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(0.0, roll, 0.0);
        auto quat2 = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(0.0, roll, 0.0);

        auto quat = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(0.0, -roll, 0.0);

        DirectX::SimpleMath::Vector3 axis = -DirectX::SimpleMath::Vector3::UnitZ;

        DirectX::SimpleMath::Vector3 light0 = XMVector3Rotate(axis, quat0);
        DirectX::SimpleMath::Vector3 light1 = XMVector3Rotate(axis, quat1);
        DirectX::SimpleMath::Vector3 light2 = XMVector3Rotate(axis, quat2);

        light0.x += 1.0;
        light0.Normalize();
        light1.x += 1.0;
        light1.Normalize();
        light2.x += 1.0;
        light2.Normalize();

        m_testNorm = XMVector3Rotate(light0, quat0);
        m_testNorm.Normalize();
        DirectX::SimpleMath::Vector3 light = XMVector3Rotate(axis, quat);
        light.x += 1.0;
        light.Normalize();

        float val = 0.1;
        DirectX::SimpleMath::Vector4 test(val, val, val, 1.0);
        m_effect->SetAmbientLightColor(test);

        ilights->SetLightDirection(0, light0);
        ilights->SetLightDirection(1, light1);
        ilights->SetLightDirection(2, light2);

        testLight0 = light0;
        testLight1 = light1;
        testLight2 = light2;
    }
    m_effect->Apply(m_d3dContext.Get());

    m_batch->DrawQuad(vertTopLeft, vertTopRight, vertBottomRight, vertBottomLeft);

    m_batch->End();
    m_batch->Begin();

    ///////////////////////
    /// Background
    ///////////////////////

    topLeft = DirectX::SimpleMath::Vector3(distance, height, -width);
    topRight = DirectX::SimpleMath::Vector3(distance, height, width);
    bottomRight = DirectX::SimpleMath::Vector3(distance, heightBase, width);
    bottomLeft = DirectX::SimpleMath::Vector3(distance, heightBase, -width);

    uStart = 0.0;
    uStop = 1.0;
    vStart = 0.0;
    vStop = 1.0;
    /////////////////////////////
    vertexNormal = -m_lightPos0 + testNorm;

    vertexNormal = -m_lightPos0;
    vertexNormal.Normalize();

    vertexNormal = m_testNorm + m_lightPos0;
    vertexNormal.Normalize();
    vertexNormal += m_lightPos0;
    vertexNormal.Normalize();
    vertexNormal = -vertexNormal;

    /////////////////////////////

    vertexNormal = -DirectX::SimpleMath::Vector3::UnitX;

    vertTopLeft = VertexPositionNormalColorTexture(topLeft, vertexNormal, vertexColor, DirectX::SimpleMath::Vector2(uStart, vStart));
    vertTopRight = VertexPositionNormalColorTexture(topRight, vertexNormal, vertexColor, DirectX::SimpleMath::Vector2(uStop, vStart));
    vertBottomRight = VertexPositionNormalColorTexture(bottomRight, vertexNormal, vertexColor, DirectX::SimpleMath::Vector2(uStop, vStop));
    vertBottomLeft = VertexPositionNormalColorTexture(bottomLeft, vertexNormal, vertexColor, DirectX::SimpleMath::Vector2(uStart, vStop));

    /////////////////////////

    if (ilights)
    {
        //testing start

        float offset = -1.0;
        float intesity = 15.;
        testLight0.x += offset;
        testLight0.Normalize();
        testLight0 *= intesity;
        testLight1.x += offset;
        testLight1.Normalize();
        testLight1 *= intesity;
        testLight2.x += offset;
        testLight2.Normalize();
        testLight2 *= intesity;

        //testing end

        ilights->SetLightDirection(0, testLight0);
        ilights->SetLightDirection(1, testLight1);
        ilights->SetLightDirection(2, testLight2);
        
    }
    m_effect->Apply(m_d3dContext.Get());

    m_batch->DrawQuad(vertTopLeft, vertTopRight, vertBottomRight, vertBottomLeft);

    m_batch->End();
    m_batch->Begin();

}

void Game::DrawTeaserScreen()
{
    m_effect->SetTexture(m_textureTeaser.Get());
    m_effect->SetNormalTexture(m_normalMapTeaser.Get());
    m_effect->SetSpecularTexture(m_specularTeaser.Get());

    const DirectX::SimpleMath::Vector3 vertexNormal = -DirectX::SimpleMath::Vector3::UnitX;
    const DirectX::SimpleMath::Vector3 vertexColor(1.000000000f, 1.000000000f, 1.000000000f);// = DirectX::Colors::White;
    const float height = .5f;
    const float width = .888888888f;
    const float distance = m_teaserScreenDistance + 0.0001f;

    DirectX::SimpleMath::Vector3 topLeft(distance, height, -width);
    DirectX::SimpleMath::Vector3 topRight(distance, height, width);
    DirectX::SimpleMath::Vector3 bottomRight(distance, -height, width);
    DirectX::SimpleMath::Vector3 bottomLeft(distance, -height, -width);
    VertexPositionNormalColorTexture vertTopLeft(topLeft, vertexNormal, vertexColor, DirectX::SimpleMath::Vector2(0, 0));
    VertexPositionNormalColorTexture vertTopRight(topRight, vertexNormal, vertexColor, DirectX::SimpleMath::Vector2(1, 0));
    VertexPositionNormalColorTexture vertBottomRight(bottomRight, vertexNormal, vertexColor, DirectX::SimpleMath::Vector2(1, 1));
    VertexPositionNormalColorTexture vertBottomLeft(bottomLeft, vertexNormal, vertexColor, DirectX::SimpleMath::Vector2(0, 1));

    m_batch->DrawQuad(vertTopLeft, vertTopRight, vertBottomRight, vertBottomLeft);
}

void Game::DrawTerrain()
{
    m_batch3->Draw(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, m_terrainVertexArrayBase, m_terrainVertexCount);
    m_batch3->Draw(D3D_PRIMITIVE_TOPOLOGY_LINELIST, m_terrainVertexArray, m_terrainVertexCount);
}

void Game::DrawTerrain2()
{
    m_batch2->Draw(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, m_terrainVertexArrayBase2, m_terrainVertexCount2);
    m_batch2->Draw(D3D_PRIMITIVE_TOPOLOGY_LINELIST, m_terrainVertexArray2, m_terrainVertexCount2);
}

void Game::DrawTerrainNew(Terrain& aTerrain)
{
    m_batch2->Draw(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, aTerrain.terrainVertexArrayBase, aTerrain.terrainVertexCount);
    m_batch2->Draw(D3D_PRIMITIVE_TOPOLOGY_LINELIST, aTerrain.terrainVertexArray, aTerrain.terrainVertexCount);
}

void Game::DrawTerrainNormals()
{
    DirectX::XMVECTORF32 lineColor = DirectX::Colors::Red;
    DirectX::SimpleMath::Vector3 origin;
    DirectX::SimpleMath::Vector3 normEnd;
    for (unsigned int i = 0; i < m_terrainVector2.size(); ++i)
    {
        origin = m_terrainVector2[i].position;
        normEnd = m_terrainVertexArray2[i].normal + origin;
        VertexPositionColor originVert(origin, lineColor);
        VertexPositionColor normEndVert(normEnd, lineColor);
        m_batch3->DrawLine(originVert, normEndVert);
    }
}

void Game::DrawTimer()
{
    std::string textLine = "Timer = " + std::to_string(m_testTimer);

    float textLinePosX = m_bitwiseFontPos.x;
    float textLinePosY = m_bitwiseFontPos.y;

    DirectX::SimpleMath::Vector2 textLinePos(textLinePosX, textLinePosY);
    DirectX::SimpleMath::Vector2 textLineOrigin = m_bitwiseFont->MeasureString(textLine.c_str()) / 2.f;
    m_font->DrawString(m_spriteBatch.get(), textLine.c_str(), m_fontPosDebug, Colors::White, 0.f, textLineOrigin);
}

void Game::DrawUIIntroScreen()
{
    float fadeDuration = m_fadeDuration;
    float logoDisplayDuration = m_logoDisplayDuration;
    float logoDisplayGap = m_logoDisplayGap;
    float startDelay = m_startDelay;

    double timeStamp = m_testTimer;
    timeStamp += m_debugStartTime;

    float fadeInStart1 = startDelay;
    float fadeInStart2 = startDelay + logoDisplayDuration + logoDisplayGap;
    float fadeInEnd1 = startDelay + fadeDuration;
    float fadeInEnd2 = startDelay + logoDisplayDuration + logoDisplayGap + fadeDuration;
    float fadeOutStart1 = startDelay + logoDisplayDuration - fadeDuration;
    float fadeOutStart2 = startDelay + logoDisplayDuration + logoDisplayGap + logoDisplayDuration - fadeDuration;
    float fadeOutEnd1 = startDelay + logoDisplayDuration;
    float fadeOutEnd2 = startDelay + logoDisplayDuration + logoDisplayGap + logoDisplayDuration;

    const float fadeInStart3 = startDelay + logoDisplayDuration + logoDisplayGap + logoDisplayDuration + logoDisplayGap;
    const float fadeInEnd3 = startDelay + logoDisplayDuration + logoDisplayGap + fadeDuration + logoDisplayDuration + logoDisplayGap + m_startScreenTimerMod;
    const float fadeOutStart3 = startDelay + logoDisplayDuration + logoDisplayGap + logoDisplayDuration + logoDisplayGap + logoDisplayDuration - fadeDuration + m_startScreenTimerMod;
    const float fadeOutEnd3 = startDelay + logoDisplayDuration + logoDisplayGap + logoDisplayDuration + logoDisplayGap + logoDisplayDuration;

    DirectX::XMVECTORF32 fadeColor = DirectX::Colors::White;

    if (timeStamp < fadeInStart1)
    {
        // Render nothing
        std::string textLine = "Insert Coin to Start";
        float textLinePosX = m_bitwiseFontPos.x;
        float textLinePosY = m_bitwiseFontPos.y;
        DirectX::SimpleMath::Vector2 textLinePos(textLinePosX, textLinePosY);
        DirectX::SimpleMath::Vector2 textLineOrigin = m_titleFont->MeasureString(textLine.c_str()) / 2.f;
        if (timeStamp < (fadeInStart1 * .5))
        {

        }
        else
        {

        }

        if (timeStamp < (fadeInStart1 * .3))  // fade in
        {
            //AudioPlaySFX(XACT_WAVEBANK_AUDIOBANK::XACT_WAVEBANK_SOUNDS_COINSFX);
            float colorIntensity = (static_cast<float>(timeStamp) - 0.0f) / (fadeInStart1 * .3f);
            fadeColor.f[0] = colorIntensity;
            fadeColor.f[1] = colorIntensity;
            fadeColor.f[2] = colorIntensity;
            //m_titleFont->DrawString(m_spriteBatch.get(), textLine.c_str(), textLinePos, fadeColor, 0.f, textLineOrigin);
        }
        else if (timeStamp > (fadeInStart1 * .8)) // fade out
        {
            //AudioPlaySFX(XACT_WAVEBANK_AUDIOBANK::XACT_WAVEBANK_SOUNDS_KNIGHTRIDERMUSIC);
            float colorIntensity = (fadeInStart1 - static_cast<float>(timeStamp)) / (fadeInStart1 * .2f);
            fadeColor.f[0] = colorIntensity;
            fadeColor.f[1] = colorIntensity;
            fadeColor.f[2] = colorIntensity;
            //m_titleFont->DrawString(m_spriteBatch.get(), textLine.c_str(), textLinePos, fadeColor, 0.f, textLineOrigin);
        }
        else // display at full intesity
        {
            //m_titleFont->DrawString(m_spriteBatch.get(), textLine.c_str(), textLinePos, fadeColor, 0.f, textLineOrigin);
        }
    }
    else if (timeStamp < fadeOutEnd1)
    {
        std::string textLine = "Proudly Presents";
        float textLinePosX = m_bitwiseFontPos.x;
        float textLinePosY = m_bitwiseFontPos.y + 100;
        DirectX::SimpleMath::Vector2 textLinePos(textLinePosX, textLinePosY);
        DirectX::SimpleMath::Vector2 textLineOrigin = m_bitwiseFont->MeasureString(textLine.c_str()) / 2.f;

        if (timeStamp < fadeInEnd1)  // fade in
        {
            float colorIntensity = (static_cast<float>(timeStamp) - fadeInStart1) / fadeDuration;
            fadeColor.f[0] = colorIntensity;
            fadeColor.f[1] = colorIntensity;
            fadeColor.f[2] = colorIntensity;
            m_bitwiseFont->DrawString(m_spriteBatch.get(), textLine.c_str(), textLinePos, fadeColor, 0.f, textLineOrigin);
        }
        else if (timeStamp > fadeOutStart1) // fade out
        {
            float colorIntensity = (fadeOutEnd1 - static_cast<float>(timeStamp)) / (fadeDuration);
            fadeColor.f[0] = colorIntensity;
            fadeColor.f[1] = colorIntensity;
            fadeColor.f[2] = colorIntensity;
            m_bitwiseFont->DrawString(m_spriteBatch.get(), textLine.c_str(), textLinePos, fadeColor, 0.f, textLineOrigin);
        }
        else // display at full intesity
        {
            m_bitwiseFont->DrawString(m_spriteBatch.get(), textLine.c_str(), textLinePos, fadeColor, 0.f, textLineOrigin);
        }
    }
    else if (timeStamp < fadeInStart2)
    {
        // render nothing
    }
    else if (timeStamp < fadeOutEnd2)
    {
        std::string textLine = "A Baltimore Magic Werks Production";
        float textLinePosX = m_bitwiseFontPos.x;
        float textLinePosY = m_bitwiseFontPos.y + 450;
        DirectX::SimpleMath::Vector2 textLinePos(textLinePosX, textLinePosY);
        DirectX::SimpleMath::Vector2 textLineOrigin = m_bitwiseFont->MeasureString(textLine.c_str()) / 2.f;
        if (timeStamp < fadeInEnd2)  // fade in
        {
            float colorIntensity = (static_cast<float>(timeStamp) - fadeInStart2) / (fadeDuration);
            fadeColor.f[0] = colorIntensity;
            fadeColor.f[1] = colorIntensity;
            fadeColor.f[2] = colorIntensity;
        }
        else if (timeStamp > fadeOutStart2) // fade out
        {
            float colorIntensity = (fadeOutEnd2 - static_cast<float>(timeStamp)) / (fadeDuration);
            fadeColor.f[0] = colorIntensity;
            fadeColor.f[1] = colorIntensity;
            fadeColor.f[2] = colorIntensity;
        }
        else
        {
            //AudioPlaySFX(XACT_WAVEBANK_AUDIOBANK::XACT_WAVEBANK_AUDIOBANK_COINSFX);
            //m_spriteBatch->Draw(m_bmwLogoTexture.Get(), m_bmwLogoPos, nullptr, fadeColor, 0.f, m_bmwLogoOrigin);
            //m_bitwiseFont->DrawString(m_spriteBatch.get(), textLine.c_str(), textLinePos, fadeColor, 0.f, textLineOrigin);
        }
    }
    else if (timeStamp < fadeInStart3)
    {
        // Render nothing
    }
    else if (timeStamp < fadeOutEnd3)
    {
        //const std::string title = "HeliGame1989";
        const std::string author = "By Lehr Jackson";
        const std::string startText = "Press Enter to Start";

        float fontTitlePosX = m_fontPos.x;
        float fontTitlePosY = m_fontPos.y / 2.f;
        DirectX::SimpleMath::Vector2 titlePos(fontTitlePosX, fontTitlePosY);
        float fontAuthorPosX = m_fontPos.x;
        float fontAuthorPosY = m_fontPos.y;
        DirectX::SimpleMath::Vector2 authorPos(fontAuthorPosX, fontAuthorPosY);
        DirectX::SimpleMath::Vector2 startTextPos(m_fontPos.x, m_fontPos.y + fontTitlePosY);

        DirectX::SimpleMath::Vector2 authorOrigin = m_font->MeasureString(author.c_str()) / 2.f;
        DirectX::SimpleMath::Vector2 startTextOrigin = m_font->MeasureString(startText.c_str()) / 2.f;

        DirectX::XMVECTORF32 fadeColor2 = DirectX::Colors::White;
        if (timeStamp < fadeInEnd3)  // fade in
        {
            float colorIntensity = (static_cast<float>(timeStamp) - fadeInStart3) / fadeDuration;
            fadeColor.f[0] = colorIntensity;
            fadeColor.f[1] = colorIntensity;
            fadeColor.f[2] = colorIntensity;
            m_font->DrawString(m_spriteBatch.get(), author.c_str(), authorPos, fadeColor, 0.f, authorOrigin);
            m_font->DrawString(m_spriteBatch.get(), startText.c_str(), startTextPos, fadeColor, 0.f, startTextOrigin);
        }
        else if (timeStamp > fadeOutStart3) // fade out
        {
            float colorIntensity = (fadeOutEnd3 - static_cast<float>(timeStamp)) / (fadeDuration);
            colorIntensity = 0.0;
            fadeColor.f[0] = colorIntensity;
            fadeColor.f[1] = colorIntensity;
            fadeColor.f[2] = colorIntensity;
        }
        else // display at full intesity
        {
            float colorIntensity = (fadeOutStart3 - static_cast<float>(timeStamp)) / (fadeDuration);
            colorIntensity = 1.0;
            fadeColor.f[0] = colorIntensity;
            fadeColor.f[1] = colorIntensity;
            fadeColor.f[2] = colorIntensity;

            m_font->DrawString(m_spriteBatch.get(), author.c_str(), authorPos, fadeColor, 0.f, authorOrigin);
            m_font->DrawString(m_spriteBatch.get(), startText.c_str(), startTextPos, fadeColor, 0.f, startTextOrigin);
        }
    }

    if (timeStamp > fadeOutEnd2 + logoDisplayGap)
    {
        //AudioPlayMusic(XACT_WAVEBANK_AUDIOBANK::XACT_WAVEBANK_AUDIOBANK_MUSIC01);
        //m_currentGameState = GameState::GAMESTATE_STARTSCREEN;
    }
}

void Game::DrawWorld()
{
    for (unsigned int i = 0; i < m_worldGrid.size(); ++i)
    {
        m_batch2->DrawLine(m_worldGrid[i].first, m_worldGrid[i].second);
    }

    DirectX::XMVECTORF32 gridColor = DirectX::Colors::Red;
    DirectX::XMVECTORF32 gridColor1 = DirectX::Colors::LawnGreen;
    DirectX::XMVECTORF32 gridColor2 = DirectX::Colors::White;

    DirectX::SimpleMath::Vector3 p0 = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 p4(1.0, 0.0, 0.0);
    DirectX::SimpleMath::Vector3 p5(0.0, 0.0, 1.0);
    DirectX::SimpleMath::Vector3 p3(0.0, 1.0, 0.0);

    VertexPositionNormalColor v0(p0, DirectX::SimpleMath::Vector3::UnitY, gridColor);
    VertexPositionNormalColor v1(p4, DirectX::SimpleMath::Vector3::UnitY, gridColor);
    VertexPositionNormalColor v2(p5, DirectX::SimpleMath::Vector3::UnitY, gridColor);
    VertexPositionNormalColor v3(p3, -DirectX::SimpleMath::Vector3::UnitX, gridColor);

    VertexPositionNormalColor v4(p5, -DirectX::SimpleMath::Vector3::UnitX, gridColor);
    VertexPositionNormalColor v5(p0, -DirectX::SimpleMath::Vector3::UnitX, gridColor);
    VertexPositionNormalColor v6(p3, -DirectX::SimpleMath::Vector3::UnitX, gridColor);

    //m_batch2->DrawTriangle(v0, v1, v2);
    //m_batch2->DrawTriangle(v5, v4, v3);

    DirectX::SimpleMath::Vector3 nw(2.0, 0.0, -2.0);
    DirectX::SimpleMath::Vector3 ne(2.0, 0.0, 2.0);
    DirectX::SimpleMath::Vector3 se(-2.0, 0.0, 2.0);
    DirectX::SimpleMath::Vector3 sw(-2.0, 0.0, -2.0);
    DirectX::XMVECTORF32 planeColor = DirectX::Colors::White;
    DirectX::SimpleMath::Vector3 planeNorm = DirectX::SimpleMath::Vector3::UnitY;

    VertexPositionNormalColor vNw(nw, planeNorm, planeColor);
    VertexPositionNormalColor vNe(ne, planeNorm, planeColor);
    VertexPositionNormalColor vSe(se, planeNorm, planeColor);
    VertexPositionNormalColor vSw(sw, planeNorm, planeColor);
    //m_batch2->DrawQuad(vNw, vNe, vSe, vSw);
}

void Game::DrawWorldCube()
{
    DirectX::XMVECTORF32 planeColor0 = DirectX::Colors::White;
    DirectX::XMVECTORF32 planeColor1 = DirectX::Colors::White;
    DirectX::XMVECTORF32 planeColor2 = DirectX::Colors::Red;
    const float halfLength = 2.0;

    DirectX::SimpleMath::Vector3 topFrontLeft(-halfLength, halfLength, -halfLength);
    DirectX::SimpleMath::Vector3 topFrontRight(-halfLength, halfLength, halfLength);
    DirectX::SimpleMath::Vector3 topBackLeft(halfLength, halfLength, -halfLength);
    DirectX::SimpleMath::Vector3 topBackRight(halfLength, halfLength, halfLength);
    DirectX::SimpleMath::Vector3 bottomFrontLeft(-halfLength, -halfLength, -halfLength);
    DirectX::SimpleMath::Vector3 bottomFrontRight(-halfLength, -halfLength, halfLength);
    DirectX::SimpleMath::Vector3 bottomBackLeft(halfLength, -halfLength, -halfLength);
    DirectX::SimpleMath::Vector3 bottomBackRight(halfLength, -halfLength, halfLength);

    DirectX::SimpleMath::Vector3 normX = DirectX::SimpleMath::Vector3::UnitX;
    DirectX::SimpleMath::Vector3 normY = DirectX::SimpleMath::Vector3::UnitY;
    DirectX::SimpleMath::Vector3 normZ = DirectX::SimpleMath::Vector3::UnitZ;

    DirectX::SimpleMath::Vector3 testNorm = DirectX::SimpleMath::Vector3::UnitX;

    // Top
    VertexPositionNormalColor vTopFrontLeft(topFrontLeft, -normY, planeColor1);
    VertexPositionNormalColor vTopFrontRight(topFrontRight, -normY, planeColor1);
    VertexPositionNormalColor vTopBackLeft(topBackLeft, -normY, planeColor1);
    VertexPositionNormalColor vTopBackRight(topBackRight, -normY, planeColor1);

    // Bottom
    VertexPositionNormalColor vBottomFrontLeft(bottomFrontLeft, normY, planeColor1);
    VertexPositionNormalColor vBottomFrontRight(bottomFrontRight, normY, planeColor1);
    VertexPositionNormalColor vBottomBackLeft(bottomBackLeft, normY, planeColor1);
    VertexPositionNormalColor vBottomBackRight(bottomBackRight, normY, planeColor1);

    // Left Side
    VertexPositionNormalColor vLeftSideFrontBottom(bottomFrontLeft, normX, planeColor1);
    VertexPositionNormalColor vLeftSideBackBottom(bottomBackLeft, normX, planeColor1);
    VertexPositionNormalColor vLeftSideBackTop(topBackLeft, normX, planeColor1);
    VertexPositionNormalColor vLeftSideFrontTop(topFrontLeft, normX, planeColor1);

    // Right Side
    VertexPositionNormalColor vRightSideFrontBottom(bottomFrontRight, -normZ, planeColor1);
    VertexPositionNormalColor vRightSideBackBottom(bottomBackRight, -normZ, planeColor1);
    VertexPositionNormalColor vRightSideBackTop(topBackRight, -normZ, planeColor1);
    VertexPositionNormalColor vRightSideFrontTop(topFrontRight, -normZ, planeColor1);

    // Back
    VertexPositionNormalColor vBackLeftBottom(bottomBackLeft, -normZ, planeColor1);
    VertexPositionNormalColor vBackRightBottom(bottomBackRight, -normZ, planeColor1);
    VertexPositionNormalColor vBackRightTop(topBackRight, -normZ, planeColor1);
    VertexPositionNormalColor vBackLeftTop(topBackLeft, -normZ, planeColor1);

    // Front
    VertexPositionNormalColor vFrontLeftBottom(bottomFrontLeft, normX, planeColor1);
    VertexPositionNormalColor vFrontRightBottom(bottomFrontRight, normX, planeColor1);
    VertexPositionNormalColor vFrontRightTop(topFrontRight, normX, planeColor1);
    VertexPositionNormalColor vFrontLeftTop(topFrontLeft, normX, planeColor1);

    m_batch2->DrawQuad(vTopFrontLeft, vTopFrontRight, vTopBackRight, vTopBackLeft);
    m_batch2->DrawQuad(vBottomFrontLeft, vBottomFrontRight, vBottomBackRight, vBottomBackLeft);
    m_batch2->DrawQuad(vLeftSideFrontBottom, vLeftSideBackBottom, vLeftSideBackTop, vLeftSideFrontTop);
    m_batch2->DrawQuad(vRightSideFrontBottom, vRightSideBackBottom, vRightSideBackTop, vRightSideFrontTop);
    m_batch2->DrawQuad(vBackLeftBottom, vBackRightBottom, vBackRightTop, vBackLeftTop);
    m_batch2->DrawQuad(vFrontLeftBottom, vFrontRightBottom, vFrontRightTop, vFrontLeftTop);
}

void Game::DrawWorldCubeTextured()
{
    DirectX::XMVECTORF32 planeColor0 = DirectX::Colors::White;
    DirectX::XMVECTORF32 planeColor1 = DirectX::Colors::White;
    DirectX::XMVECTORF32 planeColor2 = DirectX::Colors::Red;
    const float halfLength = 2.0;

    DirectX::SimpleMath::Vector3 topFrontLeft(-halfLength, halfLength, -halfLength);
    DirectX::SimpleMath::Vector3 topFrontRight(-halfLength, halfLength, halfLength);
    DirectX::SimpleMath::Vector3 topBackLeft(halfLength, halfLength, -halfLength);
    DirectX::SimpleMath::Vector3 topBackRight(halfLength, halfLength, halfLength);
    DirectX::SimpleMath::Vector3 bottomFrontLeft(-halfLength, -halfLength, -halfLength);
    DirectX::SimpleMath::Vector3 bottomFrontRight(-halfLength, -halfLength, halfLength);
    DirectX::SimpleMath::Vector3 bottomBackLeft(halfLength, -halfLength, -halfLength);
    DirectX::SimpleMath::Vector3 bottomBackRight(halfLength, -halfLength, halfLength);

    DirectX::SimpleMath::Vector3 normX = DirectX::SimpleMath::Vector3::UnitX;
    DirectX::SimpleMath::Vector3 normY = DirectX::SimpleMath::Vector3::UnitY;
    DirectX::SimpleMath::Vector3 normZ = DirectX::SimpleMath::Vector3::UnitZ;

    DirectX::SimpleMath::Vector3 testNorm = DirectX::SimpleMath::Vector3::UnitY;
    normX = testNorm;
    normY = testNorm;
    normZ = testNorm;

    // Top
    VertexPositionNormalColorTexture vTopFrontLeft(topFrontLeft, -normY, planeColor1, DirectX::SimpleMath::Vector2(0, 0));
    VertexPositionNormalColorTexture vTopFrontRight(topFrontRight, -normY, planeColor1, DirectX::SimpleMath::Vector2(1, 0));
    VertexPositionNormalColorTexture vTopBackLeft(topBackLeft, -normY, planeColor1, DirectX::SimpleMath::Vector2(0, 1));
    VertexPositionNormalColorTexture vTopBackRight(topBackRight, -normY, planeColor1, DirectX::SimpleMath::Vector2(1, 1));

    // Bottom
    VertexPositionNormalColorTexture vBottomFrontLeft(bottomFrontLeft, -normY, planeColor1, DirectX::SimpleMath::Vector2(0, 1));
    VertexPositionNormalColorTexture vBottomFrontRight(bottomFrontRight, -normY, planeColor1, DirectX::SimpleMath::Vector2(1, 1));
    VertexPositionNormalColorTexture vBottomBackLeft(bottomBackLeft, -normY, planeColor1, DirectX::SimpleMath::Vector2(0, 0));
    VertexPositionNormalColorTexture vBottomBackRight(bottomBackRight, -normY, planeColor1, DirectX::SimpleMath::Vector2(1, 0));

    // Left Side
    VertexPositionNormalColorTexture vLeftSideFrontBottom(bottomFrontLeft, -normZ, planeColor1, DirectX::SimpleMath::Vector2(0, 1));
    VertexPositionNormalColorTexture vLeftSideBackBottom(bottomBackLeft, -normZ, planeColor1, DirectX::SimpleMath::Vector2(1, 1));
    VertexPositionNormalColorTexture vLeftSideBackTop(topBackLeft, -normZ, planeColor1, DirectX::SimpleMath::Vector2(1, 0));
    VertexPositionNormalColorTexture vLeftSideFrontTop(topFrontLeft, -normZ, planeColor1, DirectX::SimpleMath::Vector2(0, 0));

    // Right Side
    VertexPositionNormalColorTexture vRightSideFrontBottom(bottomFrontRight, -normZ, planeColor1, DirectX::SimpleMath::Vector2(1, 1));
    VertexPositionNormalColorTexture vRightSideBackBottom(bottomBackRight, -normZ, planeColor1, DirectX::SimpleMath::Vector2(0, 1));
    VertexPositionNormalColorTexture vRightSideBackTop(topBackRight, -normZ, planeColor1, DirectX::SimpleMath::Vector2(0, 0));
    VertexPositionNormalColorTexture vRightSideFrontTop(topFrontRight, -normZ, planeColor1, DirectX::SimpleMath::Vector2(1, 0));

    // Back
    VertexPositionNormalColorTexture vBackLeftBottom(bottomBackLeft, -normX, planeColor1, DirectX::SimpleMath::Vector2(0, 1));
    VertexPositionNormalColorTexture vBackRightBottom(bottomBackRight, -normX, planeColor1, DirectX::SimpleMath::Vector2(1, 1));
    VertexPositionNormalColorTexture vBackRightTop(topBackRight, -normX, planeColor1, DirectX::SimpleMath::Vector2(1, 0));
    VertexPositionNormalColorTexture vBackLeftTop(topBackLeft, -normX, planeColor1, DirectX::SimpleMath::Vector2(0, 0));

    // Front
    VertexPositionNormalColorTexture vFrontLeftBottom(bottomFrontLeft, -normX, planeColor1, DirectX::SimpleMath::Vector2(1, 1));
    VertexPositionNormalColorTexture vFrontRightBottom(bottomFrontRight, -normX, planeColor1, DirectX::SimpleMath::Vector2(0, 1));
    VertexPositionNormalColorTexture vFrontRightTop(topFrontRight, -normX, planeColor1, DirectX::SimpleMath::Vector2(0, 0));
    VertexPositionNormalColorTexture vFrontLeftTop(topFrontLeft, -normX, planeColor1, DirectX::SimpleMath::Vector2(1, 0));

    m_batch->DrawQuad(vTopFrontLeft, vTopFrontRight, vTopBackRight, vTopBackLeft);
    m_batch->DrawQuad(vBottomFrontLeft, vBottomFrontRight, vBottomBackRight, vBottomBackLeft);
    m_batch->DrawQuad(vLeftSideFrontBottom, vLeftSideBackBottom, vLeftSideBackTop, vLeftSideFrontTop);
    m_batch->DrawQuad(vRightSideFrontBottom, vRightSideBackBottom, vRightSideBackTop, vRightSideFrontTop);
    m_batch->DrawQuad(vBackLeftBottom, vBackRightBottom, vBackRightTop, vBackLeftTop);
    m_batch->DrawQuad(vFrontLeftBottom, vFrontRightBottom, vFrontRightTop, vFrontLeftTop);
}

// Properties
void Game::GetDefaultSize(int& width, int& height) const noexcept
{
    // TODO: Change to desired default window size (note minimum size is 320x200).
    //width = 1600;
    //height = 900;
    width = m_outputWidthDefault;
    height = m_outputHeightDefault;
}

// Initialize the Direct3D resources required to run.
void Game::Initialize(HWND window, int width, int height)
{
    m_window = window;
    m_outputWidth = std::max(width, 1);
    m_outputHeight = std::max(height, 1);

    CreateDevice();

    CreateResources();

    // TODO: Change the timer settings if you want something other than the default variable timestep mode.
    // e.g. for 60 FPS fixed timestep update logic, call:
    /*
    m_timer.SetFixedTimeStep(true);
    m_timer.SetTargetElapsedSeconds(1.0 / 60);
    */

    // WLJ add for mouse and keybord interface
    m_keyboard = std::make_unique<DirectX::Keyboard>();
    m_mouse = std::make_unique<Mouse>();
    m_mouse->SetWindow(window);

    // Audio
    AUDIO_ENGINE_FLAGS eflags = AudioEngine_Default;
#ifdef DEBUG  //#ifdef _DEBUG
    eflags |= AudioEngine_Debug;
#endif
    m_audioEngine = std::make_unique<AudioEngine>(eflags);
    m_retryAudio = false;
    m_audioBank = std::make_unique<WaveBank>(m_audioEngine.get(), L"Art/Audio/audioBank.xwb");

    // height map intit
    bool result;
    bool isInitSuccessTrue = true;
    
    result = InitializeTerrainArray();
    if (!result)
    {
        isInitSuccessTrue = false;
    }
    /*
    result = InitializeTerrainArray2();
    if (!result)
    {
        isInitSuccessTrue = false;
    }
    */
    m_terrainStartScreen.environType = EnvironmentType::ENIVRONMENTTYPE_STARTSCREEN;
    result = InitializeTerrainArrayStartScreen(m_terrainStartScreen);
    if (!result)
    {
        isInitSuccessTrue = false;
    }
    m_terrainGamePlay.environType = EnvironmentType::ENVIRONMENTTYPE_GAMEPLAY;
    result = InitializeTerrainArrayNew(m_terrainGamePlay);
    if (!result)
    {
        isInitSuccessTrue = false;
    }
    if (!isInitSuccessTrue)
    {
        // add initialization failure testing  here;
        int errorBreak = 0;
        errorBreak++;
    }
    m_vehicle->InitializeVehicle(m_d3dContext);
    m_vehicle->SetDebugData(m_debugData);
    //m_vehicle->InitializeVehicle(m_d3dContext, m_effect2, m_inputLayout);
    // testing new terrain map
    m_terrainVector.clear();

    m_gamePad = std::make_unique<GamePad>();
}

// Testing Terrain Vertex
bool Game::InitializeTerrainArray()
{
    std::vector<DirectX::VertexPositionColor> vertexPC = m_environment->GetTerrainColorVertex();

    m_terrainVertexCount = vertexPC.size();
    m_terrainVertexArray = new DirectX::VertexPositionColor[m_terrainVertexCount];
    m_terrainVertexArrayBase = new DirectX::VertexPositionColor[m_terrainVertexCount];

    DirectX::XMFLOAT4 lineColor(.486274540f, .988235354f, 0.0, 1.0);
    DirectX::XMFLOAT4 baseColor(0.0, 0.0, 0.0, 1.0);
    DirectX::XMFLOAT4 baseColor2(0.0, 0.0, 0.0, 1.0);

    DirectX::XMFLOAT4 sandColor1(0.956862807f, 0.643137276f, 0.376470625f, 1.0);
    DirectX::XMFLOAT4 sandColor2(0.960784376f, 0.960784376f, 0.862745166f, 1.0);
    DirectX::XMFLOAT4 greenColor1 = DirectX::XMFLOAT4(0.0, 0.501960814f, 0.0, 1.0);
    DirectX::XMFLOAT4 greenColor2 = DirectX::XMFLOAT4(0.486274540f, 0.988235354f, 0.0, 1.0);

    DirectX::XMFLOAT4 grassColor1 = DirectX::XMFLOAT4(0.133333340f, 0.545098066f, 0.133333340f, 1.0);
    DirectX::XMFLOAT4 grassColor2 = DirectX::XMFLOAT4(0.000000000f, 0.392156899f, 0.0, 1.0);

    DirectX::XMFLOAT4 testWhite = DirectX::XMFLOAT4(1.0, 1.0, 1.0, 1.0);

    for (int i = 0; i < m_terrainVertexCount; ++i)
    {
        m_terrainVertexArray[i].position = vertexPC[i].position;
        m_terrainVertexArray[i].position.y += 0.0005; // Move up the lines so they don't get hidden by terrain
        m_terrainVertexArray[i].color = lineColor;
        m_terrainVertexArrayBase[i].position = vertexPC[i].position;

        if (i % 2 == 0)
        {
            m_terrainVertexArrayBase[i].color = baseColor;
        }
        else
        {
            m_terrainVertexArrayBase[i].color = baseColor2;
        }
    }

    return true;
}

bool Game::InitializeTerrainArray2()
{
    std::vector<DirectX::VertexPositionNormalColor> vertexPC = m_environment->GetTerrainPositionNormalColorVertex(EnvironmentType::ENVIRONMENTTYPE_GAMEPLAY);
    m_terrainVector2.clear();
    m_terrainVector2 = vertexPC;

    m_terrainVertexCount2 = vertexPC.size();
    m_terrainVertexArray2 = new DirectX::VertexPositionNormalColor[m_terrainVertexCount2];
    m_terrainVertexArrayBase2 = new DirectX::VertexPositionNormalColor[m_terrainVertexCount2];

    DirectX::XMFLOAT4 lineColor(.486274540f, .988235354f, 0.0, 1.0);
    DirectX::XMFLOAT4 baseColor(0.0, 0.0, 0.0, 1.0);
    DirectX::XMFLOAT4 baseColor2(1.0, 1.0, 1.0, 1.0);

    DirectX::XMFLOAT4 sandColor1(0.956862807f, 0.643137276f, 0.376470625f, 1.0);
    DirectX::XMFLOAT4 sandColor2(0.960784376f, 0.960784376f, 0.862745166f, 1.0);
    DirectX::XMFLOAT4 greenColor1 = DirectX::XMFLOAT4(0.0, 0.501960814f, 0.0, 1.0);
    DirectX::XMFLOAT4 greenColor2 = DirectX::XMFLOAT4(0.486274540f, 0.988235354f, 0.0, 1.0);

    DirectX::XMFLOAT4 grassColor1 = DirectX::XMFLOAT4(0.133333340f, 0.545098066f, 0.133333340f, 1.0);
    DirectX::XMFLOAT4 grassColor2 = DirectX::XMFLOAT4(0.000000000f, 0.392156899f, 0.0, 1.0);
    DirectX::XMFLOAT4 testRed = DirectX::XMFLOAT4(1.000000000f, 0.000000000f, 0.0, 1.0);
    DirectX::XMFLOAT4 testBlue = DirectX::XMFLOAT4(0.000000000f, 0.000000000f, 1.0, 1.0);
    DirectX::XMFLOAT4 testGray = DirectX::XMFLOAT4(0.662745118f, 0.662745118f, 0.662745118f, 1.000000000f);
    DirectX::XMFLOAT4 testWhite = DirectX::XMFLOAT4(1.0, 1.0, 1.0, 1.0);

    for (int i = 0; i < m_terrainVertexCount2; ++i)
    {
        DirectX::SimpleMath::Vector3 flipNormal = vertexPC[i].normal;
        m_terrainVertexArray2[i].position = vertexPC[i].position;
        //m_terrainVertexArray2[i].normal = vertexPC[i].normal;
        // Flip normals around for lighting;
        m_terrainVertexArray2[i].normal.x = flipNormal.x;
        m_terrainVertexArray2[i].normal.y = flipNormal.y;
        m_terrainVertexArray2[i].normal.z = flipNormal.z;
        m_terrainVertexArray2[i].color = lineColor;

        m_terrainVertexArrayBase2[i].position = vertexPC[i].position;
        //m_terrainVertexArrayBase2[i].normal = vertexPC[i].normal;
        // flip normals around for lighting
        m_terrainVertexArrayBase2[i].normal.x = flipNormal.x;
        m_terrainVertexArrayBase2[i].normal.y = flipNormal.y;
        m_terrainVertexArrayBase2[i].normal.z = flipNormal.z;
        if (i % 2 == 0)
        {
            m_terrainVertexArrayBase2[i].color = baseColor;
        }
        else
        {
            m_terrainVertexArrayBase2[i].color = baseColor;
        }

        if (i == i)
            //if (i < 96)
            //if (i % 96 == 0)
        {
            if ((i + 5) % 6 == 0)
            {
                m_terrainVertexArrayBase2[i].color = baseColor;
            }
            if ((i + 2) % 6 == 0)
            {
                m_terrainVertexArrayBase2[i].color = testWhite;
            }
            if (i % 6 == 0)
            {
                m_terrainVertexArrayBase2[i].color = testWhite;
            }
        }
    }

    std::vector<DirectX::SimpleMath::Vector3> testNorms;
    testNorms.resize(m_terrainVertexCount2);
    std::vector<DirectX::SimpleMath::Vector3> testNorms2;
    testNorms2.resize(m_terrainVertexCount2);
    for (int i = 0; i < m_terrainVertexCount2; ++i)
    {
        //m_terrainVertexArray2[i].normal = - DirectX::SimpleMath::Vector3::UnitY;
        m_terrainVertexArray2[i].position.y += 0.03;
        //m_terrainVertexArrayBase2[i].normal = - DirectX::SimpleMath::Vector3::UnitY;
        testNorms[i] = m_terrainVertexArray2[i].normal;
        testNorms2[i] = m_terrainVertexArrayBase2[i].normal;
    }

    return true;
}

void Game::TerrainDimmer()
{
    //const float dimmerDuration = 2.0f;
    //const float dimmerStart = 0.4f;
    const float dimmerDuration = 0.5f;
    const float dimmerStart = 0.1f;
    const float dimmerEnd = dimmerStart + dimmerDuration;
    const float dimmerTimer = m_loadScreenTimerStart;
    DirectX::XMFLOAT4 updateColor(0.0f, 0.0f, 0.0f, 1.0f);
    float dimmerPercentage; 
    if (dimmerTimer < dimmerStart)
    {
        dimmerPercentage = 0.0f;
    }
    else if (dimmerTimer < dimmerEnd)
    {
        dimmerPercentage = (dimmerTimer - dimmerStart) / (dimmerEnd - dimmerStart);
    }
    else
    {
        dimmerPercentage = 1.0f;
    }

    updateColor.y = m_defaultGameTerrainColor.y * dimmerPercentage;


    //m_terrainGamePlay.terrainVertexArrayBase[i].normal.y = dimmerPercentage;

    DirectX::XMFLOAT4 testColor = m_terrainGamePlay.terrainVertexArrayBase[2969].color;
    updateColor = testColor;
    updateColor.y = testColor.y * dimmerPercentage;
    int index = 2970;
    int range = index + 6;
    for (int i = index; i < range; ++i)
    {

        m_terrainGamePlay.terrainVertexArrayBase[i].color = updateColor;
        m_terrainGamePlay.terrainVertexArrayBase[i].normal.y = dimmerPercentage;
    }

    /*
    float dimmerVal = m_loadScreenTimerStart * 0.01;
    m_testTimer1 = m_loadScreenTimerStart;
    int total = 0;
    if (dimmerVal <= 1.0f)
    {
        m_testTimer2 = m_loadScreenTimerStart;
        const float colorVal = dimmerVal * 0.1f;
        DirectX::XMFLOAT4 updateColor(colorVal, colorVal, colorVal, 1.0f);
        for (int i = 0; i < m_terrainGamePlay.terrainVertexCount; ++i)
        {
            //m_terrainGamePlay.terrainVertexArrayBase[i].color = updateColor;
            total = i;
        }

        DirectX::XMFLOAT4 testColor(1.0f, 0.0f, 0.0f, 1.0f);
        int index = 2970;
        int range = index + 6;
        for (int i = index; i < range; ++i)
        {

            m_terrainGamePlay.terrainVertexArrayBase[i].color = updateColor;
            m_terrainGamePlay.terrainVertexArrayBase[i].normal.y = dimmerVal;
        }
    }
    */
}

bool Game::InitializeTerrainArrayNew(Terrain& aTerrain)
{
    std::vector<DirectX::VertexPositionNormalColor> vertexPC = m_environment->GetTerrainPositionNormalColorVertex(aTerrain.environType);
    m_terrainVector2.clear();
    m_terrainVector2 = vertexPC;
    aTerrain.terrainVertexCount = vertexPC.size();
    aTerrain.terrainVertexArray = new DirectX::VertexPositionNormalColor[aTerrain.terrainVertexCount];
    aTerrain.terrainVertexArrayBase = new DirectX::VertexPositionNormalColor[aTerrain.terrainVertexCount];

    DirectX::XMFLOAT4 lineColor(.486274540f, .988235354f, 0.0, 1.0);
    DirectX::XMFLOAT4 baseColor(0.01, 0.01, 0.01, 1.0);

    DirectX::XMFLOAT4 baseColor2(1.0, 1.0, 1.0, 1.0);
    //baseColor = baseColor2;
    m_testColor = baseColor;
    DirectX::XMFLOAT4 sandColor1(0.956862807f, 0.643137276f, 0.376470625f, 1.0);
    DirectX::XMFLOAT4 sandColor2(0.960784376f, 0.960784376f, 0.862745166f, 1.0);
    DirectX::XMFLOAT4 greenColor1 = DirectX::XMFLOAT4(0.0, 0.501960814f, 0.0, 1.0);
    DirectX::XMFLOAT4 greenColor2 = DirectX::XMFLOAT4(0.486274540f, 0.988235354f, 0.0, 1.0);

    DirectX::XMFLOAT4 grassColor1 = DirectX::XMFLOAT4(0.133333340f, 0.545098066f, 0.133333340f, 1.0);
    DirectX::XMFLOAT4 grassColor2 = DirectX::XMFLOAT4(0.000000000f, 0.392156899f, 0.0, 1.0);
    DirectX::XMFLOAT4 testRed = DirectX::XMFLOAT4(1.000000000f, 0.000000000f, 0.0, 1.0);
    DirectX::XMFLOAT4 testBlue = DirectX::XMFLOAT4(0.000000000f, 0.000000000f, 1.0, 1.0);
    DirectX::XMFLOAT4 testGray = DirectX::XMFLOAT4(0.662745118f, 0.662745118f, 0.662745118f, 1.000000000f);
    DirectX::XMFLOAT4 testWhite = DirectX::XMFLOAT4(1.0, 1.0, 1.0, 1.0);
    //baseColor = sandColor1;
    testWhite = baseColor;

    float maxY = 0.0f;

    if (aTerrain.environType == EnvironmentType::ENIVRONMENTTYPE_STARTSCREEN)
    {
        baseColor = DirectX::XMFLOAT4(0.01, 0.01, 0.01, 1.0);
        testWhite = baseColor;

    }
    if (aTerrain.environType == EnvironmentType::ENVIRONMENTTYPE_GAMEPLAY)
    {
        baseColor = DirectX::XMFLOAT4(0.000000000f, 0.292156899f, 0.000000000f, 1.000000000f);
        testWhite = baseColor;
    }
    for (int i = 0; i < aTerrain.terrainVertexCount; ++i)
    {
        DirectX::SimpleMath::Vector3 flipNormal = vertexPC[i].normal;
        aTerrain.terrainVertexArray[i].position = vertexPC[i].position;
        //m_terrainVertexArray2[i].normal = vertexPC[i].normal;
        // Flip normals around for lighting;
        aTerrain.terrainVertexArray[i].normal.x = flipNormal.x;
        aTerrain.terrainVertexArray[i].normal.y = flipNormal.y;
        aTerrain.terrainVertexArray[i].normal.z = flipNormal.z;

        int testRandom = rand() % 1000;
        float testFloat = testRandom * 0.000001;

        //DirectX::XMFLOAT4 testColor(0.0f, 0.292156899f, 0.0f, 0.0f);
        //DirectX::XMFLOAT4 baseColor = m_defaultGameTerrainColor;
        DirectX::XMFLOAT4 baseColor(0.0f, 0.292156899f, 0.0f, 0.0f);
        //float colorVal = aTerrain.terrainVertexArray[i].position.y / 2224.16675f;
        float elevationPercentage = aTerrain.terrainVertexArray[i].position.y / m_gameTerrainMaxY;
        float colorVal = elevationPercentage;

        colorVal += testFloat;
        baseColor.x = colorVal;
        const float colorMax = 0.85f;
        if (baseColor.x > colorMax)
        {
            baseColor.x = colorMax;
        }
        baseColor.y = colorVal + 0.292156899f;
        if (baseColor.y > colorMax)
        {
            baseColor.y = colorMax;
        }
        baseColor.z = colorVal;
        if (baseColor.z > colorMax)
        {
            baseColor.z = colorMax;
        }
        //baseColor.w = 1.0f;
        //testColor= DirectX::XMFLOAT4(0.0f, 0.292156899f, 0.0f, 0.0f);
        DirectX::XMFLOAT4 lineColor = baseColor;
        //lineColor.y += 0.15f;

        if (elevationPercentage > 0.4f)
        {
            lineColor.z += 0.15f;
            lineColor.y -= 0.15f;
        }
        else
        {
            lineColor.y += 0.15f;
        }

        aTerrain.terrainVertexArray[i].color = lineColor;
        aTerrain.terrainVertexArrayBase[i].position = vertexPC[i].position;
        //m_terrainVertexArrayBase2[i].normal = vertexPC[i].normal;
        // flip normals around for lighting
        aTerrain.terrainVertexArrayBase[i].normal.x = flipNormal.x;
        aTerrain.terrainVertexArrayBase[i].normal.y = flipNormal.y;
        aTerrain.terrainVertexArrayBase[i].normal.z = flipNormal.z;

        //testColor = DirectX::XMFLOAT4(0.0f, 0.292156899f, 0.0f, 0.0f);
        if (i % 2 == 0)
        {
            aTerrain.terrainVertexArrayBase[i].color = baseColor;
        }
        else
        {
            aTerrain.terrainVertexArrayBase[i].color = baseColor;
        }
        /*
        if (i == i)
            //if (i < 96)
            //if (i % 96 == 0)
        {
            if ((i + 5) % 6 == 0)
            {
                aTerrain.terrainVertexArrayBase[i].color = baseColor;
            }
            if ((i + 2) % 6 == 0)
            {
                aTerrain.terrainVertexArrayBase[i].color = testWhite;
            }
            if (i % 6 == 0)
            {
                aTerrain.terrainVertexArrayBase[i].color = testWhite;
            }
        }
        */
        if (aTerrain.terrainVertexArray[i].position.y >= maxY)
        {
            maxY = aTerrain.terrainVertexArray[i].position.y;
        }
    }

    std::vector<DirectX::SimpleMath::Vector3> testNorms;
    testNorms.resize(aTerrain.terrainVertexCount);
    std::vector<DirectX::SimpleMath::Vector3> testNorms2;
    testNorms2.resize(aTerrain.terrainVertexCount);
    float gridLineOffSetY = 0.0f;
    if (aTerrain.environType == EnvironmentType::ENIVRONMENTTYPE_STARTSCREEN)
    {
        gridLineOffSetY = 0.003f;
    }
    else if (aTerrain.environType == EnvironmentType::ENVIRONMENTTYPE_GAMEPLAY)
    {
        gridLineOffSetY = 4.7f;
    }
    for (int i = 0; i < aTerrain.terrainVertexCount; ++i)
    {
        //aTerrain.terrainVertexArray2[i].normal = - DirectX::SimpleMath::Vector3::UnitY;
        aTerrain.terrainVertexArray[i].position.y += gridLineOffSetY;
        //aTerrain.terrainVertexArrayBase2[i].normal = - DirectX::SimpleMath::Vector3::UnitY;
        testNorms[i] = aTerrain.terrainVertexArray[i].normal;
        testNorms2[i] = aTerrain.terrainVertexArrayBase[i].normal;
    }

    return true;
}

bool Game::InitializeTerrainArrayStartScreen(Terrain& aTerrain)
{
    std::vector<DirectX::VertexPositionNormalColor> vertexPC = m_environment->GetTerrainPositionNormalColorVertex(aTerrain.environType);
    m_terrainVector2.clear();
    m_terrainVector2 = vertexPC;
    aTerrain.terrainVertexCount = vertexPC.size();
    aTerrain.terrainVertexArray = new DirectX::VertexPositionNormalColor[aTerrain.terrainVertexCount];
    aTerrain.terrainVertexArrayBase = new DirectX::VertexPositionNormalColor[aTerrain.terrainVertexCount];

    DirectX::XMFLOAT4 lineColor(.486274540f, .988235354f, 0.0, 1.0);
    DirectX::XMFLOAT4 baseColor(0.01, 0.01, 0.01, 1.0);
    //DirectX::XMFLOAT4 baseColor(0.0, 0.0, 0.0, 1.0);
    //DirectX::XMFLOAT4 baseColor(0.000000000f, 0.292156899f, 0.000000000f, 1.000000000f);

    DirectX::XMFLOAT4 baseColor2(1.0, 1.0, 1.0, 1.0);
    //baseColor = baseColor2;
    m_testColor = baseColor;
    DirectX::XMFLOAT4 sandColor1(0.956862807f, 0.643137276f, 0.376470625f, 1.0);
    DirectX::XMFLOAT4 sandColor2(0.960784376f, 0.960784376f, 0.862745166f, 1.0);
    DirectX::XMFLOAT4 greenColor1 = DirectX::XMFLOAT4(0.0, 0.501960814f, 0.0, 1.0);
    DirectX::XMFLOAT4 greenColor2 = DirectX::XMFLOAT4(0.486274540f, 0.988235354f, 0.0, 1.0);

    DirectX::XMFLOAT4 grassColor1 = DirectX::XMFLOAT4(0.133333340f, 0.545098066f, 0.133333340f, 1.0);
    DirectX::XMFLOAT4 grassColor2 = DirectX::XMFLOAT4(0.000000000f, 0.392156899f, 0.0, 1.0);
    DirectX::XMFLOAT4 testRed = DirectX::XMFLOAT4(1.000000000f, 0.000000000f, 0.0, 1.0);
    DirectX::XMFLOAT4 testBlue = DirectX::XMFLOAT4(0.000000000f, 0.000000000f, 1.0, 1.0);
    DirectX::XMFLOAT4 testGray = DirectX::XMFLOAT4(0.662745118f, 0.662745118f, 0.662745118f, 1.000000000f);
    DirectX::XMFLOAT4 testWhite = DirectX::XMFLOAT4(1.0, 1.0, 1.0, 1.0);
    //baseColor = sandColor1;
    testWhite = baseColor;

    testWhite = DirectX::XMFLOAT4(1.0, 1.0, 1.0, 1.0);
    baseColor = m_defaultStartTerrainColor;
    float maxY = 0.0f;
    for (int i = 0; i < aTerrain.terrainVertexCount; ++i)
    {
        DirectX::SimpleMath::Vector3 flipNormal = vertexPC[i].normal;
        aTerrain.terrainVertexArray[i].position = vertexPC[i].position;
        // Flip normals around for lighting;
        aTerrain.terrainVertexArray[i].normal.x = flipNormal.x;
        aTerrain.terrainVertexArray[i].normal.y = flipNormal.y;
        aTerrain.terrainVertexArray[i].normal.z = flipNormal.z;

        
        int testRandom = rand() % 1000;
        float testFloat = testRandom * 0.000001;

        DirectX::XMFLOAT4 testColor(0.0f, 0.0f, 0.0f, 0.0f);
        float colorVal = aTerrain.terrainVertexArray[i].position.y / (m_startTerrainMaxY + 0.15f);
        colorVal += testFloat;
        colorVal -= 0.05;
        if(colorVal < 0.0f)
        {
            colorVal = 0.0f;
        }
        testColor.x = colorVal;
        if (testColor.x > 1.0f)
        {
            testColor.x = 1.0f;
        }
        testColor.y = colorVal;
        if (testColor.y > 1.0f)
        {
            testColor.y = 1.0f;
        }
        testColor.z = colorVal;
        if (testColor.z > 1.0f)
        {
            testColor.z = 1.0f;
        }
        DirectX::XMFLOAT4 testColor2 = testColor;
        testColor2.y -= 0.15f;
        
        

        aTerrain.terrainVertexArray[i].color = lineColor;
        aTerrain.terrainVertexArrayBase[i].position = vertexPC[i].position;

        //m_terrainVertexArrayBase2[i].normal = vertexPC[i].normal;
        // flip normals around for lighting
        aTerrain.terrainVertexArrayBase[i].normal.x = flipNormal.x;
        aTerrain.terrainVertexArrayBase[i].normal.y = flipNormal.y;
        aTerrain.terrainVertexArrayBase[i].normal.z = flipNormal.z;

        //testColor = DirectX::XMFLOAT4(0.0f, 0.292156899f, 0.0f, 0.0f);
        if (i % 2 == 0)
        {
            aTerrain.terrainVertexArrayBase[i].color = testColor;
        }
        else
        {
            aTerrain.terrainVertexArrayBase[i].color = testColor;
        }

        if (aTerrain.terrainVertexArrayBase[i].position.y > (m_startTerrainMaxY - 0.1f))
        {
            aTerrain.terrainVertexArrayBase[i].color = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
        }
        /*
        //if (i == i)
        if (i < 96)
            //if (i % 96 == 0)
        {
            if ((i + 5) % 6 == 0)
            {
                aTerrain.terrainVertexArrayBase[i].color = baseColor;
            }
            if ((i + 2) % 6 == 0)
            {
                aTerrain.terrainVertexArrayBase[i].color = testWhite;
            }
            if (i % 6 == 0)
            {
                aTerrain.terrainVertexArrayBase[i].color = testWhite;
            }
        }
        */
        if (aTerrain.terrainVertexArray[i].position.y >= maxY)
        {
            maxY = aTerrain.terrainVertexArray[i].position.y;
        }
    }

    std::vector<DirectX::SimpleMath::Vector3> testNorms;
    testNorms.resize(aTerrain.terrainVertexCount);
    std::vector<DirectX::SimpleMath::Vector3> testNorms2;
    testNorms2.resize(aTerrain.terrainVertexCount);
    float gridLineOffSetY = 0.0f;
    if (aTerrain.environType == EnvironmentType::ENIVRONMENTTYPE_STARTSCREEN)
    {
        gridLineOffSetY = 0.003f;
    }
    else if (aTerrain.environType == EnvironmentType::ENVIRONMENTTYPE_GAMEPLAY)
    {
        gridLineOffSetY = 4.7f;
    }
    for (int i = 0; i < aTerrain.terrainVertexCount; ++i)
    {
        //aTerrain.terrainVertexArray2[i].normal = - DirectX::SimpleMath::Vector3::UnitY;
        aTerrain.terrainVertexArray[i].position.y += gridLineOffSetY;
        //aTerrain.terrainVertexArrayBase2[i].normal = - DirectX::SimpleMath::Vector3::UnitY;
        testNorms[i] = aTerrain.terrainVertexArray[i].normal;
        testNorms2[i] = aTerrain.terrainVertexArrayBase[i].normal;
    }

    return true;
}

void Game::InitializeWorldGrid()
{
    m_worldGrid.clear();

    const DirectX::SimpleMath::Vector3 gridNorm = DirectX::SimpleMath::Vector3::UnitY;
    const int hightLightIncrement = 10;

    const float maxX = 2000.0;
    const float maxY = 1.0;
    const float maxZ = 2000.0;
    const float minX = -2000.0;
    const float minY = -1.0;
    const float minZ = -2000.0;
    const float increment = 0.2;
    const float yOffset = 0.005;

    DirectX::SimpleMath::Vector3 p1 = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 p2 = DirectX::SimpleMath::Vector3::Zero;

    DirectX::XMVECTORF32 gridColor0 = DirectX::Colors::Red;
    DirectX::XMVECTORF32 gridColor1 = DirectX::Colors::LawnGreen;
    DirectX::XMVECTORF32 gridColor2 = DirectX::Colors::White;
    DirectX::XMVECTORF32 gridColor3 = DirectX::Colors::Gray;
    DirectX::XMVECTORF32 gridColor4 = DirectX::Colors::LightGray;
    DirectX::XMVECTORF32 gridColor5 = DirectX::Colors::Yellow;

    // positive z draws
    p1 = DirectX::SimpleMath::Vector3::Zero;
    p2 = DirectX::SimpleMath::Vector3::Zero;
    p1.y += yOffset;
    p2.y += yOffset;
    p1.x = minX;
    p2.x = maxX;
    VertexPositionNormalColor v1(p1, gridNorm, gridColor0);
    VertexPositionNormalColor v2(p2, gridNorm, gridColor0);
    m_worldGrid.push_back(std::pair(v1, v2));
    int i = 0;
    while (p1.z <= maxZ)
    {
        ++i;
        if (i % hightLightIncrement == 0)
        {
            v1 = VertexPositionNormalColor(DirectX::SimpleMath::Vector3(p1.x, p1.y, p1.z + increment), gridNorm, gridColor2);
            v2 = VertexPositionNormalColor(DirectX::SimpleMath::Vector3(p2.x, p2.y, p2.z + increment), gridNorm, gridColor2);
        }
        else
        {
            v1 = VertexPositionNormalColor(DirectX::SimpleMath::Vector3(p1.x, p1.y, p1.z + increment), gridNorm, gridColor1);
            v2 = VertexPositionNormalColor(DirectX::SimpleMath::Vector3(p2.x, p2.y, p2.z + increment), gridNorm, gridColor1);
        }

        m_worldGrid.push_back(std::pair(v1, v2));
        p1.z += increment;
        p2.z += increment;
    }

    // negative z draws
    p1.z = 0.0;
    p2.z = 0.0;
    p1.y = 0.0;
    p1.y = 0.0;
    p1.y += yOffset;
    p2.y += yOffset;
    i = 0;
    while (p1.z >= minZ)
    {
        ++i;
        if (i % hightLightIncrement == 0)
        {
            v1 = VertexPositionNormalColor(DirectX::SimpleMath::Vector3(p1.x, p1.y, p1.z - increment), gridNorm, gridColor2);
            v2 = VertexPositionNormalColor(DirectX::SimpleMath::Vector3(p2.x, p2.y, p2.z - increment), gridNorm, gridColor2);
        }
        else
        {
            v1 = VertexPositionNormalColor(DirectX::SimpleMath::Vector3(p1.x, p1.y, p1.z - increment), gridNorm, gridColor1);
            v2 = VertexPositionNormalColor(DirectX::SimpleMath::Vector3(p2.x, p2.y, p2.z - increment), gridNorm, gridColor1);
        }
        m_worldGrid.push_back(std::pair(v1, v2));
        p1.z -= increment;
        p2.z -= increment;
    }

    // positve x draws
    p1 = DirectX::SimpleMath::Vector3::Zero;
    p1.z = minZ;
    p2 = DirectX::SimpleMath::Vector3::Zero;
    p2.z = maxZ;
    p1.y += yOffset;
    p2.y += yOffset;
    v1 = DirectX::VertexPositionNormalColor(p1, gridNorm, gridColor0);
    v2 = DirectX::VertexPositionNormalColor(p2, gridNorm, gridColor0);
    m_worldGrid.push_back(std::pair(v1, v2));
    i = 0;
    while (p1.x <= maxX)
    {
        ++i;
        if (i % hightLightIncrement == 0)
        {
            v1 = VertexPositionNormalColor(DirectX::SimpleMath::Vector3(p1.x + increment, p1.y, p1.z), gridNorm, gridColor2);
            v2 = VertexPositionNormalColor(DirectX::SimpleMath::Vector3(p2.x + increment, p2.y, p2.z), gridNorm, gridColor2);
        }
        else
        {
            v1 = VertexPositionNormalColor(DirectX::SimpleMath::Vector3(p1.x + increment, p1.y, p1.z), gridNorm, gridColor1);
            v2 = VertexPositionNormalColor(DirectX::SimpleMath::Vector3(p2.x + increment, p2.y, p2.z), gridNorm, gridColor1);
        }

        m_worldGrid.push_back(std::pair(v1, v2));
        p1.x += increment;
        p2.x += increment;
    }

    // negative x draws
    p1.x = 0.0;
    p2.x = 0.0;
    p1.y = 0.0;
    p1.y = 0.0;
    p1.y += yOffset;
    p2.y += yOffset;
    i = 0;
    while (p1.x >= minX)
    {
        ++i;
        if (i % hightLightIncrement == 0)
        {
            v1 = VertexPositionNormalColor(DirectX::SimpleMath::Vector3(p1.x - increment, p1.y, p1.z), gridNorm, gridColor2);
            v2 = VertexPositionNormalColor(DirectX::SimpleMath::Vector3(p2.x - increment, p2.y, p2.z), gridNorm, gridColor2);
        }
        else
        {
            v1 = VertexPositionNormalColor(DirectX::SimpleMath::Vector3(p1.x - increment, p1.y, p1.z), gridNorm, gridColor1);
            v2 = VertexPositionNormalColor(DirectX::SimpleMath::Vector3(p2.x - increment, p2.y, p2.z), gridNorm, gridColor1);
        }
        m_worldGrid.push_back(std::pair(v1, v2));
        p1.x -= increment;
        p2.x -= increment;
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // positve y draws
    bool isYdrawOn = true;
    if (isYdrawOn == true) // Toggle y draw on and off
    {
        p1 = DirectX::SimpleMath::Vector3::Zero;
        p1.x = minX;
        p2 = DirectX::SimpleMath::Vector3::Zero;
        p2.x = maxX;
        v1 = DirectX::VertexPositionNormalColor(p1, gridNorm, gridColor0);
        v2 = DirectX::VertexPositionNormalColor(p2, gridNorm, gridColor0);
        i = 0;
        while (p1.y <= maxY)
        {
            ++i;
            if (i % hightLightIncrement == 0)
            {
                v1 = VertexPositionNormalColor(DirectX::SimpleMath::Vector3(p1.x, p1.y + increment, p1.z), gridNorm, gridColor2);
                v2 = VertexPositionNormalColor(DirectX::SimpleMath::Vector3(p2.x, p2.y + increment, p2.z), gridNorm, gridColor2);
            }
            else
            {
                v1 = VertexPositionNormalColor(DirectX::SimpleMath::Vector3(p1.x, p1.y + increment, p1.z), gridNorm, gridColor1);
                v2 = VertexPositionNormalColor(DirectX::SimpleMath::Vector3(p2.x, p2.y + increment, p2.z), gridNorm, gridColor1);
            }

            m_worldGrid.push_back(std::pair(v1, v2));
            p1.y += increment;
            p2.y += increment;
        }
    }
}

// Message handlers
void Game::OnActivated()
{
    // TODO: Game is becoming active window.
    m_kbStateTracker.Reset();
    m_gamePad->Resume();
    m_gamePadTracker.Reset();
}

void Game::OnDeactivated()
{
    // TODO: Game is becoming background window.
    m_gamePad->Suspend();
}

void Game::OnDeviceLost()
{
    // TODO: Add Direct3D resource cleanup here.
    m_raster.Reset(); // anti-aliased lines
    m_states.reset();
    m_effect.reset();
    m_effect2.reset();
    m_effect3.reset();
    m_batch.reset();
    m_batch2.reset();
    m_batch3.reset();
    m_skyShape.reset();
    m_normalMap.Reset();
    m_texture.Reset();
    m_specular.Reset();
    m_textureSky.Reset();
    m_normalMapTest.Reset();
    m_textureTest.Reset();
    m_specularTest.Reset();

    m_normalMapJI.Reset();
    m_specularJI.Reset();
    m_textureJI.Reset();
    m_normalMapBMW.Reset();
    m_normalMapBMW2.Reset();
    m_specularBMW.Reset();
    m_textureBMW.Reset();
    m_normalMapAutoGame.Reset();
    m_specularAutoGame.Reset();
    m_textureAutoGame.Reset();
    m_normalMapTeaser.Reset();
    m_specularTeaser.Reset();
    m_textureTeaser.Reset();

    m_inputLayout.Reset();
    m_font.reset();
    m_titleFont.reset();
    m_bitwiseFont.reset();
    m_spriteBatch.reset();
    m_kbStateTracker.Reset();

    // Charcters
    m_pacSprite.reset();
    m_pacTexture.Reset();

    m_character.reset();
    m_characterTexture.Reset();
    m_character0.reset();
    m_character0Texture.Reset();
    m_character1.reset();
    m_character1Texture.Reset();
    m_character2.reset();
    m_character2Texture.Reset();
    m_characterBackgroundTexture.Reset();

    // Environment Select
    m_environSelectCalmTexture.Reset();
    m_environSelectBreezyTexture.Reset();
    m_environSelectAlienTexture.Reset();

    // Intro Screen
    m_bmwLogoTexture.Reset();
    m_jiLogoTexture.Reset();

    m_backgroundTex.Reset();

    m_depthStencilView.Reset();
    m_renderTargetView.Reset();
    m_swapChain.Reset();
    m_d3dContext.Reset();
    m_d3dDevice.Reset();

    CreateDevice();
    CreateResources();
}

void Game::OnSuspending()
{
    // TODO: Game is being power-suspended (or minimized).
    m_audioEngine->Suspend();
    m_gamePad->Suspend();
}

void Game::OnResuming()
{
    // TODO: Game is being power-resumed (or returning from minimize).
    m_timer.ResetElapsedTime();
    m_kbStateTracker.Reset();
    m_audioEngine->Resume();
    m_gamePadTracker.Reset();
}

void Game::OnWindowSizeChanged(int width, int height)
{
    m_outputWidth = std::max(width, 1);
    m_outputHeight = std::max(height, 1);

    CreateResources();

    // TODO: Game window is being resized.
    m_camera->OnResize(m_outputWidth, m_outputHeight);
    m_proj = m_camera->GetProjectionMatrix();
    m_effect->SetProjection(m_proj);
    m_effect2->SetProjection(m_proj);
    m_effect3->SetProjection(m_proj);
}

// Presents the back buffer contents to the screen.
void Game::Present()
{
    // The first argument instructs DXGI to block until VSync, putting the application
    // to sleep until the next VSync. This ensures we don't waste any cycles rendering
    // frames that will never be displayed to the screen.
    HRESULT hr = m_swapChain->Present(1, 0);

    // If the device was reset we must completely reinitialize the renderer.
    if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
    {
        OnDeviceLost();
    }
    else
    {
        DX::ThrowIfFailed(hr);
    }
}

void Game::SetFogVals(const DirectX::SimpleMath::Vector3 aTargetPos, const float aDimmerVal)
{
    const float fogStartStopGap = 1.0;
    float distanceToTarget = DirectX::SimpleMath::Vector3::Distance(m_camera->GetPos(), aTargetPos);

    float fogStart = distanceToTarget - aDimmerVal;
    float fogEnd = distanceToTarget + (fogStartStopGap - aDimmerVal);
    float testVal = fogEnd - distanceToTarget;

    m_debugValue3 = distanceToTarget;
    m_debugValue4 = testVal;
    m_debugValue5 = distanceToTarget - fogStart;

    m_effect->SetFogEnabled(true);
    m_effect->SetFogStart(fogEnd);
    m_effect->SetFogEnd(fogStart);
}

void Game::SetFogVals2(const DirectX::SimpleMath::Vector3 aTargetPos, const float aDimmerVal)
{
    const float fogStartStopGap = 1.0;
    float distanceToTarget = DirectX::SimpleMath::Vector3::Distance(m_camera->GetPos(), aTargetPos);

    float fogStart = distanceToTarget - aDimmerVal;
    float fogEnd = distanceToTarget + (fogStartStopGap - aDimmerVal);

    m_effect2->SetFogEnabled(true);
    m_effect2->SetFogStart(fogEnd);
    m_effect2->SetFogEnd(fogStart);
}

void Game::SetFogVals3(const DirectX::SimpleMath::Vector3 aTargetPos, const float aDimmerVal)
{
    const float fogStartStopGap = 1.0;
    float distanceToTarget = DirectX::SimpleMath::Vector3::Distance(m_camera->GetPos(), aTargetPos);

    float fogStart = distanceToTarget - aDimmerVal;
    float fogEnd = distanceToTarget + (fogStartStopGap - aDimmerVal);

    m_effect3->SetFogEnabled(true);
    m_effect3->SetFogStart(fogEnd);
    m_effect3->SetFogEnd(fogStart);
}

void Game::SetTerrainGridDimmer(const float aDimmerVal)
{
    /*
    for (int i = 0; i < m_terrainVertexCount; ++i)
    {
        DirectX::SimpleMath::Vector4 dimmerColor(0.486274540f, 0.988235354f, 0.000000000f, 1.000000000f);// = DirectX::Colors::LawnGreen;
        //DirectX::SimpleMath::Vector4 dimmerColor = m_terrainVertexArray[i].color;
        dimmerColor *= aDimmerVal;
        m_terrainVertexArray[i].color = dimmerColor;
    }
    */
    /*
    for (int i = 0; i < m_terrainStartScreen.terrainVertexCount; ++i)
    {
        DirectX::SimpleMath::Vector4 dimmerColor(0.486274540f, 0.988235354f, 0.000000000f, 1.000000000f);// = DirectX::Colors::LawnGreen;
        //DirectX::SimpleMath::Vector4 dimmerColor = m_terrainVertexArray[i].color;
        dimmerColor *= aDimmerVal;
        m_terrainStartScreen.terrainVertexArray[i].color = dimmerColor;
    }
    */
    DirectX::SimpleMath::Vector4 dimmerColor(0.486274540f, 0.988235354f, 0.000000000f, 1.000000000f);// = DirectX::Colors::LawnGreen;
    dimmerColor *= aDimmerVal;
    m_startScreenGridDimmerColor = dimmerColor;
}

void Game::TestDraw()
{
    const float timeStamp = static_cast<float>(m_timer.GetTotalSeconds());

    DirectX::SimpleMath::Vector3 fogTarget(1.1, 0.0, 0.0);
    DirectX::SimpleMath::Vector3 fogTarget2(1.1, 0.75, 0.0);
    DirectX::SimpleMath::Vector3 fogTarget3(0.5, 0.0, 0.0);

    float dimmer = ((cosf(static_cast<float>(timeStamp)) + 1.f) * 0.5f);

    SetFogVals(fogTarget, dimmer - .5f);
    SetFogVals2(fogTarget2, dimmer - 1.5f);
    SetFogVals3(fogTarget3, dimmer - 1.5f);

    DirectX::SimpleMath::Vector3 testCamPos = m_camera->GetPos();
    if (testCamPos.x < -1.0)
    {
        int testBreak = 0;
        ++testBreak;
    }

    if (m_lighting->GetLightingState() != Lighting::LightingState::LIGHTINGSTATE_MANUAL)
    {
        m_lighting->SetLighting(Lighting::LightingState::LIGHTINGSTATE_BMW);
    }
    m_currentGameState = GameState::GAMESTATE_STARTSCREEN;

    m_effect->SetTexture(m_textureAutoGame.Get());
    m_effect->SetNormalTexture(m_normalMapAutoGame.Get());
    m_effect->SetSpecularTexture(m_specularAutoGame.Get());

    int time = static_cast<int>(timeStamp * .5);
    if (time % 2 == 0)
    {
        //m_effect->SetNormalTexture(m_normalMapBMW2.Get());
        //m_effect->SetSpecularTexture(m_specularTeaser.Get());
    }
    else
    {
        //m_effect->SetNormalTexture(m_normalMapBMW2.Get());
        //m_effect->SetSpecularTexture(m_specularTest.Get());
    }


    DrawLogoScreen();
}

// Draws the scene.
void Game::Render()
{
    // Don't try to render anything before the first Update.
    if (m_timer.GetFrameCount() == 0)
    {
        return;
    }

    Clear();

    //UpdateLighting();

    // TODO: Add your rendering code here.
    // WLJ start
    m_d3dContext->OMSetBlendState(m_states->Opaque(), nullptr, 0xFFFFFFFF);
    //m_d3dContext->OMSetDepthStencilState(m_states->DepthNone(), 0); // doesnt facecull
    m_d3dContext->OMSetDepthStencilState(m_states->DepthDefault(), 0);
    //m_d3dContext->RSSetState(m_states->CullNone());
    //m_d3dContext->RSSetState(m_states->CullClockwise());
    //10  m_d3dContext->OMSetDepthStencilState(m_states->DepthDefault(), 0);
    //11  m_d3dContext->RSSetState(m_states->CullCounterClockwise());

    //world start
    m_d3dContext->RSSetState(m_raster.Get()); // WLJ anti-aliasing  RenderTesting
    //m_d3dContext->RSSetState(m_states->CullNone());

    void const* shaderByteCode;
    size_t byteCodeLength;
    m_effect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);
    DX::ThrowIfFailed(m_d3dDevice->CreateInputLayout(VertexType::InputElements, VertexType::InputElementCount, shaderByteCode, byteCodeLength, m_inputLayout.ReleaseAndGetAddressOf()));
    m_batch = std::make_unique<PrimitiveBatch<VertexType>>(m_d3dContext.Get());
    m_effect->SetWorld(m_world);
    //world end

    /*
    m_effect->SetFogEnabled(true);
    m_effect->SetFogStart(cos(m_timer.GetTotalSeconds()) + 0.0);
    m_effect->SetFogEnd(cos(m_timer.GetTotalSeconds()) + 1.0);
    */

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
    //auto sampler = m_states->LinearClamp();
    //m_d3dContext->PSSetSamplers(0, 1, &sampler);

    m_d3dContext->IASetInputLayout(m_inputLayout.Get());
    m_effect->Apply(m_d3dContext.Get());
    m_batch->Begin();
    /*
    m_effect->EnableDefaultLighting();
    //m_effect->SetTexture(m_textureJI.Get());
    //m_effect->SetNormalTexture(m_normalMapJI.Get());
    //m_effect->SetSpecularTexture(m_specularJI.Get());
    m_effect->SetView(m_camera->GetViewMatrix());
    m_effect->SetProjection(m_camera->GetProjectionMatrix());
    

    //DirectX::XMVECTORF32 DomeColor = DirectX::XMVECTORF32(0.1f, 0.1f, 0.1f, 1.0f);
    DirectX::SimpleMath::Vector4 DomeColor(0.0f, 0.0f, 0.15f, 1.0f);
    m_shape->Draw(DirectX::SimpleMath::Matrix::Identity, m_camera->GetViewMatrix(), m_camera->GetProjectionMatrix(), DomeColor);
    //m_shape->Draw(m_effect.get(), m_inputLayout.Get());
    //m_shape->Draw(DirectX::SimpleMath::Matrix::Identity, m_camera->GetViewMatrix(), m_camera->GetProjectionMatrix(), DirectX::Colors::White, m_effect.get());
    */

    /*
    * m_vehicle->DrawModel(m_camera->GetViewMatrix(), m_proj);
    m_heliModel.noseConeShape->Draw(m_heliModel.noseConeBellyMatrix, aView, aProj, m_heliModel.undersideColor); 
    
    auto ilights = dynamic_cast<DirectX::IEffectLights*>(m_effect.get());
    if (ilights)
    {
        double aTimer = m_timer.GetTotalSeconds();
        const float timeStamp = static_cast<float>(aTimer);
        ilights->EnableDefaultLighting();
        ilights->SetLightEnabled(0, false);
        ilights->SetLightEnabled(1, false);
        ilights->SetLightEnabled(2, false);
        auto time = static_cast<float>(aTimer);
        float yaw = time * 1.1f;
        float roll = time * 1.1f;
        roll = cosf(-timeStamp * 1.2f);
        auto quat0 = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(-yaw, 0.0, 0.0);
        auto quat1 = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(yaw, 0.0, 0.0);
        auto quat2 = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(0.0, 0.0, roll);
        auto light0 = XMVector3Rotate(DirectX::SimpleMath::Vector3::UnitX, quat0);
        auto light1 = XMVector3Rotate(DirectX::SimpleMath::Vector3::UnitX, quat1);
        auto light2 = XMVector3Rotate(DirectX::SimpleMath::Vector3::UnitX, quat2);

        float roll2 = time * 3.1f;
        auto quat = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(0.0, roll2, 0.0);
        DirectX::SimpleMath::Vector3 axis = -DirectX::SimpleMath::Vector3::UnitZ;
        DirectX::SimpleMath::Vector3 light = XMVector3Rotate(axis, quat);
        light.x += 1.0;

        light.Normalize();
        light0 = light;

        light0 = -DirectX::SimpleMath::Vector3::UnitZ;
        light1 = -DirectX::SimpleMath::Vector3::UnitZ;
        light2 = -DirectX::SimpleMath::Vector3::UnitZ;


        ilights->SetLightDirection(0, light0);
        ilights->SetLightDirection(1, light1);
        ilights->SetLightDirection(2, light2);
        m_lightPos0 = light0;
        m_lightPos1 = light1;
        m_lightPos2 = light2;

        ilights->SetAmbientLightColor(DirectX::Colors::White);

    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //m_lighting->UpdateLighting(m_effect, m_timer.GetTotalSeconds());
    */

    
    if (m_currentGameState == GameState::GAMESTATE_INTROSCREEN || m_currentGameState == GameState::GAMESTATE_STARTSCREEN)
    {
        DrawIntroScene();
    }

    //DrawDebugLines();
    //DrawStartScreen();
    if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
    {
        float maxTime1 = m_testTimer1;
        float maxTime2 = m_testTimer2;
        m_vehicle->DrawModel(m_camera->GetViewMatrix(), m_proj);
        DrawSky();
        //DrawStartScreen();
        if (m_isInDebugMode == true)
        {
            //DrawCameraFocus();
            //DrawDebugLines();
        }
    }
    if (m_currentGameState == GameState::GAMESTATE_GAMEPLAYSTART)
    {
        DrawGamePlayStart();
        m_vehicle->DrawModel(m_camera->GetViewMatrix(), m_proj);
        DrawSky();
    }
   
    m_batch->End();

    void const* shaderByteCode2;
    size_t byteCodeLength2;
    m_effect2->GetVertexShaderBytecode(&shaderByteCode2, &byteCodeLength2);
    DX::ThrowIfFailed(m_d3dDevice->CreateInputLayout(VertexType2::InputElements, VertexType2::InputElementCount, shaderByteCode2, byteCodeLength2, m_inputLayout.ReleaseAndGetAddressOf()));
    const int maxVertices = 8192;
    const int maxIndices = maxVertices * 3;
    m_batch2 = std::make_unique<PrimitiveBatch<VertexType2>>(m_d3dContext.Get(), maxIndices, maxVertices);


    //m_effect2->SetWorld(m_world);
    //m_effect2->EnableDefaultLighting();
    m_effect2->Apply(m_d3dContext.Get());

    //m_d3dContext->PSSetSamplers(0, 1, &sampler);
    m_d3dContext->IASetInputLayout(m_inputLayout.Get());

    m_batch2->Begin();
    //DrawLightBar();
    //DrawCameraFocus();
    //DrawLightFocus1();
    //DrawLightFocus2();
    //DrawLightFocus3();
    //DrawWorld(); 

    if (m_currentGameState == GameState::GAMESTATE_GAMEPLAYSTART)
    {
        DrawTerrainNew(m_terrainGamePlay);
    }
    if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
    {
        DrawTerrainNew(m_terrainGamePlay);
        //DrawTerrainNew(m_terrainStartScreen);
    }
    if (m_currentGameState == GameState::GAMESTATE_STARTSCREEN)
    {
        //DrawTerrain2();
        DrawTerrainNew(m_terrainStartScreen);
    }
    if (m_currentGameState == GameState::GAMESTATE_TEASERSCREEN)
    {
        //DrawLightBar();
    }

    //DrawLoadScreen();
    
    //m_effect2->EnableDefaultLighting();
    
    auto ilights2 = dynamic_cast<DirectX::IEffectLights*>(m_effect2.get());
    if (ilights2)
    {
        ilights2->SetLightEnabled(0, true);
        ilights2->SetLightEnabled(1, true);
        ilights2->SetLightEnabled(2, true);

        auto time = static_cast<float>(m_timer.GetTotalSeconds());

        float yaw = time * 0.4f;
        float pitch = time * 0.7f;
        float roll = time * 1.1f;

        auto quat = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(pitch, yaw, roll);

        auto light2 = XMVector3Rotate(DirectX::SimpleMath::Vector3::UnitX, quat);

        light2 = -DirectX::SimpleMath::Vector3::UnitX;

        //light2 = m_lightPos1;
        //light2 = DirectX::SimpleMath::Vector3::UnitY;
        ilights2->SetLightDirection(0, light2);
        ilights2->SetLightDirection(1, light2);
        ilights2->SetLightDirection(2, light2);
        ilights2->EnableDefaultLighting();   
    }
    


    /*
    m_effect2->SetFogEnabled(false);
    m_effect2->SetFogStart(0.2f);
    m_effect2->SetFogColor(DirectX::Colors::Black);
    m_effect2->SetFogEnd(1.0f);
    */

    //m_vehicle->DrawModel(m_camera->GetViewMatrix(), m_proj, m_effect2, m_inputLayout);
    m_effect2->Apply(m_d3dContext.Get());
   
    m_batch2->End();

    void const* shaderByteCode3;
    size_t byteCodeLength3;
    m_effect3->GetVertexShaderBytecode(&shaderByteCode3, &byteCodeLength3);
    DX::ThrowIfFailed(m_d3dDevice->CreateInputLayout(VertexType3::InputElements, VertexType3::InputElementCount, shaderByteCode3, byteCodeLength3, m_inputLayout.ReleaseAndGetAddressOf()));
    m_batch3 = std::make_unique<PrimitiveBatch<VertexType3>>(m_d3dContext.Get());
    m_d3dContext->IASetInputLayout(m_inputLayout.Get());

    m_effect3->Apply(m_d3dContext.Get());

    m_batch3->Begin();
    
    //DrawTerrainNormals();
    //DrawDebugNormalLines(m_vehicle->GetModelTestPos(), DirectX::Colors::Blue);
    DrawDebugLinesVector();
    DrawDebugLinesVectorNew();
    //DrawGridForStartScreen();
    if (m_currentGameState == GameState::GAMESTATE_STARTSCREEN)
    {
        DrawGridForStartScreen();
        //DrawTerrain();
    }
    m_batch3->End();

    m_spriteBatch->Begin();
    //DrawDebugValue();


    if (m_currentGameState == GameState::GAMESTATE_INTROSCREEN)
    {
        DrawUIIntroScreen();
    }
    if (m_currentGameState == GameState::GAMESTATE_STARTSCREEN)
    {
        //DrawStartScreenOld();
        DrawUIIntroScreen();
    }
    if (m_currentGameState == GameState::GAMESTATE_MAINMENU)
    {
        DrawMenuMain();
    }
    if (m_currentGameState == GameState::GAMESTATE_CHARACTERSELECT)
    {

    }
    if (m_currentGameState == GameState::GAMESTATE_ENVIRONTMENTSELECT)
    {
        DrawMenuEnvironmentSelect();
    }
    if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
    {
        DrawDebugDataUI();
        //DrawDebugVehicleData();
        //DrawUI();
    }
    if (m_currentGameState == GameState::GAMESTATE_TEASERSCREEN)
    {
        //DrawTeaserScreen();
    }
    //DrawDebugDataUI();
    m_spriteBatch->End();

    Present();
}

void Game::ResetGamePlay()
{
    //pAuto->ZeroUIandRenderData();
    m_swingPathStep = 0;
    m_projectilePathStep = 0;
}

// Executes the basic game loop.
void Game::Tick()
{
    m_timer.Tick([&]()
        {
            Update(m_timer);
        });

    Render();
}

void Game::Update(DX::StepTimer const& aTimer)
{
    double elapsedTime = double(aTimer.GetElapsedSeconds());
    m_testTimer = m_timer.GetTotalSeconds() + m_testTimerOffset;

    // TODO: Add your game logic here.

    if (m_currentGameState == GameState::GAMESTATE_CHARACTERSELECT)
    {
        if (m_menuSelect == 0)
        {
            m_character0->Update(static_cast<float>(elapsedTime));
        }
        if (m_menuSelect == 1)
        {
            m_character1->Update(static_cast<float>(elapsedTime));
        }
        if (m_menuSelect == 2)
        {
            m_character2->Update(static_cast<float>(elapsedTime));
        }
    }

    if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
    {

    }

    // audio
    if (m_retryAudio)
    {
        m_retryAudio = false;
        if (m_audioEngine->Reset())
        {
            // ToDo: restart any looped sounds here
            if (m_audioMusicStream)
            {
                m_audioMusicStream->Play(true);
            }
            if (m_audioEffectStream)
            {
                m_audioEffectStream->Play(); // WLJ this could lead to problems and might not be needed, maybe cause unwanted effect to play after reset?
            }
        }
    }
    else if (!m_audioEngine->Update())
    {
        if (m_audioEngine->IsCriticalError())
        {
            m_retryAudio = true;
        }
    }
    /*
    if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
    {
        m_effect->SetTexture(m_textureJI.Get());
        m_effect->SetNormalTexture(m_normalMapJI.Get());
        m_effect->SetSpecularTexture(m_specularJI.Get());
    }
    */
    //m_lighting->SetLighting(Lighting::LightingState::LIGHTINGSTATE_BMW);
    m_debugData->DebugClearUI();
    UpdateInput(aTimer);
    m_vehicle->UpdateVehicle(aTimer.GetElapsedSeconds());
    
    m_camera->UpdateCamera(aTimer);
        
    m_lighting->UpdateLighting(m_effect, aTimer.GetTotalSeconds());

    CameraState testState = m_camera->GetCameraState();
    /*
    m_lighting->UpdateLightingNormColorTextureVertex(m_effect, aTimer.GetTotalSeconds());
    m_lighting->UpdateLightingNormColorVertex2(m_effect2, aTimer.GetTotalSeconds());
    m_lighting->UpdateLightingColorVertex3(m_effect3, aTimer.GetTotalSeconds());
    */
    m_proj = m_camera->GetProjectionMatrix();
    m_effect->SetProjection(m_proj);
    m_effect2->SetProjection(m_proj);
    m_effect3->SetProjection(m_proj);

    DirectX::SimpleMath::Matrix viewMatrix = m_camera->GetViewMatrix();
    m_effect->SetView(viewMatrix);
    m_effect2->SetView(viewMatrix);
    m_effect3->SetView(viewMatrix);


}

void Game::UpdateInput(DX::StepTimer const& aTimer)
{
    // WLJ add for mouse and keybord interface   
    auto kb = m_keyboard->GetState();
    m_kbStateTracker.Update(kb);

    if (kb.Escape)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {
            m_camera->SetCameraState(CameraState::CAMERASTATE_PRESWINGVIEW);
            ResetGamePlay();
        }
        m_currentGameState = GameState::GAMESTATE_MAINMENU;
    }
    if (m_kbStateTracker.pressed.Enter)
    {
        if (m_currentGameState == GameState::GAMESTATE_ENVIRONTMENTSELECT)
        {
            if (m_menuSelect == 0)
            {

            }
            if (m_menuSelect == 1)
            {

            }
            if (m_menuSelect == 2)
            {

            }
            m_menuSelect = 0;
            m_currentGameState = GameState::GAMESTATE_STARTSCREEN;
        }
        if (m_currentGameState == GameState::GAMESTATE_CHARACTERSELECT)
        {
            if (m_menuSelect == 0)
            {

            }
            if (m_menuSelect == 1)
            {

            }
            if (m_menuSelect == 2)
            {

            }
            m_menuSelect = 0;
            //m_currentGameState = GameState::GAMESTATE_MAINMENU; // Return to Main Menu after selecting character, ToDo: using value of 1 doesn't return to main menu
            m_currentGameState = GameState::GAMESTATE_STARTSCREEN;// Return to Main Menu after selecting character, ToDo: using value of 1 doesn't return to main menu
        }
        if (m_currentGameState == GameState::GAMESTATE_MAINMENU)
        {
            if (m_menuSelect == 0) // GoTo Game State
            {
                m_currentGameState = GameState::GAMESTATE_GAMEPLAY;
                m_menuSelect = 0;
            }
            if (m_menuSelect == 1) // GoTo Character Select State
            {
                m_currentGameState = GameState::GAMESTATE_CHARACTERSELECT;
                m_menuSelect = 0;
            }
            if (m_menuSelect == 2) // GoTo Environment Select State
            {
                m_currentGameState = GameState::GAMESTATE_ENVIRONTMENTSELECT;
                m_menuSelect = 0;
            }
            /*
            if (m_menuSelect == 3) // GoTo Demo Select State
            {
                m_currentGameState = GameState::GAMESTATE_GAMEPLAY;
            }
            */
            if (m_menuSelect == 3) // Quit Game
            {
                ExitGame();
            }
            //m_menuSelect = 0;
        }
        if (m_currentGameState == GameState::GAMESTATE_STARTSCREEN)
        {

        }
    }
    if (m_kbStateTracker.pressed.Up)
    {
        if (m_currentGameState == GameState::GAMESTATE_MAINMENU)
        {
            --m_menuSelect;
        }
        if (m_currentGameState == GameState::GAMESTATE_CHARACTERSELECT)
        {
            --m_menuSelect;
        }
        if (m_currentGameState == GameState::GAMESTATE_ENVIRONTMENTSELECT)
        {
            --m_menuSelect;
        }
    }
    if (m_kbStateTracker.pressed.Down)
    {
        if (m_currentGameState == GameState::GAMESTATE_MAINMENU)
        {
            ++m_menuSelect;
        }
        if (m_currentGameState == GameState::GAMESTATE_CHARACTERSELECT)
        {
            ++m_menuSelect;
        }
        if (m_currentGameState == GameState::GAMESTATE_ENVIRONTMENTSELECT)
        {
            ++m_menuSelect;
        }
    }
    if (m_kbStateTracker.pressed.Left)
    {
        if (m_currentGameState == GameState::GAMESTATE_CHARACTERSELECT)
        {
            --m_menuSelect;
        }
        if (m_currentGameState == GameState::GAMESTATE_ENVIRONTMENTSELECT)
        {
            --m_menuSelect;
        }
    }
    if (m_kbStateTracker.pressed.Right)
    {
        if (m_currentGameState == GameState::GAMESTATE_CHARACTERSELECT)
        {
            ++m_menuSelect;
        }
        if (m_currentGameState == GameState::GAMESTATE_ENVIRONTMENTSELECT)
        {
            ++m_menuSelect;
        }
    }
    if (kb.Up)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {

        }
    }
    if (kb.Down)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {

        }
    }
    if (kb.Left)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {

        }
    }
    if (kb.Right)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {

        }
    }
    if (kb.D)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY && m_camera->GetCameraState() == CameraState::CAMERASTATE_FOLLOWVEHICLE || m_currentGameState == GameState::GAMESTATE_GAMEPLAYSTART)
        {
            m_vehicle->InputYawPedal(static_cast<float>(aTimer.GetElapsedSeconds()));
        }
        if (m_camera->GetCameraState() == CameraState::CAMERASTATE_FIRSTPERSON)
        {
            m_camera->UpdatePos(0.0f + static_cast<float>(aTimer.GetElapsedSeconds()), 0.0f, 0.0f);
        }
    }
    if (kb.S)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY && m_camera->GetCameraState() == CameraState::CAMERASTATE_FOLLOWVEHICLE || m_currentGameState == GameState::GAMESTATE_GAMEPLAYSTART)
        {
            m_vehicle->InputCollective(static_cast<float>(-aTimer.GetElapsedSeconds()));
        }
        if (m_camera->GetCameraState() == CameraState::CAMERASTATE_FIRSTPERSON)
        {
            m_camera->UpdatePos(0.0f, 0.0f, 0.0f - static_cast<float>(aTimer.GetElapsedSeconds()));
        }
    }
    if (kb.A)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY && m_camera->GetCameraState() == CameraState::CAMERASTATE_FOLLOWVEHICLE || m_currentGameState == GameState::GAMESTATE_GAMEPLAYSTART)
        {
            m_vehicle->InputYawPedal(static_cast<float>(-aTimer.GetElapsedSeconds()));
        }
        if (m_camera->GetCameraState() == CameraState::CAMERASTATE_FIRSTPERSON)
        {
            m_camera->UpdatePos(0.0f - static_cast<float>(aTimer.GetElapsedSeconds()), 0.0f, 0.0f);
        }
    }
    if (kb.W)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY && m_camera->GetCameraState() == CameraState::CAMERASTATE_FOLLOWVEHICLE || m_currentGameState == GameState::GAMESTATE_GAMEPLAYSTART)
        {
            m_vehicle->InputCollective(static_cast<float>(aTimer.GetElapsedSeconds()));
        }
        if (m_camera->GetCameraState() == CameraState::CAMERASTATE_FIRSTPERSON)
        {
            m_camera->UpdatePos(0.0f, 0.0f, 0.0f + static_cast<float>(aTimer.GetElapsedSeconds()));
        }
    }
    if (kb.Q)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY && m_camera->GetCameraState() == CameraState::CAMERASTATE_FOLLOWVEHICLE || m_currentGameState == GameState::GAMESTATE_GAMEPLAYSTART)
        {
            m_vehicle->InputThrottle(static_cast<float>(-aTimer.GetElapsedSeconds()));
        }
        if (m_camera->GetCameraState() == CameraState::CAMERASTATE_FIRSTPERSON)
        {
            m_camera->UpdatePitchYaw(0.0f, 0.0f + static_cast<float>(aTimer.GetElapsedSeconds()));
        }
    }
    if (kb.E)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY && m_camera->GetCameraState() == CameraState::CAMERASTATE_FOLLOWVEHICLE || m_currentGameState == GameState::GAMESTATE_GAMEPLAYSTART)
        {
            m_vehicle->InputThrottle(static_cast<float>(aTimer.GetElapsedSeconds()));
        }
        if (m_camera->GetCameraState() == CameraState::CAMERASTATE_FIRSTPERSON)
        {
            m_camera->UpdatePitchYaw(0.0f, 0.0f - static_cast<float>(aTimer.GetElapsedSeconds()));
        }
    }
    if (kb.F)
    {
        if (m_camera->GetCameraState() == CameraState::CAMERASTATE_FIRSTPERSON)
        {
            m_camera->UpdatePos(0.0f, 0.0f + static_cast<float>(aTimer.GetElapsedSeconds()), 0.0f);
        }
    }
    if (kb.C)
    {
        if (m_camera->GetCameraState() == CameraState::CAMERASTATE_FIRSTPERSON)
        {
            m_camera->UpdatePos(0.0f, 0.0f - static_cast<float>(aTimer.GetElapsedSeconds()), 0.0f);
        }
    }
    if (m_kbStateTracker.pressed.D1)
    {
        if (m_moveLightWithCameraFocus1 == true)
        {
            m_moveLightWithCameraFocus1 = false;
        }
        else
        {
            m_moveLightWithCameraFocus1 = true;
        }
    }
    if (m_kbStateTracker.pressed.D2)
    {
        if (m_moveLightWithCameraFocus2 == true)
        {
            m_moveLightWithCameraFocus2 = false;
        }
        else
        {
            m_moveLightWithCameraFocus2 = true;
        }
    }
    if (m_kbStateTracker.pressed.D3)
    {
        if (m_moveLightWithCameraFocus3 == true)
        {
            m_moveLightWithCameraFocus3 = false;
        }
        else
        {
            m_moveLightWithCameraFocus3 = true;
        }
    }
    if (m_kbStateTracker.pressed.T)
    {
        m_lighting->SetLighting(Lighting::LightingState::LIGHTINGSTATE_MANUAL);
    }
    if (m_kbStateTracker.pressed.Y)
    {
        if (m_camera->GetCameraState() == CameraState::CAMERASTATE_SPRINGCAMERA)
        {
            m_camera->SetCameraState(CameraState::CAMERASTATE_FOLLOWVEHICLE);
        }
        else if (m_camera->GetCameraState() == CameraState::CAMERASTATE_FOLLOWVEHICLE)
        {
            m_camera->SetCameraState(CameraState::CAMERASTATE_FIRSTPERSON);
        }
        else
        {
            m_camera->SetCameraState(CameraState::CAMERASTATE_SPRINGCAMERA);
        }
    }
    if (m_kbStateTracker.pressed.F1)
    {
        m_camera->SetCameraState(CameraState::CAMERASTATE_SWINGVIEW);
    }
    if (kb.NumPad1)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {
            m_lightPos0.y -= static_cast<float>(aTimer.GetElapsedSeconds()) * m_lightMovementSpeed;
            m_vehicle->InputCollective(static_cast<float>(-aTimer.GetElapsedSeconds()));
        }
    }
    if (kb.NumPad2)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {
            m_lightPos1.x -= static_cast<float>(aTimer.GetElapsedSeconds()) * m_lightMovementSpeed;
            m_vehicle->InputCyclicPitch(static_cast<float>(aTimer.GetElapsedSeconds()));
        }
    }
    if (kb.NumPad3)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {
            m_lightPos0.y += static_cast<float>(aTimer.GetElapsedSeconds()) * m_lightMovementSpeed;
            m_vehicle->InputCollective(static_cast<float>(aTimer.GetElapsedSeconds()));
        }
    }
    if (kb.NumPad4)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {
            m_lightPos1.z -= static_cast<float>(aTimer.GetElapsedSeconds()) * m_lightMovementSpeed;
            m_vehicle->InputCyclicRoll(static_cast<float>(-aTimer.GetElapsedSeconds()));
        }
    }
    if (kb.NumPad5)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {

        }
    }
    if (kb.NumPad6)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {
            m_lightPos1.z += static_cast<float>(aTimer.GetElapsedSeconds()) * m_lightMovementSpeed;
            m_vehicle->InputCyclicRoll(static_cast<float>(aTimer.GetElapsedSeconds()));
        }
    }
    if (kb.NumPad7)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {
            m_lightPos1.y -= static_cast<float>(aTimer.GetElapsedSeconds()) * m_lightMovementSpeed;
            m_vehicle->InputYawPedal(static_cast<float>(-aTimer.GetElapsedSeconds()));
        }
    }
    if (kb.NumPad8)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {
            m_lightPos1.x += static_cast<float>(aTimer.GetElapsedSeconds()) * m_lightMovementSpeed;
            m_vehicle->InputCyclicPitch(static_cast<float>(-aTimer.GetElapsedSeconds()));
        }
    }
    if (kb.NumPad9)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {
            m_lightPos1.y += static_cast<float>(aTimer.GetElapsedSeconds()) * m_lightMovementSpeed;
            m_vehicle->InputYawPedal(static_cast<float>(aTimer.GetElapsedSeconds()));
        }
    }
    if (kb.NumPad0)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {
            m_vehicle->InputThrottle(static_cast<float>(-aTimer.GetElapsedSeconds()));
        }
    }
    if (kb.Decimal)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {
            m_vehicle->InputThrottle(static_cast<float>(aTimer.GetElapsedSeconds()));
        }
    }
    if (kb.I)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {
            m_lightPos2.x += static_cast<float>(aTimer.GetElapsedSeconds()) * m_lightMovementSpeed;
        }
    }
    if (kb.K)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {
            m_lightPos2.x -= static_cast<float>(aTimer.GetElapsedSeconds()) * m_lightMovementSpeed;
        }
    }
    if (kb.J)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {
            m_lightPos2.z -= static_cast<float>(aTimer.GetElapsedSeconds()) * m_lightMovementSpeed;
        }
    }
    if (m_kbStateTracker.pressed.R)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {
            m_vehicle->ResetVehicle();
        }
    }
    if (m_kbStateTracker.pressed.U)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {
            DirectX::SimpleMath::Vector3 endPos(11.0, 2.0f, 0.0f);
            DirectX::SimpleMath::Vector3 targetEndPos = m_vehicle->GetPos();
            DirectX::SimpleMath::Vector3 centerPointPos = m_vehicle->GetPos();
            float rotation = Utility::ToRadians(90.0);
            m_camera->SetCameraStartPos(m_camera->GetPos());
            m_camera->SetCameraEndPos(endPos);
            m_camera->SetTargetStartPos(m_camera->GetTargetPos());
            m_camera->SetTargetEndPos(targetEndPos);
            m_camera->TurnEndPosAroundPoint(rotation, centerPointPos);
            m_camera->SetCameraState(CameraState::CAMERASTATE_TRANSITION);
        }
    }
    if (kb.I)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {
            m_camera->SpinClockwise(static_cast<float>(aTimer.GetElapsedSeconds()));
        }
    }
    if (kb.O)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {
            m_camera->SpinCounterClockwise(static_cast<float>(aTimer.GetElapsedSeconds()));
        }
    }
    if (m_kbStateTracker.pressed.N)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {
            ++m_testDisplayCount;
            if (m_testDisplayCount > m_testDisplayCountMax)
            {
                m_testDisplayCount = 0;
            }
        }
    }
    if (m_kbStateTracker.pressed.P)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {
            m_camera->SetCameraState(CameraState::CAMERASTATE_SPINCAMERA);
        }
    }
    if (m_kbStateTracker.pressed.L)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {
            //m_camera->SetSpinCameraStart();
            m_vehicle->ToggleLandingGearState();
        }
    }
    if (m_kbStateTracker.released.Z)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {
            m_vehicle->DebugToggle2();
        }
    }
    if (m_kbStateTracker.pressed.X)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {

        }
    }
    if (m_kbStateTracker.pressed.OemQuestion)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {

        }
    }
    if (m_kbStateTracker.pressed.Space)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {
            m_vehicle->Jump();
        }
    }
    if (m_kbStateTracker.released.Q)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {
            m_vehicle->DebugToggle();
        }
    }

    auto mouse = m_mouse->GetState();

    if (m_camera->GetCameraState() == CameraState::CAMERASTATE_FIRSTPERSON)
    {
        if (mouse.positionMode == Mouse::MODE_RELATIVE)
        {
            const float ROTATION_GAIN = 0.004f;
            DirectX::SimpleMath::Vector3 delta = DirectX::SimpleMath::Vector3(float(mouse.x), float(mouse.y), 0.f) * ROTATION_GAIN;

            float pitch = -delta.y;
            float yaw = -delta.x;

            m_camera->UpdatePitchYaw(pitch, yaw);
        }

        m_mouse->SetMode(mouse.leftButton ? Mouse::MODE_RELATIVE : Mouse::MODE_ABSOLUTE);
    }

    auto pad = m_gamePad->GetState(0);
    if (pad.IsConnected())
    {
        m_gamePadTracker.Update(pad);
        m_vehicle->SetGamePadConnectionState(true);

        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY || m_currentGameState == GameState::GAMESTATE_GAMEPLAYSTART)
        {
            // Pitch
            if (pad.thumbSticks.leftY > m_gamePadInputDeadZone || pad.thumbSticks.leftY < -m_gamePadInputDeadZone)
            {
                //m_vehicle->InputCyclicRoll(static_cast<float>(aTimer.GetElapsedSeconds()));
                const float inputMod = m_gamePadInputRatePitch;
                m_vehicle->InputCyclicPitchGamePad(-pad.thumbSticks.leftY * inputMod, static_cast<float>(aTimer.GetElapsedSeconds()));
                //m_vehicle->InputCyclicPitchGamePad(-pad.thumbSticks.leftY * static_cast<float>(aTimer.GetElapsedSeconds()));
                //m_debugData->DebugPushUILineDecimalNumber("pad.thumbSticks.leftX  ", pad.thumbSticks.leftX * inputMod, "");
            }
            // Roll
            if (pad.thumbSticks.leftX > m_gamePadInputDeadZone || pad.thumbSticks.leftX < -m_gamePadInputDeadZone)
            {
                //m_vehicle->InputCyclicRoll(static_cast<float>(aTimer.GetElapsedSeconds()));
                const float inputMod = m_gamePadInputRateRoll;
                //m_vehicle->InputCyclicRollGamePad(pad.thumbSticks.leftX * inputMod);
                m_vehicle->InputCyclicRollGamePad(pad.thumbSticks.leftX* inputMod, static_cast<float>(aTimer.GetElapsedSeconds()));
                //m_vehicle->InputCyclicRollGamePad(-pad.thumbSticks.leftX * static_cast<float>(aTimer.GetElapsedSeconds()));
                //m_debugData->DebugPushUILineDecimalNumber("pad.thumbSticks.leftX  ", pad.thumbSticks.leftX * inputMod, "");
            }
            // Yaw
            if (pad.IsLeftTriggerPressed() == true)
            {
                const float inputMod = m_gamePadInputRateYaw;
                m_vehicle->InputYawPedalGamePad(-pad.triggers.left * inputMod, static_cast<float>(aTimer.GetElapsedSeconds()));
            }
            if (pad.IsRightTriggerPressed() == true)
            {
                const float inputMod = m_gamePadInputRateYaw;
                m_vehicle->InputYawPedalGamePad(pad.triggers.right * inputMod, static_cast<float>(aTimer.GetElapsedSeconds()));
            }
            // Collective
            if (pad.thumbSticks.rightY > m_gamePadInputDeadZone || pad.thumbSticks.rightY < -m_gamePadInputDeadZone)
            {
                const float inputMod = m_gamePadInputRateCollective;
                m_vehicle->InputCollectiveGamePad(pad.thumbSticks.rightY * inputMod);
            }
            // Throttle
            if (pad.IsLeftShoulderPressed() == true)
            {
                const float inputMod = m_gamePadInputRateThrottle;
                m_vehicle->InputThrottleGamePad(static_cast<float>(-aTimer.GetElapsedSeconds()) * inputMod);
            }
            if (pad.IsRightShoulderPressed() == true)
            {
                const float inputMod = m_gamePadInputRateThrottle;
                m_vehicle->InputThrottleGamePad(static_cast<float>(aTimer.GetElapsedSeconds()) * inputMod);
            }
        }

        
        if (m_gamePadTracker.y == GamePad::ButtonStateTracker::PRESSED)
        {
            if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
            {
                m_vehicle->ToggleLandingGearState();
            }
        }
    }
    else
    {
        m_gamePadTracker.Reset();
        m_vehicle->SetGamePadConnectionState(false);
    }
}
